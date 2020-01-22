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
#include <dot/system/string.hpp>
#include <dot/mongo/serialization/bson_writer.hpp>
#include <dot/system/object.hpp>
#include <dot/system/type.hpp>
#include <dot/noda_time/local_date.hpp>
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_date_time.hpp>
#include <dot/mongo/serialization/bson_record_serializer.hpp>
#include <dot/serialization/data_writer.hpp>
#include <dot/serialization/tuple_writer.hpp>
#include <dot/system/reflection/activator.hpp>
#include <dot/noda_time/local_date_time_util.hpp>
#include <dot/mongo/mongo_db/bson/object_id.hpp>

namespace dot
{
    dot::object bson_record_serializer_impl::deserialize(bsoncxx::document::view doc)
    {
        // Create instance to which BSON will be deserialized
        dot::string type_name = doc["_t"].get_utf8().value.to_string();
        object result = dot::activator::create_instance("", type_name);
        tree_writer_base writer = make_data_writer(result);

        writer->write_start_document(type_name);
        deserialize_document(doc, writer);
        writer->write_end_document(type_name);
        return result;
    }

    dot::object bson_record_serializer_impl::deserialize_tuple(bsoncxx::document::view doc, dot::list<dot::field_info> props, dot::type tuple_type)
    {
        // Create instance to which BSON will be deserialized
        dot::string type_name = tuple_type->name;
        dot::object result = dot::activator::create_instance(tuple_type);
        tree_writer_base writer = make_tuple_writer(result, props);

        writer->write_start_document(type_name);
        deserialize_document(doc, writer);
        writer->write_end_document(type_name);
        return result;

    }

    void bson_record_serializer_impl::deserialize_document(const bsoncxx::document::view & doc, tree_writer_base writer)
    {
        // Each document is a dictionary at root level
        writer->write_start_dict();

        // Loop over elements until

        for (auto elem : doc)
        {
            bsoncxx::type bson_type = elem.type();

            // Read element name and value
            dot::string element_name = elem.key().to_string();
            if (bson_type == bsoncxx::type::k_null)
            {
                //reader.read_null();
            }
            else if (bson_type == bsoncxx::type::k_oid)
            {
                dot::object_id value = elem.get_oid().value;
                writer->write_value_element(element_name, value);
            }
            else if (bson_type == bsoncxx::type::k_utf8)
            {
                dot::string value = elem.get_utf8().value.to_string();

                if (element_name == "_id")
                {
                    // TODO Handle key
                }
                else if (element_name == "_t")
                {
                    // TODO Handle type
                }
                else if (element_name == "_d")
                {
                    // TODO Handle dataset
                }
                else if (element_name == "_key")
                {
                    continue;
                }
                else
                {
                    writer->write_value_element(element_name, value);
                }
            }
            else if (bson_type == bsoncxx::type::k_double)
            {
                double value = elem.get_double();
                writer->write_value_element(element_name, value);
            }
            else if (bson_type == bsoncxx::type::k_bool)
            {
                bool value = elem.get_bool();
                writer->write_value_element(element_name, value);
            }
            else if (bson_type == bsoncxx::type::k_int32)
            {
                int value = elem.get_int32();
                writer->write_value_element(element_name, value);
            }
            else if (bson_type == bsoncxx::type::k_int64)
            {
                int64_t value = elem.get_int64();
                writer->write_value_element(element_name, value);
            }
            else if (bson_type == bsoncxx::type::k_date)
            {
                bsoncxx::types::b_date value = elem.get_date();
                writer->write_value_element(element_name, dot::local_date_time_util::from_std_chrono(value.value));
            }
            else if (bson_type == bsoncxx::type::k_document)
            {
                // Read BSON stream for the embedded data element
                // byte_buffer_base document_buffer = reader.read_raw_bson_document();
                bsoncxx::document::view sub_doc = elem.get_document().view();

                // Deserialize embedded data element

                writer->write_start_element(element_name);
                deserialize_document(sub_doc, writer);
                writer->write_end_element(element_name);
            }
            else if (bson_type == bsoncxx::type::k_array)
            {
                // Array is accessed as a document BSON type inside array BSON,
                // type, where document element name is serialized array index.
                // Deserialization of sparse arrays is currently not supported.
                bsoncxx::array::view sub_doc = elem.get_array();

                // We can finally deserialize array here
                // This method checks that array is not sparse
                writer->write_start_array_element(element_name);
                deserialize_array(sub_doc, writer);
                writer->write_end_array_element(element_name);
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
            bsoncxx::type bson_type = elem.type();

            if (bson_type == bsoncxx::type::k_null)
            {
                // Unlike for dictionaries, in case of arrays we write null item values
                writer->write_value_array_item(nullptr);
            }
            else if (bson_type == bsoncxx::type::k_utf8)
            {
                dot::string value = elem.get_utf8().value.to_string();
                writer->write_value_array_item(value);
            }
            else if (bson_type == bsoncxx::type::k_double)
            {
                double value = elem.get_double();
                writer->write_value_array_item(value);
            }
            else if (bson_type == bsoncxx::type::k_bool)
            {
                bool value = elem.get_bool();
                writer->write_value_array_item(value);
            }
            else if (bson_type == bsoncxx::type::k_int32)
            {
                int value = elem.get_int32();
                writer->write_value_array_item(value);
            }
            else if (bson_type == bsoncxx::type::k_int64)
            {
                int64_t value = elem.get_int64();
                writer->write_value_array_item(value);
            }
            else if (bson_type == bsoncxx::type::k_oid)
            {
                dot::object_id value = elem.get_oid().value;
                writer->write_value_array_item(value);
            }
            else if (bson_type == bsoncxx::type::k_document)
            {
                // Read BSON stream for the embedded data element
                bsoncxx::document::view sub_doc = elem.get_document().view();

                // Deserialize embedded data element
                writer->write_start_array_item();
                deserialize_document(sub_doc, writer);
                writer->write_end_array_item();
            }
            else if (bson_type == bsoncxx::type::k_array)
            {
                throw dot::exception("Deserializaion of an array inside another array is not supported.");
            }
            else
                throw dot::exception(
                    "Deserialization of BSON type inside an array is not supported.");
        }
    }

    void bson_record_serializer_impl::serialize(tree_writer_base writer, dot::object value)
    {
        // Root name is written in JSON as _t element
        dot::string root_name = value->get_type()->full_name();

        writer->write_start_document(root_name);
        //value->serialize_to(writer);
        value->get_type()->get_method("serialize_to")->invoke(value, make_list<object>({writer}));
        writer->write_end_document(root_name);
    }
}
