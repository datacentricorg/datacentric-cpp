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
#include <dot/system/collections/generic/dictionary.hpp>
#include <dot/system/collections/generic/hash_set.hpp>
#include <dot/system/collections/generic/list.hpp>
#include <dot/mongo/mongo_db/query/query.hpp>
#include <dc/types/record/typed_record.hpp>
#include <dc/platform/data_source/database_key.hpp>
#include <dc/platform/data_source/database_server_key.hpp>
#include <dc/platform/data_set/data_set_flags.hpp>

namespace dc
{
    class data_source_key_impl; using data_source_key = dot::ptr<data_source_key_impl>;
    class data_source_data_impl; using data_source_data = dot::ptr<data_source_data_impl>;

    class key_impl; using key = dot::ptr<key_impl>;
    class data_impl; using data = dot::ptr<data_impl>;
    template <typename TKey, typename TRecord> class root_record_impl;
    class db_name_key_impl; using db_name_key = dot::ptr<db_name_key_impl>;
    class db_server_key_impl; using db_server_key = dot::ptr<db_server_key_impl>;
    class data_set_data_impl; using data_set_data = dot::ptr<data_set_data_impl>;
    class object_cursor_wrapper_impl; using object_cursor_wrapper = dot::ptr<object_cursor_wrapper_impl>;
    class mongo_query_impl; using mongo_query = dot::ptr<mongo_query_impl>;

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

        /// The returned temporal_ids have the following order guarantees:
        ///
        /// * For this data source instance, to arbitrary resolution; and
        /// * Across all processes and machines, to one second resolution
        ///
        /// One second resolution means that two temporal_ids created within
        /// the same second by different instances of the data source
        /// class may not be ordered chronologically unless they are at
        /// least one second apart.
        virtual temporal_id create_ordered_object_id() = 0;

        template <class t_record>
        t_record load_or_null(temporal_id id)
        {
            return (t_record) load_or_null(id, dot::typeof<t_record>());
        }

        /// Load record by its TemporalId.
        ///
        /// Error message if there is no record for the specified TemporalId,
        /// or if the record exists but is not derived from TRecord.
        template <class t_record>
        t_record load(temporal_id id)
        {
            return (t_record)load(id, ::dot::typeof<t_record>());
        }

        record load(temporal_id id, dot::type data_type);

        /// Load record by its temporal_id and type.
        ///
        /// Return null if not found.
        virtual record load_or_null(temporal_id id, dot::type data_type) = 0;

        /// Load record by string key from the specified dataset or
        /// its list of imports. The lookup occurs first in descending
        /// order of dataset TemporalIds, and then in the descending
        /// order of record TemporalIds within the first dataset that
        /// has at least one record. Both dataset and record TemporalIds
        /// are ordered chronologically to one second resolution,
        /// and are unique within the database server or cluster.
        ///
        /// The root dataset has empty TemporalId value that is less
        /// than any other TemporalId value. Accordingly, the root
        /// dataset is the last one in the lookup order of datasets.
        ///
        /// The first record in this lookup order is returned, or null
        /// if no records are found or if DeletedRecord is the first
        /// record.
        ///
        /// Return null if there is no record for the specified TemporalId;
        /// however an exception will be thrown if the record exists but
        /// is not derived from TRecord.
        virtual record load_or_null(key key, temporal_id load_from) = 0;

        /// Load record from context.DataSource, overriding the dataset
        /// specified in the context with the value specified as the
        /// second parameter. The lookup occurs in the specified dataset
        /// and its imports, expanded to arbitrary depth with repetitions
        /// and cyclic references removed.
        ///
        /// IMPORTANT - this overload of the method loads from loadFrom
        /// dataset, not from context.DataSet.
        ///
        /// If Record property is set, its value is returned without
        /// performing lookup in the data store; otherwise the record
        /// is loaded from storage and cached in Record and the
        /// cached value is returned from subsequent calls.
        ///
        /// Once the record has been cached, the same version will be
        /// returned in subsequent calls with the same key instance.
        /// Create a new key or call earRecord() method to force
        /// reloading new version of the record from storage.
        ///
        /// Error message if the record is not found or is a DeletedRecord.
        record load(key key, temporal_id load_from)
        {
            record result = load_or_null(key, load_from);
            if (result.is_empty())
                throw dot::exception(dot::string::format("Record with key {0} is not found in dataset with TemporalId={1}.", key->get_value(), load_from.to_string()));
            return result;
        }

        /// Load record by its key and data_set.
        ///
        /// Return null if not found.
        template <class t_key, class t_record>
        t_record load_or_null(typed_key<t_key, t_record> key, temporal_id load_from)
        {
            return (t_record)load_or_null(key, load_from);
        }

        /// Save record to the specified dataset. After the method exits,
        /// record.data_set will be set to the value of the data_set parameter.
        ///
        /// This method guarantees that temporal_ids will be in strictly increasing
        /// order for this instance of the data source class always, and across
        /// all processes and machine if they are not created within the same
        /// second.
        virtual void save_many(dot::list<record> records, temporal_id data_set) = 0;

        /// Save one record to the specified dataset. After the method exits,
        /// record.DataSet will be set to the value of the dataSet parameter.
        ///
        /// All Save methods ignore the value of record.DataSet before the
        /// Save method is called. When dataset is not specified explicitly,
        /// the value of dataset from the context, not from the record, is used.
        /// The reason for this behavior is that the record may be stored from
        /// a different dataset than the one where it is used.
        ///
        /// This method guarantees that TemporalIds of the saved records will be in
        /// strictly increasing order.
        void save_one(record obj, temporal_id data_set)
        {
            this->save_many(dot::make_list<record>({ obj }), data_set);
        }

        /// Get query for the specified type.
        ///
        /// After applying query parameters, the lookup occurs first in the
        /// reverse chronological order of datasets to one second resolution,
        /// and then in the reverse chronological order of records
        /// within the latest dataset that has at least one record.
        ///
        /// The root dataset has empty temporal_id value that is less
        /// than any other temporal_id value. Accordingly, the root
        /// dataset is the last one in the lookup order of datasets.
        virtual mongo_query get_query(temporal_id data_set, dot::type type) = 0;

        template <class TRecord>
        mongo_query get_query(temporal_id data_set)
        {
            return get_query(data_set, dot::typeof<TRecord>());
        }

        /// Write a delete marker for the specified data_set and data_key
        /// instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        virtual void delete_record(key data_key, temporal_id data_set) = 0;

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

        /// Return temporal_id for the latest dataset record with
        /// matching data_set_id string from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// Return temporal_id.empty if not found.
        ///
        /// This method will return the value from in-memory
        /// cache even if it is no longer the latest version
        /// in the data store and will only load it from storage
        /// if not found in cache. Use load_or_null method to
        /// force reloading the dataset from storage.
        ///
        /// Error message if no matching data_set_id string is found
        /// or a delete marker is found instead.
        virtual dot::nullable<temporal_id> get_data_set_or_empty(dot::string data_set_id, temporal_id load_from) = 0;

        /// Save new version of the dataset.
        ///
        /// This method sets id field of the argument to be the
        /// new temporal_id assigned to the record when it is saved.
        /// The timestamp of the new temporal_id is the current time.
        ///
        /// This method updates in-memory cache to the saved dataset.
        virtual void save_data_set(data_set_data data_set_data, temporal_id save_to) = 0;

    public: // METHODS

        /// Return temporal_id of the latest common dataset.
        ///
        /// common dataset is always stored in root dataset.
        temporal_id get_common();

        /// Return temporal_id for the latest dataset record with
        /// matching data_set_id string from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// Error message if not found.
        ///
        /// This method will return the value from in-memory
        /// cache even if it is no longer the latest version
        /// in the data store and will only load it from storage
        /// if not found in cache. Use load_or_null method to
        /// force reloading the dataset from storage.
        temporal_id get_data_set(dot::string data_set_id, temporal_id load_from);

        /// Create Common dataset with default flags.
        ///
        /// By convention, the Common dataset contains reference and
        /// configuration data and is included as import in all other
        /// datasets.
        ///
        /// The Common dataset is always stored in root dataset.
        ///
        /// This method updates in-memory dataset cache to include
        /// the created dataset.
        temporal_id create_common();

        /// Create Common dataset with the specified flags.
        ///
        /// The flags may be used, among other things, to specify
        /// that the created dataset will be NonTemporal even if the
        /// data source is itself temporal. This setting is typically
        /// used to prevent the accumulation of data where history is
        /// not needed.
        ///
        /// By convention, the Common dataset contains reference and
        /// configuration data and is included as import in all other
        /// datasets.
        ///
        /// The Common dataset is always stored in root dataset.
        ///
        /// This method updates in-memory dataset cache to include
        /// the created dataset.
        temporal_id create_common(data_set_flags flags);

        /// Create new version of the dataset with the specified data_set_id.
        ///
        /// This method updates in-memory cache to the saved dataset.
        temporal_id create_data_set(dot::string data_set_id, temporal_id save_to);

        /// Create new version of the dataset with the specified data_set_id
        /// and parent dataset temporal_ids passed as an array, and return
        /// the new temporal_id assigned to the saved dataset.
        ///
        /// This method updates in-memory cache to the saved dataset.
        temporal_id create_data_set(dot::string data_set_id, dot::list<temporal_id> parent_data_sets, temporal_id save_to);

        /// Create dataset with the specified dataSetName and flags
        /// in context.DataSet, and make context.DataSet its sole import.
        ///
        /// This method updates in-memory dataset cache to include
        /// the created dataset.
        temporal_id create_data_set(dot::string data_set_id, data_set_flags flags, temporal_id save_to);

        /// Create dataset with the specified dataSetName, imports,
        /// and flags in parentDataSet.
        ///
        /// This method updates in-memory dataset cache to include
        /// the created dataset.
        temporal_id create_data_set(dot::string data_set_id, dot::list<temporal_id> parent_data_sets, data_set_flags flags, temporal_id save_to);

        dot::string to_string() { return get_key(); }

    public: // PROPERTIES

        /// This class enforces strict naming conventions
        /// for database naming. While format of the resulting database
        /// name is specific to data store type, it always consists
        /// of three tokens: instance_type, instance_name, and env_name.
        /// The meaning of instance_name and env_name tokens depends on
        /// the value of instance_type enumeration.
        db_name_key db_name;

        /// Identifies the database server used by this data source.
        db_server_key db_server;

        /// Use this flag to mark dataset as readonly, but use either
        /// is_read_only() or check_not_readonly() method to determine the
        /// readonly status because the dataset may be readonly for
        /// two reasons:
        ///
        /// * read_only flag is true; or
        /// * One of revised_before or revised_before_id is set
        dot::nullable<bool> read_only;

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
