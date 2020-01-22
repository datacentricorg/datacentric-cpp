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
    variant::variant()
        : value_(nullptr)
    {}

    variant::variant(dot::object value)
    {
        if (value == nullptr)
        {
            value_ = nullptr;
            return;
        }

        dot::type value_type = value->get_type();

        if (value_type->equals(dot::typeof<dot::string>())
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
            //throw dot::exception(get_wrong_type_error_message(value));
            value_ = nullptr;
        }
    }

    bool variant::is_empty()
    {
        return value_ == nullptr;
    }

    dot::string variant::to_string()
    {
        if (value_ != nullptr)
        {
            // Use as_string() for custom serialization of certain value types
            return value_->to_string();
        }
        else
        {
            // Returns empty string as per standard to_string() convention, rather than null like as_string() does
            return dot::string::empty;
        }
    }

    size_t variant::hash_code()
    {
        if (value_ != nullptr) return value_->hash_code();
        return 0;
    }

    bool variant::equals(const variant& other)
    {
        if (value_ == nullptr)
            return other.value_ == nullptr;

        dot::type value_type = value_->get_type();
        dot::type other_value_type = other.value_->get_type();

        // The purpose of this check is to ensure that variant holds only one of the supported types
        if (value_type->equals(dot::typeof<dot::string>()))
        {
            return other_value_type->equals(dot::typeof<dot::string>())
                && (dot::string) value_ == (dot::string) other.value_;
        }
        if (value_type->equals(dot::typeof<double>()))
        {
            // Perform comparison of doubles by function that uses numerical tolerance
            return other_value_type->equals(dot::typeof<double>())
                && double_util::equal(value_, other.value_);
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
        throw dot::exception(get_wrong_type_error_message(value_));
    }

    bool variant::operator==(const variant& other)
    {
        return equals(other);
    }

    bool variant::operator!=(const variant& other)
    {
        return !equals(other);
    }

    variant variant::parse(value_type value_type, dot::string value)
    {
        if (dot::string::is_null_or_empty(value))
        {
            // Empty value
            return variant();
        }

        // Switch on type of default value
        switch (value_type)
        {
        case value_type::string_type:           return variant(value);
        case value_type::double_type:           return variant(dot::double_impl::parse(value));
        case value_type::bool_type:             return variant(dot::bool_impl::parse(value));
        case value_type::int_type:              return variant(dot::int_impl::parse(value));
        case value_type::long_type:             return variant(dot::long_impl::parse(value));
        case value_type::local_date_type:        return variant(dot::LocalDateUtil::parse(value));
        case value_type::local_time_type:        return variant(dot::LocalTimeUtil::parse(value));
        case value_type::local_minute_type:      return variant(dot::LocalMinuteUtil::parse(value));
        case value_type::local_date_time_type:    return variant(dot::LocalDateTimeUtil::parse(value));
        case value_type::enum_type:
            throw dot::exception("variant cannot be created as enum without specifying enum typename.");
        default:
            // Error message if any other type
            throw dot::exception("Unknown value type when parsing string into variant.");
        }
    }

    dot::string variant::get_wrong_type_error_message(dot::object value)
    {
        return dot::string::format(
            "variant cannot hold {0} type. Available types are "
            "string, double, bool, int, long, LocalDate, LocalTime, LocalMinute, LocalDateTime, or enum.",
            value->get_type());
    }
}
