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
#include <fmt/core.h>
#include <dot/system/string.hpp>
#include <dot/system/environment.hpp>

namespace dot
{
    class StringBuilderImpl; using StringBuilder = Ptr<StringBuilderImpl>;

    /// Builds a String incrementally.
    class StringBuilderImpl final : public virtual ObjectImpl, public std::string
    {
        typedef std::string base;
        friend StringBuilder make_string_builder();

    private: // CONSTRUCTORS

        /// Initializes an empty instance of the StringBuilder class.
        ///
        /// This constructor is private. Use make_string_builder(...)
        /// function with matching signature instead.
        StringBuilderImpl() = default;

    public: // METHODS

        /// A String representing the current type.
        virtual String to_string() override
        {
            // Returns a copy of the String, not a reference to the same String
            // Further changes to StringBuilder will not affect the previously
            // returned String.
            return make_string(*this);
        }

        /// Appends the String returned by processing a composite format String, which contains
        /// zero or more format items, to this instance. Each format item is replaced by the
        /// String representation of a corresponding Object argument.
        template <typename First, typename ...Args>
        void append_format(const String& format, const First& f, const Args& ...args)
        {
            *this += *String::format(format, f, args...);
        }

        /// Appends the String representation of a specified Object to this instance.
        template <typename T>
        void append(const T& arg)
        {
            *this += *String::format("{0}", arg);
        }

        /// Appends a copy of a specified String and the default
        /// line terminator, to the end of this instance.
        template <typename ...Args>
        void append_line(const Args& ...args)
        {
            append(args...);
            append_line();
        }

        /// Appends the default line terminator, to the end of this instance.
        void append_line()
        {
            *this += *Environment::make_line;
        }

        /// Removes all characters from the current StringBuilder instance.
        void clear()
        {
            this->clear();
        }
    };

    /// Initializes an empty instance of the StringBuilder class.
    inline StringBuilder make_string_builder() { return new StringBuilderImpl(); }
}
