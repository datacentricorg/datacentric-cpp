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

#include <dc/implement.hpp>
#include <dc/serialization/JsonRecordSerializer.hpp>
#include <dc/serialization/JsonWriter.hpp>
#include <dc/serialization/DataWriter.hpp>
#include <dc/serialization/TupleWriter.hpp>
#include <dc/types/local_date/LocalDate.hpp>
#include <dc/types/local_time/LocalTime.hpp>
#include <dc/types/local_date_time/LocalDateTime.hpp>
#include <dot/system/Array1D.hpp>
#include <dot/system/String.hpp>
#include <dot/system/Object.hpp>
#include <dot/system/collections/IObjectEnumerable.hpp>
#include <dot/system/Type.hpp>
#include <dot/system/reflection/Activator.hpp>
#include <dot/noda_time/LocalDate.hpp>
#include <dot/noda_time/LocalTime.hpp>
#include <dot/noda_time/LocalDateTime.hpp>

namespace dc
{
    Data JsonRecordSerializerImpl::Deserialize(const rapidjson::Document& doc)
    {
        // Create instance to which JSON will be deserialized
        dot::string typeName = doc["_t"].GetString();
        Data result = (Data)dot::Activator::CreateInstance("", typeName);
        ITreeWriter writer = new_DataWriter(result);

        writer->WriteStartDocument(typeName);
        DeserializeDocument(doc.GetObject(), writer);
        writer->WriteEndDocument(typeName);
        return result;
    }

    dot::object JsonRecordSerializerImpl::DeserializeTuple(rapidjson::Document::ConstObject doc, dot::List<dot::field_info> props, dot::type_t tupleType)
    {
        // Create instance to which JSON will be deserialized
        dot::string typeName = tupleType->Name;
        dot::object result = dot::Activator::CreateInstance(tupleType);
        ITreeWriter writer = new_TupleWriter(result, props);

        writer->WriteStartDocument(typeName);
        DeserializeDocument(doc, writer);
        writer->WriteEndDocument(typeName);
        return result;

    }

    void JsonRecordSerializerImpl::DeserializeDocument(rapidjson::Document::ConstObject doc, ITreeWriter writer)
    {
        // Each document is a dictionary at root level
        writer->WriteStartDict();

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
                    writer->WriteValueElement(elementName, value);
                }
            }
            else if (jsonType == rapidjson::Type::kNumberType)
            {
                if (elem.value.IsDouble())
                {
                    double value = elem.value.GetDouble();
                    writer->WriteValueElement(elementName, value);
                }
                else if (elem.value.IsInt())
                {
                    int value = elem.value.GetInt();
                    writer->WriteValueElement(elementName, value);
                }
                else
                {
                    int64_t value = elem.value.GetInt64();
                    writer->WriteValueElement(elementName, value);
                }
            }
            else if (jsonType == rapidjson::Type::kFalseType || jsonType == rapidjson::Type::kTrueType)
            {
                bool value = elem.value.GetBool();
                writer->WriteValueElement(elementName, value);
            }
            else if (jsonType == rapidjson::Type::kObjectType)
            {
                // Read JSON stream for the embedded data element
                //IByteBuffer documentBuffer = reader.ReadRawBsonDocument();
                rapidjson::Document::ConstObject sub_doc = elem.value.GetObject();

                // Deserialize embedded data element

                writer->WriteStartElement(elementName);
                DeserializeDocument(sub_doc, writer);
                writer->WriteEndElement(elementName);
            }
            else if (jsonType == rapidjson::Type::kArrayType)
            {
                // Array is accessed as a document JSON type inside array JSON,
                // type, where document element name is serialized array index.
                // Deserialization of sparse arrays is currently not supported.
                rapidjson::Document::ConstArray sub_doc = elem.value.GetArray();

                // We can finally deserialize array here
                // This method checks that array is not sparse
                writer->WriteStartArrayElement(elementName);
                DeserializeArray(sub_doc, writer);
                writer->WriteEndArrayElement(elementName);
            }
            else throw dot::new_Exception(
                "Deserialization of JSON type {0} is not supported.");
        }

        // Each document is a dictionary at root level
        writer->WriteEndDict();
    }

    void JsonRecordSerializerImpl::DeserializeArray(rapidjson::Document::ConstArray arr, ITreeWriter writer)
    {
        // Loop over elements until
        for (auto& elem : arr)
        {
            rapidjson::Type jsonType = elem.GetType();

            if (jsonType == rapidjson::Type::kNullType)
            {
                // Unlike for dictionaries, in case of arrays we write null item values
                writer->WriteValueArrayItem(nullptr);
            }
            else if (jsonType == rapidjson::Type::kStringType)
            {
                dot::string value = elem.GetString();
                writer->WriteValueArrayItem(value);
            }
            else if (jsonType == rapidjson::Type::kNumberType)
            {
                if (elem.IsDouble())
                {
                    double value = elem.GetDouble();
                    writer->WriteValueArrayItem(value);
                }
                else if (elem.IsInt())
                {
                    int value = elem.GetInt();
                    writer->WriteValueArrayItem(value);
                }
                else
                {
                    int64_t value = elem.GetInt64();
                    writer->WriteValueArrayItem(value);
                }
            }
            else if (jsonType == rapidjson::Type::kFalseType || jsonType == rapidjson::Type::kTrueType)
            {
                bool value = elem.GetBool();
                writer->WriteValueArrayItem(value);
            }
            else if (jsonType == rapidjson::Type::kObjectType)
            {
                // Read JSON stream for the embedded data element
                rapidjson::Document::ConstObject sub_doc = elem.GetObject();

                // Deserialize embedded data element
                writer->WriteStartArrayItem();
                DeserializeDocument(sub_doc, writer);
                writer->WriteEndArrayItem();
            }
            else if (jsonType == rapidjson::Type::kArrayType)
            {
                throw dot::new_Exception("Deserializaion of an array inside another array is not supported.");
            }
            else
                throw dot::new_Exception(
                    "Deserialization of JSON type inside an array is not supported.");
        }
    }

    void JsonRecordSerializerImpl::Serialize(ITreeWriter writer, Data value)
    {
        // Root name is written in JSON as _t element
        dot::string rootName = value->GetType()->getFullName();

        writer->WriteStartDocument(rootName);
        value->SerializeTo(writer);
        writer->WriteEndDocument(rootName);
    }

}
