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
#include <dc/types/record/typed_key.hpp>
#include <dc/platform/data_source/env_type.hpp>
#include <dc/platform/data_set/data_set_flags.hpp>

namespace dc
{
    class DataSourceKeyImpl; using DataSourceKey = dot::Ptr<DataSourceKeyImpl>;
    class DataSourceImpl; using DataSource = dot::Ptr<DataSourceImpl>;

    class KeyImpl; using Key = dot::Ptr<KeyImpl>;
    class DataImpl; using Data = dot::Ptr<DataImpl>;
    template <typename TKey, typename TRecord> class root_record_impl;
    class DbNameKeyImpl; using DbNameKey = dot::Ptr<DbNameKeyImpl>;
    class DbServerKeyImpl; using DbServerKey = dot::Ptr<DbServerKeyImpl>;
    class DataSetImpl; using DataSet = dot::Ptr<DataSetImpl>;
    class TemporalMongoQueryImpl; using TemporalMongoQuery = dot::Ptr<TemporalMongoQueryImpl>;

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
    class DC_CLASS DataSourceImpl : public RootRecordImpl<DataSourceKeyImpl, DataSourceImpl>
    {
        typedef DataSourceImpl self;

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
        virtual TemporalId create_ordered_object_id() = 0;

        template <class TRecord>
        TRecord load_or_null(TemporalId id)
        {
            return (TRecord) load_or_null(id, dot::typeof<TRecord>());
        }

        /// Load record by its TemporalId.
        ///
        /// Error message if there is no record for the specified TemporalId,
        /// or if the record exists but is not derived from TRecord.
        template <class TRecord>
        TRecord load(TemporalId id)
        {
            return (TRecord)load(id, ::dot::typeof<TRecord>());
        }

        Record load(TemporalId id, dot::Type data_type);

        /// Load record by its TemporalId and Type.
        ///
        /// Return null if not found.
        virtual Record load_or_null(TemporalId id, dot::Type data_type) = 0;

        /// Load record by String key from the specified dataset or
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
        virtual Record load_or_null(Key key, TemporalId load_from) = 0;

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
        Record load(Key key, TemporalId load_from)
        {
            Record result = load_or_null(key, load_from);
            if (result.is_empty())
                throw dot::Exception(dot::String::format("Record with key {0} is not found in dataset with TemporalId={1}.", key->get_value(), load_from.to_string()));
            return result;
        }

        /// Load record by its key and data_set.
        ///
        /// Return null if not found.
        template <class TKey, class TRecord>
        TRecord load_or_null(TypedKey<TKey, TRecord> key, TemporalId load_from)
        {
            return (TRecord)load_or_null(key, load_from);
        }

        /// Save record to the specified dataset. After the method exits,
        /// record.data_set will be set to the value of the data_set parameter.
        ///
        /// This method guarantees that temporal_ids will be in strictly increasing
        /// order for this instance of the data source class always, and across
        /// all processes and machine if they are not created within the same
        /// second.
        virtual void save_many(dot::List<Record> records, TemporalId data_set) = 0;

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
        void save_one(Record obj, TemporalId data_set)
        {
            this->save_many(dot::make_list<Record>({ obj }), data_set);
        }

        /// Get query for the specified Type.
        ///
        /// After applying query parameters, the lookup occurs first in the
        /// reverse chronological order of datasets to one second resolution,
        /// and then in the reverse chronological order of records
        /// within the latest dataset that has at least one record.
        ///
        /// The root dataset has empty TemporalId value that is less
        /// than any other TemporalId value. Accordingly, the root
        /// dataset is the last one in the lookup order of datasets.
        virtual TemporalMongoQuery get_query(TemporalId data_set, dot::Type type) = 0;

        template <class TRecord>
        TemporalMongoQuery get_query(TemporalId data_set)
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
        virtual void delete_record(Key data_key, TemporalId data_set) = 0;

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

        /// Return TemporalId for the latest dataset record with
        /// matching data_set_name String from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// Return TemporalId.empty if not found.
        ///
        /// This method will return the value from in-memory
        /// cache even if it is no longer the latest version
        /// in the data store and will only load it from storage
        /// if not found in cache. Use load_or_null method to
        /// force reloading the dataset from storage.
        ///
        /// Error message if no matching data_set_name String is found
        /// or a delete marker is found instead.
        virtual dot::Nullable<TemporalId> get_data_set_or_empty(dot::String data_set_name, TemporalId load_from) = 0;

        /// Save new version of the dataset.
        ///
        /// This method sets id field of the argument to be the
        /// new TemporalId assigned to the record when it is saved.
        /// The timestamp of the new TemporalId is the current time.
        ///
        /// This method updates in-memory cache to the saved dataset.
        virtual void save_data_set(DataSet data_set_data, TemporalId save_to) = 0;

    public: // METHODS

        /// Return TemporalId of the latest common dataset.
        ///
        /// common dataset is always stored in root dataset.
        TemporalId get_common();

        /// Return TemporalId for the latest dataset record with
        /// matching data_set_name String from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// Error message if not found.
        ///
        /// This method will return the value from in-memory
        /// cache even if it is no longer the latest version
        /// in the data store and will only load it from storage
        /// if not found in cache. Use load_or_null method to
        /// force reloading the dataset from storage.
        TemporalId get_data_set(dot::String data_set_name, TemporalId load_from);

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
        TemporalId create_common();

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
        TemporalId create_common(DataSetFlags flags);

        /// Create new version of the dataset with the specified data_set_name.
        ///
        /// This method updates in-memory cache to the saved dataset.
        TemporalId create_data_set(dot::String data_set_name, TemporalId save_to);

        /// Create new version of the dataset with the specified data_set_name
        /// and parent dataset temporal_ids passed as an array, and return
        /// the new TemporalId assigned to the saved dataset.
        ///
        /// This method updates in-memory cache to the saved dataset.
        TemporalId create_data_set(dot::String data_set_name, dot::List<TemporalId> parent_data_sets, TemporalId save_to);

        /// Create dataset with the specified dataSetName and flags
        /// in context.DataSet, and make context.DataSet its sole import.
        ///
        /// This method updates in-memory dataset cache to include
        /// the created dataset.
        TemporalId create_data_set(dot::String data_set_name, DataSetFlags flags, TemporalId save_to);

        /// Create dataset with the specified dataSetName, imports,
        /// and flags in parentDataSet.
        ///
        /// This method updates in-memory dataset cache to include
        /// the created dataset.
        TemporalId create_data_set(dot::String data_set_name, dot::List<TemporalId> parent_data_sets, DataSetFlags flags, TemporalId save_to);

        dot::String to_string() { return get_key(); }

    public: // PROPERTIES

        /// Unique data source name.
        dot::String data_source_name;

        /// Environment type enumeration.
        ///
        /// Some API functions are restricted based on the environment type.
        EnvType env_type;

        /// The meaning of environment group depends on the environment type.
        ///
        /// * For PROD, UAT, and DEV environment types, environment group
        ///   identifies the endpoint.
        ///
        /// * For USER environment type, environment group is user alias.
        ///
        /// * For TEST environment type, environment group is the name of
        ///   the unit test class (test fixture).
        dot::String env_group;

        /// The meaning of environment name depends on the environment type.
        ///
        /// * For PROD, UAT, DEV, and USER environment types, it is the
        ///   name of the user environment selected in the client.
        ///
        /// * For TEST environment type, it is the test method name.
        dot::String env_name;

        /// Flag indicating that the data source is non-temporal.
        ///
        /// For the data stored in data sources where NonTemporal == false,
        /// the data source keeps permanent history of changes to each
        /// record (except where dataset or record are marked as NonTemporal),
        /// and provides the ability to access the record as of the specified
        /// TemporalId, where TemporalId serves as a timeline (records created
        /// later have greater TemporalId than records created earlier).
        ///
        /// For the data stored in data source where NonTemporal == true,
        /// the data source keeps only the latest version of the record. All
        /// datasets created by a NonTemporal data source must also be non-temporal.
        ///
        /// In a non-temporal data source, this flag is ignored as all
        /// datasets in such data source are non-temporal.
        bool non_temporal;

        /// Use this flag to mark data source as readonly.
        ///
        /// Data source may also be readonly because CutoffTime is set.
        bool read_only;

        DOT_TYPE_BEGIN("dc", "DataSource")
            DOT_TYPE_PROP(data_source_name)
            DOT_TYPE_PROP(env_type)
            DOT_TYPE_PROP(env_group)
            DOT_TYPE_PROP(env_name)
            DOT_TYPE_PROP(non_temporal)
            DOT_TYPE_PROP(read_only)
            DOT_TYPE_BASE(RootRecord<DataSourceKeyImpl, DataSourceImpl>)
        DOT_TYPE_END()
    };
}
