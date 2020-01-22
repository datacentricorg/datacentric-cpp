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
#include <dc/types/record/record.hpp>
#include <dot/system/object.hpp>
#include <dc/types/record/key_base.hpp>
#include <dc/types/record/cached_record.hpp>

namespace dc
{
    template <typename TKey, typename TRecord> class key_impl;
    template <typename TKey, typename TRecord> using key = dot::ptr<key_impl<TKey, TRecord>>;

    class cached_record_impl; using cached_record = dot::ptr<cached_record_impl>;
    class context_base_impl; using context_base = dot::ptr<context_base_impl>;

    /// Keys must derive from this type
    template <typename TKey, typename TRecord>
    class key_impl : public virtual key_base_impl
    {
        typedef key_impl<TKey, TRecord> self;

    private: // PROPERTIES

        /// Cached record is used in two situations.
        ///
        /// First, to avoid getting the record from storage multiple times.
        /// The first value loaded from storage will be cached in Record
        /// and returned on all subsequent calls without storage lookup.
        ///
        /// Second, to avoid accessing storage when two objects are
        /// created in memory, one having a property that is a key
        /// to the other. Use AssignRecord(record) method to assign
        /// an in-memory object to a key which will also set values
        /// of the elements of the key to the corresponding values
        /// of the record.
        cached_record cached_record_;

    public: // METHODS

        /// Load record from context.DataSource. The lookup occurs in
        /// context.data_set and its parents, expanded to arbitrary
        /// depth with repetitions and cyclic references removed.
        ///
        /// If Record property is set, its value is returned without
        /// performing lookup in the data store; otherwise the record
        /// is loaded from storage and cached in Record and the
        /// cached value is returned from subsequent calls.
        ///
        /// Once the record has been cached, the same version will be
        /// returned in subsequent calls with the same key instance.
        /// Create a new key or call ClearRecord() method to force
        /// reloading new version of the record from storage.
        ///
        /// Error message if the record is not found or is a delete marker.
        dot::ptr<TRecord> Load(context_base context);

        /// Load record from context.DataSource, overriding the dataset
        /// specified in the context with the value specified as the
        /// second parameter. The lookup occurs in the specified dataset
        /// and its parents, expanded to arbitrary depth with repetitions
        /// and cyclic references removed.
        ///
        /// ATTENTION - this method ignores context.data_set
        /// because the second parameter dataSet overrides it.
        ///
        /// If Record property is set, its value is returned without
        /// performing lookup in the data store; otherwise the record
        /// is loaded from storage and cached in Record and the
        /// cached value is returned from subsequent calls.
        ///
        /// Once the record has been cached, the same version will be
        /// returned in subsequent calls with the same key instance.
        /// Create a new key or call ClearRecord() method to force
        /// reloading new version of the record from storage.
        ///
        /// Error message if the record is not found or is a delete marker.
        dot::ptr<TRecord> Load(context_base context, dot::object_id dataSet);



        /// Load record from context.DataSource. The lookup occurs in
        /// context.data_set and its parents, expanded to arbitrary
        /// depth with repetitions and cyclic references removed.
        ///
        /// If Record property is set, its value is returned without
        /// performing lookup in the data store; otherwise the record
        /// is loaded from storage and cached in Record and the
        /// cached value is returned from subsequent calls.
        ///
        /// Once the record has been cached, the same version will be
        /// returned in subsequent calls with the same key instance.
        /// Create a new key or call ClearRecord() method to force
        /// reloading new version of the record from storage.
        ///
        /// Return null if the record is not found or is a delete marker.
        dot::ptr<TRecord> load_or_null(context_base context);

        /// Load record from context.DataSource, overriding the dataset
        /// specified in the context with the value specified as the
        /// second parameter. The lookup occurs in the specified dataset
        /// and its parents, expanded to arbitrary depth with repetitions
        /// and cyclic references removed.
        ///
        /// ATTENTION - this method ignores context.data_set
        /// because the second parameter dataSet overrides it.
        ///
        /// If Record property is set, its value is returned without
        /// performing lookup in the data store; otherwise the record
        /// is loaded from storage and cached in Record and the
        /// cached value is returned from subsequent calls.
        ///
        /// Once the record has been cached, the same version will be
        /// returned in subsequent calls with the same key instance.
        /// Create a new key or call ClearRecord() method to force
        /// reloading new version of the record from storage.
        ///
        /// Return null if the record is not found or is a delete marker.
        dot::ptr<TRecord> load_or_null(context_base context, dot::object_id loadFrom);

        /// Write a delete marker for the dataset of the context and the specified
        /// key instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        void Delete(context_base context);

        /// Write a delete marker in deleteIn dataset for the specified key
        /// instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        void Delete(context_base context, dot::object_id deleteIn);

        /// Return true if the key holds a cached record,
        /// irrespective of whether or not that cached
        /// record is null.
        bool HasCachedRecord();

        /// Use SetCachedRecord(record, dataSet) method to cache a
        /// reference to the record inside the key.
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
        ///
        /// The parameter dataSet is separate from record because
        /// the record may be saved in a dataset that is different
        /// from the dataset for which it is looked up, e.g. it could
        /// be a parent dataset. The cached reference is stored with
        /// the dataset where the object has been looked up, not the
        /// one where it is stored.
        void SetCachedRecord(record<TKey, TRecord> record, dot::object_id dataSet);

        /// Clear the previously cached record so that a
        /// new value can be loaded from storage or set using
        /// SetCachedRecord(record).
        void ClearCachedRecord();

        /// Assign key elements from record to key.
        void AssignKeyElements(record<TKey, TRecord> record);

        DOT_TYPE_BEGIN("dc", "Key")
            DOT_TYPE_BASE(key_base)
            DOT_TYPE_GENERIC_ARGUMENT(dot::ptr<TKey>)
            DOT_TYPE_GENERIC_ARGUMENT(dot::ptr<TRecord>)
        DOT_TYPE_END()
    };

}
