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
#include <dot/noda_time/local_date_util.hpp>
#include <dot/noda_time/local_time_util.hpp>
#include <dot/noda_time/local_minute_util.hpp>
#include <dot/noda_time/local_date_time_util.hpp>
#include <dot/system/Enum.hpp>
#include <dot/system/object.hpp>
#include <dot/system/string.hpp>
#include <dot/system/type.hpp>
#include <dot/noda_time/local_date_time.hpp>
#include <dot/noda_time/local_minute.hpp>

namespace dc
{
    /// Variant type can hold any atomic value or be empty.
    class DC_CLASS Variant
    {
        typedef Variant self;

    private: // FIELDS

        dot::object value_;

    public: // CONSTRUCTORS

        /// Default constructor.
        Variant();

        /// Create from object of supported types, error message if argument type is unsupported.
        Variant(dot::object value);

    public: // PROPERTIES

        /// Type of the value held by the variant.
        ValueType getValueType()
        {
            if (value_ == nullptr)
                return ValueType::empty;

            dot::type_t valueType = value_->type();

            // The purpose of this check is to ensure that variant holds only one of the supported types
            if (valueType->equals(dot::typeof<dot::string>()))        return ValueType::String;
            if (valueType->equals(dot::typeof<double>()))             return ValueType::Double;
            if (valueType->equals(dot::typeof<bool>()))               return ValueType::Bool;
            if (valueType->equals(dot::typeof<int>()))                return ValueType::Int;
            if (valueType->equals(dot::typeof<int64_t>()))            return ValueType::Long;
            if (valueType->equals(dot::typeof<dot::local_date>()))     return ValueType::local_date;
            if (valueType->equals(dot::typeof<dot::local_time>()))     return ValueType::local_time;
            if (valueType->equals(dot::typeof<dot::local_minute>()))   return ValueType::local_minute;
            if (valueType->equals(dot::typeof<dot::local_date_time>())) return ValueType::local_date_time;
            if (valueType->is_enum)                                    return ValueType::Enum;

            // Error message if any other type, should normally not get to here
            throw dot::exception(GetWrongTypeErrorMessage(value_));
        }

        /// Value held by the variant, which may be null.
        dot::object getValue()
        {
            return value_;
        }

    public: // METHODS

        /// Check if the variant is equal to default constructed object.
        bool is_empty();

        /// Provides alternate serialization of certain value types.
        dot::string to_string();

        /// Hash code is zero for null objects.
        size_t GetHashCode();

        /// Variants are equal when both types and values are equal.
        /// Comparison of doubles is performed with roundoff tolerance.
        bool Equals(const Variant& other);

    public: // OPERATORS

        /// Variants are equal when both types and values are equal.
        /// Comparison of doubles is performed with roundoff tolerance.
        bool operator==(const Variant& other);

        /// Variants are equal when both types and values are equal.
        /// Comparison of doubles is performed with roundoff tolerance.
        bool operator!=(const Variant& other);

    public: // STATIC

        static Variant parse(ValueType valueType, dot::string value);

        template <class T>
        static Variant parse(dot::string value)
        {
            if (dot::string::is_null_or_empty(value))
            {
                // Empty value
                return Variant();
            }

            Type valueType = dot::typeof<T>();

            // The purpose of this check is to ensure that variant holds only one of the supported types
            if (valueType->equals(dot::typeof<string>()))        return Variant(value);
            if (valueType->equals(dot::typeof<double>()))        return Variant(Double::parse(value));
            if (valueType->equals(dot::typeof<bool>()))          return Variant(Bool::parse(value));
            if (valueType->equals(dot::typeof<int>()))           return Variant(Int::parse(value));
            if (valueType->equals(dot::typeof<int64_t>()))       return Variant(Long::parse(value));
            if (valueType->equals(dot::typeof<local_date>()))     return Variant(dot::local_date::parse(value));
            if (valueType->equals(dot::typeof<local_time>()))     return Variant(dot::local_time::parse(value));
            if (valueType->equals(dot::typeof<local_minute>()))   return Variant(dot::local_minute::parse(value));
            if (valueType->equals(dot::typeof<local_date_time>())) return Variant(dot::local_date_time::parse(value));
            if (valueType->is_enum)                               return Variant(Enum::parse(valueType, value));

            // Error message if any other type
            throw dot::exception(GetWrongTypeErrorMessage(T()));
        }

    private: // PRIVATE

        static dot::string GetWrongTypeErrorMessage(dot::object value);
    };
}
