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
    template <typename TKey, typename TRecord> class root_record_impl;
    class db_name_key_impl; using db_name_key = dot::ptr<db_name_key_impl>;
    class db_server_key_impl; using db_server_key = dot::ptr<db_server_key_impl>;
    class query_impl; using query = dot::ptr<query_impl>;
    class data_set_data_impl; using data_set_data = dot::ptr<data_set_data_impl>;
    class object_cursor_wrapper_impl; using object_cursor_wrapper = dot::ptr<object_cursor_wrapper_impl>;

    class dot::object_id;

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
    class DC_CLASS data_source_data_impl : public root_record_for_impl<data_source_key_impl, data_source_data_impl>
    {
        typedef data_source_data_impl self;

    public: // ABSTRACT

        /// The returned dot::object_ids have the following order guarantees:
        ///
        /// * For this data source instance, to arbitrary resolution; and
        /// * Across all processes and machines, to one second resolution
        ///
        /// One second resolution means that two dot::object_ids created within
        /// the same second by different instances of the data source
        /// class may not be ordered chronologically unless they are at
        /// least one second apart.
        virtual dot::object_id create_ordered_object_id() = 0;

        template <class t_record>
        t_record load_or_null(dot::object_id id)
        {
            return (t_record) load_or_null(id, dot::typeof<t_record>());
        }

        /// Load record by its dot::object_id and Type.
        ///
        /// Return null if not found.
        virtual record_base load_or_null(dot::object_id id, dot::type_t data_type) = 0;

        /// This method does not use cached value inside the key
        /// and always retrieves a new record from storage. To get
        /// the record cached inside the key instead (if present), use
        /// the caching variant of this method:
        ///
        /// load_or_null(key, loadFrom)
        ///
        /// Load record by string key from the specified dataset or
        /// its parent. The lookup occurs first in the reverse
        /// chronological order of datasets to one second resolution,
        /// and then in the reverse chronological order of records
        /// within the latest dataset that has at least one record.
        ///
        /// The root dataset has empty dot::object_id value that is less
        /// than any other dot::object_id value. Accordingly, the root
        /// dataset is the last one in the lookup order of datasets.
        ///
        /// The first record in this lookup order is returned, or null
        /// if no records are found or if delete marker is the first
        /// record.
        ///
        /// Return null if there is no record for the specified dot::object_id;
        /// however an exception will be thrown if the record exists but
        /// is not derived from TRecord.
        virtual record_base reload_or_null(key_base key, dot::object_id load_from) = 0;

        /// Save record to the specified dataset. After the method exits,
        /// record.data_set will be set to the value of the dataSet parameter.
        ///
        /// This method guarantees that dot::object_ids will be in strictly increasing
        /// order for this instance of the data source class always, and across
        /// all processes and machine if they are not created within the same
        /// second.
        virtual void save(record_base record, dot::object_id data_set) = 0;

        /// Get query for the specified type.
        ///
        /// After applying query parameters, the lookup occurs first in the
        /// reverse chronological order of datasets to one second resolution,
        /// and then in the reverse chronological order of records
        /// within the latest dataset that has at least one record.
        ///
        /// The root dataset has empty dot::object_id value that is less
        /// than any other dot::object_id value. Accordingly, the root
        /// dataset is the last one in the lookup order of datasets.
        virtual query get_query(dot::object_id data_set, dot::type_t type) = 0;

        template <class TRecord>
        query get_query(dot::object_id data_set)
        {
            return get_query(data_set, dot::typeof<TRecord>());
        }

        /// Write a delete marker for the specified dataSet and dataKey
        /// instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        virtual void delete_record(key_base data_key, dot::object_id data_set) = 0;

        /// Permanently deletes (drops) the database with all records
        /// in it without the possibility to recover them later.
        ///
        /// This method should only be used to free storage. For
        /// all other purposes, methods that preserve history should
        /// be used.
        ///
        /// ATTENTION - THIS METHOD WILL DELETE ALL DATA WITHOUT
        /// THE POSSIBILITY OF RECOVERY. USE WITH CAUTION.
        virtual void delete_db() = 0;

    public: // METHODS

        /// Returns true if the data source is readonly,
        /// which may be for the following reasons:
        ///
        /// * ReadOnly flag is true; or
        /// * One of RevisedBefore or RevisedBeforeId is set
        bool is_read_only();

        /// Error message if the data source is readonly,
        /// which may be for the following reasons:
        ///
        /// * ReadOnly flag is true; or
        /// * One of RevisedBefore or RevisedBeforeId is set
        void check_not_read_only();

        /// Return dot::object_id for the latest dataset record with
        /// matching dataSetID string from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// Return dot::object_id.Empty if not found.
        ///
        /// This method will return the value from in-memory
        /// cache even if it is no longer the latest version
        /// in the data store and will only load it from storage
        /// if not found in cache. Use load_or_null method to
        /// force reloading the dataset from storage.
        ///
        /// Error message if no matching dataSetID string is found
        /// or a delete marker is found instead.
        dot::object_id get_data_set_or_empty(dot::string data_set_id, dot::object_id load_from);

        /// Save new version of the dataset.
        ///
        /// This method sets ID field of the argument to be the
        /// new dot::object_id assigned to the record when it is saved.
        /// The timestamp of the new dot::object_id is the current time.
        ///
        /// This method updates in-memory cache to the saved dataset.
        void save_data_set(data_set_data data_set_data, dot::object_id save_to);

        /// Load enumeration of record by query
        /// The lookup occurs first in the reverse
        /// chronological order of datasets to one second resolution,
        /// and then in the reverse chronological order of records
        /// within the latest dataset that has at least one record.
        ///
        /// The root dataset has empty dot::object_id value that is less
        /// than any other dot::object_id value. Accordingly, the root
        /// dataset is the last one in the lookup order of datasets.
        ///
        /// The first record in this lookup order is returned, or null
        /// if no records are found or if delete marker is the first
        /// record.
        virtual object_cursor_wrapper load_by_query(query query) = 0;

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
        dot::hash_set<dot::object_id> get_data_set_lookup_list(dot::object_id load_from);

    public: // METHODS

        /// Return dot::object_id of the latest common dataset.
        ///
        /// common dataset is always stored in root dataset.
        dot::object_id get_common();

        /// Return dot::object_id for the latest dataset record with
        /// matching dataSetID string from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// Error message if not found.
        ///
        /// This method will return the value from in-memory
        /// cache even if it is no longer the latest version
        /// in the data store and will only load it from storage
        /// if not found in cache. Use load_or_null method to
        /// force reloading the dataset from storage.
        dot::object_id get_data_set(dot::string data_set_id, dot::object_id load_from);

        /// Create new version of the dataset with the specified dataSetID.
        ///
        /// This method updates in-memory cache to the saved dataset.
        dot::object_id create_data_set(dot::string data_set_id, dot::object_id save_to);

        /// Create new version of the dataset with the specified dataSetID
        /// and parent dataset dot::object_ids passed as an array, and return
        /// the new dot::object_id assigned to the saved dataset.
        ///
        /// This method updates in-memory cache to the saved dataset.
        dot::object_id create_data_set(dot::string data_set_id, dot::list<dot::object_id> parent_data_sets, dot::object_id save_to);

        /// Create new version of the common dataset. By convention,
        /// the common dataset has no parents and is the ultimate
        /// parent of all dataset hierarchies, except for those
        /// datasets that do not have any parents. The common dataset
        /// is always saved in root dataset.
        ///
        /// This method sets ID field of the argument to be the
        /// new dot::object_id assigned to the record when it is saved.
        /// The timestamp of the new dot::object_id is the current time.
        ///
        /// This method updates in-memory cache to the saved dataset.
        dot::object_id create_common();

    protected: // PROTECTED

        /// The data source will return records for which _id is strictly
        /// less than RevisionTimeConstraint.
        ///
        /// This field is set based on either RevisedBefore and RevisedBeforeId
        /// elements that are alternates; only one of them can be specified.
        dot::nullable<dot::object_id> get_revision_time_constraint();

    private: // PRIVATE

        /// Load dot::object_id for the latest dataset record with
        /// matching dataSetID string from storage even if
        /// present in in-memory cache. Update the cache with
        /// the loaded value.
        ///
        /// Return dot::object_id.Empty if not found.
        ///
        /// This method will always load the latest data from
        /// storage. Consider using the corresponding Get...
        /// method when there is no need to load the latest
        /// value from storage. The Get... method is faster
        /// because it will return the value from in-memory
        /// cache when present.
        dot::object_id load_data_set_or_empty(dot::string data_set_id, dot::object_id load_from);

        /// Builds hashset of parent datasets for specified dataset data,
        /// including parents of parents to unlimited depth with cyclic
        /// references and duplicates removed. This method uses cached lookup
        /// list for the parent datasets but not for the argument dataset.
        ///
        /// This overload of the method will return the result hashset.
        ///
        /// This private helper method should not be used directly.
        /// It provides functionality for the public API of this class.
        dot::hash_set<dot::object_id> build_data_set_lookup_list(data_set_data data_set_data);

        /// Builds hashset of parent datasets for specified dataset data,
        /// including parents of parents to unlimited depth with cyclic
        /// references and duplicates removed. This method uses cached lookup
        /// list for the parent datasets but not for the argument dataset.
        ///
        /// This overload of the method will return the result hashset.
        ///
        /// This private helper method should not be used directly.
        /// It provides functionality for the public API of this class.
        void build_data_set_lookup_list(data_set_data data_set_data, dot::hash_set<dot::object_id> result);

        dot::string to_string() { return get_key(); }

    private: // FIELDS

        /// Dictionary of dataset dot::object_ids stored under string dataSetID.
        dot::dictionary<dot::string, dot::object_id> data_set_dict_ = dot::make_dictionary<dot::string, dot::object_id>();

        /// Dictionary of the expanded list of parent dot::object_ids of dataset, including
        /// parents of parents to unlimited depth with cyclic references and duplicates
        /// removed, under dot::object_id of the dataset.
        dot::dictionary<dot::object_id, dot::hash_set<dot::object_id>> data_set_parent_dict_ = dot::make_dictionary<dot::object_id, dot::hash_set<dot::object_id>>();

    public: // PROPERTIES

        /// This class enforces strict naming conventions
        /// for database naming. While format of the resulting database
        /// name is specific to data store type, it always consists
        /// of three tokens: InstanceType, InstanceName, and EnvName.
        /// The meaning of InstanceName and EnvName tokens depends on
        /// the value of InstanceType enumeration.
        db_name_key db_name;

        /// Identifies the database server used by this data source.
        db_server_key db_server;

        /// Use this flag to mark dataset as readonly, but use either
        /// IsReadOnly() or CheckNotReadonly() method to determine the
        /// readonly status because the dataset may be readonly for
        /// two reasons:
        ///
        /// * ReadOnly flag is true; or
        /// * One of RevisedBefore or RevisedBeforeId is set
        bool read_only;

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
        dot::nullable<dot::local_date_time> revised_before;

        /// The data source will return records for which _id is strictly
        /// less than the specified dot::object_id.
        ///
        /// RevisedBefore and RevisedBeforeId elements are alternates;
        /// they cannot be specified at the same time.
        ///
        /// If either RevisedBefore or RevisedBeforeId is specified, the
        /// data source is readonly and its IsReadOnly() method returns true.
        dot::nullable<dot::object_id> revised_before_id;

        /// Unique data source identifier.
        dot::string data_source_id;

        DOT_TYPE_BEGIN("dc", "data_source_data")
            DOT_TYPE_PROP(data_source_id)
            DOT_TYPE_PROP(db_name)
            DOT_TYPE_PROP(db_server)
            DOT_TYPE_BASE(root_record_for<data_source_key_impl, data_source_data_impl>)
        DOT_TYPE_END()
    };
}
