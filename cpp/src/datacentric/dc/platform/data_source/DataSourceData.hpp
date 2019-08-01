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
#include <dot/system/Nullable.hpp>
#include <dot/system/collections/generic/Dictionary.hpp>
#include <dot/system/collections/generic/HashSet.hpp>
#include <dot/system/collections/generic/List.hpp>
#include <dc/types/record/RecordFor.hpp>
#include <dc/platform/data_source/IDataSource.hpp>
#include <dc/platform/data_source/DatabaseKey.hpp>
#include <dc/platform/data_source/DatabaseServerKey.hpp>
#include <dc/platform/query/IQuery.hpp>

namespace dc
{
    class DataSourceKeyImpl; using DataSourceKey = dot::ptr<DataSourceKeyImpl>;
    class DataSourceDataImpl; using DataSourceData = dot::ptr<DataSourceDataImpl>;

    class KeyTypeImpl; using KeyType = dot::ptr<KeyTypeImpl>;
    class DataImpl; using Data = dot::ptr<DataImpl>;
    template <typename TKey, typename TRecord> class RootRecordForImpl;
    class DbNameKeyImpl; using DbNameKey = dot::ptr<DbNameKeyImpl>;
    class DbServerKeyImpl; using DbServerKey = dot::ptr<DbServerKeyImpl>;
    class IQueryImpl; using IQuery = dot::ptr<IQueryImpl>;

    class ObjectId;

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
    class DC_CLASS DataSourceDataImpl : public RootRecordForImpl<DataSourceKeyImpl, DataSourceDataImpl>, public IDataSourceImpl
    {
        typedef DataSourceDataImpl self;

    private: // FIELDS

        /// <summary>Dictionary of dataset ObjectIds stored under string dataSetID.</summary>
        dot::Dictionary<dot::string, ObjectId> dataSetDict_ = dot::new_Dictionary<dot::string, ObjectId>();

        /// <summary>
        /// Dictionary of the expanded list of parent ObjectIds of dataset, including
        /// parents of parents to unlimited depth with cyclic references and duplicates
        /// removed, under ObjectId of the dataset.</summary>
        dot::Dictionary<ObjectId, dot::HashSet<ObjectId>> dataSetParentDict_ = dot::new_Dictionary<ObjectId, dot::HashSet<ObjectId>>();

    public: // PROPERTIES

        /// <summary>
        /// This class enforces strict naming conventions
        /// for database naming. While format of the resulting database
        /// name is specific to data store type, it always consists
        /// of three tokens: InstanceType, InstanceName, and EnvName.
        /// The meaning of InstanceName and EnvName tokens depends on
        /// the value of InstanceType enumeration.
        /// </summary>
        DbNameKey DbName;

        /// <summary>
        /// Identifies the database server used by this data source.
        /// </summary>
        DbServerKey DbServer;

        /// <summary>
        /// Use this flag to mark dataset as readonly, but use either
        /// IsReadOnly() or CheckNotReadonly() method to determine the
        /// readonly status because the dataset may be readonly for
        /// two reasons:
        ///
        /// * ReadOnly flag is true; or
        /// * One of RevisedBefore or RevisedBeforeId is set
        /// </summary>
        bool ReadOnly;

        /// <summary>
        /// The data source will return records revised strictly before
        /// the specified datetime in UTC timezone. The filter works
        /// by comparing the timestep component of the record's _id with
        /// the value of this element.
        ///
        /// The value of this element must fall precisely on the second,
        /// error message otherwise.
        ///
        /// RevisedBefore and RevisedBeforeId elements are alternates;
        /// they cannot be specified at the same time.
        ///
        /// If either RevisedBefore or RevisedBeforeId is specified, the
        /// data source is readonly and its IsReadOnly() method returns true.
        /// </summary>
        dot::Nullable<dot::local_date_time> RevisedBefore;

        /// <summary>
        /// The data source will return records for which _id is strictly
        /// less than the specified ObjectId.
        ///
        /// RevisedBefore and RevisedBeforeId elements are alternates;
        /// they cannot be specified at the same time.
        ///
        /// If either RevisedBefore or RevisedBeforeId is specified, the
        /// data source is readonly and its IsReadOnly() method returns true.
        /// </summary>
        dot::Nullable<ObjectId> RevisedBeforeId;

    public: // ABSTRACT

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

        template <class TRecord>
        TRecord LoadOrNull(ObjectId id)
        {
            return (TRecord) LoadOrNull(id, dot::typeof<TRecord>());
        }

        /// <summary>
        /// Load record by its ObjectId and Type.
        ///
        /// Return null if not found.
        /// </summary>
        virtual RecordType LoadOrNull(ObjectId id, dot::type_t dataType) = 0;

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
        /// This method guarantees that ObjectIds will be in strictly increasing
        /// order for this instance of the data source class always, and across
        /// all processes and machine if they are not created within the same
        /// second.
        /// </summary>
        virtual void Save(RecordType record, ObjectId dataSet) = 0;

        /// <summary>
        /// Get query for the specified type.
        ///
        /// After applying query parameters, the lookup occurs first in the
        /// reverse chronological order of datasets to one second resolution,
        /// and then in the reverse chronological order of records
        /// within the latest dataset that has at least one record.
        ///
        /// The root dataset has empty ObjectId value that is less
        /// than any other ObjectId value. Accordingly, the root
        /// dataset is the last one in the lookup order of datasets.
        /// </summary>
        virtual IQuery GetQuery(ObjectId dataSet, dot::type_t type) = 0;

        template <class DataType>
        IQuery GetQuery(ObjectId dataSet)
        {
            return GetQuery(dataSet, dot::typeof<DataType>());
        }

        /// <summary>
        /// Write a delete marker for the specified dataSet and dataKey
        /// instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        /// </summary>
        virtual void Delete(KeyType dataKey, ObjectId dataSet) = 0;

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

    public: // METHODS

        /// <summary>
        /// Returns true if the data source is readonly,
        /// which may be for the following reasons:
        ///
        /// * ReadOnly flag is true; or
        /// * One of RevisedBefore or RevisedBeforeId is set
        /// </summary>
        bool IsReadOnly();

        /// <summary>
        /// Error message if the data source is readonly,
        /// which may be for the following reasons:
        ///
        /// * ReadOnly flag is true; or
        /// * One of RevisedBefore or RevisedBeforeId is set
        /// </summary>
        void CheckNotReadOnly();

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
        ObjectId GetDataSetOrEmpty(dot::string dataSetID, ObjectId loadFrom);

        /// <summary>
        /// Save new version of the dataset.
        ///
        /// This method sets ID field of the argument to be the
        /// new ObjectId assigned to the record when it is saved.
        /// The timestamp of the new ObjectId is the current time.
        ///
        /// This method updates in-memory cache to the saved dataset.
        /// </summary>
        void SaveDataSet(DataSetData dataSetData, ObjectId saveTo);

        /// <summary>
        /// Load enumeration of record by query
        /// The lookup occurs first in the reverse
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
        /// </summary>
        virtual dot::IObjectEnumerable LoadByQuery(IQuery query) = 0;

        /// <summary>
        /// Returns enumeration of parent datasets for specified dataset data,
        /// including parents of parents to unlimited depth with cyclic
        /// references and duplicates removed.
        ///
        /// The list will not include datasets that are not strictly earlier
        /// than RevisionTimeConstraint, or their parents (even if the parents
        /// are earlier than the constraint).
        ///
        /// The list will not include datasets that are not strictly earlier
        /// than RevisionTimeConstraint, or their parents (even if the parents
        /// are earlier than the constraint)
        /// </summary>
        dot::IEnumerable<ObjectId> GetDataSetLookupList(ObjectId loadFrom);

    protected: // PROTECTED

        /// <summary>
        /// The data source will return records for which _id is strictly
        /// less than RevisionTimeConstraint.
        ///
        /// This field is set based on either RevisedBefore and RevisedBeforeId
        /// elements that are alternates; only one of them can be specified.
        /// </summary>
        dot::Nullable<ObjectId> GetRevisionTimeConstraint();

    private: // PRIVATE

        /// <summary>
        /// Load ObjectId for the latest dataset record with
        /// matching dataSetID string from storage even if
        /// present in in-memory cache. Update the cache with
        /// the loaded value.
        ///
        /// Return ObjectId.Empty if not found.
        ///
        /// This method will always load the latest data from
        /// storage. Consider using the corresponding Get...
        /// method when there is no need to load the latest
        /// value from storage. The Get... method is faster
        /// because it will return the value from in-memory
        /// cache when present.
        /// </summary>
        ObjectId LoadDataSetOrEmpty(dot::string dataSetID, ObjectId loadFrom);

        /// <summary>
        /// Builds hashset of parent datasets for specified dataset data,
        /// including parents of parents to unlimited depth with cyclic
        /// references and duplicates removed. This method uses cached lookup
        /// list for the parent datasets but not for the argument dataset.
        ///
        /// This overload of the method will return the result hashset.
        ///
        /// This private helper method should not be used directly.
        /// It provides functionality for the public API of this class.
        /// </summary>
        dot::HashSet<ObjectId> BuildDataSetLookupList(DataSetData dataSetData);

        /// <summary>
        /// Builds hashset of parent datasets for specified dataset data,
        /// including parents of parents to unlimited depth with cyclic
        /// references and duplicates removed. This method uses cached lookup
        /// list for the parent datasets but not for the argument dataset.
        ///
        /// This overload of the method will return the result hashset.
        ///
        /// This private helper method should not be used directly.
        /// It provides functionality for the public API of this class.
        /// </summary>
        void BuildDataSetLookupList(DataSetData dataSetData, dot::HashSet<ObjectId> result);

        dot::string ToString() { return getKey(); }

        DOT_TYPE_BEGIN(".Runtime.Main", "DataSourceData")
            DOT_TYPE_PROP(DataSourceID)
            DOT_TYPE_PROP(DbName)
            DOT_TYPE_PROP(DbServer)
            DOT_TYPE_BASE(RootRecordFor<DataSourceKeyImpl, DataSourceDataImpl>)
        DOT_TYPE_END()
    };
}
