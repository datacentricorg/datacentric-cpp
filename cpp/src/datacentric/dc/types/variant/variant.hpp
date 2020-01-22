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
#include <dc/types/record/value_type.hpp>
#include <dot/noda_time/local_date_util.hpp>
#include <dot/noda_time/local_time_util.hpp>
#include <dot/noda_time/local_minute_util.hpp>
#include <dot/noda_time/local_date_time_util.hpp>
#include <dot/system/enum.hpp>
#include <dot/system/object.hpp>
#include <dot/system/string.hpp>
#include <dot/system/type.hpp>
#include <dot/noda_time/local_date_time.hpp>
#include <dot/noda_time/local_minute.hpp>

namespace dc
{
    /// variant type can hold any atomic value or be empty.
    class DC_CLASS variant
    {
        typedef variant self;

    private: // FIELDS

        dot::object value_;

    public: // CONSTRUCTORS

        /// Default constructor.
        variant();

        /// Create from object of supported types, error message if argument type is unsupported.
        variant(dot::object value);

    public: // PROPERTIES

        /// Type of the value held by the variant.
        ValueType get_value_type()
        {
            if (value_ == nullptr)
                return ValueType::empty;

            dot::type value_type = value_->get_type();

            // The purpose of this check is to ensure that variant holds only one of the supported types
            if (value_type->equals(dot::typeof<dot::string>()))        return ValueType::String;
            if (value_type->equals(dot::typeof<double>()))             return ValueType::Double;
            if (value_type->equals(dot::typeof<bool>()))               return ValueType::Bool;
            if (value_type->equals(dot::typeof<int>()))                return ValueType::Int;
            if (value_type->equals(dot::typeof<int64_t>()))            return ValueType::Long;
            if (value_type->equals(dot::typeof<dot::local_date>()))     return ValueType::local_date;
            if (value_type->equals(dot::typeof<dot::local_time>()))     return ValueType::local_time;
            if (value_type->equals(dot::typeof<dot::local_minute>()))   return ValueType::local_minute;
            if (value_type->equals(dot::typeof<dot::local_date_time>())) return ValueType::local_date_time;
            if (value_type->is_enum)                                    return ValueType::Enum;

            // Error message if any other type, should normally not get to here
            throw dot::exception(get_wrong_type_error_message(value_));
        }

        /// Value held by the variant, which may be null.
        dot::object get_value()
        {
            return value_;
        }

    public: // METHODS

        /// Check if the variant is equal to default constructed object.
        bool is_empty();

        /// Provides alternate serialization of certain value types.
        dot::string to_string();

        /// Hash code is zero for null objects.
        size_t hash_code();

        /// Variants are equal when both types and values are equal.
        /// Comparison of doubles is performed with roundoff tolerance.
        bool equals(const variant& other);

    public: // OPERATORS

        /// Variants are equal when both types and values are equal.
        /// Comparison of doubles is performed with roundoff tolerance.
        bool operator==(const variant& other);

        /// Variants are equal when both types and values are equal.
        /// Comparison of doubles is performed with roundoff tolerance.
        bool operator!=(const variant& other);

    public: // STATIC

        static variant parse(ValueType value_type, dot::string value);

        template <class T>
        static variant parse(dot::string value)
        {
            if (dot::string::is_null_or_empty(value))
            {
                // Empty value
                return variant();
            }

            dot::type value_type = dot::typeof<T>();

            // The purpose of this check is to ensure that variant holds only one of the supported types
            if (value_type->equals(dot::typeof<dot::string>()))        return variant(value);
            if (value_type->equals(dot::typeof<double>()))        return variant(dot::double_impl::parse(value));
            if (value_type->equals(dot::typeof<bool>()))          return variant(dot::bool_impl::parse(value));
            if (value_type->equals(dot::typeof<int>()))           return variant(dot::int_impl::parse(value));
            if (value_type->equals(dot::typeof<int64_t>()))       return variant(dot::long_impl::parse(value));
            if (value_type->equals(dot::typeof<dot::local_date>()))     return variant(dot::local_date_util::parse(value));
            if (value_type->equals(dot::typeof<dot::local_time>()))     return variant(dot::local_time_util::parse(value));
            if (value_type->equals(dot::typeof<dot::local_minute>()))   return variant(dot::local_minute_util::parse(value));
            if (value_type->equals(dot::typeof<dot::local_date_time>())) return variant(dot::local_date_time_util::parse(value));
            if (value_type->is_enum)                               return variant(dot::enum_base::parse(value_type, value));

            // Error message if any other type
            throw dot::exception(get_wrong_type_error_message(T()));
        }

    private: // PRIVATE

        static dot::string get_wrong_type_error_message(dot::object value);
    };
}
