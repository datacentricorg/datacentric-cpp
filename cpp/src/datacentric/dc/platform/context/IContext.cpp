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

    record_base IContextImpl::LoadOrNull(ObjectId id, dot::type_t dataType)
    {
        return DataSource->load_or_null(id, dataType);
    }

    record_base IContextImpl::ReloadOrNull(key_base key, ObjectId loadFrom)
    {
        return DataSource->reload_or_null(key, loadFrom);
    }

    void IContextImpl::Save(record_base record)
    {
        DataSource->save(record, DataSet);
    }

    void IContextImpl::Save(record_base record, ObjectId saveTo)
    {
        DataSource->save(record, saveTo);
    }

    void IContextImpl::Delete(key_base key)
    {
        DataSource->delete_record(key, DataSet);
    }

    void IContextImpl::Delete(key_base key, ObjectId deleteIn)
    {
        DataSource->delete_record(key, deleteIn);
    }

    void IContextImpl::DeleteDb()
    {
        DataSource->delete_db();
    }

    ObjectId IContextImpl::GetCommon()
    {
        return DataSource->get_common();
    }

    ObjectId IContextImpl::GetDataSet(dot::string dataSetID)
    {
        return DataSource->get_data_set(dataSetID, DataSet);
    }

    ObjectId IContextImpl::GetDataSet(dot::string dataSetID, ObjectId loadFrom)
    {
        return DataSource->get_data_set(dataSetID, loadFrom);
    }

    ObjectId IContextImpl::GetDataSetOrEmpty(dot::string dataSetID)
    {
        return DataSource->get_data_set_or_empty(dataSetID, DataSet);
    }

    ObjectId IContextImpl::GetDataSetOrEmpty(dot::string dataSetID, ObjectId loadFrom)
    {
        return DataSource->get_data_set_or_empty(dataSetID, loadFrom);
    }

    ObjectId IContextImpl::CreateCommon()
    {
        return DataSource->create_common();
    }

    ObjectId IContextImpl::CreateDataSet(dot::string dataSetID)
    {
        return DataSource->create_data_set(dataSetID, DataSet);
    }

    ObjectId IContextImpl::CreateDataSet(dot::string dataSetID, ObjectId saveTo)
    {
        return DataSource->create_data_set(dataSetID, saveTo);
    }

    ObjectId IContextImpl::CreateDataSet(dot::string dataSetID, dot::list<ObjectId> parentDataSets)
    {
        return DataSource->create_data_set(dataSetID, parentDataSets, DataSet);
    }

    ObjectId IContextImpl::CreateDataSet(dot::string dataSetID, dot::list<ObjectId> parentDataSets, ObjectId saveTo)
    {
        return DataSource->create_data_set(dataSetID, parentDataSets, saveTo);
    }

    void IContextImpl::SaveDataSet(DataSetData dataSetData)
    {
        DataSource->save_data_set(dataSetData, DataSet);
    }

    void IContextImpl::SaveDataSet(DataSetData dataSetData, ObjectId saveTo)
    {
        DataSource->save_data_set(dataSetData, saveTo);
    }
}
