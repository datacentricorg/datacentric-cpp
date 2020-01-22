﻿/*
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
#include <dot/detail/const_string_base.hpp>
#include <dot/system/object_impl.hpp>

namespace dot
{
    /// Wrapper around char to make it convertible to Object (boxing).
    class CharImpl : public virtual ObjectImpl
    {
        friend Object;
        char value_;

    public: // CONSTRUCTORS

        /// Create from value (box).
        CharImpl(char value) : value_(value) {}

    public: // METHODS

        /// Returns a value indicating whether this instance is equal to a specified Object.
        bool equals(Object obj) override;

        /// Returns the hash code for this instance.
        virtual size_t hash_code() override;

        /// Converts the numeric value of this instance to its equivalent String representation.
        virtual String to_string() override;

        /// Indicates whether a Unicode character is categorized as white space.
        static bool is_white_space(char c);

    public: // REFLECTION

        static type typeof();
        type get_type() override;
    };
}
