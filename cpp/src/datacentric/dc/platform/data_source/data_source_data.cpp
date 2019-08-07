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
#include <dc/platform/data_source/data_source_data.hpp>
#include <dc/platform/context/context_base.hpp>

namespace dc
{
    bool data_source_data_impl::is_read_only()
    {
        return read_only == true || revised_before != nullptr || revised_before_id != nullptr;
    }

    void data_source_data_impl::check_not_read_only()
    {
        if (is_read_only())
            throw dot::exception(dot::string::format(
                "Attempting write operation for readonly data source {0}. "
                "A data source is readonly if either (a) its ReadOnly flag is set, or (b) "
                "one of RevisedBefore or RevisedBeforeId is set.", data_source_id));
    }

    dot::object_id data_source_data_impl::get_data_set_or_empty(dot::string data_set_id, dot::object_id load_from)
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
            return load_data_set_or_empty(data_set_id, load_from);
        }
    }

    void data_source_data_impl::save_data_set(data_set_data data_set_data, dot::object_id save_to)
    {
        // Save dataset to storage. This updates its ID
        // to the new dot::object_id created during save
        save(data_set_data, save_to);

        // Update dataset dictionary with the new ID
        data_set_dict_[data_set_data->get_key()] = data_set_data->ID;

        // Update lookup list dictionary
        dot::hash_set<dot::object_id> lookupList = build_data_set_lookup_list(data_set_data);
        data_set_parent_dict_->add(data_set_data->ID, lookupList);
    }

    dot::hash_set<dot::object_id> data_source_data_impl::get_data_set_lookup_list(dot::object_id load_from)
    {
        // Root dataset has no parents, return list containing
        // root dataset identifier only (dot::object_id.Empty) and exit
        if (load_from == dot::object_id::empty)
        {
            dot::hash_set<dot::object_id> res = dot::make_hash_set<dot::object_id>();
            res->add(dot::object_id::empty);
            return res;
        }

        dot::hash_set<dot::object_id> result;
        if (data_set_parent_dict_->try_get_value(load_from, result))
        {
            // Check if the lookup list is already cached, return if yes
            return result;
        }
        else
        {
            // Otherwise load from storage (returns null if not found)
            data_set_data data_set_data_obj = load_or_null<data_set_data>(load_from).template as<data_set_data>();

            if (data_set_data_obj == nullptr)
                throw dot::exception(dot::string::format("Dataset with dot::object_id={0} is not found.", load_from.to_string()));
            if ((dot::object_id) data_set_data_obj->data_set != dot::object_id::empty)
                throw dot::exception(dot::string::format("Dataset with dot::object_id={0} is not stored in root dataset.", load_from.to_string()));

            // Build the lookup list
            result = build_data_set_lookup_list(data_set_data_obj);

            // Add to dictionary and return
            data_set_parent_dict_->add(load_from, result);
            return result;
        }
    }

    dot::nullable<dot::object_id> data_source_data_impl::get_revision_time_constraint()
    {
        // Set revisionTimeConstraint_ based on either RevisedBefore or RevisedBeforeId element
        if (revised_before == nullptr && revised_before_id == nullptr)
        {
            // Clear the revision time constraint.
            //
            // This is only required when  running Init(...) again
            // on an object that has been initialized before.
            return nullptr;
        }
        else if (revised_before != nullptr && revised_before_id == nullptr)
        {
            // We already know that RevisedBefore is not null,
            // but we need to check separately that it is not empty
            //RevisedBefore.CheckHasValue(); // TODO uncomment

            // Convert to the least value of dot::object_id with the specified timestamp
            dot::local_date_time date = ((dot::nullable<dot::local_date_time>) revised_before).value();
            return dot::object_id(date);
        }
        else if (revised_before == nullptr && revised_before_id != nullptr)
        {
            // We already know that RevisedBeforeId is not null,
            // but we need to check separately that it is not empty
            //RevisedBeforeId.Value.CheckHasValue(); // TODO uncomment

            // Set the revision time constraint
            return revised_before_id;
        }
        else
        {
            throw dot::exception(
                "Elements RevisedBefore and RevisedBeforeId are alternates; "
                "they cannot be specified at the same time.");
        }
    }

    dot::object_id data_source_data_impl::load_data_set_or_empty(dot::string data_set_id, dot::object_id load_from)
    {
        // Always load even if present in cache
        data_set_key data_set_key = make_data_set_key();
        data_set_key->data_set_id = data_set_id;
        data_set_data data_set_data_obj = (data_set_data) reload_or_null(data_set_key, load_from);

        // If not found, return dot::object_id.Empty
        if (data_set_data_obj == nullptr) return dot::object_id::empty;

        // If found, cache result in dot::object_id dictionary
        data_set_dict_[data_set_id] = data_set_data_obj->ID;

        // Build and cache dataset lookup list if not found
        dot::hash_set<dot::object_id> parent_set;
        if (!data_set_parent_dict_->try_get_value(data_set_data_obj->ID, parent_set))
        {
            parent_set = build_data_set_lookup_list(data_set_data_obj);
            data_set_parent_dict_->add(data_set_data_obj->ID, parent_set);
        }

        return data_set_data_obj->ID;
    }

    dot::hash_set<dot::object_id> data_source_data_impl::build_data_set_lookup_list(data_set_data data_set_data)
    {
        // Delegate to the second overload
        dot::hash_set<dot::object_id> result = dot::make_hash_set<dot::object_id>();
        build_data_set_lookup_list(data_set_data, result);
        return result;
    }

    void data_source_data_impl::build_data_set_lookup_list(data_set_data data_set_data, dot::hash_set<dot::object_id> result)
    {
        // Return if the dataset is null or has no parents
        if (data_set_data == nullptr) return;

        // Error message if dataset has no ID or Key
        //data_set_data->ID->CheckHasValue();
        //data_set_data->getKey()->CheckHasValue();
        //! TODO uncomment

        // Add self to the result
        result->add(data_set_data->ID);

        // Add parents to the result
        if (!((dot::list<dot::object_id>)data_set_data->Parents).is_empty())
        {
            for(dot::object_id data_set_id : data_set_data->Parents)
            {
                // Dataset cannot include itself as parent
                if (data_set_data->ID == data_set_id)
                    throw dot::exception(
                        dot::string::format("Dataset {0} with dot::object_id={1} includes itself in the list of parents.", (dot::string)data_set_data->get_key(), dot::object_id(data_set_data->ID).to_string()));

                // The Add method returns true if the argument is not yet present in the list
                if (!result->contains(data_set_id))
                {
                    result->add(data_set_id);
                    // Add recursively if not already present in the hashset
                    dot::hash_set<dot::object_id> cached_parent_set = get_data_set_lookup_list(data_set_id);
                    for (dot::object_id cached_parent_id : cached_parent_set)
                    {
                        result->add(cached_parent_id);
                    }
                }
            }
        }
    }

    dot::object_id data_source_data_impl::get_common()
    {
        return get_data_set(data_set_key_impl::common->data_set_id, dot::object_id::empty);
    }

    dot::object_id data_source_data_impl::get_data_set(dot::string data_set_id, dot::object_id load_from)
    {
        auto result = get_data_set_or_empty(data_set_id, load_from);
        if (result == dot::object_id::empty) throw dot::exception(
            dot::string::format("Dataset {0} is not found in data store {1}.", data_set_id, data_source_id));
        return result;
    }

    dot::object_id data_source_data_impl::create_data_set(dot::string dataSetID, dot::object_id saveTo)
    {
        // Delegate to the overload taking IEnumerable as second parameter
        return create_data_set(dataSetID, nullptr, saveTo);
    }

    dot::object_id data_source_data_impl::create_data_set(dot::string data_set_id, dot::list<dot::object_id> parent_data_sets, dot::object_id save_to)
    {
        // Create dataset record
        auto result = make_data_set_data();
        result->data_set_id = data_set_id;

        if (parent_data_sets != nullptr)
        {
            // Add parents if second argument is not null
            result->Parents = dot::make_list<dot::object_id>();
            for (auto parent_data_set : parent_data_sets)
            {
                result->Parents->add(parent_data_set);
            }
        }

        // Save the record (this also updates the dictionaries)
        save_data_set(result, save_to);

        // Return dot::object_id that was assigned to the
        // record inside the save_data_set method
        return result->ID;
    }

    dot::object_id data_source_data_impl::create_common()
    {
        auto result = make_data_set_data();
        result->data_set_id = data_set_key_impl::common->data_set_id;

        // Save in root dataset
        save_data_set(result, dot::object_id::empty);
        return result->ID;
    }
}
