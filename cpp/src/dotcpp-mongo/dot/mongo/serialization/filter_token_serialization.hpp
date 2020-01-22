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

#pragma once

#include <dot/system/byte_array.hpp>
#include <dot/mongo/serialization/filter_token_serialization_attribute.hpp>

namespace dot
{
    /// Writes primitive value to bson builder.
    void append_token(bsoncxx::builder::core& builder, Object value)
    {
        dot::Type value_type = value->get_type();

        // Convert value to supported type
        List<Attribute> value_attributes = value_type->get_custom_attributes(dot::typeof<FilterTokenSerializationAttribute>(), true);
        if (value_attributes->count())
        {
             value = ((FilterTokenSerializationAttribute) value_attributes[0])->serialize(value);
             value_type = value->get_type();
        }

        // Write value to bson
        if (value_type->equals(dot::typeof<dot::String>()))
            builder.append(*(dot::String)value);
        else
        if (value_type->equals(dot::typeof<double>())) // ? TODO check dot::typeof<Double>() dot::typeof<NullableDouble>()
            builder.append((double)value);
        else
        if (value_type->equals(dot::typeof<bool>()))
            builder.append((bool)value);
        else
        if (value_type->equals(dot::typeof<int>()))
            builder.append((int)value);
        else
        if (value_type->equals(dot::typeof<int64_t>()))
            builder.append((int64_t)value);
        else
        if (value_type->equals(dot::typeof<dot::LocalDate>()))
            builder.append(dot::LocalDateUtil::to_iso_int((dot::LocalDate)value));
        else
        if (value_type->equals(dot::typeof<dot::LocalTime>()))
            builder.append(dot::LocalTimeUtil::to_iso_int((dot::LocalTime)value));
        else
        if (value_type->equals(dot::typeof<dot::LocalMinute>()))
            builder.append(dot::LocalMinuteUtil::to_iso_int((dot::LocalMinute) value));
        else
        if (value_type->equals(dot::typeof<dot::LocalDateTime>()))
            builder.append(bsoncxx::types::b_date{ dot::LocalDateTimeUtil::to_std_chrono((dot::LocalDateTime)value) });
        else
        if (value_type->equals(dot::typeof<dot::ObjectId>()))
            builder.append(((dot::struct_wrapper<dot::ObjectId>)value)->oid());
        else
        if (value_type->is_enum())
            builder.append(*value->to_string());
        else
        if (value.is<dot::ListBase>())
        {
            ListBase l = value.as<ListBase>();
            builder.open_array();
            for (int i = 0; i < l->get_length(); ++i)
            {
                append_token(builder, l->get_item(i));
            }
            builder.close_array();
        }
        else
        if (value.is<dot::ByteArray>())
        {
            ByteArray arr = value.as<ByteArray>();
            builder.append(BsonWriterImpl::to_bson_binary(arr));
        }
        else
            throw dot::Exception("Unknown query token");
    }

    /// Returns bson document serialized from filter tokens (AndList, OrList, OperatorWrapper).
    bsoncxx::document::view_or_value serialize_tokens(FilterTokenBase value)
    {
        if (value.is<OperatorWrapper>())
        {
            // OperatorWrapper -> { field_name : { operation : value } }
            // e.g. { sub_doc_name.double_field : { $lt : 2.5 } }
            OperatorWrapper wrapper = value.as<OperatorWrapper>();
            bsoncxx::builder::core builder(false);
            builder.key_view(*(wrapper->prop_name_));
            builder.open_document();
            builder.key_view(*(wrapper->op_));
            append_token(builder, wrapper->rhs_);
            builder.close_document();

            return builder.extract_document();
        }
        else if (value.is<AndList>())
        {
            // AndList -> { $and : [ token1, token2, ... ] }
            AndList list = value.as<AndList>();
            bsoncxx::builder::core builder(false);
            builder.key_view("$and");
            builder.open_array();
            for (FilterTokenBase const& item : list->values_list_)
            {
                builder.append(serialize_tokens(item));
            }
            builder.close_array();

            return builder.extract_document();

        }
        else if (value.is<OrList>())
        {
            // OrList -> { $or : [ token1, token2, ... ] }
            OrList list = value.as<OrList>();
            bsoncxx::builder::core builder(false);
            builder.key_view("$or");
            builder.open_array();
            for (FilterTokenBase const& item : list->values_list_)
            {
                builder.append(serialize_tokens(item));
            }
            builder.close_array();

            return builder.extract_document();
        }
        else throw dot::Exception("Unknown query token");
    }
}
