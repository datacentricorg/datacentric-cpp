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
#include <dc/platform/context/IContext.hpp>
#include <dc/platform/data_source/DataSourceData.hpp>
#include <dc/platform/data_source/DataSourceKey.hpp>


namespace dc
{
    void IContextImpl::SetDataSource(data_source_data data_source)
    {
        DataSource = data_source;
    }

    record_base IContextImpl::load_or_null(dot::object_id id, dot::type_t dataType)
    {
        return DataSource->load_or_null(id, dataType);
    }

    record_base IContextImpl::reload_or_null(key_base key, dot::object_id loadFrom)
    {
        return DataSource->reload_or_null(key, loadFrom);
    }

    void IContextImpl::Save(record_base record)
    {
        DataSource->save(record, data_set);
    }

    void IContextImpl::Save(record_base record, dot::object_id saveTo)
    {
        DataSource->save(record, saveTo);
    }

    void IContextImpl::Delete(key_base key)
    {
        DataSource->delete_record(key, data_set);
    }

    void IContextImpl::Delete(key_base key, dot::object_id deleteIn)
    {
        DataSource->delete_record(key, deleteIn);
    }

    void IContextImpl::delete_db()
    {
        DataSource->delete_db();
    }

    dot::object_id IContextImpl::get_common()
    {
        return DataSource->get_common();
    }

    dot::object_id IContextImpl::get_data_set(dot::string dataSetID)
    {
        return DataSource->get_data_set(dataSetID, data_set);
    }

    dot::object_id IContextImpl::get_data_set(dot::string dataSetID, dot::object_id loadFrom)
    {
        return DataSource->get_data_set(dataSetID, loadFrom);
    }

    dot::object_id IContextImpl::get_data_set_or_empty(dot::string dataSetID)
    {
        return DataSource->get_data_set_or_empty(dataSetID, data_set);
    }

    dot::object_id IContextImpl::get_data_set_or_empty(dot::string dataSetID, dot::object_id loadFrom)
    {
        return DataSource->get_data_set_or_empty(dataSetID, loadFrom);
    }

    dot::object_id IContextImpl::create_common()
    {
        return DataSource->create_common();
    }

    dot::object_id IContextImpl::create_data_set(dot::string dataSetID)
    {
        return DataSource->create_data_set(dataSetID, data_set);
    }

    dot::object_id IContextImpl::create_data_set(dot::string dataSetID, dot::object_id saveTo)
    {
        return DataSource->create_data_set(dataSetID, saveTo);
    }

    dot::object_id IContextImpl::create_data_set(dot::string dataSetID, dot::list<dot::object_id> parentdata_sets)
    {
        return DataSource->create_data_set(dataSetID, parentdata_sets, data_set);
    }

    dot::object_id IContextImpl::create_data_set(dot::string dataSetID, dot::list<dot::object_id> parentdata_sets, dot::object_id saveTo)
    {
        return DataSource->create_data_set(dataSetID, parentdata_sets, saveTo);
    }

    void IContextImpl::save_data_set(data_set_data data_set_data)
    {
        DataSource->save_data_set(data_set_data, data_set);
    }

    void IContextImpl::save_data_set(data_set_data data_set_data, dot::object_id saveTo)
    {
        DataSource->save_data_set(data_set_data, saveTo);
    }
}
