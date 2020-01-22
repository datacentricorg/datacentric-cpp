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
    dot::ptr<TRecord> key_impl<TKey, TRecord>::load(context_base context)
    {
        auto result = load_or_null(context, context->data_set);
        if (result == nullptr) throw dot::exception(
                    dot::string::format("Record with key {0} is not found.", this->to_string()));
        return result;
    }

    template <typename TKey, typename TRecord>
    dot::ptr<TRecord> key_impl<TKey, TRecord>::load(context_base context, dot::object_id data_set)
    {
        // This method will return null if the record is
        // not found or the found record is a delete marker
        dot::ptr<TRecord> result = load_or_null(context, data_set);

        // Error message if null, otherwise return
        if (result == nullptr) throw dot::exception(dot::string::format(
                    "Record with key {0} is not found in dataset with dot::object_id={1}.", this->to_string(), data_set.to_string()));

        return result;
    }

    template <typename TKey, typename TRecord>
    dot::ptr<TRecord> key_impl<TKey, TRecord>::load_or_null(context_base context)
    {
        return load_or_null(context, context->data_set);
    }

    template <typename TKey, typename TRecord>
    dot::ptr<TRecord> key_impl<TKey, TRecord>::load_or_null(context_base context, dot::object_id load_from)
    {
        // First check if the record has been
        // cached for the same dataset as the
        // argument to this method. Note that
        // the dataset of the record may not
        // be the same as the dataset where
        // the record is looked up.
        if (cached_record_ != nullptr && cached_record_->data_set == load_from)
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
            clear_cached_record();

            // This method will return null if the record is
            // not found or the found record has deleted flag set
            dot::ptr<TRecord> result = (dot::ptr<TRecord>) context->reload_or_null(this, load_from);

            if (result == nullptr || result.template is<delete_marker>())
            {
                // If not null, it is a delete marker;
                // check that has a matching key
                if (result != nullptr && get_value() != result->get_key())
                    throw dot::exception(dot::string::format(
                            "Delete marker with type={0} stored "
                            "for key={1} has a non-matching key={2}.", result->get_type()->name(), get_value(), result->get_key()));

                // Record not found or is a delete marker,
                // cache an empty record and return null
                cached_record_ = make_cached_record(load_from);
                return nullptr;
            }
            else
            {
                // Check that the found record has a matching key
                if (get_value() != result->get_key())
                    throw dot::exception(dot::string::format(
                            "Record with type={0} stored "
                            "for key={1} has a non-matching key={2}.", result->get_type()->name(), get_value(), result->get_key()));

                // Cache the record; the ctor of cached_record
                // will cache null if the record is a delete marker
                cached_record_ = make_cached_record(load_from, result);

                // Return the result after caching it inside the key
                return result;
            }
        }
    }

    template <typename TKey, typename TRecord>
    void key_impl<TKey, TRecord>::delete_record(context_base context)
    {
        // Delete in the dataset of the context
        context->data_source->delete_record(this, context->data_set);
    }

    template <typename TKey, typename TRecord>
    void key_impl<TKey, TRecord>::delete_record(context_base context, dot::object_id delete_in)
    {
        context->data_source->delete_record(this, delete_in);
    }

    template <typename TKey, typename TRecord>
    bool key_impl<TKey, TRecord>::has_cached_record()
    {
        return cached_record_ != nullptr;
    }

    template <typename TKey, typename TRecord>
    void key_impl<TKey, TRecord>::set_cached_record(record<TKey, TRecord> record, dot::object_id data_set)
    {
        // Before doing anything else, clear the cached record
        // This will ensure that the previous cached copy is
        // no longer present even in case of an exception in the
        // following code, or if it does not set the new cached
        // record value.
        clear_cached_record();

        // Assign key elements to match the record
        assign_key_elements(record);

        // Cache self inside the key
        cached_record_ = make_cached_record(data_set, record);
    }

    template <typename TKey, typename TRecord>
    void key_impl<TKey, TRecord>::clear_cached_record()
    {
        cached_record_ = nullptr;
    }

    template <typename TKey, typename TRecord>
    void key_impl<TKey, TRecord>::assign_key_elements(record<TKey, TRecord> record)
    {
        // The remaining code assigns elements of the record
        // to the matching elements of the key. This will also
        // make string representation of the key return the
        // proper value for the record.
        //
        // Get field_info arrays for TKey and TRecord
        dot::list<dot::field_info> key_element_info_array = dot::typeof<dot::ptr<TKey>>()->get_fields();
        dot::list<dot::field_info> record_element_info_array = dot::typeof<dot::ptr<TRecord>>()->get_fields();

        // Check that TRecord has the same or greater number of elements
        // as TKey (all elements of TKey must also be present in TRecord)
        if (record_element_info_array->count() < key_element_info_array->count()) throw dot::exception(dot::string::format(
                    "Record type {0} has fewer elements than key type {1}.", dot::typeof<dot::ptr<TRecord>>()->name(), dot::typeof<dot::ptr<TKey>>()->name()));

        // Iterate over the key properties
        for (dot::field_info key_element_info : key_element_info_array)
        {
            auto record_element_info_iterator = std::find_if(record_element_info_array->begin(), record_element_info_array->end()
                    , [&key_element_info](dot::field_info record_prop) -> bool
                                                          {
                                                              return key_element_info->name() == record_prop->name();
                                                          } );

            // Check that names match
            if (record_element_info_iterator == record_element_info_array->end()) throw dot::exception(dot::string::format(
                        "Element {0} of key type {1} "
                        "is not found in the root data type {2}.", key_element_info->name(), dot::typeof<dot::ptr<TKey>>()->name(), get_type()->name()
            ));

            dot::field_info record_element_info = *record_element_info_iterator;

            // Check that types match
            if (key_element_info->field_type() != record_element_info->field_type())
                throw dot::exception(dot::string::format(
                        "Property {0} of key type {1} "
                        "has type {2} "
                        "while property {3} of record type {4} "
                        "has type {5}."
                        , key_element_info->name()
                        , dot::typeof<dot::ptr<TKey>>()->name()
                        , key_element_info->field_type()->name()
                        , record_element_info->name()
                        , dot::typeof<dot::ptr<TRecord>>()->name()
                        , record_element_info->field_type()->name()
            ));

            // Read from the record and assign to the key
            dot::object element_value = record_element_info->get_value(record);
            key_element_info->set_value(this, element_value);
        }
    }
}
