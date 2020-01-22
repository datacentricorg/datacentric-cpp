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


namespace dc
{

    template <typename TKey, typename TRecord>
    dot::ptr<TRecord> key_impl<TKey, TRecord>::Load(context_base context)
    {
        auto result = load_or_null(context, context->data_set);
        if (result == nullptr) throw dot::exception(
                    dot::string::format("Record with key {0} is not found.", this->to_string()));
        return result;
    }

    template <typename TKey, typename TRecord>
    dot::ptr<TRecord> key_impl<TKey, TRecord>::Load(context_base context, dot::object_id dataSet)
    {
        // This method will return null if the record is
        // not found or the found record is a delete marker
        dot::ptr<TRecord> result = load_or_null(context, dataSet);

        // Error message if null, otherwise return
        if (result == nullptr) throw dot::exception(dot::string::format(
                    "Record with key {0} is not found in dataset with dot::object_id={1}.", this->to_string(), dataSet.to_string()));

        return result;
    }



    template <typename TKey, typename TRecord>
    dot::ptr<TRecord> key_impl<TKey, TRecord>::load_or_null(context_base context)
    {
        return load_or_null(context, context->data_set);
    }

    template <typename TKey, typename TRecord>
    dot::ptr<TRecord> key_impl<TKey, TRecord>::load_or_null(context_base context, dot::object_id loadFrom)
    {
        // First check if the record has been
        // cached for the same dataset as the
        // argument to this method. Note that
        // the dataset of the record may not
        // be the same as the dataset where
        // the record is looked up.
        if (cached_record_ != nullptr && cached_record_->data_set == loadFrom)
        {
            // If cached for the argument dataset, return the cached
            // value unless it is the delete marker, in which case
            // return null
            if (cached_record_->record == nullptr) return nullptr;
            else return (dot::ptr<TRecord>)(record_base)(cached_record_->record);
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
                            "for Key={1} has a non-matching Key={2}.", result->get_type()->name, getValue(), result->get_key()));

                // Record not found or is a delete marker,
                // cache an empty record and return null
                cached_record_ = make_cached_record(loadFrom);
                return nullptr;
            }
            else
            {
                // Check that the found record has a matching key
                if (getValue() != result->get_key())
                    throw dot::exception(dot::string::format(
                            "Record with Type={0} stored "
                            "for Key={1} has a non-matching Key={2}.", result->get_type()->name, getValue(), result->get_key()));

                // Cache the record; the ctor of CachedRecord
                // will cache null if the record is a delete marker
                cached_record_ = make_cached_record(loadFrom, result);

                // Return the result after caching it inside the key
                return result;
            }
        }
    }

    template <typename TKey, typename TRecord>
    void key_impl<TKey, TRecord>::Delete(context_base context)
    {
        // Delete in the dataset of the context
        context->data_source->delete_record(this, context->data_set);
    }

    template <typename TKey, typename TRecord>
    void key_impl<TKey, TRecord>::Delete(context_base context, dot::object_id deleteIn)
    {
        context->data_source->delete_record(this, deleteIn);
    }

    template <typename TKey, typename TRecord>
    bool key_impl<TKey, TRecord>::HasCachedRecord()
    {
        return cached_record_ != nullptr;
    }

    template <typename TKey, typename TRecord>
    void key_impl<TKey, TRecord>::SetCachedRecord(record<TKey, TRecord> record, dot::object_id dataSet)
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
        cached_record_ = make_cached_record(dataSet, record);
    }

    template <typename TKey, typename TRecord>
    void key_impl<TKey, TRecord>::ClearCachedRecord()
    {
        cached_record_ = nullptr;
    }

    template <typename TKey, typename TRecord>
    void key_impl<TKey, TRecord>::AssignKeyElements(record<TKey, TRecord> record)
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
                        "is not found in the root data type {2}.", keyElementInfo->name, dot::typeof<dot::ptr<TKey>>()->name, get_type()->name
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
}
