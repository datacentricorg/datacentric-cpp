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
#include <dc/types/record/data_type_info.hpp>
#include <dc/attributes/class/index_elements_attribute.hpp>

#include <dot/mongo/mongo_db/mongo/collection.hpp>
#include <dc/platform/data_source/mongo/mongo_query.hpp>

namespace dc
{
    Record MongoDataSourceImpl::load_or_null(TemporalId id, dot::Type data_type)
    {
        if (cutoff_time != nullptr)
        {
            // If revision_time_constraint is not null, return null for any
            // id that is not strictly before the constraint TemporalId
            if (id >= cutoff_time.value()) return nullptr;
        }

        dot::Query query = dot::make_query(get_or_create_collection(data_type), data_type);
        dot::ObjectCursorWrapperBase cursor = query->where(new dot::OperatorWrapperImpl("_id", "$eq", id))
            ->limit(1)
            ->get_cursor();

        if (cursor->begin() != cursor->end())
        {
            dot::Object obj = *(cursor->begin());
            if (!obj.is<DeletedRecord>())
            {
                Record rec = obj.as<Record>();

                if (!data_type->is_assignable_from(rec->get_type()))
                {
                    // If cast result is null, the record was found but it is an instance
                    // of class that is not derived from TRecord, in this case the API
                    // requires error message, not returning null
                    throw dot::Exception(dot::String::format(
                        "Stored Type {0} for TemporalId={1} and "
                        "Key={2} is not an instance of the requested Type {3}.", rec->get_type()->name(),
                        id.to_string(), rec->get_key(), data_type->name()
                    ));
                }

                // Now we use get_cutoff_time() for the full check
                dot::Nullable<TemporalId> cutoff_time = get_cutoff_time(rec->data_set);
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

    Record MongoDataSourceImpl::load_or_null(Key key, TemporalId load_from)
    {
        // dot::String key in semicolon delimited format used in the lookup
        dot::String key_value = key->to_string();

        dot::Type record_type = dot::typeof<Record>();

        dot::Query base_query = dot::make_query(get_or_create_collection(key->get_type()), key->get_type())
            ->where(new dot::OperatorWrapperImpl("_key", "$eq", key_value))
            ;

        dot::Query query_with_final_constraints = apply_final_constraints(base_query, load_from);

        dot::ObjectCursorWrapperBase cursor = query_with_final_constraints
            ->sort_by_descending(record_type->get_field("_dataset"))
            ->then_by_descending(record_type->get_field("_id"))
            ->limit(1)
            ->get_cursor();

        if (cursor->begin() != cursor->end())
        {
            dot::Object obj = *(cursor->begin());
            if (!obj.is<DeletedRecord>())
            {
                Record rec = obj.as<Record>();
                rec->init(context);
                return rec;
            }
        }

        return nullptr;
    }

    void MongoDataSourceImpl::save_many(dot::List<Record> records, TemporalId save_to)
    {
        check_not_read_only(save_to);

        auto collection = get_or_create_collection(records[0]->get_type());


        // Iterate over list elements to populate fields
        for(Record rec : records)
        {
            // This method guarantees that TemporalIds will be in strictly increasing
            // order for this instance of the data source class always, and across
            // all processes and machine if they are not created within the same
            // second.
            TemporalId object_id = create_ordered_object_id();

            // TemporalId of the record must be strictly later
            // than TemporalId of the dataset where it is stored
            if (object_id <= save_to)
                throw dot::Exception(dot::String::format(
                    "Attempting to save a record with TemporalId={0} that is later "
                    "than TemporalId={1} of the dataset where it is being saved.", object_id.to_string(), save_to.to_string()));

            // Assign ID and DataSet, and only then initialize, because
            // initialization code may use record.ID and record.DataSet
            rec->id = object_id;
            rec->data_set = save_to;
            rec->init(context);
        }

        collection->insert_many(records);
    }

    MongoQuery MongoDataSourceImpl::get_query(TemporalId data_set, dot::Type type)
    {
        return make_mongo_query(get_or_create_collection(type), type, this, data_set);
    }


    void MongoDataSourceImpl::delete_record(Key key, TemporalId delete_in)
    {
        check_not_read_only(delete_in);

        dot::Collection collection = get_or_create_collection(key->get_type());

        DeletedRecord record = make_deleted_record(key);

        TemporalId object_id = create_ordered_object_id();

        // TemporalId of the record must be strictly later
        // than TemporalId of the dataset where it is stored
        if (object_id <= delete_in)
            throw dot::Exception(dot::String::format(
                "Attempting to save a record with TemporalId={0} that is later "
                "than TemporalId={1} of the dataset where it is being saved.", object_id.to_string(), delete_in.to_string()));

        // Assign id and data_set, and only then initialize, because
        // initialization code may use record.id and record.data_set
        record->id = object_id;
        record->data_set = delete_in;

        collection->insert_one(record);
    }

    dot::Query MongoDataSourceImpl::apply_final_constraints(dot::Query query, TemporalId load_from)
    {
        // Get lookup list by expanding the list of imports to arbitrary
        // depth with duplicates and cyclic references removed.
        //
        // The list will not include datasets that are after the value of
        // CutoffTime if specified, or their imports (including
        // even those imports that are earlier than the constraint).
        dot::HashSet<TemporalId> lookup_set = get_data_set_lookup_list(load_from);
        dot::List<TemporalId> lookup_list = dot::make_list<TemporalId>(std::vector<TemporalId>(lookup_set->begin(), lookup_set->end()));


        // Apply constraint that the value is _dataset is
        // one of the elements of dataSetLookupList_
        dot::Query result = query->where(new dot::OperatorWrapperImpl("_dataset", "$in", lookup_list));

        // Apply revision time constraint. By making this constraint the
        // last among the constraints, we optimize the use of the index.
        //
        // The property savedBy_ is set using either CutoffTime element.
        // Only one of these two elements can be set at a given time.
        dot::Nullable<TemporalId> cutoff_time = get_cutoff_time(load_from);
        if (cutoff_time != nullptr)
        {
            result = result->where(new dot::OperatorWrapperImpl("_id", "$lt", cutoff_time.value()));
        }

        return result;
    }

    dot::Nullable<TemporalId> MongoDataSourceImpl::get_data_set_or_empty(dot::String data_set_id, TemporalId load_from)
    {
        TemporalId result;
        if (data_set_dict_->try_get_value(data_set_id, result))
        {
            // Check if already cached, return if found
            return result;
        }
        else
        {
            // Otherwise load from storage (this also updates the dictionaries)
            DataSetKey data_set_key = make_data_set_key();
            data_set_key->data_set_id = data_set_id;
            DataSet data_set_data = (dc::DataSet)load_or_null(data_set_key, load_from);

            // If not found, return TemporalId.Empty
            if (data_set_data == nullptr) return nullptr;

            // Get or create dataset detail record
            DataSetDetailKey data_set_detail_key = make_data_set_detail_key();
            data_set_detail_key->data_set_id = data_set_data->id;
            DataSetDetail data_set_detail_data = (dc::DataSetDetail)load_or_null(data_set_detail_key, load_from);
            if (data_set_detail_data == nullptr)
            {
                data_set_detail_data = make_data_set_detail_data();
                data_set_detail_key->data_set_id = data_set_data->id;
                context.lock()->save_one(data_set_detail_data, load_from);
            }

            // Cache TemporalId for the dataset and its parent
            data_set_dict_[data_set_id] = data_set_data->id;
            data_set_owners_dict_[data_set_data->id] = data_set_data->data_set;

            dot::HashSet<TemporalId> import_set;
            // Build and cache dataset lookup list if not found
            if (!data_set_parent_dict_->try_get_value(data_set_data->id, import_set))
            {
                import_set = build_data_set_lookup_list(data_set_data);
                data_set_parent_dict_->add(data_set_data->id, import_set);
            }

            return data_set_data->id;
        }
    }

    void MongoDataSourceImpl::save_data_set(DataSet data_set_data, TemporalId save_to)
    {
        // Save dataset to storage. This updates its Id
            // to the new TemporalId created during save
        context.lock()->save_one(data_set_data, save_to);

        // Cache TemporalId for the dataset and its parent
        data_set_dict_[data_set_data->get_key()] = data_set_data->id;
        data_set_owners_dict_[data_set_data->id] = data_set_data->data_set;

        // Update lookup list dictionary
        dot::HashSet<TemporalId> lookup_list = build_data_set_lookup_list(data_set_data);
        data_set_parent_dict_->add(data_set_data->id, lookup_list);
    }

    dot::HashSet<TemporalId> MongoDataSourceImpl::get_data_set_lookup_list(TemporalId load_from)
    {
        dot::HashSet<TemporalId> result;

        // Root dataset has no imports (there is not even a record
        // where these imports can be specified).
        //
        // Return list containing only the root dataset (TemporalId.Empty) and exit
        if (load_from == TemporalId::empty)
        {
            result = dot::make_hash_set<TemporalId>();
            result->add(TemporalId::empty);
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
            DataSet data_set_data = (dc::DataSet)load_or_null(load_from, dot::typeof<dc::DataSet>());

            if (data_set_data == nullptr) throw dot::Exception(dot::String::format("Dataset with TemporalId={0} is not found.", load_from.to_string()));
            if (data_set_data->data_set != TemporalId::empty) throw dot::Exception(dot::String::format("Dataset with TemporalId={0} is not stored in root dataset.", load_from.to_string()));

            // Build the lookup list
            result = build_data_set_lookup_list(data_set_data);

            // Add to dictionary and return
            data_set_parent_dict_->add(load_from, result);
            return result;
        }
    }

    DataSetDetail MongoDataSourceImpl::get_data_set_detail_or_empty(TemporalId detail_for)
    {
        DataSetDetail result;

        if (detail_for == TemporalId::empty)
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
            TemporalId parent_id = data_set_owners_dict_[detail_for];

            // Otherwise try loading from storage (this also updates the dictionaries)
            DataSetDetailKey data_set_detail_key = make_data_set_detail_key();
            data_set_detail_key->data_set_id = detail_for;
            result = (DataSetDetail)load_or_null(data_set_detail_key, parent_id);

            // Cache in dictionary even if null
            data_set_detail_dict_[detail_for] = result;
            return result;
        }
    }

    dot::Nullable<TemporalId> MongoDataSourceImpl::get_cutoff_time(TemporalId data_set_id)
    {
        // Get imports cutoff time for the dataset detail record.
        // If the record is not found, consider its CutoffTime null.
        DataSetDetail data_set_detail_data = get_data_set_detail_or_empty(data_set_id);
        dot::Nullable<TemporalId> data_set_cutoff_time = data_set_detail_data != nullptr ? data_set_detail_data->cutoff_time : nullptr;

        // If CutoffTime is set for both data source and dataset,
        // this method returns the earlier of the two values.
        dot::Nullable<TemporalId> result = TemporalId::min(cutoff_time, data_set_cutoff_time);
        return result;
    }

    dot::Nullable<TemporalId> MongoDataSourceImpl::get_imports_cutoff_time(TemporalId data_set_id)
    {
        // Get dataset detail record
        DataSetDetail data_set_detail_data = get_data_set_detail_or_empty(data_set_id);

        // Return null if the record is not found
        if (data_set_detail_data != nullptr) return data_set_detail_data->imports_cutoff_time;
        else return nullptr;
    }

    dot::Collection MongoDataSourceImpl::get_or_create_collection(dot::Type data_type)
    {
        // Check if collection Object has already been cached
        // for this type and return cached result if found
        dot::Object collection_obj;
        if (collection_dict_->try_get_value(data_type, collection_obj))
        {
            dot::Collection cached_result = collection_obj.as<dot::Collection>();
            return cached_result;
        }

        // Collection name is root class name of the record without prefix
        dot::String collection_name = DataTypeInfoImpl::get_or_create(data_type)->get_collection_name();

        // Get interfaces to base and typed collections for the same name
        dot::Collection typed_collection = db_->get_collection(collection_name);

        //--- Load standard index types

        // Each data type has an index for optimized loading by key.
        // This index consists of Key in ascending order, followed by
        // DataSet and ID in descending order.
        dot::List<std::tuple<dot::String, int>> load_index_keys = dot::make_list<std::tuple<dot::String, int>>();
        load_index_keys->add({ "_key", 1 }); // .key
        load_index_keys->add({ "_dataset", -1 }); // .data_set
        load_index_keys->add({ "_id", -1 }); // .id

        // Use index definition convention to specify the index name
        dot::String load_index_name = "Key-DataSet-Id";
        dot::IndexOptions load_index_options = dot::make_index_options();
        load_index_options->name = load_index_name;
        typed_collection->create_index(load_index_keys, load_index_options);

        //--- Load custom index types

        // Additional indices are provided using IndexAttribute for the class.
        // Get a sorted dictionary of (definition, name) pairs
        // for the inheritance chain of the specified type.
        dot::Dictionary<dot::String, dot::String> index_dict = IndexElementsAttributeImpl::get_attributes_dict(data_type);

        // Iterate over the dictionary to define the index
        for (auto index_info : index_dict)
        {
            dot::String index_definition = index_info.first;
            dot::String index_name = index_info.second;

            // Parse index definition to get a list of (ElementName,SortOrder) tuples
            dot::List<std::tuple<dot::String, int>> index_tokens = IndexElementsAttributeImpl::parse_definition(index_definition, data_type);

            if (index_name == nullptr) throw dot::Exception("Index name cannot be null.");

            // Add to indexes for the collection
            dot::IndexOptions index_opt = dot::make_index_options();
            index_opt->name = index_name;
            typed_collection->create_index(index_tokens, index_opt);
        }

        // Add the result to the collection dictionary and return
        collection_dict_->add(data_type, typed_collection);
        return typed_collection;
    }

    dot::HashSet<TemporalId> MongoDataSourceImpl::build_data_set_lookup_list(DataSet data_set_data)
    {
        // Delegate to the second overload
        dot::HashSet<TemporalId> result = dot::make_hash_set<TemporalId>();
        build_data_set_lookup_list(data_set_data, result);
        return result;
    }

    void MongoDataSourceImpl::build_data_set_lookup_list(DataSet data_set_data, dot::HashSet<TemporalId> result)
    {
        // Return if the dataset is null or has no imports
        if (data_set_data == nullptr) return;

        // Error message if dataset has no Id or Key set
        if (data_set_data->id.is_empty())
            throw dot::Exception("Required TemporalId value is not set.");
        if (data_set_data->get_key().is_empty())
            throw dot::Exception("Required String value is not set.");

        dot::Nullable<TemporalId> cutoff_time = get_cutoff_time(data_set_data->data_set);
        if (cutoff_time != nullptr && data_set_data->id >= cutoff_time.value())
        {
            // Do not add if revision time constraint is set and is before this dataset.
            // In this case the import datasets should not be added either, even if they
            // do not fail the revision time constraint
            return;
        }

        // Add self to the result
        result->add(data_set_data->id);

        // Add imports to the result
        if (data_set_data->parents != nullptr)
        {
            for (TemporalId data_set_id : data_set_data->parents)
            {
                // Dataset cannot include itself as its import
                if (data_set_data->id == data_set_id)
                    throw dot::Exception(dot::String::format(
                        "Dataset {0} with TemporalId={1} includes itself in the list of its imports."
                        , data_set_data->get_key(), data_set_data->id.to_string()));

                if (!result->contains(data_set_id))
                {
                    result->add(data_set_id);
                    // Add recursively if not already present in the hashset
                    dot::HashSet<TemporalId> cached_import_list = get_data_set_lookup_list(data_set_id);
                    for (TemporalId import_id : cached_import_list)
                    {
                        result->add(import_id);
                    }
                }
            }
        }
    }

    void MongoDataSourceImpl::check_not_read_only(TemporalId data_set_id)
    {
        if (read_only.has_value() && read_only.value())
            throw dot::Exception(dot::String::format(
                "Attempting write operation for data source {0} where ReadOnly flag is set.", data_source_id));

        DataSetDetail data_set_detail_data = get_data_set_detail_or_empty(data_set_id);
        if (data_set_detail_data != nullptr && data_set_detail_data->read_only.has_value() && data_set_detail_data->read_only.value())
            throw dot::Exception(dot::String::format(
                "Attempting write operation for dataset {0} where ReadOnly flag is set.", data_set_id.to_string()));

        if (cutoff_time != nullptr)
            throw dot::Exception(dot::String::format(
                "Attempting write operation for data source {0} where "
                "cutoff_time is set. Historical view of the data cannot be written to.", data_source_id));

        if (data_set_detail_data != nullptr && data_set_detail_data->cutoff_time != nullptr)
            throw dot::Exception(dot::String::format(
                "Attempting write operation for the dataset {0} where "
                "CutoffTime is set. Historical view of the data cannot be written to.", data_set_id.to_string()));
    }
}
