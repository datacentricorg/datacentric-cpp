/*
Copyright (C) 2015-present The DotCpp Authors.

This file is part of .C++, a native C++ implementation of
popular .NET class library APIs developed to facilitate
code reuse between C# and C++.

    http://github.com/dotcpp/dotcpp (source)
    http://dotcpp.org (documentation)

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

#include <dot/declare.hpp>
#include <dot/detail/enum_macro.hpp>
#include <dot/system/type.hpp>
#include <dot/system/collections/generic/dictionary.hpp>

namespace dot
{
    /// EnumBase wrapper for use in data structures.
    /// Provides the base class for enumerations.
    class DOT_CLASS EnumBase
    {
        typedef EnumBase self;

    public:
        typedef self element_type;

    protected: // FIELDS

        int value_ = 0;

    public: // CONSTRUCTORS

        /// Default constructor.
        EnumBase() = default;

        /// Create from int.
        EnumBase(int value) : value_(value) {}

        /// Copy constructor.
        EnumBase(const EnumBase& rhs) : value_(rhs.value_) {}

        /// Constructor from Object.
        EnumBase(Object rhs)
        {
            value_ = dynamic_cast<EnumBase&>(*rhs).value_;
        }

    public: // METHODS

        /// Converts the value of this instance to its equivalent String representation.
        String to_string();

        /// Returns the hash code for the value of this instance.
        size_t hash_code();

        /// Returns a value indicating whether this instance is equal to a specified Object.
        bool equals(Object obj);

    public: // STATIC

        /// Converts the String representation of the name or numeric value of
        /// one or more enumerated constants to an equivalent enumerated Object.
        static Object parse(Type enum_type, String value);

    protected: // PROTECTED

        virtual dictionary<String, int> get_enum_map() = 0;

    public: // OPERATORS

        /// Convert to int.
        operator int() const { return value_; }

        /// Assign int to EnumBase.
        EnumBase& operator=(int rhs) { value_ = rhs; return *this; }

        /// Copy assign.
        EnumBase& operator=(const EnumBase& rhs) { value_ = rhs.value_; return *this; }

    public:

        DOT_TYPE_BEGIN("dot", "enum")
            ->is_enum()
        DOT_TYPE_END()
    };
}
