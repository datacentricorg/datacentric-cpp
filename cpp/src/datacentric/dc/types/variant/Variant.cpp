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
#include <dot/system/Exception.hpp>
#include <dot/system/Nullable.hpp>

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

        dot::Type valueType = value->GetType();

        if (valueType->Equals(dot::typeof<dot::String>())
            || valueType->Equals(dot::typeof<double>())
            || valueType->Equals(dot::typeof<bool>())
            || valueType->Equals(dot::typeof<int>())
            || valueType->Equals(dot::typeof<int64_t>()))
        {
            value_ = value;
        }
        else if (valueType->Equals(dot::typeof<dot::LocalDate>())
            || valueType->Equals(dot::typeof<dot::LocalTime>())
            || valueType->Equals(dot::typeof<dot::LocalMinute>())
            || valueType->Equals(dot::typeof<dot::LocalDateTime>()))
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
            //throw dot::new_Exception(GetWrongTypeErrorMessage(value));
            value_ = nullptr;
        }
    }

    bool Variant::IsEmpty()
    {
        return value_ == nullptr;
    }

    dot::String Variant::ToString()
    {
        if (value_ != nullptr)
        {
            // Use AsString() for custom serialization of certain value types
            return value_->ToString();
        }
        else
        {
            // Returns empty string as per standard ToString() convention, rather than null like AsString() does
            return dot::String::Empty;
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

        dot::Type valueType = value_->GetType();
        dot::Type otherValueType = other.value_->GetType();

        // The purpose of this check is to ensure that variant holds only one of the supported types
        if (valueType->Equals(dot::typeof<dot::String>()))
        {
            return otherValueType->Equals(dot::typeof<dot::String>())
                && (dot::String) value_ == (dot::String) other.value_;
        }
        if (valueType->Equals(dot::typeof<double>()))
        {
            // Perform comparison of doubles by function that uses numerical tolerance
            return otherValueType->Equals(dot::typeof<double>())
                && DoubleHelper::Equal(value_, other.value_);
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
        if (valueType->Equals(dot::typeof<dot::LocalDate>()))
        {
            return otherValueType->Equals(dot::typeof<dot::LocalDate>())
                && (dot::LocalDate) value_ == (dot::LocalDate) other.value_;
        }
        if (valueType->Equals(dot::typeof<dot::LocalTime>()))
        {
            return otherValueType->Equals(dot::typeof<dot::LocalTime>())
                && (dot::LocalTime) value_ == (dot::LocalTime) other.value_;
        }
        if (valueType->Equals(dot::typeof<dot::LocalMinute>()))
        {
            return otherValueType->Equals(dot::typeof<dot::LocalMinute>())
                && (dot::LocalMinute) value_ == (dot::LocalMinute) other.value_;
        }
        if (valueType->Equals(dot::typeof<dot::LocalDateTime>()))
        {
            return otherValueType->Equals(dot::typeof<dot::LocalDateTime>())
                && (dot::LocalDateTime) value_ == (dot::LocalDateTime) other.value_;
        }
        if (valueType->IsEnum)
        {
            // Use Equals(other) to avoid unintended reference comparison
            return otherValueType->IsEnum
                && value_->Equals(other.value_);
        }

        // Error message if any other type, should normally not get here
        throw dot::new_Exception(GetWrongTypeErrorMessage(value_));
    }

    bool Variant::operator==(const Variant& other)
    {
        return Equals(other);
    }

    bool Variant::operator!=(const Variant& other)
    {
        return !Equals(other);
    }

    Variant Variant::Parse(ValueType_ valueType, dot::String value)
    {
        if (dot::String::IsNullOrEmpty(value))
        {
            // Empty value
            return Variant();
        }

        // Switch on type of default value
        switch (valueType)
        {
        case ValueType_::String:           return Variant(value);
        case ValueType_::Double:           return Variant(dot::Double::Parse(value));
        case ValueType_::Bool:             return Variant(dot::Bool::Parse(value));
        case ValueType_::Int:              return Variant(dot::Int::Parse(value));
        case ValueType_::Long:             return Variant(dot::Long::Parse(value));
        case ValueType_::LocalDate:        return Variant(LocalDateHelper::Parse(value));
        case ValueType_::LocalTime:        return Variant(LocalTimeHelper::Parse(value));
        case ValueType_::LocalMinute:      return Variant(LocalMinuteHelper::Parse(value));
        case ValueType_::LocalDateTime:    return Variant(LocalDateTimeHelper::Parse(value));
        case ValueType_::Enum:
            throw dot::new_Exception("Variant cannot be created as enum without specifying enum typename.");
        default:
            // Error message if any other type
            throw dot::new_Exception("Unknown value type when parsing string into variant.");
        }
    }

    dot::String Variant::GetWrongTypeErrorMessage(dot::Object value)
    {
        return dot::String::Format(
            "Variant cannot hold {0} type. Available types are "
            "string, double, bool, int, long, LocalDate, LocalTime, LocalMinute, LocalDateTime, or Enum.",
            value->GetType());
    }
}
