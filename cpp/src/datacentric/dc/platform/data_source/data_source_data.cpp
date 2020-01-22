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
    record data_source_data_impl::load(temporal_id id, dot::type data_type)
    {
        record result = load_or_null(id, data_type);
        if (result.is_empty())
            throw dot::exception(dot::string::format("Record with TemporalId={0} is not found in data store {1}.", id.to_string(), this->data_source_id));
        return result;
    }

    temporal_id data_source_data_impl::get_common()
    {
        return get_data_set(data_set_key_impl::common->data_set_id, temporal_id::empty);
    }

    temporal_id data_source_data_impl::get_data_set(dot::string data_set_id, temporal_id load_from)
    {
        auto result = get_data_set_or_empty(data_set_id, load_from);
        if (result == temporal_id::empty) throw dot::exception(
            dot::string::format("Dataset {0} is not found in data store {1}.", data_set_id, data_source_id));
        return result;
    }

    temporal_id data_source_data_impl::create_common(data_set_flags flags)
    {
        return create_data_set("Common", flags, temporal_id::empty);
    }

    temporal_id data_source_data_impl::create_common()
    {
        return create_common(data_set_flags::default);
    }

    temporal_id data_source_data_impl::create_data_set(dot::string data_set_id, temporal_id save_to)
    {
        // Create with default flags in parentDataSet
        return create_data_set(data_set_id, nullptr, data_set_flags::default, save_to);
    }

    temporal_id data_source_data_impl::create_data_set(dot::string data_set_id, dot::list<temporal_id> parent_data_sets, temporal_id save_to)
    {
        // Create with default flags in parentDataSet
        return create_data_set(data_set_id, parent_data_sets, data_set_flags::default, save_to);
    }
    temporal_id data_source_data_impl::create_data_set(dot::string data_set_id, data_set_flags flags, temporal_id save_to)
    {
        // Create with specified flags in parentDataSet
        return create_data_set(data_set_id, nullptr, flags, save_to);
    }
    temporal_id data_source_data_impl::create_data_set(dot::string data_set_id, dot::list<temporal_id> parent_data_sets, data_set_flags flags, temporal_id save_to)
    {
        // Create dataset record
        auto result = make_data_set_data();
        result->data_set_id = data_set_id;

        if (parent_data_sets != nullptr)
        {
            // Add parents if second argument is not null
            result->parents = dot::make_list<temporal_id>();
            for (auto parent_data_set : parent_data_sets)
            {
                result->parents->add(parent_data_set);
            }
        }

        // Save the record (this also updates the dictionaries)
        save_data_set(result, save_to);

        // Return temporal_id that was assigned to the
        // record inside the save_data_set method
        return result->id;
    }
}
