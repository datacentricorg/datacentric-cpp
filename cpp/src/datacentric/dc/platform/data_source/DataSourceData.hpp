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
#include <dot/system/ptr.hpp>
#include <dot/system/nullable.hpp>
#include <dot/system/collections/generic/Dictionary.hpp>
#include <dot/system/collections/generic/hash_set.hpp>
#include <dot/system/collections/generic/list.hpp>
#include <dc/types/record/record.hpp>
#include <dc/platform/data_source/DatabaseKey.hpp>
#include <dc/platform/data_source/DatabaseServerKey.hpp>
#include <dc/platform/query/Query.hpp>

namespace dc
{
    class data_source_key_impl; using data_source_key = dot::ptr<data_source_key_impl>;
    class data_source_data_impl; using data_source_data = dot::ptr<data_source_data_impl>;

    class key_base_impl; using key_base = dot::ptr<key_base_impl>;
    class data_impl; using data = dot::ptr<data_impl>;
    template <typename TKey, typename TRecord> class RootRecordForImpl;
    class db_name_key_impl; using db_name_key = dot::ptr<db_name_key_impl>;
    class db_server_key_impl; using db_server_key = dot::ptr<db_server_key_impl>;
    class query_impl; using query = dot::ptr<query_impl>;
    class DataSetDataImpl; using DataSetData = dot::ptr<DataSetDataImpl>;
    class object_cursor_wrapper_impl; using object_cursor_wrapper = dot::ptr<object_cursor_wrapper_impl>;

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
    class DC_CLASS data_source_data_impl : public root_record_for_impl<data_source_key_impl, data_source_data_impl>
    {
        typedef data_source_data_impl self;

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
        virtual ObjectId create_ordered_object_id() = 0;

        template <class t_record>
        t_record load_or_null(ObjectId id)
        {
            return (t_record) load_or_null(id, dot::typeof<t_record>());
        }

        /// <summary>
        /// Load record by its ObjectId and Type.
        ///
        /// Return null if not found.
        /// </summary>
        virtual record_base load_or_null(ObjectId id, dot::type_t data_type) = 0;

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
        virtual record_base reload_or_null(key_base key, ObjectId load_from) = 0;

        /// <summary>
        /// Save record to the specified dataset. After the method exits,
        /// record.DataSet will be set to the value of the dataSet parameter.
        ///
        /// This method guarantees that ObjectIds will be in strictly increasing
        /// order for this instance of the data source class always, and across
        /// all processes and machine if they are not created within the same
        /// second.
        /// </summary>
        virtual void save(record_base record, ObjectId data_set) = 0;

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
        virtual query get_query(ObjectId data_set, dot::type_t type) = 0;

        template <class TRecord>
        query get_query(ObjectId data_set)
        {
            return get_query(data_set, dot::typeof<TRecord>());
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
        virtual void delete_record(key_base data_key, ObjectId data_set) = 0;

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
        virtual void delete_db() = 0;

    public: // METHODS

        /// <summary>
        /// Returns true if the data source is readonly,
        /// which may be for the following reasons:
        ///
        /// * ReadOnly flag is true; or
        /// * One of RevisedBefore or RevisedBeforeId is set
        /// </summary>
        bool is_read_only();

        /// <summary>
        /// Error message if the data source is readonly,
        /// which may be for the following reasons:
        ///
        /// * ReadOnly flag is true; or
        /// * One of RevisedBefore or RevisedBeforeId is set
        /// </summary>
        void check_not_read_only();

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
        ObjectId get_data_set_or_empty(dot::string data_set_id, ObjectId load_from);

        /// <summary>
        /// Save new version of the dataset.
        ///
        /// This method sets ID field of the argument to be the
        /// new ObjectId assigned to the record when it is saved.
        /// The timestamp of the new ObjectId is the current time.
        ///
        /// This method updates in-memory cache to the saved dataset.
        /// </summary>
        void save_data_set(DataSetData data_set_data, ObjectId save_to);

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
        virtual object_cursor_wrapper load_by_query(query query) = 0;

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
        dot::hash_set<ObjectId> get_data_set_lookup_list(ObjectId load_from);

    public: // METHODS

        /// <summary>
        /// Return ObjectId of the latest Common dataset.
        ///
        /// Common dataset is always stored in root dataset.
        /// </summary>
        ObjectId get_common();

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
        ObjectId get_data_set(dot::string data_set_id, ObjectId load_from);

        /// <summary>
        /// Create new version of the dataset with the specified dataSetID.
        ///
        /// This method updates in-memory cache to the saved dataset.
        /// </summary>
        ObjectId create_data_set(dot::string data_set_id, ObjectId save_to);

        /// <summary>
        /// Create new version of the dataset with the specified dataSetID
        /// and parent dataset ObjectIds passed as an array, and return
        /// the new ObjectId assigned to the saved dataset.
        ///
        /// This method updates in-memory cache to the saved dataset.
        /// </summary>
        ObjectId create_data_set(dot::string data_set_id, dot::list<ObjectId> parent_data_sets, ObjectId save_to);

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
        ObjectId create_common();

    protected: // PROTECTED

        /// <summary>
        /// The data source will return records for which _id is strictly
        /// less than RevisionTimeConstraint.
        ///
        /// This field is set based on either RevisedBefore and RevisedBeforeId
        /// elements that are alternates; only one of them can be specified.
        /// </summary>
        dot::nullable<ObjectId> get_revision_time_constraint();

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
        ObjectId load_data_set_or_empty(dot::string data_set_id, ObjectId load_from);

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
        dot::hash_set<ObjectId> build_data_set_lookup_list(DataSetData data_set_data);

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
        void build_data_set_lookup_list(DataSetData data_set_data, dot::hash_set<ObjectId> result);

        dot::string to_string() { return get_key(); }

    private: // FIELDS

        /// <summary>Dictionary of dataset ObjectIds stored under string dataSetID.</summary>
        dot::dictionary<dot::string, ObjectId> data_set_dict_ = dot::make_dictionary<dot::string, ObjectId>();

        /// <summary>
        /// Dictionary of the expanded list of parent ObjectIds of dataset, including
        /// parents of parents to unlimited depth with cyclic references and duplicates
        /// removed, under ObjectId of the dataset.</summary>
        dot::dictionary<ObjectId, dot::hash_set<ObjectId>> data_set_parent_dict_ = dot::make_dictionary<ObjectId, dot::hash_set<ObjectId>>();

    public: // PROPERTIES

        /// <summary>
        /// This class enforces strict naming conventions
        /// for database naming. While format of the resulting database
        /// name is specific to data store type, it always consists
        /// of three tokens: InstanceType, InstanceName, and EnvName.
        /// The meaning of InstanceName and EnvName tokens depends on
        /// the value of InstanceType enumeration.
        /// </summary>
        db_name_key db_name;

        /// <summary>
        /// Identifies the database server used by this data source.
        /// </summary>
        db_server_key db_server;

        /// <summary>
        /// Use this flag to mark dataset as readonly, but use either
        /// IsReadOnly() or CheckNotReadonly() method to determine the
        /// readonly status because the dataset may be readonly for
        /// two reasons:
        ///
        /// * ReadOnly flag is true; or
        /// * One of RevisedBefore or RevisedBeforeId is set
        /// </summary>
        bool read_only;

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
        dot::nullable<dot::local_date_time> revised_before;

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
        dot::nullable<ObjectId> revised_before_id;

        /// <summary>Unique data source identifier.</summary>
        dot::string data_source_id;

        DOT_TYPE_BEGIN("DataCentric", "DataSourceData")
            DOT_TYPE_PROP(data_source_id)
            DOT_TYPE_PROP(db_name)
            DOT_TYPE_PROP(db_server)
            DOT_TYPE_BASE(root_record_for<data_source_key_impl, data_source_data_impl>)
        DOT_TYPE_END()
    };
}
