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
#include <dc/types/variant/variant.hpp>
#include <dc/types/double/double_compare.hpp>
#include <dot/system/exception.hpp>
#include <dot/system/nullable.hpp>

namespace dc
{
    Variant::Variant()
        : value_(nullptr)
    {}

    Variant::Variant(dot::Object value)
    {
        if (value == nullptr)
        {
            value_ = nullptr;
            return;
        }

        dot::Type value_type = value->get_type();

        if (value_type->equals(dot::typeof<dot::String>())
            || value_type->equals(dot::typeof<double>())
            || value_type->equals(dot::typeof<bool>())
            || value_type->equals(dot::typeof<int>())
            || value_type->equals(dot::typeof<int64_t>()))
        {
            value_ = value;
        }
        else if (value_type->equals(dot::typeof<dot::LocalDate>())
            || value_type->equals(dot::typeof<dot::LocalTime>())
            || value_type->equals(dot::typeof<dot::LocalMinute>())
            || value_type->equals(dot::typeof<dot::LocalDateTime>()))
        {
            value_ = value;
        }
        else if (value_type->is_enum())
        {
            value_ = value;
        }
        else
        {
            // Argument type is unsupported, error message
            //throw dot::Exception(get_wrong_type_error_message(value));
            value_ = nullptr;
        }
    }

    bool Variant::is_empty()
    {
        return value_ == nullptr;
    }

    dot::String Variant::to_string()
    {
        if (value_ != nullptr)
        {
            // Use as_string() for custom serialization of certain value types
            return value_->to_string();
        }
        else
        {
            // Returns empty String as per standard to_string() convention, rather than null like as_string() does
            return dot::String::empty;
        }
    }

    size_t Variant::hash_code()
    {
        if (value_ != nullptr) return value_->hash_code();
        return 0;
    }

    bool Variant::equals(const Variant& other)
    {
        if (value_ == nullptr)
            return other.value_ == nullptr;

        dot::Type value_type = value_->get_type();
        dot::Type other_value_type = other.value_->get_type();

        // The purpose of this check is to ensure that Variant holds only one of the supported types
        if (value_type->equals(dot::typeof<dot::String>()))
        {
            return other_value_type->equals(dot::typeof<dot::String>())
                && (dot::String) value_ == (dot::String) other.value_;
        }
        if (value_type->equals(dot::typeof<double>()))
        {
            // Perform comparison of doubles by function that uses numerical tolerance
            return other_value_type->equals(dot::typeof<double>())
                && DoubleUtil::equal(value_, other.value_);
        }
        if (value_type->equals(dot::typeof<bool>()))
        {
            return other_value_type->equals(dot::typeof<bool>())
                && (bool) value_ == (bool) other.value_;
        }
        if (value_type->equals(dot::typeof<int>()))
        {
            return other_value_type->equals(dot::typeof<int>())
                && (int) value_ == (int) other.value_;
        }
        if (value_type->equals(dot::typeof<int64_t>()))
        {
            return other_value_type->equals(dot::typeof<int64_t>())
                && (int64_t) value_ == (int64_t) other.value_;
        }
        if (value_type->equals(dot::typeof<dot::LocalDate>()))
        {
            return other_value_type->equals(dot::typeof<dot::LocalDate>())
                && (dot::LocalDate) value_ == (dot::LocalDate) other.value_;
        }
        if (value_type->equals(dot::typeof<dot::LocalTime>()))
        {
            return other_value_type->equals(dot::typeof<dot::LocalTime>())
                && (dot::LocalTime) value_ == (dot::LocalTime) other.value_;
        }
        if (value_type->equals(dot::typeof<dot::LocalMinute>()))
        {
            return other_value_type->equals(dot::typeof<dot::LocalMinute>())
                && (dot::LocalMinute) value_ == (dot::LocalMinute) other.value_;
        }
        if (value_type->equals(dot::typeof<dot::LocalDateTime>()))
        {
            return other_value_type->equals(dot::typeof<dot::LocalDateTime>())
                && (dot::LocalDateTime) value_ == (dot::LocalDateTime) other.value_;
        }
        if (value_type->is_enum())
        {
            // Use equals(other) to avoid unintended reference comparison
            return other_value_type->is_enum()
                && value_->equals(other.value_);
        }

        // Error message if any other type, should normally not get here
        throw dot::Exception(get_wrong_type_error_message(value_));
    }

    bool Variant::operator==(const Variant& other)
    {
        return equals(other);
    }

    bool Variant::operator!=(const Variant& other)
    {
        return !equals(other);
    }

    Variant Variant::parse(ValueType value_type, dot::String value)
    {
        if (dot::String::is_null_or_empty(value))
        {
            // Empty value
            return Variant();
        }

        // Switch on type of default value
        switch (value_type)
        {
        case ValueType::string_type:           return Variant(value);
        case ValueType::double_type:           return Variant(dot::DoubleImpl::parse(value));
        case ValueType::bool_type:             return Variant(dot::BoolImpl::parse(value));
        case ValueType::int_type:              return Variant(dot::IntImpl::parse(value));
        case ValueType::long_type:             return Variant(dot::LongImpl::parse(value));
        case ValueType::local_date_type:        return Variant(dot::LocalDateUtil::parse(value));
        case ValueType::local_time_type:        return Variant(dot::LocalTimeUtil::parse(value));
        case ValueType::local_minute_type:      return Variant(dot::LocalMinuteUtil::parse(value));
        case ValueType::local_date_time_type:    return Variant(dot::LocalDateTimeUtil::parse(value));
        case ValueType::enum_type:
            throw dot::Exception("Variant cannot be created as enum without specifying enum typename.");
        default:
            // Error message if any other type
            throw dot::Exception("Unknown value type when parsing String into Variant.");
        }
    }

    dot::String Variant::get_wrong_type_error_message(dot::Object value)
    {
        return dot::String::format(
            "Variant cannot hold {0} type. Available types are "
            "String, double, bool, int, long, LocalDate, LocalTime, LocalMinute, LocalDateTime, or enum.",
            value->get_type());
    }
}
