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
#include <dot/system/object_impl.hpp>

namespace dot
{
    /// Wrapper around bool to make it convertible to Object (boxing).
    class DOT_CLASS BoolImpl : public virtual ObjectImpl
    {
        friend Object;
        bool value_;

    public: // CONSTRUCTORS

        /// Create from value (box).
        BoolImpl(bool value) : value_(value) {}

    public: //  CONSTANTS

        /// Sentinel value representing uninitialized state.
        static constexpr int empty = INT32_MIN;

        /// Represents the boolean value false as a String. This field is read-only.
        static const String false_string;

        /// Represents the boolean value true as a String. This field is read-only.
        static const String true_string;

    public: // STATIC

        /// Converts the specified String representation of a logical value to its boolean equivalent.
        static bool parse(String s);

    public: // METHODS

        /// Returns a value indicating whether this instance is equal to a specified Object.
        bool equals(Object obj) override;

        /// Returns the hash code for this instance.
        virtual size_t hash_code() override;

        /// Converts the value of this instance to its equivalent String representation (either "True" or "False").
        virtual String to_string() override;

        static Type typeof();
        Type get_type() override;
    };
}
