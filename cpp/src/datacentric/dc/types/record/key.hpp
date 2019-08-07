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
#include <dc/types/record/CachedRecord.hpp>

namespace dc
{
    template <typename TKey, typename TRecord> class key_impl;
    template <typename TKey, typename TRecord> using key = dot::ptr<key_impl<TKey, TRecord>>;

    class CachedRecordImpl; using CachedRecord = dot::ptr<CachedRecordImpl>;
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
        CachedRecord cachedRecord_;

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
        dot::ptr<TRecord> Load(context_base context)
        {
            auto result = load_or_null(context, context->data_set);
            if (result == nullptr) throw dot::exception(
                dot::string::format("Record with key {0} is not found.", this->to_string()));
            return result;
        }

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
        dot::ptr<TRecord> Load(context_base context, dot::object_id dataSet)
        {
            // This method will return null if the record is
            // not found or the found record is a delete marker
            dot::ptr<TRecord> result = load_or_null(context, dataSet);

            // Error message if null, otherwise return
            if (result == nullptr) throw dot::exception(dot::string::format(
                "Record with key {0} is not found in dataset with dot::object_id={1}.", this->to_string(), dataSet.to_string()));

            return result;
        }



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
        dot::ptr<TRecord> load_or_null(context_base context)
        {
            return load_or_null(context, context->data_set);
        }

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
        dot::ptr<TRecord> load_or_null(context_base context, dot::object_id loadFrom)
        {
            // First check if the record has been
            // cached for the same dataset as the
            // argument to this method. Note that
            // the dataset of the record may not
            // be the same as the dataset where
            // the record is looked up.
            if (cachedRecord_ != nullptr && cachedRecord_->data_set == loadFrom)
            {
                // If cached for the argument dataset, return the cached
                // value unless it is the delete marker, in which case
                // return null
                if (cachedRecord_->Record == nullptr) return nullptr;
                else return (dot::ptr<TRecord>)(record_base)(cachedRecord_->Record);
            }
            else
            {
                // Before doing anything else, clear the cached record
                // This will ensure that the previous cached copy is
                // no longer present even in case of an exception in the
                // following code, or if it does not set the new cached
                // record value.
                ClearCachedRecord();

                // This method will return null if the record is
                // not found or the found record has Deleted flag set
                dot::ptr<TRecord> result = (dot::ptr<TRecord>) context->reload_or_null(this, loadFrom);

                if (result == nullptr || result.template is<DeleteMarker>())
                {
                    // If not null, it is a delete marker;
                    // check that has a matching key
                    if (result != nullptr && getValue() != result->get_key())
                        throw dot::exception(dot::string::format(
                            "Delete marker with Type={0} stored "
                            "for Key={1} has a non-matching Key={2}.", result->type()->name, getValue(), result->get_key()));

                    // Record not found or is a delete marker,
                    // cache an empty record and return null
                    cachedRecord_ = make_CachedRecord(loadFrom);
                    return nullptr;
                }
                else
                {
                    // Check that the found record has a matching key
                    if (getValue() != result->get_key())
                        throw dot::exception(dot::string::format(
                            "Record with Type={0} stored "
                            "for Key={1} has a non-matching Key={2}.", result->type()->name, getValue(), result->get_key()));

                    // Cache the record; the ctor of CachedRecord
                    // will cache null if the record is a delete marker
                    cachedRecord_ = make_CachedRecord(loadFrom, result);

                    // Return the result after caching it inside the key
                    return result;
                }
            }
        }

        /// Write a delete marker for the dataset of the context and the specified
        /// key instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        void Delete(context_base context)
        {
            // Delete in the dataset of the context
            context->DataSource->delete_record(this, context->data_set);
        }

        /// Write a delete marker in deleteIn dataset for the specified key
        /// instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        void Delete(context_base context, dot::object_id deleteIn)
        {
            context->DataSource->delete_record(this, deleteIn);
        }

        /// Return true if the key holds a cached record,
        /// irrespective of whether or not that cached
        /// record is null.
        bool HasCachedRecord()
        {
            return cachedRecord_ != nullptr;
        }

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
        void SetCachedRecord(record<TKey, TRecord> record, dot::object_id dataSet)
        {
            // Before doing anything else, clear the cached record
            // This will ensure that the previous cached copy is
            // no longer present even in case of an exception in the
            // following code, or if it does not set the new cached
            // record value.
            ClearCachedRecord();

            // Assign key elements to match the record
            AssignKeyElements(record);

            // Cache self inside the key
            cachedRecord_ = make_CachedRecord(dataSet, record);
        }

        /// Clear the previously cached record so that a
        /// new value can be loaded from storage or set using
        /// SetCachedRecord(record).
        void ClearCachedRecord()
        {
            cachedRecord_ = nullptr;
        }

        /// Assign key elements from record to key.
        void AssignKeyElements(record<TKey, TRecord> record)
        {
            // The remaining code assigns elements of the record
            // to the matching elements of the key. This will also
            // make string representation of the key return the
            // proper value for the record.
            //
            // Get field_info arrays for TKey and TRecord
            dot::list<dot::field_info> keyElementInfoArray = dot::typeof<dot::ptr<TKey>>()->get_fields();
            dot::list<dot::field_info> recordElementInfoArray = dot::typeof<dot::ptr<TRecord>>()->get_fields();

            // Check that TRecord has the same or greater number of elements
            // as TKey (all elements of TKey must also be present in TRecord)
            if (recordElementInfoArray->count() < keyElementInfoArray->count()) throw dot::exception(dot::string::format(
                "Record type {0} has fewer elements than key type {1}.", dot::typeof<dot::ptr<TRecord>>()->name, dot::typeof<dot::ptr<TKey>>()->name));

            // Iterate over the key properties
            for (dot::field_info keyElementInfo : keyElementInfoArray)
            {
                auto recordElementInfoIterator = std::find_if(recordElementInfoArray->begin(), recordElementInfoArray->end()
                    , [&keyElementInfo](dot::field_info recordProp) -> bool
                    {
                        return keyElementInfo->name == recordProp->name;
                    } );

                // Check that names match
                if (recordElementInfoIterator == recordElementInfoArray->end()) throw dot::exception(dot::string::format(
                    "Element {0} of key type {1} "
                    "is not found in the root data type {2}.", keyElementInfo->name, dot::typeof<dot::ptr<TKey>>()->name, type()->name
                ));

                dot::field_info recordElementInfo = *recordElementInfoIterator;

                // Check that types match
                if (keyElementInfo->field_type != recordElementInfo->field_type)
                    throw dot::exception(dot::string::format(
                        "Property {0} of key type {1} "
                        "has type {2} "
                        "while property {3} of record type {4} "
                        "has type {5}."
                        , keyElementInfo->name
                        , dot::typeof<dot::ptr<TKey>>()->name
                        , keyElementInfo->field_type->name
                        , recordElementInfo->name
                        , dot::typeof<dot::ptr<TRecord>>()->name
                        , recordElementInfo->field_type->name
                        ));

                // Read from the record and assign to the key
                dot::object elementValue = recordElementInfo->get_value(record);
                keyElementInfo->set_value(this, elementValue);
            }
        }

        DOT_TYPE_BEGIN("dc", "Key")
            DOT_TYPE_BASE(key_base)
            DOT_TYPE_GENERIC_ARGUMENT(dot::ptr<TKey>)
            DOT_TYPE_GENERIC_ARGUMENT(dot::ptr<TRecord>)
        DOT_TYPE_END()
    };
}
