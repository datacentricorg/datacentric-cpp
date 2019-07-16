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
#include <dc/platform/data_source/DataSourceData.hpp>
#include <dc/platform/context/IContext.hpp>
#include <dot/system/Array1D.hpp>

namespace dc
{
    bool DataSourceDataImpl::IsReadOnly()
    {
        return ReadOnly == true || RevisedBefore != nullptr || RevisedBeforeId != nullptr;
    }

    void DataSourceDataImpl::CheckNotReadOnly()
    {
        if (IsReadOnly())
            throw dot::new_Exception(dot::String::Format(
                "Attempting write operation for readonly data source {0}. "
                "A data source is readonly if either (a) its ReadOnly flag is set, or (b) "
                "one of RevisedBefore or RevisedBeforeId is set.", DataSourceID));
    }

    ObjectId DataSourceDataImpl::GetDataSetOrEmpty(dot::String dataSetID, ObjectId loadFrom)
    {
        ObjectId result;

        if (dataSetDict_->TryGetValue(dataSetID, result))
        {
            // Check if already cached, return if found
            return result;
        }
        else
        {
            // Otherwise load from storage (this also updates the dictionaries)
            return LoadDataSetOrEmpty(dataSetID, loadFrom);
        }
    }

    void DataSourceDataImpl::SaveDataSet(DataSetData dataSetData, ObjectId saveTo)
    {
        // Save dataset to storage. This updates its ID
        // to the new ObjectId created during save
        Save(dataSetData, saveTo);

        // Update dataset dictionary with the new ID
        dataSetDict_[dataSetData->Key] = dataSetData->ID;

        // Update lookup list dictionary
        dot::HashSet<ObjectId> lookupList = BuildDataSetLookupList(dataSetData);
        dataSetParentDict_->Add(dataSetData->ID, lookupList);
    }

    dot::IEnumerable<ObjectId> DataSourceDataImpl::GetDataSetLookupList(ObjectId loadFrom)
    {
        // Root dataset has no parents, return list containing
        // root dataset identifier only (ObjectId.Empty) and exit
        if (loadFrom == ObjectId::Empty)
        {
            dot::HashSet<ObjectId> res = dot::new_HashSet<ObjectId>();
            res->Add(ObjectId::Empty);
            return res;
        }

        dot::HashSet<ObjectId> result;
        if (dataSetParentDict_->TryGetValue(loadFrom, result))
        {
            // Check if the lookup list is already cached, return if yes
            return result;
        }
        else
        {
            // Otherwise load from storage (returns null if not found)
            DataSetData dataSetData = LoadOrNull<DataSetData>(loadFrom).as<DataSetData>();

            if (dataSetData == nullptr) throw dot::new_Exception(dot::String::Format("Dataset with ObjectId={0} is not found.", loadFrom.ToString()));
            if ((ObjectId) dataSetData->DataSet != ObjectId::Empty) throw dot::new_Exception(dot::String::Format("Dataset with ObjectId={0} is not stored in root dataset.", loadFrom.ToString()));

            // Build the lookup list
            result = BuildDataSetLookupList(dataSetData);

            // Add to dictionary and return
            dataSetParentDict_->Add(loadFrom, result);
            return result;
        }
    }

    dot::Nullable<ObjectId> DataSourceDataImpl::GetRevisionTimeConstraint()
    {
        // Set revisionTimeConstraint_ based on either RevisedBefore or RevisedBeforeId element
        if (RevisedBefore == nullptr && RevisedBeforeId == nullptr)
        {
            // Clear the revision time constraint.
            //
            // This is only required when  running Init(...) again
            // on an object that has been initialized before.
            return nullptr;
        }
        else if (RevisedBefore != nullptr && RevisedBeforeId == nullptr)
        {
            // We already know that RevisedBefore is not null,
            // but we need to check separately that it is not empty
            //RevisedBefore.CheckHasValue(); // TODO uncomment

            // Convert to the least value of ObjectId with the specified timestamp
            dot::LocalDateTime date = ((dot::Nullable<dot::LocalDateTime>) RevisedBefore).Value;
            return ObjectId(date);
        }
        else if (RevisedBefore == nullptr && RevisedBeforeId != nullptr)
        {
            // We already know that RevisedBeforeId is not null,
            // but we need to check separately that it is not empty
            //RevisedBeforeId.Value.CheckHasValue(); // TODO uncomment

            // Set the revision time constraint
            return RevisedBeforeId;
        }
        else
        {
            throw dot::new_Exception(
                "Elements RevisedBefore and RevisedBeforeId are alternates; "
                "they cannot be specified at the same time.");
        }
    }

    ObjectId DataSourceDataImpl::LoadDataSetOrEmpty(dot::String dataSetID, ObjectId loadFrom)
    {
        // Always load even if present in cache
        DataSetKey dataSetKey = new_DataSetKey();
        dataSetKey->DataSetID = dataSetID;
        DataSetData dataSetData = (DataSetData) ReloadOrNull(dataSetKey, loadFrom);

        // If not found, return ObjectId.Empty
        if (dataSetData == nullptr) return ObjectId::Empty;

        // If found, cache result in ObjectId dictionary
        dataSetDict_[dataSetID] = dataSetData->ID;

        // Build and cache dataset lookup list if not found
        dot::HashSet<ObjectId> parentSet;
        if (!dataSetParentDict_->TryGetValue(dataSetData->ID, parentSet))
        {
            parentSet = BuildDataSetLookupList(dataSetData);
            dataSetParentDict_->Add(dataSetData->ID, parentSet);
        }

        return dataSetData->ID;
    }

    dot::HashSet<ObjectId> DataSourceDataImpl::BuildDataSetLookupList(DataSetData dataSetData)
    {
        // Delegate to the second overload
        dot::HashSet<ObjectId> result = dot::new_HashSet<ObjectId>();
        BuildDataSetLookupList(dataSetData, result);
        return result;
    }

    void DataSourceDataImpl::BuildDataSetLookupList(DataSetData dataSetData, dot::HashSet<ObjectId> result)
    {
        // Return if the dataset is null or has no parents
        if (dataSetData == nullptr) return;

        // Error message if dataset has no ID or Key
        //dataSetData->ID->CheckHasValue();
        //dataSetData->Key->CheckHasValue();
        //! TODO uncomment

        // Add self to the result
        result->Add(dataSetData->ID);

        // Add parents to the result
        if (!((dot::List<ObjectId>)dataSetData->Parents).IsEmpty())
        {
            for(ObjectId dataSetId : dataSetData->Parents)
            {
                // Dataset cannot include itself as parent
                if (dataSetData->ID == dataSetId)
                    throw dot::new_Exception(
                        dot::String::Format("Dataset {0} with ObjectId={1} includes itself in the list of parents.", (dot::String)dataSetData->Key, ObjectId(dataSetData->ID).ToString()));

                // The Add method returns true if the argument is not yet present in the list
                if (!result->Contains(dataSetId))
                {
                    result->Add(dataSetId);
                    // Add recursively if not already present in the hashset
                    dot::IEnumerable<ObjectId> cachedParentSet = GetDataSetLookupList(dataSetId);
                    for (ObjectId cachedParentId : cachedParentSet)
                    {
                        result->Add(cachedParentId);
                    }
                }
            }
        }
    }
}
