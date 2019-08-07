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
#include <dc/platform/context/context_base.hpp>
#include <dc/platform/data_source/data_source_data.hpp>
#include <dc/platform/data_source/data_source_key.hpp>


namespace dc
{
    void context_base_impl::SetDataSource(data_source_data value)
    {
        DataSource = value;
    }

    record_base context_base_impl::load_or_null(dot::object_id id, dot::type_t dataType)
    {
        return DataSource->load_or_null(id, dataType);
    }

    record_base context_base_impl::reload_or_null(key_base key, dot::object_id loadFrom)
    {
        return DataSource->reload_or_null(key, loadFrom);
    }

    void context_base_impl::save(record_base record)
    {
        DataSource->save(record, data_set);
    }

    void context_base_impl::save(record_base record, dot::object_id saveTo)
    {
        DataSource->save(record, saveTo);
    }

    void context_base_impl::Delete(key_base key)
    {
        DataSource->delete_record(key, data_set);
    }

    void context_base_impl::Delete(key_base key, dot::object_id deleteIn)
    {
        DataSource->delete_record(key, deleteIn);
    }

    void context_base_impl::delete_db()
    {
        DataSource->delete_db();
    }

    dot::object_id context_base_impl::get_common()
    {
        return DataSource->get_common();
    }

    dot::object_id context_base_impl::get_data_set(dot::string dataSetID)
    {
        return DataSource->get_data_set(dataSetID, data_set);
    }

    dot::object_id context_base_impl::get_data_set(dot::string dataSetID, dot::object_id loadFrom)
    {
        return DataSource->get_data_set(dataSetID, loadFrom);
    }

    dot::object_id context_base_impl::get_data_set_or_empty(dot::string dataSetID)
    {
        return DataSource->get_data_set_or_empty(dataSetID, data_set);
    }

    dot::object_id context_base_impl::get_data_set_or_empty(dot::string dataSetID, dot::object_id loadFrom)
    {
        return DataSource->get_data_set_or_empty(dataSetID, loadFrom);
    }

    dot::object_id context_base_impl::create_common()
    {
        return DataSource->create_common();
    }

    dot::object_id context_base_impl::create_data_set(dot::string dataSetID)
    {
        return DataSource->create_data_set(dataSetID, data_set);
    }

    dot::object_id context_base_impl::create_data_set(dot::string dataSetID, dot::object_id saveTo)
    {
        return DataSource->create_data_set(dataSetID, saveTo);
    }

    dot::object_id context_base_impl::create_data_set(dot::string dataSetID, dot::list<dot::object_id> parentdata_sets)
    {
        return DataSource->create_data_set(dataSetID, parentdata_sets, data_set);
    }

    dot::object_id context_base_impl::create_data_set(dot::string dataSetID, dot::list<dot::object_id> parentdata_sets, dot::object_id saveTo)
    {
        return DataSource->create_data_set(dataSetID, parentdata_sets, saveTo);
    }

    void context_base_impl::save_data_set(data_set_data data_set_data)
    {
        DataSource->save_data_set(data_set_data, data_set);
    }

    void context_base_impl::save_data_set(data_set_data data_set_data, dot::object_id saveTo)
    {
        DataSource->save_data_set(data_set_data, saveTo);
    }
}
