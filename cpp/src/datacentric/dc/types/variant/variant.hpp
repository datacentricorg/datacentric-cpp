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
    /// Variant type can hold any atomic value or be empty.
    class DC_CLASS Variant
    {
        typedef Variant self;

    private: // FIELDS

        dot::Object value_;

    public: // CONSTRUCTORS

        /// Default constructor.
        Variant();

        /// Create from Object of supported types, error message if argument type is unsupported.
        Variant(dot::Object value);

    public: // PROPERTIES

        /// Type of the value held by the Variant.
        ValueType get_value_type()
        {
            if (value_ == nullptr)
                return ValueType::empty_type;

            dot::Type value_type = value_->get_type();

            // The purpose of this check is to ensure that Variant holds only one of the supported types
            if (value_type->equals(dot::typeof<dot::String>()))        return ValueType::string_type;
            if (value_type->equals(dot::typeof<double>()))             return ValueType::double_type;
            if (value_type->equals(dot::typeof<bool>()))               return ValueType::bool_type;
            if (value_type->equals(dot::typeof<int>()))                return ValueType::int_type;
            if (value_type->equals(dot::typeof<int64_t>()))            return ValueType::long_type;
            if (value_type->equals(dot::typeof<dot::LocalDate>()))     return ValueType::local_date_type;
            if (value_type->equals(dot::typeof<dot::LocalTime>()))     return ValueType::local_time_type;
            if (value_type->equals(dot::typeof<dot::LocalMinute>()))   return ValueType::local_minute_type;
            if (value_type->equals(dot::typeof<dot::LocalDateTime>())) return ValueType::local_date_time_type;
            if (value_type->is_enum())                                   return ValueType::enum_type;

            // Error message if any other type, should normally not get to here
            throw dot::Exception(get_wrong_type_error_message(value_));
        }

        /// Value held by the Variant, which may be null.
        dot::Object get_value()
        {
            return value_;
        }

    public: // METHODS

        /// Check if the Variant is equal to default constructed Object.
        bool is_empty();

        /// Provides alternate serialization of certain value types.
        dot::String to_string();

        /// Hash code is zero for null objects.
        size_t hash_code();

        /// Variants are equal when both types and values are equal.
        /// Comparison of doubles is performed with roundoff tolerance.
        bool equals(const Variant& other);

    public: // OPERATORS

        /// Variants are equal when both types and values are equal.
        /// Comparison of doubles is performed with roundoff tolerance.
        bool operator==(const Variant& other);

        /// Variants are equal when both types and values are equal.
        /// Comparison of doubles is performed with roundoff tolerance.
        bool operator!=(const Variant& other);

    public: // STATIC

        static Variant parse(ValueType value_type, dot::String value);

        template <class T>
        static Variant parse(dot::String value)
        {
            if (dot::String::is_null_or_empty(value))
            {
                // Empty value
                return Variant();
            }

            dot::Type value_type = dot::typeof<T>();

            // The purpose of this check is to ensure that Variant holds only one of the supported types
            if (value_type->equals(dot::typeof<dot::String>()))        return Variant(value);
            if (value_type->equals(dot::typeof<double>()))        return Variant(dot::DoubleImpl::parse(value));
            if (value_type->equals(dot::typeof<bool>()))          return Variant(dot::BoolImpl::parse(value));
            if (value_type->equals(dot::typeof<int>()))           return Variant(dot::IntImpl::parse(value));
            if (value_type->equals(dot::typeof<int64_t>()))       return Variant(dot::LongImpl::parse(value));
            if (value_type->equals(dot::typeof<dot::LocalDate>()))     return Variant(dot::LocalDateUtil::parse(value));
            if (value_type->equals(dot::typeof<dot::LocalTime>()))     return Variant(dot::LocalTimeUtil::parse(value));
            if (value_type->equals(dot::typeof<dot::LocalMinute>()))   return Variant(dot::LocalMinuteUtil::parse(value));
            if (value_type->equals(dot::typeof<dot::LocalDateTime>())) return Variant(dot::LocalDateTimeUtil::parse(value));
            if (value_type->is_enum())                               return Variant(dot::EnumBase::parse(value_type, value));

            // Error message if any other type
            throw dot::Exception(get_wrong_type_error_message(T()));
        }

    private: // PRIVATE

        static dot::String get_wrong_type_error_message(dot::Object value);
    };
}
