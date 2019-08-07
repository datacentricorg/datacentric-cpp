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
#include <dc/types/record/DeleteMarker.hpp>
#include <dot/system/object_impl.hpp>

namespace dc
{
    class CachedRecordImpl; using CachedRecord = dot::ptr<CachedRecordImpl>;
    class dot::object_id;
    class record_base_impl; using record_base = dot::ptr<record_base_impl>;

    /// Reference to a cached record inside the key.
    ///
    /// The dataset is stored in a separate variable, not inside
    /// the record variable. This is to avoid the change in
    /// dataset value when it changes for the record.
    ///
    /// This reference is used in two cases:
    ///
    /// First, to avoid getting the record from storage multiple times.
    /// The first value loaded from storage will be cached in Record
    /// and returned on all subsequent calls for the same dataset
    /// without storage lookup.
    ///
    /// Second, to avoid accessing storage when two objects are
    /// created in memory, one having a property that is a key
    /// to the other. Use SetCachedRecord(record) method to assign
    /// an in-memory object to a key which will also set values
    /// of the elements of the key to the corresponding values
    /// of the record.
    class DC_CLASS CachedRecordImpl : public dot::object_impl
    {
        typedef CachedRecordImpl self;

        friend CachedRecord make_CachedRecord(dot::object_id, record_base);

    public:

        /// Dataset for which the record is cached.
        dot::object_id data_set;

        /// Record passed to the constructor, or null for an
        /// empty cached record or a delete marker.
        record_base Record;

    private:
        /// Cache dataset and record.
        ///
        /// Delete marker will be cached as null.
        CachedRecordImpl(dot::object_id dataSet, record_base record = nullptr)
        {
            // Dataset for which the record is cached
            data_set = dataSet;

            if (!record.is_empty() && !record.is<DeleteMarker>())
            {
                // Cache only if not a delete marker,
                // otherwise Record will remain null
                // after the constructor exits
                Record = record;
            }
        }

    };

    inline CachedRecord make_CachedRecord(dot::object_id dataSet, record_base record = nullptr)
    {
        return new CachedRecordImpl(dataSet, record);
    }
}