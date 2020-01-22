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
#include <dc/serialization/tree_writer_base.hpp>
#include <dot/system/reflection/activator.hpp>
#include <dc/types/record/key_base.hpp>


namespace dc
{

    void serialize_to(dot::list_base obj, dot::string element_name, tree_writer_base writer)
    {
        // Write start element tag
        writer->WriteStartArrayElement(element_name);

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
            dot::type item_type = item->get_type();

            if (item_type->equals(dot::typeof<dot::string>())
                || item_type->equals(dot::typeof<double>())
                || item_type->equals(dot::typeof<bool>())
                || item_type->equals(dot::typeof<int>())
                || item_type->equals(dot::typeof<int64_t>())
                || item_type->equals(dot::typeof<dot::local_date>())
                || item_type->equals(dot::typeof<dot::local_date_time>())
                || item_type->equals(dot::typeof<dot::local_time>())
                || item_type->equals(dot::typeof<dot::local_minute>())
                || item_type->is_enum
                || item_type->equals(dot::typeof<dot::object_id>())
                )
            {
                writer->WriteStartValue();
                writer->WriteValue(item);
                writer->WriteEndValue();
            }
            else if (!item_type->get_interface("ListBase").is_empty()) // TODO - refactor after removing the interface
            {
                throw dot::exception(dot::string::format("Serialization is not supported for element {0} "
                    "which is collection containing another collection.", element_name));
            }
            else
            if (item.is<data>())
            {
                if (item_type->name->ends_with("Key"))
                {
                    // Embedded as string key
                    writer->WriteStartValue();
                    writer->WriteValue(item->to_string());
                    writer->WriteEndValue();
                }
                else
                {
                    ((data)item)->serialize_to(writer);
                }
            }
            else
            {
                throw dot::exception(dot::string::format(
                    "Element type {0} is not supported for tree serialization.", item_type->name));
            }


            // Write array item end tag
            writer->WriteEndArrayItem();
        }

        // Write matching end element tag
        writer->WriteEndArrayElement(element_name);
    }


    void data_impl::serialize_to(tree_writer_base writer)
    {
        // Write start tag
        writer->WriteStartDict();

        // Iterate over the list of elements
        dot::list<dot::field_info> inner_element_info_list = get_type()->get_fields();
        for (dot::field_info inner_element_info : inner_element_info_list)
        {
            // Get element name and value
            dot::string inner_element_name = inner_element_info->name;
            dot::object inner_element_value = inner_element_info->get_value(this);

            if (inner_element_value.is_empty())
            {
                continue;
            }

            dot::type element_type = inner_element_value->get_type();

            if (   element_type->equals(dot::typeof<dot::string>())
                || element_type->equals(dot::typeof<double>())
                || element_type->equals(dot::typeof<bool>())
                || element_type->equals(dot::typeof<int>())
                || element_type->equals(dot::typeof<int64_t>())
                || element_type->equals(dot::typeof<dot::local_date>())
                || element_type->equals(dot::typeof<dot::local_date_time>())
                || element_type->equals(dot::typeof<dot::local_time>())
                || element_type->equals(dot::typeof<dot::local_minute>())
                || element_type->is_enum
                || element_type->equals(dot::typeof<dot::object_id>())
                )
            {
                writer->WriteValueElement(inner_element_name, inner_element_value);
            }
            else
            if (!element_type->get_interface("ListBase").is_empty()) // TODO - refactor after removing the interface
            {
                dc::serialize_to((dot::list_base)inner_element_value, inner_element_name, writer);
            }
            else
            if (inner_element_value.is<data>())
            {
                if (inner_element_value->get_type()->name->ends_with("Key"))
                {
                    // Embedded as string key
                    writer->WriteValueElement(inner_element_name, inner_element_value->to_string());
                }
                else
                {
                    // Embedded as data
                    writer->WriteStartElement(inner_element_name);
                    ((data)inner_element_value)->serialize_to(writer);
                    writer->WriteEndElement(inner_element_name);
                }
            }
            else
            {
                throw dot::exception(dot::string::format("Element type {0} is not supported for tree serialization.", inner_element_info->field_type));
            }
        }

        // Write end tag
        writer->WriteEndDict();
    }

}
