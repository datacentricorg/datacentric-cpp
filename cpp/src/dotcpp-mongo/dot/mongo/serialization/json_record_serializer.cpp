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

#include <dot/mongo/precompiled.hpp>
#include <dot/mongo/implement.hpp>
#include <dot/mongo/serialization/json_record_serializer.hpp>
#include <dot/mongo/serialization/json_writer.hpp>
#include <dot/mongo/serialization/data_writer.hpp>
#include <dot/mongo/serialization/tuple_writer.hpp>
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
        //value->serialize_to(writer);
        value->get_type()->get_method("serialize_to")->invoke(value, make_list<object>({ writer }));
        writer->write_end_document(root_name);
    }
}
