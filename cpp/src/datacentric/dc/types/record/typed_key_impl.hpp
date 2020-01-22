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
    void TypedKeyImpl<TKey, TRecord>::populate_from(TypedRecord<TKey, TRecord> record)
    {
        // The remaining code assigns elements of the record
        // to the matching elements of the key. This will also
        // make String representation of the key return the
        // proper value for the record.
        //
        // Get FieldInfo arrays for TKey and TRecord
        dot::List<dot::FieldInfo> key_element_info_array = dot::typeof<dot::Ptr<TKey>>()->get_fields();
        dot::List<dot::FieldInfo> record_element_info_array = dot::typeof<dot::Ptr<TRecord>>()->get_fields();

        // Check that TRecord has the same or greater number of elements
        // as TKey (all elements of TKey must also be present in TRecord)
        if (record_element_info_array->count() < key_element_info_array->count()) throw dot::Exception(dot::String::format(
                    "Record type {0} has fewer elements than key type {1}.", dot::typeof<dot::Ptr<TRecord>>()->name(), dot::typeof<dot::Ptr<TKey>>()->name()));

        // Iterate over the key properties
        for (dot::FieldInfo key_element_info : key_element_info_array)
        {
            auto record_element_info_iterator = std::find_if(record_element_info_array->begin(), record_element_info_array->end()
                    , [&key_element_info](dot::FieldInfo record_prop) -> bool
                                                          {
                                                              return key_element_info->name() == record_prop->name();
                                                          } );

            // Check that names match
            if (record_element_info_iterator == record_element_info_array->end()) throw dot::Exception(dot::String::format(
                        "Element {0} of key type {1} "
                        "is not found in the root data type {2}.", key_element_info->name(), dot::typeof<dot::Ptr<TKey>>()->name(), get_type()->name()
            ));

            dot::FieldInfo record_element_info = *record_element_info_iterator;

            // Check that types match
            if (key_element_info->field_type() != record_element_info->field_type())
                throw dot::Exception(dot::String::format(
                        "Property {0} of key type {1} "
                        "has type {2} "
                        "while property {3} of record type {4} "
                        "has type {5}."
                        , key_element_info->name()
                        , dot::typeof<dot::Ptr<TKey>>()->name()
                        , key_element_info->field_type()->name()
                        , record_element_info->name()
                        , dot::typeof<dot::Ptr<TRecord>>()->name()
                        , record_element_info->field_type()->name()
            ));

            // Read from the record and assign to the key
            dot::Object element_value = record_element_info->get_value(record);
            key_element_info->set_value(this, element_value);
        }
    }
}
