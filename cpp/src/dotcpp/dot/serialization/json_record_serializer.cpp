/*
Copyright (C) 2015-present The DotCpp Authors.

This file is part of .C++, a native C++ implementation of
popular .NET class library APIs developed to facilitate
code reuse between C# and C++.

    http://github.com/dotcpp/dotcpp (source)
    http://dotcpp.org (documentation)

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

#include <dot/precompiled.hpp>
#include <dot/implement.hpp>
#include <dot/serialization/json_record_serializer.hpp>
#include <dot/serialization/serialize_attribute.hpp>
#include <dot/serialization/json_writer.hpp>
#include <dot/serialization/data_writer.hpp>
#include <dot/serialization/tuple_writer.hpp>
#include <dot/noda_time/local_date_util.hpp>
#include <dot/noda_time/local_time_util.hpp>
#include <dot/noda_time/local_date_time_util.hpp>
#include <dot/system/string.hpp>
#include <dot/system/object.hpp>
#include <dot/system/type.hpp>
#include <dot/system/reflection/activator.hpp>
#include <dot/noda_time/local_date.hpp>
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_date_time.hpp>

namespace dot
{
    object json_record_serializer_impl::deserialize(const rapidjson::Document& doc)
    {
        // Create instance to which JSON will be deserialized
        dot::string type_name = doc["_t"].GetString();
        object result = dot::activator::create_instance("", type_name);
        tree_writer_base writer = make_data_writer(result);

        writer->write_start_document(type_name);
        deserialize_document(doc.GetObject(), writer);
        writer->write_end_document(type_name);
        return result;
    }

    dot::object json_record_serializer_impl::deserialize_tuple(rapidjson::Document::ConstObject doc, dot::list<dot::field_info> props, dot::type tuple_type)
    {
        // Create instance to which JSON will be deserialized
        dot::string type_name = tuple_type->name;
        dot::object result = dot::activator::create_instance(tuple_type);
        tree_writer_base writer = make_tuple_writer(result, props);

        writer->write_start_document(type_name);
        deserialize_document(doc, writer);
        writer->write_end_document(type_name);
        return result;

    }

    void json_record_serializer_impl::deserialize_document(rapidjson::Document::ConstObject doc, tree_writer_base writer)
    {
        // Each document is a dictionary at root level
        writer->write_start_dict();

        // Loop over elements until

        for (auto& elem : doc)
        {
            rapidjson::Type json_type = elem.value.GetType();

            // Read element name and value
            dot::string element_name = elem.name.GetString();
            if (json_type == rapidjson::Type::kNullType)
            {
                //reader.read_null();
            }
            else if (json_type == rapidjson::Type::kStringType)
            {
                dot::string value = elem.value.GetString();

                if (element_name == "_t")
                {
                    // TODO Handle type
                }
                else
                {
                    writer->write_value_element(element_name, value);
                }
            }
            else if (json_type == rapidjson::Type::kNumberType)
            {
                if (elem.value.IsDouble())
                {
                    double value = elem.value.GetDouble();
                    writer->write_value_element(element_name, value);
                }
                else if (elem.value.IsInt())
                {
                    int value = elem.value.GetInt();
                    writer->write_value_element(element_name, value);
                }
                else
                {
                    int64_t value = elem.value.GetInt64();
                    writer->write_value_element(element_name, value);
                }
            }
            else if (json_type == rapidjson::Type::kFalseType || json_type == rapidjson::Type::kTrueType)
            {
                bool value = elem.value.GetBool();
                writer->write_value_element(element_name, value);
            }
            else if (json_type == rapidjson::Type::kObjectType)
            {
                // Read JSON stream for the embedded data element
                //byte_buffer_base document_buffer = reader.read_raw_bson_document();
                rapidjson::Document::ConstObject sub_doc = elem.value.GetObject();

                // Deserialize embedded data element

                writer->write_start_element(element_name);
                deserialize_document(sub_doc, writer);
                writer->write_end_element(element_name);
            }
            else if (json_type == rapidjson::Type::kArrayType)
            {
                // Array is accessed as a document JSON type inside array JSON,
                // type, where document element name is serialized array index.
                // Deserialization of sparse arrays is currently not supported.
                rapidjson::Document::ConstArray sub_doc = elem.value.GetArray();

                // We can finally deserialize array here
                // This method checks that array is not sparse
                writer->write_start_array_element(element_name);
                deserialize_array(sub_doc, writer);
                writer->write_end_array_element(element_name);
            }
            else throw dot::exception(
                "Deserialization of JSON type {0} is not supported.");
        }

        // Each document is a dictionary at root level
        writer->write_end_dict();
    }

    void json_record_serializer_impl::deserialize_array(rapidjson::Document::ConstArray arr, tree_writer_base writer)
    {
        // Loop over elements until
        for (auto& elem : arr)
        {
            rapidjson::Type json_type = elem.GetType();

            if (json_type == rapidjson::Type::kNullType)
            {
                // Unlike for dictionaries, in case of arrays we write null item values
                writer->write_value_array_item(nullptr);
            }
            else if (json_type == rapidjson::Type::kStringType)
            {
                dot::string value = elem.GetString();
                writer->write_value_array_item(value);
            }
            else if (json_type == rapidjson::Type::kNumberType)
            {
                if (elem.IsDouble())
                {
                    double value = elem.GetDouble();
                    writer->write_value_array_item(value);
                }
                else if (elem.IsInt())
                {
                    int value = elem.GetInt();
                    writer->write_value_array_item(value);
                }
                else
                {
                    int64_t value = elem.GetInt64();
                    writer->write_value_array_item(value);
                }
            }
            else if (json_type == rapidjson::Type::kFalseType || json_type == rapidjson::Type::kTrueType)
            {
                bool value = elem.GetBool();
                writer->write_value_array_item(value);
            }
            else if (json_type == rapidjson::Type::kObjectType)
            {
                // Read JSON stream for the embedded data element
                rapidjson::Document::ConstObject sub_doc = elem.GetObject();

                // Deserialize embedded data element
                writer->write_start_array_item();
                deserialize_document(sub_doc, writer);
                writer->write_end_array_item();
            }
            else if (json_type == rapidjson::Type::kArrayType)
            {
                throw dot::exception("Deserializaion of an array inside another array is not supported.");
            }
            else
                throw dot::exception(
                    "Deserialization of JSON type inside an array is not supported.");
        }
    }

    void json_record_serializer_impl::serialize(tree_writer_base writer, object value)
    {
        // Root name is written in JSON as _t element
        dot::string root_name = value->get_type()->full_name();

        writer->write_start_document(root_name);
        // Check for custom serializator
        if (value->get_type()->get_custom_attributes(dot::typeof<serialize_class_attribute>(), true)->size())
        {
            serialize_class_attribute(value->get_type()->get_custom_attributes(dot::typeof<serialize_class_attribute>(), true)[0])->serialize(writer, value);
        }
        else
        {
            standard_serialize(writer, value);
        }
        writer->write_end_document(root_name);
    }

    void json_record_serializer_impl::standard_serialize(dot::list_base obj, dot::string element_name, dot::tree_writer_base writer)
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
                || item_type->is_enum
                )
            {
                writer->write_start_value();
                writer->write_value(item);
                writer->write_end_value();
            }
            else if (!item_type->get_interface("list_base").is_empty()) // TODO - refactor after removing the interface
            {
                throw dot::exception(dot::string::format("Serialization is not supported for element {0} "
                    "which is collection containing another collection.", element_name));
            }
            else
            {
                standard_serialize(writer, item);
            }

            // Write array item end tag
            writer->write_end_array_item();
        }

        // Write matching end element tag
        writer->write_end_array_element(element_name);
    }

    void json_record_serializer_impl::standard_serialize(tree_writer_base writer, dot::object value)
    {
        // Write start tag
        writer->write_start_dict();

        writer->write_value_element("_t", value->get_type()->name);

        // Iterate over the list of elements
        dot::list<dot::field_info> inner_element_info_list = value->get_type()->get_fields();
        for (dot::field_info inner_element_info : inner_element_info_list)
        {
            // Get element name and value
            dot::string inner_element_name = inner_element_info->name;

            dot::object inner_element_value = inner_element_info->get_value(value);

            if (inner_element_value.is_empty())
            {
                continue;
            }

            dot::type element_type = inner_element_value->get_type();

            if (element_type->equals(dot::typeof<dot::string>())
                || element_type->equals(dot::typeof<double>())
                || element_type->equals(dot::typeof<bool>())
                || element_type->equals(dot::typeof<int>())
                || element_type->equals(dot::typeof<int64_t>())
                || element_type->equals(dot::typeof<dot::local_date>())
                || element_type->equals(dot::typeof<dot::local_date_time>())
                || element_type->equals(dot::typeof<dot::local_time>())
                || element_type->equals(dot::typeof<dot::local_minute>())
                || element_type->is_enum
                )
            {
                writer->write_value_element(inner_element_name, inner_element_value);
            }
            else
                if (!element_type->get_interface("list_base").is_empty()) // TODO - refactor after removing the interface
                {
                    standard_serialize((dot::list_base)inner_element_value, inner_element_name, writer);
                }
                else
                {
                    // Embedded as data
                    writer->write_start_element(inner_element_name);
                    standard_serialize(writer, inner_element_value);
                    writer->write_end_element(inner_element_name);
                }
        }

        // Write end tag
        writer->write_end_dict();
    }
}
