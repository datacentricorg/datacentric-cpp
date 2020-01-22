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
    void typed_key_impl<TKey, TRecord>::populate_from(typed_record<TKey, TRecord> record)
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
