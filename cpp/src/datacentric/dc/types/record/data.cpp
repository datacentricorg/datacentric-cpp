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

#include <dc/precompiled.hpp>
#include <dc/implement.hpp>
#include <dc/types/record/data.hpp>
#include <dc/serialization/ITreeWriter.hpp>
#include <dot/system/reflection/activator.hpp>
#include <dc/types/record/key_base.hpp>


namespace dc
{

    void SerializeTo(dot::list_base obj, dot::string elementName, ITreeWriter writer)
    {
        // Write start element tag
        writer->WriteStartArrayElement(elementName);

        int length = obj->get_length();

        // Iterate over sequence elements
        for (int i = 0; i < length; ++i)
        {
            dot::object item = obj->get_item(i);

            // Write array item start tag
            writer->WriteStartArrayItem();

            if (item.is_empty())
            {
                writer->WriteStartValue();
                writer->WriteValue(item);
                writer->WriteEndValue();
                writer->WriteEndArrayItem();
                continue;
            }

            // Serialize based on type of the item
            dot::type_t itemType = item->type();

            if (itemType->equals(dot::typeof<dot::string>())
                || itemType->equals(dot::typeof<double>())
                || itemType->equals(dot::typeof<bool>())
                || itemType->equals(dot::typeof<int>())
                || itemType->equals(dot::typeof<int64_t>())
                || itemType->equals(dot::typeof<dot::local_date>())
                || itemType->equals(dot::typeof<dot::local_date_time>())
                || itemType->equals(dot::typeof<dot::local_time>())
                || itemType->equals(dot::typeof<dot::local_minute>())
                || itemType->is_enum
                || itemType->equals(dot::typeof<dot::object_id>())
                )
            {
                writer->WriteStartValue();
                writer->WriteValue(item);
                writer->WriteEndValue();
            }
            else if (!itemType->get_interface("ListBase").is_empty()) // TODO - refactor after removing the interface
            {
                throw dot::exception(dot::string::format("Serialization is not supported for element {0} "
                    "which is collection containing another collection.", elementName));
            }
            else
            if (item.is<data>())
            {
                if (itemType->name->ends_with("Key"))
                {
                    // Embedded as string key
                    writer->WriteStartValue();
                    writer->WriteValue(item->to_string());
                    writer->WriteEndValue();
                }
                else
                {
                    ((data)item)->SerializeTo(writer);
                }
            }
            else
            {
                throw dot::exception(dot::string::format(
                    "Element type {0} is not supported for tree serialization.", itemType->name));
            }


            // Write array item end tag
            writer->WriteEndArrayItem();
        }

        // Write matching end element tag
        writer->WriteEndArrayElement(elementName);
    }


    void data_impl::SerializeTo(ITreeWriter writer)
    {
        // Write start tag
        writer->WriteStartDict();

        // Iterate over the list of elements
        dot::list<dot::field_info> innerElementInfoList = type()->get_fields();
        for (dot::field_info innerElementInfo : innerElementInfoList)
        {
            // Get element name and value
            dot::string innerElementName = innerElementInfo->name;
            dot::object innerElementValue = innerElementInfo->get_value(this);

            if (innerElementValue.is_empty())
            {
                continue;
            }

            dot::type_t elementType = innerElementValue->type();

            if (   elementType->equals(dot::typeof<dot::string>())
                || elementType->equals(dot::typeof<double>())
                || elementType->equals(dot::typeof<bool>())
                || elementType->equals(dot::typeof<int>())
                || elementType->equals(dot::typeof<int64_t>())
                || elementType->equals(dot::typeof<dot::local_date>())
                || elementType->equals(dot::typeof<dot::local_date_time>())
                || elementType->equals(dot::typeof<dot::local_time>())
                || elementType->equals(dot::typeof<dot::local_minute>())
                || elementType->is_enum
                || elementType->equals(dot::typeof<dot::object_id>())
                )
            {
                writer->WriteValueElement(innerElementName, innerElementValue);
            }
            else
            if (!elementType->get_interface("ListBase").is_empty()) // TODO - refactor after removing the interface
            {
                dc::SerializeTo((dot::list_base)innerElementValue, innerElementName, writer);
            }
            else
            if (innerElementValue.is<data>())
            {
                if (innerElementValue->type()->name->ends_with("Key"))
                {
                    // Embedded as string key
                    writer->WriteValueElement(innerElementName, innerElementValue->to_string());
                }
                else
                {
                    // Embedded as data
                    writer->WriteStartElement(innerElementName);
                    ((data)innerElementValue)->SerializeTo(writer);
                    writer->WriteEndElement(innerElementName);
                }
            }
            else
            {
                throw dot::exception(dot::string::format("Element type {0} is not supported for tree serialization.", innerElementInfo->field_type));
            }
        }

        // Write end tag
        writer->WriteEndDict();
    }

}
