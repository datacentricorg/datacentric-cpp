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
#include <dc/platform/data_source/mongo/mongo_data_source_base_data.hpp>

namespace dc
{
    class mongo_data_source_data_impl; using mongo_data_source_data = dot::ptr<mongo_data_source_data_impl>;

    /// Data source implementation for MongoDB.
    class DC_CLASS mongo_data_source_data_impl : public mongo_data_source_base_data_impl
    {
        typedef mongo_data_source_data_impl self;

    public: // METHODS

        /// Load record by its dot::object_id and type.
        ///
        /// Return null if there is no record for the specified dot::object_id;
        /// however an exception will be thrown if the record exists but
        /// is not derived from TRecord.
        virtual record_base load_or_null(dot::object_id id, dot::type data_type) override;

        /// This method does not use cached value inside the key
        /// and always retrieves a new record from storage. To get
        /// the record cached inside the key instead (if present), use
        /// the caching variant of this method:
        ///
        /// load_or_null(key, load_from)
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
        virtual record_base reload_or_null(key_base key, dot::object_id load_from) override;

        /// Save record to the specified dataset. After the method exits,
        /// record.data_set will be set to the value of the data_set parameter.
        ///
        /// This method guarantees that dot::object_ids will be in strictly increasing
        /// order for this instance of the data source class always, and across
        /// all processes and machine if they are not created within the same
        /// second.
        virtual void save(record_base record, dot::object_id save_to) override;

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
        virtual mongo_query get_query(dot::object_id data_set, dot::type type) override;

        /// Write a delete marker for the specified data_set and data_key
        /// instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        virtual void delete_record(key_base key, dot::object_id delete_in) override;
    };
}
