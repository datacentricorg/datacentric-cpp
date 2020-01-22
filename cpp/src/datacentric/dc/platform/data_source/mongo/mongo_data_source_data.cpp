/*
Copyright (C) 2013-present The DataCentric Authors.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <dc/precompiled.hpp>
#include <dc/implement.hpp>
#include <dc/platform/data_source/mongo/mongo_data_source_data.hpp>
#include <dc/platform/context/context_base.hpp>
#include <dc/types/record/deleted_record.hpp>

#include <dot/mongo/mongo_db/mongo/collection.hpp>
#include <dot/mongo/mongo_db/query/query.hpp>

namespace dc
{
    record mongo_data_source_data_impl::load_or_null(dot::object_id id, dot::type data_type)
    {
        if (cutoff_time != nullptr)
        {
            // If revision_time_constraint is not null, return null for any
            // id that is not strictly before the constraint dot::object_id
            if (id >= cutoff_time.value()) return nullptr;
        }

        dot::query query = dot::make_query(get_collection(data_type), data_type);
        dot::object_cursor_wrapper_base cursor = query->where(new dot::operator_wrapper_impl("_id", "$eq", id))
            ->limit(1)
            ->get_cursor();

        if (cursor->begin() != cursor->end())
        {
            dot::object obj = *(cursor->begin());
            if (!obj.is<deleted_record>())
            {
                record rec = obj.as<record>();

                if (data_type->is_assignable_from(rec->get_type()))
                {
                    // If cast result is null, the record was found but it is an instance
                    // of class that is not derived from TRecord, in this case the API
                    // requires error message, not returning null
                    throw dot::exception(dot::string::format(
                        "Stored type {0} for TemporalId={1} and "
                        "Key={2} is not an instance of the requested type {3}.", rec->get_type()->name(),
                        id.to_string(), rec->get_key(), data_type->name()
                    ));
                }

                // Now we use get_cutoff_time() for the full check
                dot::nullable<dot::object> cutoff_time = get_cutoff_time(rec->data_set);
                if (cutoff_time != nullptr)
                {
                    // Return null for any record that has TemporalId
                    // that is greater than or equal to cutoff_time.
                    if (id >= cutoff_time.value()) return nullptr;
                }

                rec->init(context);
                return rec;
            }
        }

        return nullptr;
    }

    record mongo_data_source_data_impl::load_or_null(key key, dot::object_id load_from)
    {
        // dot::string key in semicolon delimited format used in the lookup
        dot::string key_value = key->to_string();

        dot::type record_type = dot::typeof<record>();

        dot::query base_query = dot::make_query(get_collection(key->get_type()), key->get_type())
            ->where(new dot::operator_wrapper_impl("_key", "$eq", key_value))
            ;

        dot::query query_with_final_constraints = apply_final_constraints(base_query, load_from);

        dot::object_cursor_wrapper_base cursor = query_with_final_constraints
            ->sort_by_descending(record_type->get_field("_dataset"))
            ->then_by_descending(record_type->get_field("_id"))
            ->limit(1)
            ->get_cursor();

        if (cursor->begin() != cursor->end())
        {
            dot::object obj = *(cursor->begin());
            if (!obj.is<deleted_record>())
            {
                record rec = obj.as<record>();
                rec->init(context);
                return rec;
            }
        }

        return nullptr;
    }

    void mongo_data_source_data_impl::save_many(dot::list<record> records, dot::object_id save_to)
    {
        check_not_read_only(save_to);

        auto collection = get_collection(records[0]->get_type());


        // Iterate over list elements to populate fields
        for(record rec : records)
        {
            // This method guarantees that TemporalIds will be in strictly increasing
            // order for this instance of the data source class always, and across
            // all processes and machine if they are not created within the same
            // second.
            dot::object_id object_id = create_ordered_object_id();

            // TemporalId of the record must be strictly later
            // than TemporalId of the dataset where it is stored
            if (object_id <= save_to)
                throw dot::exception(dot::string::format(
                    "Attempting to save a record with dot::object_id={0} that is later "
                    "than dot::object_id={1} of the dataset where it is being saved.", object_id.to_string(), save_to.to_string()));

            // Assign ID and DataSet, and only then initialize, because
            // initialization code may use record.ID and record.DataSet
            rec->id = object_id;
            rec->data_set = save_to;
            rec->init(context);
        }

        collection->insert_many(records);
    }

    mongo_query mongo_data_source_data_impl::get_query(dot::object_id data_set, dot::type type)
    {
        return make_mongo_query(get_collection(type), type, this, data_set);
    }


    void mongo_data_source_data_impl::delete_record(key key, dot::object_id delete_in)
    {
        check_not_read_only(delete_in);

        dot::collection collection = get_collection(key->get_type());

        deleted_record record = make_deleted_record(key);

        dot::object_id object_id = create_ordered_object_id();

        // dot::object_id of the record must be strictly later
        // than dot::object_id of the dataset where it is stored
        if (object_id <= delete_in)
            throw dot::exception(dot::string::format(
                "Attempting to save a record with dot::object_id={0} that is later "
                "than dot::object_id={1} of the dataset where it is being saved.", object_id.to_string(), delete_in.to_string()));

        // Assign id and data_set, and only then initialize, because
        // initialization code may use record.id and record.data_set
        record->id = object_id;
        record->data_set = delete_in;

        collection->insert_one(record);
    }

    dot::query mongo_data_source_data_impl::apply_final_constraints(dot::query query, dot::object_id load_from)
    {
        // Get lookup list by expanding the list of imports to arbitrary
        // depth with duplicates and cyclic references removed.
        //
        // The list will not include datasets that are after the value of
        // CutoffTime if specified, or their imports (including
        // even those imports that are earlier than the constraint).
        dot::hash_set<dot::object_id> lookup_set = get_data_set_lookup_list(load_from);
        dot::list<dot::object_id> lookup_list = dot::make_list<dot::object_id>(std::vector<dot::object_id>(lookup_set->begin(), lookup_set->end()));


        // Apply constraint that the value is _dataset is
        // one of the elements of dataSetLookupList_
        dot::query result = query->where(new dot::operator_wrapper_impl("_dataset", "$in", lookup_list));

        // Apply revision time constraint. By making this constraint the
        // last among the constraints, we optimize the use of the index.
        //
        // The property savedBy_ is set using either CutoffTime element.
        // Only one of these two elements can be set at a given time.
        dot::nullable<dot::object_id> cutoff_time = get_cutoff_time(load_from);
        if (cutoff_time != nullptr)
        {
            result = result->where(new dot::operator_wrapper_impl("_id", "$lt", cutoff_time.value()));
        }

        return result;
    }

    dot::object_id mongo_data_source_data_impl::get_data_set_or_empty(dot::string data_set_id, dot::object_id load_from)
    {
        dot::object_id result;
        if (data_set_dict_->try_get_value(data_set_id, result))
        {
            // Check if already cached, return if found
            return result;
        }
        else
        {
            // Otherwise load from storage (this also updates the dictionaries)
            data_set_key data_set_key = make_data_set_key();
            data_set_key->data_set_id = data_set_id;
            data_set_data data_set_data = (dc::data_set_data)load_or_null(data_set_key, load_from);

            // If not found, return TemporalId.Empty
            if (data_set_data == nullptr) return nullptr;

            // Get or create dataset detail record
            data_set_detail_key data_set_detail_key = make_data_set_detail_key();
            data_set_detail_key->data_set_id = data_set_data->id;
            data_set_detail_data data_set_detail_data = (dc::data_set_detail_data)load_or_null(data_set_detail_key, load_from);
            if (data_set_detail_data == nullptr)
            {
                data_set_detail_data = make_data_set_detail_data();
                data_set_detail_key->data_set_id = data_set_data->id;
                context.lock()->save_one(data_set_detail_data, load_from);
            }

            // Cache TemporalId for the dataset and its parent
            data_set_dict_[data_set_id] = data_set_data->id;
            data_set_owners_dict_[data_set_data->id] = data_set_data->data_set;

            dot::hash_set<dot::object_id> import_set;
            // Build and cache dataset lookup list if not found
            if (!data_set_parent_dict_->try_get_value(data_set_data->id, import_set))
            {
                import_set = build_data_set_lookup_list(data_set_data);
                data_set_parent_dict_->add(data_set_data->id, import_set);
            }

            return data_set_data->id;
        }
    }

    void mongo_data_source_data_impl::save_data_set(data_set_data data_set_data, dot::object_id save_to)
    {
        // Save dataset to storage. This updates its Id
            // to the new TemporalId created during save
        context.lock()->save_one(data_set_data, save_to);

        // Cache TemporalId for the dataset and its parent
        data_set_dict_[data_set_data->get_key()] = data_set_data->id;
        data_set_owners_dict_[data_set_data->id] = data_set_data->data_set;

        // Update lookup list dictionary
        dot::hash_set<dot::object_id> lookup_list = build_data_set_lookup_list(data_set_data);
        data_set_parent_dict_->add(data_set_data->id, lookup_list);
    }

    dot::hash_set<dot::object_id> mongo_data_source_data_impl::get_data_set_lookup_list(dot::object_id load_from)
    {
        dot::hash_set<dot::object_id> result;

        // Root dataset has no imports (there is not even a record
        // where these imports can be specified).
        //
        // Return list containing only the root dataset (TemporalId.Empty) and exit
        if (load_from == dot::object_id::empty)
        {
            result = dot::make_hash_set<dot::object_id>();
            result->add(dot::object_id::empty);
            return result;
        }

        if (data_set_parent_dict_->try_get_value(load_from, result))
        {
            // Check if the lookup list is already cached, return if yes
            return result;
        }
        else
        {
            // Otherwise load from storage (returns null if not found)
            data_set_data data_set_data = (dc::data_set_data)load_or_null(load_from, dot::typeof<dc::data_set_data>());

            if (data_set_data == nullptr) throw dot::exception(dot::string::format("Dataset with TemporalId={0} is not found.", load_from.to_string()));
            if (data_set_data->data_set != dot::object_id::empty) throw dot::exception(dot::string::format("Dataset with TemporalId={0} is not stored in root dataset.", load_from.to_string()));

            // Build the lookup list
            result = build_data_set_lookup_list(data_set_data);

            // Add to dictionary and return
            data_set_parent_dict_->add(load_from, result);
            return result;
        }
    }

    data_set_detail_data mongo_data_source_data_impl::get_data_set_detail_or_empty(dot::object_id detail_for)
    {
        data_set_detail_data result;

        if (detail_for == dot::object_id::empty)
        {
            // Root dataset does not have details
            // as it has no parent where the details
            // would be stored, and storing details
            // in the dataset itself would subject
            // them to their own settings.
            //
            // Accordingly, return null.
            return nullptr;
        }
        else if (data_set_detail_dict_->try_get_value(detail_for, result))
        {
            // Check if already cached, return if found
            return result;
        }
        else
        {
            // Get dataset parent from the dictionary.
            // We should not get here unless the value
            // is already cached.
            dot::object_id parent_id = data_set_owners_dict_[detail_for];

            // Otherwise try loading from storage (this also updates the dictionaries)
            data_set_detail_key data_set_detail_key = make_data_set_detail_key();
            data_set_detail_key->data_set_id = detail_for;
            result = (data_set_detail_data)load_or_null(data_set_detail_key, parent_id);

            // Cache in dictionary even if null
            data_set_detail_dict_[detail_for] = result;
            return result;
        }
    }

    dot::nullable<dot::object_id> mongo_data_source_data_impl::get_cutoff_time(dot::object_id data_set_id)
    {
        // Get imports cutoff time for the dataset detail record.
        // If the record is not found, consider its CutoffTime null.
        data_set_detail_data data_set_detail_data = get_data_set_detail_or_empty(data_set_id);
        dot::nullable<dot::object_id> data_set_cutoff_time = data_set_detail_data != nullptr ? data_set_detail_data->cutoff_time : nullptr;

        // If CutoffTime is set for both data source and dataset,
        // this method returns the earlier of the two values.
        dot::object_id result = dot::object_id::min(cutoff_time, data_set_cutoff_time.value_or_default());
        return result;
    }

    dot::nullable<dot::object_id> mongo_data_source_data_impl::get_imports_cutoff_time(dot::object_id data_set_id)
    {
        return dot::nullable<dot::object_id>();
    }
    dot::collection mongo_data_source_data_impl::get_collection(dot::type data_type)
    {
        return dot::collection();
    }
    dot::hash_set<dot::object_id> mongo_data_source_data_impl::build_data_set_lookup_list(data_set_data data_set_data)
    {
        return dot::hash_set<dot::object_id>();
    }
    void mongo_data_source_data_impl::build_data_set_lookup_list(data_set_data data_set_data, dot::hash_set<dot::object_id> result)
    {
    }
    void mongo_data_source_data_impl::check_not_read_only(dot::object_id dataSetId)
    {
    }
}
