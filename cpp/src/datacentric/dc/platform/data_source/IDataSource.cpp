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
#include <dc/platform/data_source/IDataSource.hpp>
#include <dc/platform/dataset/DataSetKey.hpp>
#include <dc/platform/context/IContext.hpp>

namespace dc
{
    ObjectId IDataSourceImpl::GetCommon()
    {
        return GetDataSet(DataSetKeyImpl::Common->DataSetID, ObjectId::Empty);
    }

    ObjectId IDataSourceImpl::GetDataSet(dot::String dataSetID, ObjectId loadFrom)
    {
        auto result = GetDataSetOrEmpty(dataSetID, loadFrom);
        if (result == ObjectId::Empty) throw dot::new_Exception(
            dot::String::Format("Dataset {0} is not found in data store {1}.", dataSetID, DataSourceID));
        return result;
    }

    ObjectId IDataSourceImpl::CreateDataSet(dot::String dataSetID, ObjectId saveTo)
    {
        // Delegate to the overload taking IEnumerable as second parameter
        return CreateDataSet(dataSetID, nullptr, saveTo);
    }

    ObjectId IDataSourceImpl::CreateDataSet(dot::String dataSetID, dot::IEnumerable<ObjectId> parentDataSets, ObjectId saveTo)
    {
        // Create dataset record
        auto result = new_DataSetData();
        result->DataSetID = dataSetID;

        if (parentDataSets != nullptr)
        {
            // Add parents if second argument is not null
            result->Parents = dot::new_List<ObjectId>();
            for (auto parentDataSet : parentDataSets)
            {
                result->Parents->Add(parentDataSet);
            }
        }

        // Save the record (this also updates the dictionaries)
        SaveDataSet(result, saveTo);

        // Return ObjectId that was assigned to the
        // record inside the SaveDataSet method
        return result->ID;
    }

    ObjectId IDataSourceImpl::CreateCommon()
    {
        auto result = new_DataSetData();
        result->DataSetID = DataSetKeyImpl::Common->DataSetID;

        // Save in root dataset
        SaveDataSet(result, ObjectId::Empty);
        return result->ID;
    }
}
