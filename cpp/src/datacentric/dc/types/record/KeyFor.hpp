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
#include <dc/types/record/RecordFor.hpp>
#include <dot/system/Object.hpp>
#include <dc/types/record/KeyType.hpp>
#include <dc/types/record/CachedRecord.hpp>
//#include <dc/platform/data_source/mongo/ObjectId.hpp>

namespace dc
{
    template <typename TKey, typename TRecord> class KeyForImpl;
    template <typename TKey, typename TRecord> using KeyFor = dot::Ptr<KeyForImpl<TKey, TRecord>>;

    class CachedRecordImpl; using CachedRecord = dot::Ptr<CachedRecordImpl>;
    class IContextImpl; using IContext = dot::Ptr<IContextImpl>;

    /// <summary>
    /// Record derived from KeyType rather than KeyType is recorded without a dataset.
    /// </summary>
    template <typename TKey, typename TRecord>
    class KeyForImpl : public virtual KeyTypeImpl
    {
        typedef KeyForImpl<TKey, TRecord> self;

    private: // PROPERTIES

        /// <summary>
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
        /// </summary>
        CachedRecord cachedRecord_;

    public: // METHODS

        /// <summary>
        /// Load record from context.DataSource. The lookup occurs in
        /// context.DataSet and its parents, expanded to arbitrary
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
        /// </summary>
        dot::Ptr<TRecord> Load(IContext context)
        {
            auto result = LoadOrNull(context, context->DataSet);
            if (result == nullptr) throw dot::new_Exception(
                dot::String::Format("Record with key {0} is not found.", this->ToString()));
            return result;
        }

        /// <summary>
        /// Load record from context.DataSource, overriding the dataset
        /// specified in the context with the value specified as the
        /// second parameter. The lookup occurs in the specified dataset
        /// and its parents, expanded to arbitrary depth with repetitions
        /// and cyclic references removed.
        ///
        /// ATTENTION - this method ignores context.DataSet
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
        /// </summary>
        dot::Ptr<TRecord> Load(IContext context, ObjectId dataSet)
        {
            // This method will return null if the record is
            // not found or the found record is a delete marker
            dot::Ptr<TRecord> result = LoadOrNull(context, dataSet);

            // Error message if null, otherwise return
            if (result == nullptr) throw dot::new_Exception(dot::String::Format(
                "Record with key {0} is not found in dataset with ObjectId={1}.", this->ToString(), dataSet.ToString()));

            return result;
        }



        /// <summary>
        /// Load record from context.DataSource. The lookup occurs in
        /// context.DataSet and its parents, expanded to arbitrary
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
        /// </summary>
        dot::Ptr<TRecord> LoadOrNull(IContext context)
        {
            return LoadOrNull(context, context->DataSet);
        }

        /// <summary>
        /// Load record from context.DataSource, overriding the dataset
        /// specified in the context with the value specified as the
        /// second parameter. The lookup occurs in the specified dataset
        /// and its parents, expanded to arbitrary depth with repetitions
        /// and cyclic references removed.
        ///
        /// ATTENTION - this method ignores context.DataSet
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
        /// </summary>
        dot::Ptr<TRecord> LoadOrNull(IContext context, ObjectId loadFrom)
        {
            // First check if the record has been
            // cached for the same dataset as the
            // argument to this method. Note that
            // the dataset of the record may not
            // be the same as the dataset where
            // the record is looked up.
            if (cachedRecord_ != nullptr && cachedRecord_->DataSet == loadFrom)
            {
                // If cached for the argument dataset, return the cached
                // value unless it is the delete marker, in which case
                // return null
                if (cachedRecord_->Record == nullptr) return nullptr;
                else return (dot::Ptr<TRecord>)(RecordType)(cachedRecord_->Record);
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
                dot::Ptr<TRecord> result = (dot::Ptr<TRecord>) context->ReloadOrNull(this, loadFrom);

                if (result == nullptr || result.template is<DeleteMarker>())
                {
                    // If not null, it is a delete marker;
                    // check that has a matching key
                    if (result != nullptr && Value != result->getKey())
                        throw dot::new_Exception(dot::String::Format(
                            "Delete marker with Type={0} stored "
                            "for Key={1} has a non-matching Key={2}.", result->GetType()->Name, Value, result->getKey()));

                    // Record not found or is a delete marker,
                    // cache an empty record and return null
                    cachedRecord_ = new_CachedRecord(loadFrom);
                    return nullptr;
                }
                else
                {
                    // Check that the found record has a matching key
                    if (Value != result->getKey())
                        throw dot::new_Exception(dot::String::Format(
                            "Record with Type={0} stored "
                            "for Key={1} has a non-matching Key={2}.", result->GetType()->Name, Value, result->getKey()));

                    // Cache the record; the ctor of CachedRecord
                    // will cache null if the record is a delete marker
                    cachedRecord_ = new_CachedRecord(loadFrom, result);

                    // Return the result after caching it inside the key
                    return result;
                }
            }
        }

        /// <summary>
        /// Write a delete marker for the dataset of the context and the specified
        /// key instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        /// </summary>
        void Delete(IContext context)
        {
            // Delete in the dataset of the context
            context->DataSource->Delete(this, context->DataSet);
        }

        /// <summary>
        /// Write a delete marker in deleteIn dataset for the specified key
        /// instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        /// </summary>
        void Delete(IContext context, ObjectId deleteIn)
        {
            context->DataSource->Delete(this, deleteIn);
        }

        /// <summary>
        /// Return true if the key holds a cached record,
        /// irrespective of whether or not that cached
        /// record is null.
        /// </summary>
        bool HasCachedRecord()
        {
            return cachedRecord_ != nullptr;
        }

        /// <summary>
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
        /// </summary>
        void SetCachedRecord(RecordFor<TKey, TRecord> record, ObjectId dataSet)
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
            cachedRecord_ = new_CachedRecord(dataSet, record);
        }

        /// <summary>
        /// Clear the previously cached record so that a
        /// new value can be loaded from storage or set using
        /// SetCachedRecord(record).
        /// </summary>
        void ClearCachedRecord()
        {
            cachedRecord_ = nullptr;
        }

        /// <summary>Assign key elements from record to key.</summary>
        void AssignKeyElements(RecordFor<TKey, TRecord> record)
        {
            // The remaining code assigns elements of the record
            // to the matching elements of the key. This will also
            // make string representation of the key return the
            // proper value for the record.
            //
            // Get PropertyInfo arrays for TKey and TRecord
            dot::Array1D<dot::PropertyInfo> keyElementInfoArray = dot::typeof<dot::Ptr<TKey>>()->GetProperties();
            dot::Array1D<dot::PropertyInfo> recordElementInfoArray = dot::typeof<dot::Ptr<TRecord>>()->GetProperties();

            // Check that TRecord has the same or greater number of elements
            // as TKey (all elements of TKey must also be present in TRecord)
            if (recordElementInfoArray->Count < keyElementInfoArray->Count) throw dot::new_Exception(dot::String::Format(
                "Record type {0} has fewer elements than key type {1}.", dot::typeof<dot::Ptr<TRecord>>()->Name, dot::typeof<dot::Ptr<TKey>>()->Name));

            // Iterate over the key properties
            for (dot::PropertyInfo keyElementInfo : keyElementInfoArray)
            {
                auto recordElementInfoIterator = std::find_if(recordElementInfoArray->begin(), recordElementInfoArray->end()
                    , [&keyElementInfo](dot::PropertyInfo recordProp) -> bool
                    {
                        return keyElementInfo->Name == recordProp->Name;
                    } );

                // Check that names match
                if (recordElementInfoIterator == recordElementInfoArray->end()) throw dot::new_Exception(dot::String::Format(
                    "Element {0} of key type {1} "
                    "is not found in the root data type {2}.", keyElementInfo->Name, dot::typeof<dot::Ptr<TKey>>()->Name, GetType()->Name
                ));

                dot::PropertyInfo recordElementInfo = *recordElementInfoIterator;

                // Check that types match
                if (keyElementInfo->PropertyType != recordElementInfo->PropertyType)
                    throw dot::new_Exception(dot::String::Format(
                        "Property {0} of key type {1} "
                        "has type {2} "
                        "while property {3} of record type {4} "
                        "has type {5}."
                        , keyElementInfo->Name
                        , dot::typeof<dot::Ptr<TKey>>()->Name
                        , keyElementInfo->PropertyType->Name
                        , recordElementInfo->Name
                        , dot::typeof<dot::Ptr<TRecord>>()->Name
                        , recordElementInfo->PropertyType->Name
                        ));

                // Read from the record and assign to the key
                dot::Object elementValue = recordElementInfo->GetValue(record);
                keyElementInfo->SetValue(this, elementValue);
            }
        }

        DOT_TYPE_BEGIN(".Runtime.Main", "KeyFor")
            DOT_TYPE_BASE(KeyType)
            DOT_TYPE_GENERIC_ARGUMENT(dot::Ptr<TKey>)
            DOT_TYPE_GENERIC_ARGUMENT(dot::Ptr<TRecord>)
        DOT_TYPE_END()

    };
}

