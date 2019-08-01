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
    void IContextImpl::SetDataSource(DataSourceData dataSource)
    {
        DataSource = dataSource;
    }

    RecordType IContextImpl::LoadOrNull(ObjectId id, dot::type_t dataType)
    {
        return DataSource->LoadOrNull(id, dataType);
    }

    RecordType IContextImpl::ReloadOrNull(KeyType key, ObjectId loadFrom)
    {
        return DataSource->ReloadOrNull(key, loadFrom);
    }

    void IContextImpl::Save(RecordType record)
    {
        DataSource->Save(record, DataSet);
    }

    void IContextImpl::Save(RecordType record, ObjectId saveTo)
    {
        DataSource->Save(record, saveTo);
    }

    void IContextImpl::Delete(KeyType key)
    {
        DataSource->Delete(key, DataSet);
    }

    void IContextImpl::Delete(KeyType key, ObjectId deleteIn)
    {
        DataSource->Delete(key, deleteIn);
    }

    void IContextImpl::DeleteDb()
    {
        DataSource->DeleteDb();
    }

    ObjectId IContextImpl::GetCommon()
    {
        return DataSource->GetCommon();
    }

    ObjectId IContextImpl::GetDataSet(dot::string dataSetID)
    {
        return DataSource->GetDataSet(dataSetID, DataSet);
    }

    ObjectId IContextImpl::GetDataSet(dot::string dataSetID, ObjectId loadFrom)
    {
        return DataSource->GetDataSet(dataSetID, loadFrom);
    }

    ObjectId IContextImpl::GetDataSetOrEmpty(dot::string dataSetID)
    {
        return DataSource->GetDataSetOrEmpty(dataSetID, DataSet);
    }

    ObjectId IContextImpl::GetDataSetOrEmpty(dot::string dataSetID, ObjectId loadFrom)
    {
        return DataSource->GetDataSetOrEmpty(dataSetID, loadFrom);
    }

    ObjectId IContextImpl::CreateCommon()
    {
        return DataSource->CreateCommon();
    }

    ObjectId IContextImpl::CreateDataSet(dot::string dataSetID)
    {
        return DataSource->CreateDataSet(dataSetID, DataSet);
    }

    ObjectId IContextImpl::CreateDataSet(dot::string dataSetID, ObjectId saveTo)
    {
        return DataSource->CreateDataSet(dataSetID, saveTo);
    }

    ObjectId IContextImpl::CreateDataSet(dot::string dataSetID, dot::IEnumerable<ObjectId> parentDataSets)
    {
        return DataSource->CreateDataSet(dataSetID, parentDataSets, DataSet);
    }

    ObjectId IContextImpl::CreateDataSet(dot::string dataSetID, dot::IEnumerable<ObjectId> parentDataSets, ObjectId saveTo)
    {
        return DataSource->CreateDataSet(dataSetID, parentDataSets, saveTo);
    }

    void IContextImpl::SaveDataSet(DataSetData dataSetData)
    {
        DataSource->SaveDataSet(dataSetData, DataSet);
    }

    void IContextImpl::SaveDataSet(DataSetData dataSetData, ObjectId saveTo)
    {
        DataSource->SaveDataSet(dataSetData, saveTo);
    }
}
