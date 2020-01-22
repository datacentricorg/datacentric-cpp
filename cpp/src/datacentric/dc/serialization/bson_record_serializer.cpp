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
#include <dot/system/string.hpp>
#include <dc/serialization/bson_writer.hpp>
#include <dot/system/object.hpp>
#include <dot/system/type.hpp>
#include <dot/noda_time/local_date.hpp>
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_date_time.hpp>
#include <dc/serialization/bson_record_serializer.hpp>
#include <dc/serialization/data_writer.hpp>
#include <dc/serialization/tuple_writer.hpp>
#include <dot/system/reflection/activator.hpp>
#include <dot/noda_time/local_date_time_util.hpp>

namespace dc
{
    data bson_record_serializer_impl::deserialize(bsoncxx::document::view doc)
    {
        // Create instance to which BSON will be deserialized
        dot::string typeName = doc["_t"].get_utf8().value.to_string();
        data result = (data)dot::activator::create_instance("", typeName);
        tree_writer_base writer = make_data_writer(result);

        writer->write_start_document(typeName);
        deserialize_document(doc, writer);
        writer->write_end_document(typeName);
        return result;
    }

    dot::object bson_record_serializer_impl::deserialize_tuple(bsoncxx::document::view doc, dot::list<dot::field_info> props, dot::type tupleType)
    {
        // Create instance to which BSON will be deserialized
        dot::string typeName = tupleType->name;
        dot::object result = dot::activator::create_instance(tupleType);
        tree_writer_base writer = make_tuple_writer(result, props);

        writer->write_start_document(typeName);
        deserialize_document(doc, writer);
        writer->write_end_document(typeName);
        return result;

    }

    void bson_record_serializer_impl::deserialize_document(const bsoncxx::document::view & doc, tree_writer_base writer)
    {
        // Each document is a dictionary at root level
        writer->write_start_dict();

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
                dot::object_id value = elem.get_oid().value;
                writer->write_value_element(elementName, value);
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
                    writer->write_value_element(elementName, value);
                }
            }
            else if (bsonType == bsoncxx::type::k_double)
            {
                double value = elem.get_double();
                writer->write_value_element(elementName, value);
            }
            else if (bsonType == bsoncxx::type::k_bool)
            {
                bool value = elem.get_bool();
                writer->write_value_element(elementName, value);
            }
            else if (bsonType == bsoncxx::type::k_int32)
            {
                int value = elem.get_int32();
                writer->write_value_element(elementName, value);
            }
            else if (bsonType == bsoncxx::type::k_int64)
            {
                int64_t value = elem.get_int64();
                writer->write_value_element(elementName, value);
            }
            else if (bsonType == bsoncxx::type::k_date)
            {
                bsoncxx::types::b_date value = elem.get_date();
                writer->write_value_element(elementName, dot::local_date_time_util::from_std_chrono(value.value));
            }
            else if (bsonType == bsoncxx::type::k_document)
            {
                // Read BSON stream for the embedded data element
                //IByteBuffer documentBuffer = reader.ReadRawBsonDocument();
                bsoncxx::document::view sub_doc = elem.get_document().view();

                // Deserialize embedded data element

                writer->write_start_element(elementName);
                deserialize_document(sub_doc, writer);
                writer->write_end_element(elementName);
            }
            else if (bsonType == bsoncxx::type::k_array)
            {
                // Array is accessed as a document BSON type inside array BSON,
                // type, where document element name is serialized array index.
                // Deserialization of sparse arrays is currently not supported.
                bsoncxx::array::view sub_doc = elem.get_array();

                // We can finally deserialize array here
                // This method checks that array is not sparse
                writer->write_start_array_element(elementName);
                deserialize_array(sub_doc, writer);
                writer->write_end_array_element(elementName);
            }
            else throw dot::exception(
                "Deserialization of BSON type {0} is not supported.");
        }

        // Each document is a dictionary at root level
        writer->write_end_dict();
    }

    void bson_record_serializer_impl::deserialize_array(const bsoncxx::array::view & arr, tree_writer_base writer)
    {
        // Loop over elements until
        for (auto elem : arr)
        {
            bsoncxx::type bsonType = elem.type();

            if (bsonType == bsoncxx::type::k_null)
            {
                // Unlike for dictionaries, in case of arrays we write null item values
                writer->write_value_array_item(nullptr);
            }
            else if (bsonType == bsoncxx::type::k_utf8)
            {
                dot::string value = elem.get_utf8().value.to_string();
                writer->write_value_array_item(value);
            }
            else if (bsonType == bsoncxx::type::k_double)
            {
                double value = elem.get_double();
                writer->write_value_array_item(value);
            }
            else if (bsonType == bsoncxx::type::k_bool)
            {
                bool value = elem.get_bool();
                writer->write_value_array_item(value);
            }
            else if (bsonType == bsoncxx::type::k_int32)
            {
                int value = elem.get_int32();
                writer->write_value_array_item(value);
            }
            else if (bsonType == bsoncxx::type::k_int64)
            {
                int64_t value = elem.get_int64();
                writer->write_value_array_item(value);
            }
            else if (bsonType == bsoncxx::type::k_oid)
            {
                dot::object_id value = elem.get_oid().value;
                writer->write_value_array_item(value);
            }
            else if (bsonType == bsoncxx::type::k_document)
            {
                // Read BSON stream for the embedded data element
                bsoncxx::document::view sub_doc = elem.get_document().view();

                // Deserialize embedded data element
                writer->write_start_array_item();
                deserialize_document(sub_doc, writer);
                writer->write_end_array_item();
            }
            else if (bsonType == bsoncxx::type::k_array)
            {
                throw dot::exception("Deserializaion of an array inside another array is not supported.");
            }
            else
                throw dot::exception(
                    "Deserialization of BSON type inside an array is not supported.");
        }
    }

    void bson_record_serializer_impl::serialize(tree_writer_base writer, data value)
    {
        // Root name is written in JSON as _t element
        dot::string rootName = value->get_type()->full_name();

        writer->write_start_document(rootName);
        value->serialize_to(writer);
        writer->write_end_document(rootName);
    }

}