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
#include <dot/system/collections/generic/list.hpp>
#include <dot/system/reflection/activator.hpp>
#include <dc/types/record/key_base.hpp>
#include <dc/platform/reflection/class_info.hpp>
#include <dc/platform/context/context_base.hpp>
#include <dc/types/record/record_base.hpp>
#include <dot/mongo/mongo_db/mongo/settings.hpp>

namespace dc
{
    void serialize_to(dot::list_base obj, dot::string element_name, dot::tree_writer_base writer)
    {
        // Write start element tag
        writer->write_start_array_element(element_name);

        int length = obj->get_length();

        // Iterate over sequence elements
        for (int i = 0; i < length; ++i)
        {
            dot::object item = obj->get_item(i);

            // Write array item start tag
            writer->write_start_array_item();

            if (item.is_empty())
            {
                writer->write_start_value();
                writer->write_value(item);
                writer->write_end_value();
                writer->write_end_array_item();
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
                || item_type->is_enum()
                || item_type->equals(dot::typeof<dot::object_id>())
                )
            {
                writer->write_start_value();
                writer->write_value(item);
                writer->write_end_value();
            }
            else if (dot::typeof<dot::list_base>()->is_assignable_from(item_type))
            {
                throw dot::exception(dot::string::format("Serialization is not supported for element {0} "
                    "which is collection containing another collection.", element_name));
            }
            else
            if (item.is<data>())
            {
                if (item_type->is_subclass_of(dot::typeof<key_base>()))
                {
                    // Embedded as string key
                    writer->write_start_value();
                    writer->write_value(item->to_string());
                    writer->write_end_value();
                }
                else
                {
                    ((data)item)->serialize_to(writer);
                }
            }
            else
            {
                throw dot::exception(dot::string::format(
                    "Element type {0} is not supported for tree serialization.", item_type->name()));
            }


            // Write array item end tag
            writer->write_end_array_item();
        }

        // Write matching end element tag
        writer->write_end_array_element(element_name);
    }


    void data_impl::serialize_to(dot::tree_writer_base writer)
    {
        // Write start tag
        writer->write_start_dict(class_info_impl::get_or_create(this->get_type())->raw_class_name);

        // Iterate over the list of elements
        dot::list<dot::field_info> inner_element_info_list = get_type()->get_fields();
        for (dot::field_info inner_element_info : inner_element_info_list)
        {
            // Get element name and value
            dot::string inner_element_name = inner_element_info->name();

            if (inner_element_name == "_t")
            {
                continue;
            }
            if (inner_element_name == "_key")
            {
                writer->write_value_element(inner_element_name, dynamic_cast<record_base_impl*>(this)->get_key());
                continue;
            }

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
                || element_type->is_enum()
                || element_type->equals(dot::typeof<dot::object_id>())
                )
            {
                writer->write_value_element(inner_element_name, inner_element_value);
            }
            else
            if (dot::typeof<dot::list_base>()->is_assignable_from(element_type))
            {
                dc::serialize_to((dot::list_base)inner_element_value, inner_element_name, writer);
            }
            else
            if (inner_element_value.is<data>())
            {
                if (inner_element_value->get_type()->is_subclass_of(dot::typeof<key_base>()))
                {
                    // Embedded as string key
                    writer->write_value_element(inner_element_name, inner_element_value->to_string());
                }
                else
                {
                    // Embedded as data
                    writer->write_start_element(inner_element_name);
                    ((data)inner_element_value)->serialize_to(writer);
                    writer->write_end_element(inner_element_name);
                }
            }
            else
            {
                throw dot::exception(dot::string::format("Element type {0} is not supported for tree serialization.", inner_element_info->field_type()));
            }
        }

        // Write end tag
        writer->write_end_dict(class_info_impl::get_or_create(this->get_type())->raw_class_name);
    }
}
