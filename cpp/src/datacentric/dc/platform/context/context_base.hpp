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
#include <dc/types/record/temporal_id.hpp>
#include <dc/platform/data_set/data_set_data.hpp>
#include <dc/platform/data_set/data_set_flags.hpp>
#include <dc/platform/data_source/data_source_data.hpp>
#include <dc/platform/data_source/mongo/temporal_mongo_query.hpp>

namespace dc
{
    class ContextBaseImpl; using ContextBase = dot::Ptr<ContextBaseImpl>;
    class DataSourceImpl; using DataSource = dot::Ptr<DataSourceImpl>;
    class DataSetImpl; using DataSet = dot::Ptr<DataSetImpl>;

    /// Context defines dataset and provides access to data,
    /// logging, and other supporting functionality.
    class DC_CLASS ContextBaseImpl : public virtual dot::ObjectImpl
    {
        typedef ContextBaseImpl self;

    public: // PROPERTIES

        /// Get the default data source of the context.
        DataSource data_source;

        /// Returns TemporalId of the context dataset.
        TemporalId data_set;

    public: // CONSTRUCTORS

    protected:

        /// Initialize data_source.
        void set_data_source(DataSource value);

    public: // METHODS

        /// Load record by its TemporalId.
        ///
        /// Error message if there is no record for the specified ObjectId,
        /// or if the record exists but is not derived from TRecord.
        template <class TRecord>
        TRecord load(TemporalId id)
        {
            return (TRecord)load(id, ::dot::typeof<TRecord>());
        }

        /// Load record by its TemporalId.
        ///
        /// Return null if not found.
        template <class TRecord>
        TRecord load_or_null(TemporalId id)
        {
            return (TRecord) load_or_null(id, ::dot::typeof<TRecord>());
        }

        /// Load record from context.DataSource, overriding the dataset
        /// specified in the context with the value specified as the
        /// second parameter. The lookup occurs in the specified dataset
        /// and its imports, expanded to arbitrary depth with repetitions
        /// and cyclic references removed.
        ///
        /// This overload of the method loads from from context.DataSet.
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
        template <class TKey, class TRecord>
        dot::Ptr<TRecord> load(TypedKey<TKey, TRecord> key)
        {
            return (dot::Ptr<TRecord>)load(key, data_set);
        }

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
        template <class TKey, class TRecord>
        dot::Ptr<TRecord> load(TypedKey<TKey, TRecord> key, TemporalId load_from)
        {
            return (dot::Ptr<TRecord>)load(key, load_from);
        }

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
        template <class TKey, class TRecord>
        dot::Ptr<TRecord> load_or_null(TypedKey<TKey, TRecord> key, TemporalId load_from)
        {
            return (dot::Ptr<TRecord>)load_or_null((dc::Key)key, load_from);
        }

        /// Get query for the specified Type.
        ///
        /// After applying query parameters, the lookup occurs first in
        /// descending order of dataset TemporalIds, and then in the descending
        /// order of record TemporalIds within the first dataset that
        /// has at least one record. Both dataset and record TemporalIds
        /// are ordered chronologically to one second resolution,
        /// and are unique within the database server or cluster.
        ///
        /// The root dataset has empty TemporalId value that is less
        /// than any other TemporalId value. Accordingly, the root
        /// dataset is the last one in the lookup order of datasets.
        ///
        /// Generic parameter TRecord is not necessarily the root data Type;
        /// it may also be a Type derived from the root data Type.
        template <class TRecord>
        TemporalMongoQuery get_query()
        {
            return get_query(this->data_set, dot::typeof<TRecord>());
        }

        /// Get query for the specified Type.
        ///
        /// After applying query parameters, the lookup occurs first in
        /// descending order of dataset TemporalIds, and then in the descending
        /// order of record TemporalIds within the first dataset that
        /// has at least one record. Both dataset and record TemporalIds
        /// are ordered chronologically to one second resolution,
        /// and are unique within the database server or cluster.
        ///
        /// The root dataset has empty TemporalId value that is less
        /// than any other TemporalId value. Accordingly, the root
        /// dataset is the last one in the lookup order of datasets.
        ///
        /// Generic parameter TRecord is not necessarily the root data Type;
        /// it may also be a Type derived from the root data Type.
        template <class TRecord>
        TemporalMongoQuery get_query(TemporalId load_from)
        {
            return get_query(load_from, dot::typeof<TRecord>());
        }

        /// Save record to the specified dataset. After the method exits,
        /// record.data_set will be set to the value of the data_set parameter.
        ///
        /// All save methods ignore the value of record.data_set before the
        /// save method is called. When dataset is not specified explicitly,
        /// the value of dataset from the context, not from the record, is used.
        /// The reason for this behavior is that the record may be stored from
        /// a different dataset than the one where it is used.
        ///
        /// This method guarantees that temporal_ids will be in strictly increasing
        /// order for this instance of the data source class always, and across
        /// all processes and machine if they are not created within the same
        /// second.
        void save_one(Record record);

        /// Save record to the specified dataset. After the method exits,
        /// record.data_set will be set to the value of the data_set parameter.
        ///
        /// All save methods ignore the value of record.data_set before the
        /// save method is called. When dataset is not specified explicitly,
        /// the value of dataset from the context, not from the record, is used.
        /// The reason for this behavior is that the record may be stored from
        /// a different dataset than the one where it is used.
        ///
        /// This method guarantees that temporal_ids will be in strictly increasing
        /// order for this instance of the data source class always, and across
        /// all processes and machine if they are not created within the same
        /// second.
        void save_one(Record record, TemporalId save_to);

        /// Save multiple records to the specified dataset. After the method exits,
        /// for each record the property record.DataSet will be set to the value of
        /// the saveTo parameter.
        ///
        /// All Save methods ignore the value of record.DataSet before the
        /// Save method is called. When dataset is not specified explicitly,
        /// the value of dataset from the context, not from the record, is used.
        /// The reason for this behavior is that the record may be stored from
        /// a different dataset than the one where it is used.
        ///
        /// This method guarantees that TemporalIds of the saved records will be in
        /// strictly increasing order.
        void save_many(dot::List<Record> record);

        /// Save multiple records to the specified dataset. After the method exits,
        /// for each record the property record.DataSet will be set to the value of
        /// the saveTo parameter.
        ///
        /// All Save methods ignore the value of record.DataSet before the
        /// Save method is called. When dataset is not specified explicitly,
        /// the value of dataset from the context, not from the record, is used.
        /// The reason for this behavior is that the record may be stored from
        /// a different dataset than the one where it is used.
        ///
        /// This method guarantees that TemporalIds of the saved records will be in
        /// strictly increasing order.
        void save_many(dot::List<Record> record, TemporalId save_to);

        /// Write a delete marker for the dataset of the context and the specified
        /// key instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        void delete_record(Key key);

        /// Write a DeletedRecord in deleteIn dataset for the specified key
        /// instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        void delete_record(Key key, TemporalId delete_in);

        /// Permanently deletes (drops) the database with all records
        /// in it without the possibility to recover them later.
        ///
        /// This method should only be used to free storage. For
        /// all other purposes, methods that preserve history should
        /// be used.
        ///
        /// ATTENTION - THIS METHOD WILL DELETE ALL DATA WITHOUT
        /// THE POSSIBILITY OF RECOVERY. USE WITH CAUTION.
        void delete_db();

        /// Return TemporalId of the latest common dataset.
        ///
        /// common dataset is always stored in root dataset.
        TemporalId get_common();

        /// Return TemporalId for the latest dataset record with
        /// matching data_set_name String from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// This overload of the get_data_set method does not
        /// specify the load_from parameter explicitly and instead
        /// uses context.data_set for its value.
        ///
        /// Error message if not found.
        ///
        /// This method will return the value from in-memory
        /// cache even if it is no longer the latest version
        /// in the data store and will only load it from storage
        /// if not found in cache. Use load_or_null method to
        /// force reloading the dataset from storage.
        TemporalId get_data_set(dot::String data_set_name);

        /// Return TemporalId for the latest dataset record with
        /// matching data_set_name String from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// This overload of the get_data_set method specifies
        /// the load_from parameter explicitly.
        ///
        /// Error message if not found.
        ///
        /// This method will return the value from in-memory
        /// cache even if it is no longer the latest version
        /// in the data store and will only load it from storage
        /// if not found in cache. Use load_or_null method to
        /// force reloading the dataset from storage.
        TemporalId get_data_set(dot::String data_set_name, TemporalId load_from);

        /// Return TemporalId for the latest dataset record with
        /// matching data_set_name String from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// This overload of the get_data_set_or_empty method does not
        /// specify the load_from parameter explicitly and instead
        /// uses context.data_set for its value.
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
        dot::Nullable<TemporalId> get_data_set_or_empty(dot::String data_set_name);

        /// Return TemporalId for the latest dataset record with
        /// matching data_set_name String from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// This overload of the get_data_set_or_empty method specifies
        /// the load_from parameter explicitly.
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
        dot::Nullable<TemporalId> get_data_set_or_empty(dot::String data_set_name, TemporalId load_from);

        /// Create new version of the common dataset. By convention,
        /// the common dataset has no parents and is the ultimate
        /// parent of all dataset hierarchies, except for those
        /// datasets that do not have any parents. The common dataset
        /// is always saved in root dataset.
        ///
        /// This method sets id field of the argument to be the
        /// new TemporalId assigned to the record when it is saved.
        /// The timestamp of the new TemporalId is the current time.
        ///
        /// This method updates in-memory cache to the saved dataset.
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

        /// Create new version of the dataset with the specified data_set_name
        /// and no parent datasets.
        ///
        /// This overload of the create_data_set method does not
        /// specify the save_to parameter explicitly and instead
        /// uses context.data_set for its value.
        ///
        /// This method updates in-memory cache to the saved dataset.
        TemporalId create_data_set(dot::String data_set_name);

        /// Create new version of the dataset with the specified data_set_name
        /// and no parent datasets.
        ///
        /// This overload of the create_data_set method specifies
        /// the save_to parameter explicitly.
        ///
        /// This method updates in-memory cache to the saved dataset.
        TemporalId create_data_set(dot::String data_set_name, TemporalId save_to);

        /// Create new version of the dataset with the specified data_set_name
        /// and parent dataset temporal_ids passed as an array, and return
        /// the new TemporalId assigned to the saved dataset.
        ///
        /// This overload of the create_data_set method does not
        /// specify the save_to parameter explicitly and instead
        /// uses context.data_set for its value.
        ///
        /// This method updates in-memory cache to the saved dataset.
        TemporalId create_data_set(dot::String data_set_name, dot::List<TemporalId> parent_data_sets);

        /// Create new version of the dataset with the specified data_set_name
        /// and parent dataset temporal_ids passed as an array, and return
        /// the new TemporalId assigned to the saved dataset.
        ///
        /// This overload of the create_data_set method specifies
        /// the save_to parameter explicitly.
        ///
        /// This method updates in-memory cache to the saved dataset.
        TemporalId create_data_set(dot::String data_set_name, dot::List<TemporalId> parent_data_sets, TemporalId save_to);

        /// Create dataset with the specified dataSetName and flags
        /// in context.DataSet, and make context.DataSet its sole import.
        ///
        /// The flags may be used, among other things, to specify
        /// that the created dataset will be NonTemporal even if the
        /// data source is itself temporal. This setting is typically
        /// used to prevent the accumulation of data where history is
        /// not needed.
        ///
        /// This method updates in-memory dataset cache to include
        /// the created dataset.
        TemporalId create_data_set(dot::String data_set_name, DataSetFlags flags);

        /// Create dataset with the specified dataSetName and flags
        /// in parentDataSet, and make parentDataSet its sole import.
        ///
        /// The flags may be used, among other things, to specify
        /// that the created dataset will be NonTemporal even if the
        /// data source is itself temporal. This setting is typically
        /// used to prevent the accumulation of data where history is
        /// not needed.
        ///
        /// This method updates in-memory dataset cache to include
        /// the created dataset.
        TemporalId create_data_set(dot::String data_set_name, DataSetFlags flags, TemporalId save_to);

        /// Create dataset with the specified dataSetName, imports,
        /// and flags in context.DataSet.
        ///
        /// The flags may be used, among other things, to specify
        /// that the created dataset will be NonTemporal even if the
        /// data source is itself temporal. This setting is typically
        /// used to prevent the accumulation of data where history is
        /// not needed.
        ///
        /// This method updates in-memory dataset cache to include
        /// the created dataset.
        TemporalId create_data_set(dot::String data_set_name, dot::List<TemporalId> parent_data_sets, DataSetFlags flags);

        /// Create dataset with the specified dataSetName, imports,
        /// and flags in parentDataSet.
        ///
        /// The flags may be used, among other things, to specify
        /// that the created dataset will be NonTemporal even if the
        /// data source is itself temporal. This setting is typically
        /// used to prevent the accumulation of data where history is
        /// not needed.
        ///
        /// This method updates in-memory dataset cache to include
        /// the created dataset.
        TemporalId create_data_set(dot::String data_set_name, dot::List<TemporalId> parent_data_sets, DataSetFlags flags, TemporalId save_to);

        /// Save new version of the dataset.
        ///
        /// This overload of the save_data_set method does not
        /// specify the save_to parameter explicitly and instead
        /// uses context.data_set for its value.
        ///
        /// This method sets id field of the argument to be the
        /// new TemporalId assigned to the record when it is saved.
        /// The timestamp of the new TemporalId is the current time.
        ///
        /// This method updates in-memory cache to the saved dataset.
        void save_data_set(DataSet value);

        /// Save new version of the dataset.
        ///
        /// This overload of the save_data_set method specifies
        /// the save_to parameter explicitly.
        ///
        /// This method sets id field of the argument to be the
        /// new TemporalId assigned to the record when it is saved.
        /// The timestamp of the new TemporalId is the current time.
        ///
        /// This method updates in-memory cache to the saved dataset.
        void save_data_set(DataSet value, TemporalId save_to);

    public: // NON TEMPLATE METHODS

        /// Load record by its TemporalId and Type.
        ///
        /// Return null if not found.
        Record load_or_null(TemporalId id, dot::Type data_type);

        Record load(TemporalId id, dot::Type data_type);

        Record load(Key key, TemporalId load_from);

        Record load_or_null(Key key, TemporalId load_from);

        TemporalMongoQuery get_query(TemporalId load_from, dot::Type data_type);
    };
}

#include <dc/types/record/typed_key_impl.hpp>
