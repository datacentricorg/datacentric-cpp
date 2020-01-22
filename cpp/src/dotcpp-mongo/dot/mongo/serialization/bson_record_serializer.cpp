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
#include <dot/mongo/mongo_db/mongo/settings.hpp>
#include <dot/serialization/serialize_attribute.hpp>

namespace dot
{
    dot::Object BsonRecordSerializerImpl::deserialize(bsoncxx::document::view doc)
    {
        // Create instance to which BSON will be deserialized
        dot::String type_name;
        if (dot::MongoClientSettings::get_discriminator_convention() == dot::DiscriminatorConvention::scalar)
        {
            type_name = doc["_t"].get_utf8().value.to_string();
        }
        else if (dot::MongoClientSettings::get_discriminator_convention() == dot::DiscriminatorConvention::hierarchical)
        {
            bsoncxx::array::view type_array_view = doc["_t"].get_array();
            size_t type_array_length = std::distance(type_array_view.begin(), type_array_view.end());

            // Exception on empty array
            if (type_array_length == 0)
                throw Exception("_t array has no elements.");

            // Get last item from array
            type_name = type_array_view.find(type_array_length - 1)->get_utf8().value.to_string();
        }
        else
        {
            throw dot::Exception("Unknown DiscriminatorConvention.");
        }

        Object result = dot::Activator::create_instance("", type_name);
        tree_writer_base writer = make_data_writer(result);

        writer->write_start_document(type_name);
        deserialize_document(doc, writer);
        writer->write_end_document(type_name);
        return result;
    }

    dot::Object BsonRecordSerializerImpl::deserialize_tuple(bsoncxx::document::view doc, dot::List<dot::FieldInfo> props, dot::Type tuple_type)
    {
        // Create instance to which BSON will be deserialized
        dot::String type_name = tuple_type->name();
        dot::Object result = dot::Activator::create_instance(tuple_type);
        tree_writer_base writer = make_tuple_writer(result, props);

        writer->write_start_document(type_name);
        deserialize_document(doc, writer);
        writer->write_end_document(type_name);
        return result;

    }

    void BsonRecordSerializerImpl::deserialize_document(const bsoncxx::document::view & doc, tree_writer_base writer)
    {
        dot::String type_name;

        auto type_iter = doc.find("_t");

        if (type_iter != doc.end() && type_iter->type() == bsoncxx::type::k_utf8)
        {
            type_name = doc["_t"].get_utf8().value.to_string();
        }

        // Each document is a dictionary at root level
        writer->write_start_dict(type_name);

        // Loop over elements until

        for (auto elem : doc)
        {
            bsoncxx::type bson_type = elem.type();

            // Read element name and value
            dot::String element_name = elem.key().to_string();
            if (element_name == "_t")
            {
                continue;
            }
            if (bson_type == bsoncxx::type::k_null)
            {
            }
            else if (bson_type == bsoncxx::type::k_oid)
            {
                dot::ObjectId value = elem.get_oid().value;
                writer->write_value_element(element_name, value);
            }
            else if (bson_type == bsoncxx::type::k_utf8)
            {
                dot::String value = elem.get_utf8().value.to_string();

                    writer->write_value_element(element_name, value);
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
                writer->write_value_element(element_name, dot::LocalDateTimeUtil::from_std_chrono(value.value));
            }
            else if (bson_type == bsoncxx::type::k_binary)
            {
                bsoncxx::types::b_binary value = elem.get_binary();
                writer->write_value_element(element_name, to_byte_array(value));
            }
            else if (bson_type == bsoncxx::type::k_document)
            {
                // Read BSON stream for the embedded data element
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
            else throw dot::Exception(
                "Deserialization of BSON type {0} is not supported.");
        }

        // Each document is a dictionary at root level
        writer->write_end_dict(type_name);
    }

    void BsonRecordSerializerImpl::deserialize_array(const bsoncxx::array::view & arr, tree_writer_base writer)
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
                dot::String value = elem.get_utf8().value.to_string();
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
                dot::ObjectId value = elem.get_oid().value;
                writer->write_value_array_item(value);
            }
            else if (bson_type == bsoncxx::type::k_binary)
            {
                bsoncxx::types::b_binary value = elem.get_binary();
                writer->write_value_array_item(to_byte_array(value));
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
                throw dot::Exception("Deserializaion of an array inside another array is not supported.");
            }
            else
                throw dot::Exception(
                    "Deserialization of BSON type inside an array is not supported.");
        }
    }

    void BsonRecordSerializerImpl::serialize(tree_writer_base writer, dot::Object value)
    {
        // Root name is written in JSON as _t element
        dot::String root_name = value->get_type()->name();

        writer->write_start_document(root_name);

        // Check for custom serializator
        if (value->get_type()->get_custom_attributes(dot::typeof<SerializeClassAttribute>(), true)->size())
        {
            SerializeClassAttribute(value->get_type()->get_custom_attributes(dot::typeof<SerializeClassAttribute>(), true)[0])->serialize(writer, value);
        }
        else
        {
            standard_serialize(writer, value);
        }

        writer->write_end_document(root_name);
    }

    void BsonRecordSerializerImpl::standard_serialize(dot::ListBase obj, dot::String element_name, dot::tree_writer_base writer)
    {
        // Write start element tag
        writer->write_start_array_element(element_name);

        int length = obj->get_length();

        // Iterate over sequence elements
        for (int i = 0; i < length; ++i)
        {
            dot::Object item = obj->get_item(i);

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

            // Serialize based on Type of the item
            dot::Type item_type = item->get_type();

            if (item_type->get_custom_attributes(dot::typeof<SerializeClassAttribute>(), true)->size())
            {
                SerializeClassAttribute(item_type->get_custom_attributes(dot::typeof<SerializeClassAttribute>(), true)[0])->serialize(writer, item);
            }
            else
            if (item_type->equals(dot::typeof<dot::String>())
                || item_type->equals(dot::typeof<double>())
                || item_type->equals(dot::typeof<bool>())
                || item_type->equals(dot::typeof<int>())
                || item_type->equals(dot::typeof<int64_t>())
                || item_type->equals(dot::typeof<dot::LocalDate>())
                || item_type->equals(dot::typeof<dot::LocalDateTime>())
                || item_type->equals(dot::typeof<dot::LocalTime>())
                || item_type->equals(dot::typeof<dot::LocalMinute>())
                || item_type->equals(dot::typeof<dot::ByteArray>())
                || item_type->is_enum()
                || item_type->equals(dot::typeof<dot::ObjectId>())
                )
            {
                writer->write_start_value();
                writer->write_value(item);
                writer->write_end_value();
            }
            else if (dot::typeof<dot::ListBase>()->is_assignable_from(item_type))
            {
                throw dot::Exception(dot::String::format("Serialization is not supported for element {0} "
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

    ByteArray BsonRecordSerializerImpl::to_byte_array(const bsoncxx::types::b_binary& bin_array)
    {
        return make_byte_array((const char*) bin_array.bytes, bin_array.size);
    }

    void BsonRecordSerializerImpl::standard_serialize(tree_writer_base writer, dot::Object value)
    {
        // Write start tag
        writer->write_start_dict(value->get_type()->name());

        // Iterate over the list of elements
        dot::List<dot::FieldInfo> inner_element_info_list = value->get_type()->get_fields();
        for (dot::FieldInfo inner_element_info : inner_element_info_list)
        {

            if (inner_element_info->get_custom_attributes(dot::typeof<SerializeFieldAttribute>(), true)->size())
            {
                SerializeFieldAttribute(inner_element_info->get_custom_attributes(dot::typeof<SerializeFieldAttribute>(), true)[0])->serialize(writer, value);
                continue;
            }

            // Get element name and value
            dot::String inner_element_name = inner_element_info->name();

            dot::Object inner_element_value = inner_element_info->get_value(value);

            if (inner_element_value.is_empty())
            {
                continue;
            }

            dot::Type element_type = inner_element_value->get_type();

            if (element_type->get_custom_attributes(dot::typeof<SerializeClassAttribute>(), true)->size())
            {
                writer->write_start_element(inner_element_name);
                SerializeClassAttribute(element_type->get_custom_attributes(dot::typeof<SerializeClassAttribute>(), true)[0])->serialize(writer, inner_element_value);
                writer->write_end_element(inner_element_name);
            }
            else
            if (element_type->equals(dot::typeof<dot::String>())
                || element_type->equals(dot::typeof<double>())
                || element_type->equals(dot::typeof<bool>())
                || element_type->equals(dot::typeof<int>())
                || element_type->equals(dot::typeof<int64_t>())
                || element_type->equals(dot::typeof<dot::LocalDate>())
                || element_type->equals(dot::typeof<dot::LocalDateTime>())
                || element_type->equals(dot::typeof<dot::LocalTime>())
                || element_type->equals(dot::typeof<dot::LocalMinute>())
                || element_type->equals(dot::typeof<dot::ByteArray>())
                || element_type->is_enum()
                || element_type->equals(dot::typeof<dot::ObjectId>())
                )
            {
                writer->write_value_element(inner_element_name, inner_element_value);
            }
            else
            if (dot::typeof<dot::ListBase>()->is_assignable_from(element_type))
            {
                standard_serialize((dot::ListBase)inner_element_value, inner_element_name, writer);
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
        writer->write_end_dict(value->get_type()->name());
    }
}
