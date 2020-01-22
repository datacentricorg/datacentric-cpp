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
#include <dc/serialization/json_record_serializer.hpp>
#include <dc/serialization/json_writer.hpp>
#include <dc/serialization/data_writer.hpp>
#include <dc/serialization/tuple_writer.hpp>
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

namespace dc
{
    data json_record_serializer_impl::Deserialize(const rapidjson::Document& doc)
    {
        // Create instance to which JSON will be deserialized
        dot::string typeName = doc["_t"].GetString();
        data result = (data)dot::activator::create_instance("", typeName);
        tree_writer_base writer = make_data_writer(result);

        writer->write_start_document(typeName);
        DeserializeDocument(doc.GetObject(), writer);
        writer->write_end_document(typeName);
        return result;
    }

    dot::object json_record_serializer_impl::DeserializeTuple(rapidjson::Document::ConstObject doc, dot::list<dot::field_info> props, dot::type tupleType)
    {
        // Create instance to which JSON will be deserialized
        dot::string typeName = tupleType->name;
        dot::object result = dot::activator::create_instance(tupleType);
        tree_writer_base writer = make_tuple_writer(result, props);

        writer->write_start_document(typeName);
        DeserializeDocument(doc, writer);
        writer->write_end_document(typeName);
        return result;

    }

    void json_record_serializer_impl::DeserializeDocument(rapidjson::Document::ConstObject doc, tree_writer_base writer)
    {
        // Each document is a dictionary at root level
        writer->write_start_dict();

        // Loop over elements until

        for (auto& elem : doc)
        {
            rapidjson::Type jsonType = elem.value.GetType();

            // Read element name and value
            dot::string elementName = elem.name.GetString();
            if (jsonType == rapidjson::Type::kNullType)
            {
                //reader.ReadNull();
            }
            else if (jsonType == rapidjson::Type::kStringType)
            {
                dot::string value = elem.value.GetString();

                if (elementName == "_t")
                {
                    // TODO Handle type
                }
                else
                {
                    writer->write_value_element(elementName, value);
                }
            }
            else if (jsonType == rapidjson::Type::kNumberType)
            {
                if (elem.value.IsDouble())
                {
                    double value = elem.value.GetDouble();
                    writer->write_value_element(elementName, value);
                }
                else if (elem.value.IsInt())
                {
                    int value = elem.value.GetInt();
                    writer->write_value_element(elementName, value);
                }
                else
                {
                    int64_t value = elem.value.GetInt64();
                    writer->write_value_element(elementName, value);
                }
            }
            else if (jsonType == rapidjson::Type::kFalseType || jsonType == rapidjson::Type::kTrueType)
            {
                bool value = elem.value.GetBool();
                writer->write_value_element(elementName, value);
            }
            else if (jsonType == rapidjson::Type::kObjectType)
            {
                // Read JSON stream for the embedded data element
                //IByteBuffer documentBuffer = reader.ReadRawBsonDocument();
                rapidjson::Document::ConstObject sub_doc = elem.value.GetObject();

                // Deserialize embedded data element

                writer->write_start_element(elementName);
                DeserializeDocument(sub_doc, writer);
                writer->write_end_element(elementName);
            }
            else if (jsonType == rapidjson::Type::kArrayType)
            {
                // Array is accessed as a document JSON type inside array JSON,
                // type, where document element name is serialized array index.
                // Deserialization of sparse arrays is currently not supported.
                rapidjson::Document::ConstArray sub_doc = elem.value.GetArray();

                // We can finally deserialize array here
                // This method checks that array is not sparse
                writer->write_start_array_element(elementName);
                DeserializeArray(sub_doc, writer);
                writer->write_end_array_element(elementName);
            }
            else throw dot::exception(
                "Deserialization of JSON type {0} is not supported.");
        }

        // Each document is a dictionary at root level
        writer->write_end_dict();
    }

    void json_record_serializer_impl::DeserializeArray(rapidjson::Document::ConstArray arr, tree_writer_base writer)
    {
        // Loop over elements until
        for (auto& elem : arr)
        {
            rapidjson::Type jsonType = elem.GetType();

            if (jsonType == rapidjson::Type::kNullType)
            {
                // Unlike for dictionaries, in case of arrays we write null item values
                writer->write_value_array_item(nullptr);
            }
            else if (jsonType == rapidjson::Type::kStringType)
            {
                dot::string value = elem.GetString();
                writer->write_value_array_item(value);
            }
            else if (jsonType == rapidjson::Type::kNumberType)
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
            else if (jsonType == rapidjson::Type::kFalseType || jsonType == rapidjson::Type::kTrueType)
            {
                bool value = elem.GetBool();
                writer->write_value_array_item(value);
            }
            else if (jsonType == rapidjson::Type::kObjectType)
            {
                // Read JSON stream for the embedded data element
                rapidjson::Document::ConstObject sub_doc = elem.GetObject();

                // Deserialize embedded data element
                writer->write_start_array_item();
                DeserializeDocument(sub_doc, writer);
                writer->write_end_array_item();
            }
            else if (jsonType == rapidjson::Type::kArrayType)
            {
                throw dot::exception("Deserializaion of an array inside another array is not supported.");
            }
            else
                throw dot::exception(
                    "Deserialization of JSON type inside an array is not supported.");
        }
    }

    void json_record_serializer_impl::Serialize(tree_writer_base writer, data value)
    {
        // Root name is written in JSON as _t element
        dot::string rootName = value->get_type()->full_name();

        writer->write_start_document(rootName);
        value->serialize_to(writer);
        writer->write_end_document(rootName);
    }

}
