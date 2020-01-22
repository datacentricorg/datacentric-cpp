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
    void append_token(bsoncxx::builder::core& builder, object value)
    {
        dot::type value_type = value->get_type();

        // Convert value to supported type
        list<attribute> value_attributes = value_type->get_custom_attributes(dot::typeof<filter_token_serialization_attribute>(), true);
        if (value_attributes->count())
        {
             value = ((filter_token_serialization_attribute) value_attributes[0])->serialize(value);
             value_type = value->get_type();
        }

        // Write value to bson
        if (value_type->equals(dot::typeof<dot::string>()))
            builder.append(*(dot::string)value);
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
        if (value_type->equals(dot::typeof<dot::object_id>()))
            builder.append(((dot::struct_wrapper<dot::object_id>)value)->oid());
        else
        if (value_type->is_enum())
            builder.append(*value->to_string());
        else
        if (value.is<dot::list_base>())
        {
            list_base l = value.as<list_base>();
            builder.open_array();
            for (int i = 0; i < l->get_length(); ++i)
            {
                append_token(builder, l->get_item(i));
            }
            builder.close_array();
        }
        else
        if (value.is<dot::byte_array>())
        {
            byte_array arr = value.as<byte_array>();
            builder.append(bson_writer_impl::to_bson_binary(arr));
        }
        else
            throw dot::exception("Unknown query token");
    }

    /// Returns bson document serialized from filter tokens (and_list, or_list, operator_wrapper).
    bsoncxx::document::view_or_value serialize_tokens(filter_token_base value)
    {
        if (value.is<operator_wrapper>())
        {
            // operator_wrapper -> { field_name : { operation : value } }
            // e.g. { sub_doc_name.double_field : { $lt : 2.5 } }
            operator_wrapper wrapper = value.as<operator_wrapper>();
            bsoncxx::builder::core builder(false);
            builder.key_view(*(wrapper->prop_name_));
            builder.open_document();
            builder.key_view(*(wrapper->op_));
            append_token(builder, wrapper->rhs_);
            builder.close_document();

            return builder.extract_document();
        }
        else if (value.is<and_list>())
        {
            // and_list -> { $and : [ token1, token2, ... ] }
            and_list list = value.as<and_list>();
            bsoncxx::builder::core builder(false);
            builder.key_view("$and");
            builder.open_array();
            for (filter_token_base const& item : list->values_list_)
            {
                builder.append(serialize_tokens(item));
            }
            builder.close_array();

            return builder.extract_document();

        }
        else if (value.is<or_list>())
        {
            // or_list -> { $or : [ token1, token2, ... ] }
            or_list list = value.as<or_list>();
            bsoncxx::builder::core builder(false);
            builder.key_view("$or");
            builder.open_array();
            for (filter_token_base const& item : list->values_list_)
            {
                builder.append(serialize_tokens(item));
            }
            builder.close_array();

            return builder.extract_document();
        }
        else throw dot::exception("Unknown query token");
    }

}
