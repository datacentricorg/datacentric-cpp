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

#pragma once

#include <dc/declare.hpp>
#include <dot/system/Ptr.hpp>
#include <dot/system/collections/generic/List.hpp>
//#include <dc/platform/dataset/DataSetData.hpp>
#include <dc/platform/data_source/mongo/ObjectId.hpp>

namespace dc
{
    class IDataSourceImpl; using IDataSource = dot::Ptr<IDataSourceImpl>;
    class IContextImpl; using IContext = dot::Ptr<IContextImpl>;
    class KeyTypeImpl; using KeyType = dot::Ptr<KeyTypeImpl>;
    class DataImpl; using Data = dot::Ptr<DataImpl>;
    class RecordTypeImpl; using RecordType = dot::Ptr<RecordTypeImpl>;
    class ObjectId;
    class DataSetDataImpl; using DataSetData = dot::Ptr<DataSetDataImpl>;

    /// <summary>
    /// Data source is a logical concept similar to database
    /// that can be implemented for a document DB, relational DB,
    /// key-value store, or filesystem.
    ///
    /// Data source API provides the ability to:
    ///
    /// (a) store and query datasets;
    /// (b) store records in a specific dataset; and
    /// (c) query record across a group of datasets.
    ///
    /// This record is stored in root dataset.
    /// </summary>
    class DC_CLASS IDataSourceImpl : public virtual dot::ObjectImpl
    {
        typedef IDataSourceImpl self;

    public: // PROPERTIES

        /// <summary>Unique data source identifier.</summary>
        DOT_DECL_GET(dot::String, DataSourceID)

    public: // METHODS

        /// <summary>
        /// The returned ObjectIds have the following order guarantees:
        ///
        /// * For this data source instance, to arbitrary resolution; and
        /// * Across all processes and machines, to one second resolution
        ///
        /// One second resolution means that two ObjectIds created within
        /// the same second by different instances of the data source
        /// class may not be ordered chronologically unless they are at
        /// least one second apart.
        /// </summary>
        virtual ObjectId CreateOrderedObjectId() = 0;

        /// <summary>
        /// Load record by its ObjectId.
        ///
        /// Return null if there is no record for the specified ObjectId;
        /// however an exception will be thrown if the record exists but
        /// is not derived from TRecord.
        /// </summary>
        template <class TRecord>
        TRecord LoadOrNull(ObjectId id)
        {
            return (TRecord) LoadOrNull(id, ::dot::typeof<TRecord>());
        }

        /// <summary>
        /// Load record by its ObjectId and Type.
        ///
        /// Return null if there is no record for the specified ObjectId;
        /// however an exception will be thrown if the record exists but
        /// is not derived from TRecord.
        /// </summary>
        virtual RecordType LoadOrNull(ObjectId id, dot::Type dataType) = 0;

        /// <summary>
        /// This method does not use cached value inside the key
        /// and always retrieves a new record from storage. To get
        /// the record cached inside the key instead (if present), use
        /// the caching variant of this method:
        ///
        /// LoadOrNull(key, loadFrom)
        ///
        /// Load record by string key from the specified dataset or
        /// its parent. The lookup occurs first in the reverse
        /// chronological order of datasets to one second resolution,
        /// and then in the reverse chronological order of records
        /// within the latest dataset that has at least one record.
        ///
        /// The root dataset has empty ObjectId value that is less
        /// than any other ObjectId value. Accordingly, the root
        /// dataset is the last one in the lookup order of datasets.
        ///
        /// The first record in this lookup order is returned, or null
        /// if no records are found or if delete marker is the first
        /// record.
        ///
        /// Return null if there is no record for the specified ObjectId;
        /// however an exception will be thrown if the record exists but
        /// is not derived from TRecord.
        /// </summary>
        virtual RecordType ReloadOrNull(KeyType key, ObjectId loadFrom) = 0;

        /// <summary>
        /// Save record to the specified dataset. After the method exits,
        /// record.DataSet will be set to the value of the dataSet parameter.
        ///
        /// All Save methods ignore the value of record.DataSet before the
        /// Save method is called. When dataset is not specified explicitly,
        /// the value of dataset from the context, not from the record, is used.
        /// The reason for this behavior is that the record may be stored from
        /// a different dataset than the one where it is used.
        ///
        /// This method guarantees that ObjectIds will be in strictly increasing
        /// order for this instance of the data source class always, and across
        /// all processes and machine if they are not created within the same
        /// second.
        /// </summary>
        virtual void Save(RecordType record, ObjectId saveTo) = 0;

        /// <summary>
        /// Write a delete marker in deleteIn dataset for the specified key
        /// instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        /// </summary>
        virtual void Delete(KeyType dataKey, ObjectId deleteIn) = 0;

        /// <summary>
        /// Permanently deletes (drops) the database with all records
        /// in it without the possibility to recover them later.
        ///
        /// This method should only be used to free storage. For
        /// all other purposes, methods that preserve history should
        /// be used.
        ///
        /// ATTENTION - THIS METHOD WILL DELETE ALL DATA WITHOUT
        /// THE POSSIBILITY OF RECOVERY. USE WITH CAUTION.
        /// </summary>
        virtual void DeleteDb() = 0;

        /// <summary>
        /// Return ObjectId for the latest dataset record with
        /// matching dataSetID string from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// Return ObjectId.Empty if not found.
        ///
        /// This method will return the value from in-memory
        /// cache even if it is no longer the latest version
        /// in the data store and will only load it from storage
        /// if not found in cache. Use LoadDataSet method to
        /// force reloading the dataset from storage.
        ///
        /// Error message if no matching dataSetID string is found
        /// or a delete marker is found instead.
        /// </summary>
        virtual ObjectId GetDataSetOrEmpty(dot::String dataSetID, ObjectId loadFrom) = 0;

        /// <summary>
        /// Save new version of the dataset.
        ///
        /// This method sets ID field of the argument to be the
        /// new ObjectId assigned to the record when it is saved.
        /// The timestamp of the new ObjectId is the current time.
        ///
        /// This method updates in-memory cache to the saved dataset.
        /// </summary>
        virtual void SaveDataSet(DataSetData dataSetData, ObjectId saveTo) = 0;

    public: // METHODS

        /// <summary>
        /// Return ObjectId of the latest Common dataset.
        ///
        /// Common dataset is always stored in root dataset.
        /// </summary>
        ObjectId GetCommon();

        /// <summary>
        /// Return ObjectId for the latest dataset record with
        /// matching dataSetID string from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// Error message if not found.
        ///
        /// This method will return the value from in-memory
        /// cache even if it is no longer the latest version
        /// in the data store and will only load it from storage
        /// if not found in cache. Use LoadDataSet method to
        /// force reloading the dataset from storage.
        /// </summary>
        ObjectId GetDataSet(dot::String dataSetID, ObjectId loadFrom);

        /// <summary>
        /// Create new version of the dataset with the specified dataSetID.
        ///
        /// This method updates in-memory cache to the saved dataset.
        /// </summary>
        ObjectId CreateDataSet(dot::String dataSetID, ObjectId saveTo);

        /// <summary>
        /// Create new version of the dataset with the specified dataSetID
        /// and parent dataset ObjectIds passed as an array, and return
        /// the new ObjectId assigned to the saved dataset.
        ///
        /// This method updates in-memory cache to the saved dataset.
        /// </summary>
        ObjectId CreateDataSet(dot::String dataSetID, dot::IEnumerable<ObjectId> parentDataSets, ObjectId saveTo);

        /// <summary>
        /// Create new version of the Common dataset. By convention,
        /// the Common dataset has no parents and is the ultimate
        /// parent of all dataset hierarchies, except for those
        /// datasets that do not have any parents. The Common dataset
        /// is always saved in root dataset.
        ///
        /// This method sets ID field of the argument to be the
        /// new ObjectId assigned to the record when it is saved.
        /// The timestamp of the new ObjectId is the current time.
        ///
        /// This method updates in-memory cache to the saved dataset.
        /// </summary>
        ObjectId CreateCommon();
    };
}
