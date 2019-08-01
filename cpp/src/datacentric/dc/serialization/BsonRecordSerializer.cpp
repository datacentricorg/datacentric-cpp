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
#include <dot/system/Array1D.hpp>
#include <dot/system/String.hpp>
#include <dc/serialization/BsonWriter.hpp>
#include <dot/system/Object.hpp>
#include <dot/system/collections/IObjectEnumerable.hpp>
#include <dot/system/Type.hpp>
#include <dot/noda_time/LocalDate.hpp>
#include <dot/noda_time/LocalTime.hpp>
#include <dot/noda_time/LocalDateTime.hpp>
#include <dc/serialization/BsonRecordSerializer.hpp>
#include <dc/serialization/DataWriter.hpp>
#include <dc/serialization/TupleWriter.hpp>
#include <dot/system/reflection/Activator.hpp>
#include <dc/types/local_date_time/LocalDateTime.hpp>

namespace dc
{
    Data BsonRecordSerializerImpl::Deserialize(bsoncxx::document::view doc)
    {
        // Create instance to which BSON will be deserialized
        dot::string typeName = doc["_t"].get_utf8().value.to_string();
        Data result = (Data)dot::Activator::CreateInstance("", typeName);
        ITreeWriter writer = new_DataWriter(result);

        writer->WriteStartDocument(typeName);
        DeserializeDocument(doc, writer);
        writer->WriteEndDocument(typeName);
        return result;
    }

    dot::object BsonRecordSerializerImpl::DeserializeTuple(bsoncxx::document::view doc, dot::List<dot::PropertyInfo> props, dot::type_t tupleType)
    {
        // Create instance to which BSON will be deserialized
        dot::string typeName = tupleType->Name;
        dot::object result = dot::Activator::CreateInstance(tupleType);
        ITreeWriter writer = new_TupleWriter(result, props);

        writer->WriteStartDocument(typeName);
        DeserializeDocument(doc, writer);
        writer->WriteEndDocument(typeName);
        return result;

    }

    void BsonRecordSerializerImpl::DeserializeDocument(const bsoncxx::document::view & doc, ITreeWriter writer)
    {
        // Each document is a dictionary at root level
        writer->WriteStartDict();

        // Loop over elements until

        for (auto elem : doc)
        {
            bsoncxx::type bsonType = elem.type();

            // Read element name and value
            dot::string elementName = elem.key().to_string();
            if (bsonType == bsoncxx::type::k_null)
            {
                //reader.ReadNull();
            }
            else if (bsonType == bsoncxx::type::k_oid)
            {
                ObjectId value = elem.get_oid().value;
                writer->WriteValueElement(elementName, value);
            }
            else if (bsonType == bsoncxx::type::k_utf8)
            {
                dot::string value = elem.get_utf8().value.to_string();

                if (elementName == "_id")
                {
                    // TODO Handle key
                }
                else if (elementName == "_t")
                {
                    // TODO Handle type
                }
                else if (elementName == "_d")
                {
                    // TODO Handle dataset
                }
                else if (elementName == "_key")
                {
                    continue;
                }

                else
                {
                    writer->WriteValueElement(elementName, value);
                }
            }
            else if (bsonType == bsoncxx::type::k_double)
            {
                double value = elem.get_double();
                writer->WriteValueElement(elementName, value);
            }
            else if (bsonType == bsoncxx::type::k_bool)
            {
                bool value = elem.get_bool();
                writer->WriteValueElement(elementName, value);
            }
            else if (bsonType == bsoncxx::type::k_int32)
            {
                int value = elem.get_int32();
                writer->WriteValueElement(elementName, value);
            }
            else if (bsonType == bsoncxx::type::k_int64)
            {
                int64_t value = elem.get_int64();
                writer->WriteValueElement(elementName, value);
            }
            else if (bsonType == bsoncxx::type::k_date)
            {
                bsoncxx::types::b_date value = elem.get_date();
                writer->WriteValueElement(elementName, LocalDateTimeHelper::FromStdChrono(value.value));
            }
            else if (bsonType == bsoncxx::type::k_document)
            {
                // Read BSON stream for the embedded data element
                //IByteBuffer documentBuffer = reader.ReadRawBsonDocument();
                bsoncxx::document::view sub_doc = elem.get_document().view();

                // Deserialize embedded data element

                writer->WriteStartElement(elementName);
                DeserializeDocument(sub_doc, writer);
                writer->WriteEndElement(elementName);
            }
            else if (bsonType == bsoncxx::type::k_array)
            {
                // Array is accessed as a document BSON type inside array BSON,
                // type, where document element name is serialized array index.
                // Deserialization of sparse arrays is currently not supported.
                bsoncxx::array::view sub_doc = elem.get_array();

                // We can finally deserialize array here
                // This method checks that array is not sparse
                writer->WriteStartArrayElement(elementName);
                DeserializeArray(sub_doc, writer);
                writer->WriteEndArrayElement(elementName);
            }
            else throw dot::new_Exception(
                "Deserialization of BSON type {0} is not supported.");
        }

        // Each document is a dictionary at root level
        writer->WriteEndDict();
    }

    void BsonRecordSerializerImpl::DeserializeArray(const bsoncxx::array::view & arr, ITreeWriter writer)
    {
        // Loop over elements until
        for (auto elem : arr)
        {
            bsoncxx::type bsonType = elem.type();

            if (bsonType == bsoncxx::type::k_null)
            {
                // Unlike for dictionaries, in case of arrays we write null item values
                writer->WriteValueArrayItem(nullptr);
            }
            else if (bsonType == bsoncxx::type::k_utf8)
            {
                dot::string value = elem.get_utf8().value.to_string();
                writer->WriteValueArrayItem(value);
            }
            else if (bsonType == bsoncxx::type::k_double)
            {
                double value = elem.get_double();
                writer->WriteValueArrayItem(value);
            }
            else if (bsonType == bsoncxx::type::k_bool)
            {
                bool value = elem.get_bool();
                writer->WriteValueArrayItem(value);
            }
            else if (bsonType == bsoncxx::type::k_int32)
            {
                int value = elem.get_int32();
                writer->WriteValueArrayItem(value);
            }
            else if (bsonType == bsoncxx::type::k_int64)
            {
                int64_t value = elem.get_int64();
                writer->WriteValueArrayItem(value);
            }
            else if (bsonType == bsoncxx::type::k_oid)
            {
                ObjectId value = elem.get_oid().value;
                writer->WriteValueArrayItem(value);
            }
            else if (bsonType == bsoncxx::type::k_document)
            {
                // Read BSON stream for the embedded data element
                bsoncxx::document::view sub_doc = elem.get_document().view();

                // Deserialize embedded data element
                writer->WriteStartArrayItem();
                DeserializeDocument(sub_doc, writer);
                writer->WriteEndArrayItem();
            }
            else if (bsonType == bsoncxx::type::k_array)
            {
                throw dot::new_Exception("Deserializaion of an array inside another array is not supported.");
            }
            else
                throw dot::new_Exception(
                    "Deserialization of BSON type inside an array is not supported.");
        }
    }

    void BsonRecordSerializerImpl::Serialize(ITreeWriter writer, Data value)
    {
        // Root name is written in JSON as _t element
        dot::string rootName = value->GetType()->getFullName();

        writer->WriteStartDocument(rootName);
        value->SerializeTo(writer);
        writer->WriteEndDocument(rootName);
    }

}