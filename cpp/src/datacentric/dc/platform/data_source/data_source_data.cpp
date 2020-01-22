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
    Record DataSourceImpl::load(TemporalId id, dot::Type data_type)
    {
        Record result = load_or_null(id, data_type);
        if (result.is_empty())
            throw dot::Exception(dot::String::format("Record with TemporalId={0} is not found in data store {1}.", id.to_string(), this->data_source_name));
        return result;
    }

    TemporalId DataSourceImpl::get_common()
    {
        return get_data_set(DataSetKeyImpl::common->data_set_name, TemporalId::empty);
    }

    TemporalId DataSourceImpl::get_data_set(dot::String data_set_name, TemporalId load_from)
    {
        auto result = get_data_set_or_empty(data_set_name, load_from);
        if (result == nullptr) throw dot::Exception(
            dot::String::format("Dataset {0} is not found in data store {1}.", data_set_name, data_source_name));
        return result.value();
    }

    TemporalId DataSourceImpl::create_common(DataSetFlags flags)
    {
        return create_data_set("Common", flags, TemporalId::empty);
    }

    TemporalId DataSourceImpl::create_common()
    {
        return create_common(DataSetFlags::default_option);
    }

    TemporalId DataSourceImpl::create_data_set(dot::String data_set_name, TemporalId save_to)
    {
        // Create with default flags in parentDataSet
        return create_data_set(data_set_name, nullptr, DataSetFlags::default_option, save_to);
    }

    TemporalId DataSourceImpl::create_data_set(dot::String data_set_name, dot::List<TemporalId> parent_data_sets, TemporalId save_to)
    {
        // Create with default flags in parentDataSet
        return create_data_set(data_set_name, parent_data_sets, DataSetFlags::default_option, save_to);
    }
    TemporalId DataSourceImpl::create_data_set(dot::String data_set_name, DataSetFlags flags, TemporalId save_to)
    {
        // Create with specified flags in parentDataSet
        return create_data_set(data_set_name, nullptr, flags, save_to);
    }
    TemporalId DataSourceImpl::create_data_set(dot::String data_set_name, dot::List<TemporalId> parent_data_sets, DataSetFlags flags, TemporalId save_to)
    {
        // Create dataset record
        auto result = make_data_set_data();
        result->data_set_name = data_set_name;

        if (parent_data_sets != nullptr)
        {
            // Add parents if second argument is not null
            result->imports = dot::make_list<TemporalId>();
            for (auto parent_data_set : parent_data_sets)
            {
                result->imports->add(parent_data_set);
            }
        }

        // Save the record (this also updates the dictionaries)
        save_data_set(result, save_to);

        // Return TemporalId that was assigned to the
        // record inside the save_data_set method
        return result->id;
    }
}
