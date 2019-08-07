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
#include <dot/mongo/mongo_db/bson/object_id.hpp>
#include <dc/platform/dataset/data_set_data.hpp>
#include <dc/platform/data_source/data_source_data.hpp>

namespace dc
{
    class context_base_impl; using context_base = dot::ptr<context_base_impl>;
    class data_source_data_impl; using data_source_data = dot::ptr<data_source_data_impl>;
    class data_set_data_impl; using data_set_data = dot::ptr<data_set_data_impl>;

    /// Context defines dataset and provides access to data,
    /// logging, and other supporting functionality.
    class DC_CLASS context_base_impl : public virtual dot::object_impl
    {
        typedef context_base_impl self;

    public: // PROPERTIES

        /// Get the default data source of the context.
        data_source_data DataSource;

        /// Returns dot::object_id of the context dataset.
        dot::object_id data_set;

    public: // CONSTRUCTORS

    protected:

        /// Initialize data_source.
        void SetDataSource(data_source_data dataSource);

    public: // METHODS

        /// Load record by its dot::object_id.
        ///
        /// Return null if not found.
        template <class TRecord>
        TRecord load_or_null(dot::object_id id)
        {
            return (TRecord) load_or_null(id, ::dot::typeof<TRecord>());
        }

        /// Load record by its dot::object_id and Type.
        ///
        /// Return null if not found.
        record_base load_or_null(dot::object_id id, dot::type_t dataType);

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
        record_base reload_or_null(key_base key, dot::object_id loadFrom);

        /// Save record to the specified dataset. After the method exits,
        /// record.data_set will be set to the value of the dataSet parameter.
        ///
        /// All Save methods ignore the value of record.data_set before the
        /// Save method is called. When dataset is not specified explicitly,
        /// the value of dataset from the context, not from the record, is used.
        /// The reason for this behavior is that the record may be stored from
        /// a different dataset than the one where it is used.
        ///
        /// This method guarantees that dot::object_ids will be in strictly increasing
        /// order for this instance of the data source class always, and across
        /// all processes and machine if they are not created within the same
        /// second.
        void save(record_base record);

        /// Save record to the specified dataset. After the method exits,
        /// record.data_set will be set to the value of the dataSet parameter.
        ///
        /// All Save methods ignore the value of record.data_set before the
        /// Save method is called. When dataset is not specified explicitly,
        /// the value of dataset from the context, not from the record, is used.
        /// The reason for this behavior is that the record may be stored from
        /// a different dataset than the one where it is used.
        ///
        /// This method guarantees that dot::object_ids will be in strictly increasing
        /// order for this instance of the data source class always, and across
        /// all processes and machine if they are not created within the same
        /// second.
        void save(record_base record, dot::object_id saveTo);

        /// Write a delete marker for the dataset of the context and the specified
        /// key instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        void Delete(key_base key);

        /// Write a delete marker in deleteIn dataset for the specified key
        /// instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        void Delete(key_base key, dot::object_id deleteIn);

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

        /// Return dot::object_id of the latest common dataset.
        ///
        /// common dataset is always stored in root dataset.
        dot::object_id get_common();

        /// Return dot::object_id for the latest dataset record with
        /// matching dataSetID string from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// This overload of the get_data_set method does not
        /// specify the loadFrom parameter explicitly and instead
        /// uses context.data_set for its value.
        ///
        /// Error message if not found.
        ///
        /// This method will return the value from in-memory
        /// cache even if it is no longer the latest version
        /// in the data store and will only load it from storage
        /// if not found in cache. Use load_or_null method to
        /// force reloading the dataset from storage.
        dot::object_id get_data_set(dot::string dataSetID);

        /// Return dot::object_id for the latest dataset record with
        /// matching dataSetID string from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// This overload of the get_data_set method specifies
        /// the loadFrom parameter explicitly.
        ///
        /// Error message if not found.
        ///
        /// This method will return the value from in-memory
        /// cache even if it is no longer the latest version
        /// in the data store and will only load it from storage
        /// if not found in cache. Use load_or_null method to
        /// force reloading the dataset from storage.
        dot::object_id get_data_set(dot::string dataSetID, dot::object_id loadFrom);

        /// Return dot::object_id for the latest dataset record with
        /// matching dataSetID string from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// This overload of the get_data_set_or_empty method does not
        /// specify the loadFrom parameter explicitly and instead
        /// uses context.data_set for its value.
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
        dot::object_id get_data_set_or_empty(dot::string dataSetID);

        /// Return dot::object_id for the latest dataset record with
        /// matching dataSetID string from in-memory cache. Try
        /// loading from storage only if not found in cache.
        ///
        /// This overload of the get_data_set_or_empty method specifies
        /// the loadFrom parameter explicitly.
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
        dot::object_id get_data_set_or_empty(dot::string dataSetID, dot::object_id loadFrom);

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

        /// Create new version of the dataset with the specified dataSetID
        /// and no parent datasets.
        ///
        /// This overload of the create_data_set method does not
        /// specify the saveTo parameter explicitly and instead
        /// uses context.data_set for its value.
        ///
        /// This method updates in-memory cache to the saved dataset.
        dot::object_id create_data_set(dot::string dataSetID);

        /// Create new version of the dataset with the specified dataSetID
        /// and no parent datasets.
        ///
        /// This overload of the create_data_set method specifies
        /// the saveTo parameter explicitly.
        ///
        /// This method updates in-memory cache to the saved dataset.
        dot::object_id create_data_set(dot::string dataSetID, dot::object_id saveTo);

        /// Create new version of the dataset with the specified dataSetID
        /// and parent dataset dot::object_ids passed as an array, and return
        /// the new dot::object_id assigned to the saved dataset.
        ///
        /// This overload of the create_data_set method does not
        /// specify the saveTo parameter explicitly and instead
        /// uses context.data_set for its value.
        ///
        /// This method updates in-memory cache to the saved dataset.
        dot::object_id create_data_set(dot::string dataSetID, dot::list<dot::object_id> parentdata_sets);

        /// Create new version of the dataset with the specified dataSetID
        /// and parent dataset dot::object_ids passed as an array, and return
        /// the new dot::object_id assigned to the saved dataset.
        ///
        /// This overload of the create_data_set method specifies
        /// the saveTo parameter explicitly.
        ///
        /// This method updates in-memory cache to the saved dataset.
        dot::object_id create_data_set(dot::string dataSetID, dot::list<dot::object_id> parentdata_sets, dot::object_id saveTo);

        /// Save new version of the dataset.
        ///
        /// This overload of the save_data_set method does not
        /// specify the saveTo parameter explicitly and instead
        /// uses context.data_set for its value.
        ///
        /// This method sets ID field of the argument to be the
        /// new dot::object_id assigned to the record when it is saved.
        /// The timestamp of the new dot::object_id is the current time.
        ///
        /// This method updates in-memory cache to the saved dataset.
        void save_data_set(data_set_data data_set_data);

        /// Save new version of the dataset.
        ///
        /// This overload of the save_data_set method specifies
        /// the saveTo parameter explicitly.
        ///
        /// This method sets ID field of the argument to be the
        /// new dot::object_id assigned to the record when it is saved.
        /// The timestamp of the new dot::object_id is the current time.
        ///
        /// This method updates in-memory cache to the saved dataset.
        void save_data_set(data_set_data data_set_data, dot::object_id saveTo);
    };
}
