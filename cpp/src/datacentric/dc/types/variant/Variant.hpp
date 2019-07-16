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

#pragma once

#include <dc/declare.hpp>
#include <dc/types/record/ValueType.hpp>
#include <dc/types/local_date/LocalDate.hpp>
#include <dc/types/local_time/LocalTime.hpp>
#include <dc/types/local_minute/LocalMinute.hpp>
#include <dc/types/local_date_time/LocalDateTime.hpp>
#include <dot/system/Enum.hpp>
#include <dot/system/Object.hpp>
#include <dot/system/String.hpp>
#include <dot/system/Type.hpp>
#include <dot/noda_time/LocalDateTime.hpp>
#include <dot/noda_time/LocalMinute.hpp>

namespace dc
{
    /// <summary>Variant type can hold any atomic value or be empty.</summary>
    class DC_CLASS Variant
    {
        typedef Variant self;
        typedef ValueType ValueType_;

    private: // FIELDS

        dot::Object value_;

    public: // CONSTRUCTORS

        /// <summary>Default constructor.</summary>
        Variant();

        /// <summary>Create from object of supported types, error message if argument type is unsupported.</summary>
        Variant(dot::Object value);

    public: // PROPERTIES

        /// <summary>Type of the value held by the variant.</summary>
        DOT_GET(ValueType_, ValueType,
        {
            if (value_ == nullptr)
                return ValueType_::Empty;

            dot::Type valueType = value_->GetType();

            // The purpose of this check is to ensure that variant holds only one of the supported types
            if (valueType->Equals(dot::typeof<dot::String>()))        return ValueType_::String;
            if (valueType->Equals(dot::typeof<double>()))             return ValueType_::Double;
            if (valueType->Equals(dot::typeof<bool>()))               return ValueType_::Bool;
            if (valueType->Equals(dot::typeof<int>()))                return ValueType_::Int;
            if (valueType->Equals(dot::typeof<int64_t>()))            return ValueType_::Long;
            if (valueType->Equals(dot::typeof<dot::LocalDate>()))     return ValueType_::LocalDate;
            if (valueType->Equals(dot::typeof<dot::LocalTime>()))     return ValueType_::LocalTime;
            if (valueType->Equals(dot::typeof<dot::LocalMinute>()))   return ValueType_::LocalMinute;
            if (valueType->Equals(dot::typeof<dot::LocalDateTime>())) return ValueType_::LocalDateTime;
            if (valueType->IsEnum)                                    return ValueType_::Enum;

            // Error message if any other type, should normally not get to here
            throw dot::new_Exception(GetWrongTypeErrorMessage(value_));
        })

        /// <summary>Value held by the variant, which may be null.</summary>
        DOT_GET(dot::Object, Value, { return value_; })

    public: // METHODS

        /// <summary>Check if the variant is equal to default constructed object.</summary>
        bool IsEmpty();

        /// <summary>Provides alternate serialization of certain value types.</summary>
        dot::String ToString();

        /// <summary>Hash code is zero for null objects.</summary>
        size_t GetHashCode();

        /// <summary>Variants are equal when both types and values are equal.
        /// Comparison of doubles is performed with roundoff tolerance.</summary>
        bool Equals(const Variant& other);

    public: // OPERATORS

        /// <summary>Variants are equal when both types and values are equal.
        /// Comparison of doubles is performed with roundoff tolerance.</summary>
        bool operator==(const Variant& other);

        /// <summary>Variants are equal when both types and values are equal.
        /// Comparison of doubles is performed with roundoff tolerance.</summary>
        bool operator!=(const Variant& other);

    public: // STATIC

        static Variant Parse(ValueType_ valueType, dot::String value);

        template <class T>
        static Variant Parse(dot::String value)
        {
            if (dot::String::IsNullOrEmpty(value))
            {
                // Empty value
                return Variant();
            }

            Type valueType = dot::typeof<T>();

            // The purpose of this check is to ensure that variant holds only one of the supported types
            if (valueType->Equals(dot::typeof<String>()))        return Variant(value);
            if (valueType->Equals(dot::typeof<double>()))        return Variant(Double::Parse(value));
            if (valueType->Equals(dot::typeof<bool>()))          return Variant(Bool::Parse(value));
            if (valueType->Equals(dot::typeof<int>()))           return Variant(Int::Parse(value));
            if (valueType->Equals(dot::typeof<int64_t>()))       return Variant(Long::Parse(value));
            if (valueType->Equals(dot::typeof<LocalDate>()))     return Variant(LocalDate::Parse(value));
            if (valueType->Equals(dot::typeof<LocalTime>()))     return Variant(LocalTime::Parse(value));
            if (valueType->Equals(dot::typeof<LocalMinute>()))   return Variant(LocalMinute::Parse(value));
            if (valueType->Equals(dot::typeof<LocalDateTime>())) return Variant(LocalDateTime::Parse(value));
            if (valueType->IsEnum)                               return Variant(Enum::Parse(valueType, value));

            // Error message if any other type
            throw dot::new_Exception(GetWrongTypeErrorMessage(T()));
        }

    private: // PRIVATE

        static dot::String GetWrongTypeErrorMessage(dot::Object value);
    };
}
