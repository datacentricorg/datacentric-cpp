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
#include <dc/types/variant/Variant.hpp>
#include <dc/types/double/Double.hpp>
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

        if (valueType->Equals(dot::typeof<dot::string>())
            || valueType->Equals(dot::typeof<double>())
            || valueType->Equals(dot::typeof<bool>())
            || valueType->Equals(dot::typeof<int>())
            || valueType->Equals(dot::typeof<int64_t>()))
        {
            value_ = value;
        }
        else if (valueType->Equals(dot::typeof<dot::local_date>())
            || valueType->Equals(dot::typeof<dot::local_time>())
            || valueType->Equals(dot::typeof<dot::local_minute>())
            || valueType->Equals(dot::typeof<dot::local_date_time>()))
        {
            value_ = value;
        }
        else if (valueType->IsEnum)
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

    bool Variant::IsEmpty()
    {
        return value_ == nullptr;
    }

    dot::string Variant::ToString()
    {
        if (value_ != nullptr)
        {
            // Use AsString() for custom serialization of certain value types
            return value_->ToString();
        }
        else
        {
            // Returns empty string as per standard ToString() convention, rather than null like AsString() does
            return dot::string::Empty;
        }
    }

    size_t Variant::GetHashCode()
    {
        if (value_ != nullptr) return value_->GetHashCode();
        return 0;
    }

    bool Variant::Equals(const Variant& other)
    {
        if (value_ == nullptr)
            return other.value_ == nullptr;

        dot::type_t valueType = value_->type();
        dot::type_t otherValueType = other.value_->type();

        // The purpose of this check is to ensure that variant holds only one of the supported types
        if (valueType->Equals(dot::typeof<dot::string>()))
        {
            return otherValueType->Equals(dot::typeof<dot::string>())
                && (dot::string) value_ == (dot::string) other.value_;
        }
        if (valueType->Equals(dot::typeof<double>()))
        {
            // Perform comparison of doubles by function that uses numerical tolerance
            return otherValueType->Equals(dot::typeof<double>())
                && double_util::Equal(value_, other.value_);
        }
        if (valueType->Equals(dot::typeof<bool>()))
        {
            return otherValueType->Equals(dot::typeof<bool>())
                && (bool) value_ == (bool) other.value_;
        }
        if (valueType->Equals(dot::typeof<int>()))
        {
            return otherValueType->Equals(dot::typeof<int>())
                && (int) value_ == (int) other.value_;
        }
        if (valueType->Equals(dot::typeof<int64_t>()))
        {
            return otherValueType->Equals(dot::typeof<int64_t>())
                && (int64_t) value_ == (int64_t) other.value_;
        }
        if (valueType->Equals(dot::typeof<dot::local_date>()))
        {
            return otherValueType->Equals(dot::typeof<dot::local_date>())
                && (dot::local_date) value_ == (dot::local_date) other.value_;
        }
        if (valueType->Equals(dot::typeof<dot::local_time>()))
        {
            return otherValueType->Equals(dot::typeof<dot::local_time>())
                && (dot::local_time) value_ == (dot::local_time) other.value_;
        }
        if (valueType->Equals(dot::typeof<dot::local_minute>()))
        {
            return otherValueType->Equals(dot::typeof<dot::local_minute>())
                && (dot::local_minute) value_ == (dot::local_minute) other.value_;
        }
        if (valueType->Equals(dot::typeof<dot::local_date_time>()))
        {
            return otherValueType->Equals(dot::typeof<dot::local_date_time>())
                && (dot::local_date_time) value_ == (dot::local_date_time) other.value_;
        }
        if (valueType->IsEnum)
        {
            // Use Equals(other) to avoid unintended reference comparison
            return otherValueType->IsEnum
                && value_->Equals(other.value_);
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

    Variant Variant::Parse(ValueType valueType, dot::string value)
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
        case ValueType::Double:           return Variant(dot::Double::Parse(value));
        case ValueType::Bool:             return Variant(dot::Bool::Parse(value));
        case ValueType::Int:              return Variant(dot::Int::Parse(value));
        case ValueType::Long:             return Variant(dot::Long::Parse(value));
        case ValueType::local_date:        return Variant(local_date_util::Parse(value));
        case ValueType::local_time:        return Variant(local_time_util::Parse(value));
        case ValueType::local_minute:      return Variant(local_minute_util::Parse(value));
        case ValueType::local_date_time:    return Variant(local_date_time_util::Parse(value));
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
