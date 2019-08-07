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
#include <dc/types/variant/Variant.hpp>
#include <dc/types/double/double_compare.hpp>
#include <dot/system/exception.hpp>
#include <dot/system/nullable.hpp>

namespace dc
{
    Variant::Variant()
        : value_(nullptr)
    {}

    Variant::Variant(dot::object value)
    {
        if (value == nullptr)
        {
            value_ = nullptr;
            return;
        }

        dot::type_t valueType = value->type();

        if (valueType->equals(dot::typeof<dot::string>())
            || valueType->equals(dot::typeof<double>())
            || valueType->equals(dot::typeof<bool>())
            || valueType->equals(dot::typeof<int>())
            || valueType->equals(dot::typeof<int64_t>()))
        {
            value_ = value;
        }
        else if (valueType->equals(dot::typeof<dot::local_date>())
            || valueType->equals(dot::typeof<dot::local_time>())
            || valueType->equals(dot::typeof<dot::local_minute>())
            || valueType->equals(dot::typeof<dot::local_date_time>()))
        {
            value_ = value;
        }
        else if (valueType->is_enum)
        {
            value_ = value;
        }
        else
        {
            // Argument type is unsupported, error message
            //throw dot::exception(GetWrongTypeErrorMessage(value));
            value_ = nullptr;
        }
    }

    bool Variant::is_empty()
    {
        return value_ == nullptr;
    }

    dot::string Variant::to_string()
    {
        if (value_ != nullptr)
        {
            // Use AsString() for custom serialization of certain value types
            return value_->to_string();
        }
        else
        {
            // Returns empty string as per standard to_string() convention, rather than null like AsString() does
            return dot::string::empty;
        }
    }

    size_t Variant::GetHashCode()
    {
        if (value_ != nullptr) return value_->hash_code();
        return 0;
    }

    bool Variant::Equals(const Variant& other)
    {
        if (value_ == nullptr)
            return other.value_ == nullptr;

        dot::type_t valueType = value_->type();
        dot::type_t otherValueType = other.value_->type();

        // The purpose of this check is to ensure that variant holds only one of the supported types
        if (valueType->equals(dot::typeof<dot::string>()))
        {
            return otherValueType->equals(dot::typeof<dot::string>())
                && (dot::string) value_ == (dot::string) other.value_;
        }
        if (valueType->equals(dot::typeof<double>()))
        {
            // Perform comparison of doubles by function that uses numerical tolerance
            return otherValueType->equals(dot::typeof<double>())
                && double_util::Equal(value_, other.value_);
        }
        if (valueType->equals(dot::typeof<bool>()))
        {
            return otherValueType->equals(dot::typeof<bool>())
                && (bool) value_ == (bool) other.value_;
        }
        if (valueType->equals(dot::typeof<int>()))
        {
            return otherValueType->equals(dot::typeof<int>())
                && (int) value_ == (int) other.value_;
        }
        if (valueType->equals(dot::typeof<int64_t>()))
        {
            return otherValueType->equals(dot::typeof<int64_t>())
                && (int64_t) value_ == (int64_t) other.value_;
        }
        if (valueType->equals(dot::typeof<dot::local_date>()))
        {
            return otherValueType->equals(dot::typeof<dot::local_date>())
                && (dot::local_date) value_ == (dot::local_date) other.value_;
        }
        if (valueType->equals(dot::typeof<dot::local_time>()))
        {
            return otherValueType->equals(dot::typeof<dot::local_time>())
                && (dot::local_time) value_ == (dot::local_time) other.value_;
        }
        if (valueType->equals(dot::typeof<dot::local_minute>()))
        {
            return otherValueType->equals(dot::typeof<dot::local_minute>())
                && (dot::local_minute) value_ == (dot::local_minute) other.value_;
        }
        if (valueType->equals(dot::typeof<dot::local_date_time>()))
        {
            return otherValueType->equals(dot::typeof<dot::local_date_time>())
                && (dot::local_date_time) value_ == (dot::local_date_time) other.value_;
        }
        if (valueType->is_enum)
        {
            // Use Equals(other) to avoid unintended reference comparison
            return otherValueType->is_enum
                && value_->equals(other.value_);
        }

        // Error message if any other type, should normally not get here
        throw dot::exception(GetWrongTypeErrorMessage(value_));
    }

    bool Variant::operator==(const Variant& other)
    {
        return Equals(other);
    }

    bool Variant::operator!=(const Variant& other)
    {
        return !Equals(other);
    }

    Variant Variant::parse(ValueType valueType, dot::string value)
    {
        if (dot::string::is_null_or_empty(value))
        {
            // Empty value
            return Variant();
        }

        // Switch on type of default value
        switch (valueType)
        {
        case ValueType::String:           return Variant(value);
        case ValueType::Double:           return Variant(dot::double_impl::parse(value));
        case ValueType::Bool:             return Variant(dot::bool_impl::parse(value));
        case ValueType::Int:              return Variant(dot::int_impl::parse(value));
        case ValueType::Long:             return Variant(dot::long_impl::parse(value));
        case ValueType::local_date:        return Variant(dot::local_date_util::parse(value));
        case ValueType::local_time:        return Variant(dot::local_time_util::parse(value));
        case ValueType::local_minute:      return Variant(dot::local_minute_util::parse(value));
        case ValueType::local_date_time:    return Variant(dot::local_date_time_util::parse(value));
        case ValueType::Enum:
            throw dot::exception("Variant cannot be created as enum without specifying enum typename.");
        default:
            // Error message if any other type
            throw dot::exception("Unknown value type when parsing string into variant.");
        }
    }

    dot::string Variant::GetWrongTypeErrorMessage(dot::object value)
    {
        return dot::string::format(
            "Variant cannot hold {0} type. Available types are "
            "string, double, bool, int, long, local_date, local_time, local_minute, local_date_time, or Enum.",
            value->type());
    }
}
