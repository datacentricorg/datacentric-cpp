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

    private: // FIELDS

        dot::object value_;

    public: // CONSTRUCTORS

        /// <summary>Default constructor.</summary>
        Variant();

        /// <summary>Create from object of supported types, error message if argument type is unsupported.</summary>
        Variant(dot::object value);

    public: // PROPERTIES

        /// <summary>Type of the value held by the variant.</summary>
        ValueType getValueType()
        {
            if (value_ == nullptr)
                return ValueType::Empty;

            dot::type_t valueType = value_->type();

            // The purpose of this check is to ensure that variant holds only one of the supported types
            if (valueType->equals(dot::typeof<dot::string>()))        return ValueType::String;
            if (valueType->equals(dot::typeof<double>()))             return ValueType::Double;
            if (valueType->equals(dot::typeof<bool>()))               return ValueType::Bool;
            if (valueType->equals(dot::typeof<int>()))                return ValueType::Int;
            if (valueType->equals(dot::typeof<int64_t>()))            return ValueType::Long;
            if (valueType->equals(dot::typeof<dot::local_date>()))     return ValueType::LocalDate;
            if (valueType->equals(dot::typeof<dot::local_time>()))     return ValueType::LocalTime;
            if (valueType->equals(dot::typeof<dot::local_minute>()))   return ValueType::LocalMinute;
            if (valueType->equals(dot::typeof<dot::local_date_time>())) return ValueType::LocalDateTime;
            if (valueType->is_enum)                                    return ValueType::Enum;

            // Error message if any other type, should normally not get to here
            throw dot::exception(GetWrongTypeErrorMessage(value_));
        }

        /// <summary>Value held by the variant, which may be null.</summary>
        dot::object getValue()
        {
            return value_;
        }

    public: // METHODS

        /// <summary>Check if the variant is equal to default constructed object.</summary>
        bool IsEmpty();

        /// <summary>Provides alternate serialization of certain value types.</summary>
        dot::string ToString();

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

        static Variant Parse(ValueType valueType, dot::string value);

        template <class T>
        static Variant Parse(dot::string value)
        {
            if (dot::string::IsNullOrEmpty(value))
            {
                // Empty value
                return Variant();
            }

            Type valueType = dot::typeof<T>();

            // The purpose of this check is to ensure that variant holds only one of the supported types
            if (valueType->equals(dot::typeof<string>()))        return Variant(value);
            if (valueType->equals(dot::typeof<double>()))        return Variant(Double::Parse(value));
            if (valueType->equals(dot::typeof<bool>()))          return Variant(Bool::Parse(value));
            if (valueType->equals(dot::typeof<int>()))           return Variant(Int::Parse(value));
            if (valueType->equals(dot::typeof<int64_t>()))       return Variant(Long::Parse(value));
            if (valueType->equals(dot::typeof<local_date>()))     return Variant(LocalDate::Parse(value));
            if (valueType->equals(dot::typeof<local_time>()))     return Variant(LocalTime::Parse(value));
            if (valueType->equals(dot::typeof<local_minute>()))   return Variant(LocalMinute::Parse(value));
            if (valueType->equals(dot::typeof<local_date_time>())) return Variant(LocalDateTime::Parse(value));
            if (valueType->is_enum)                               return Variant(Enum::Parse(valueType, value));

            // Error message if any other type
            throw dot::exception(GetWrongTypeErrorMessage(T()));
        }

    private: // PRIVATE

        static dot::string GetWrongTypeErrorMessage(dot::object value);
    };
}
