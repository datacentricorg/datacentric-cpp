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
    class byte_array_impl; using byte_array = ptr<byte_array_impl>;

    /// Represents array of bytes.
    class DOT_CLASS byte_array_impl : public object_impl
    {
        friend byte_array make_byte_array();
        friend byte_array make_byte_array(int size);
        friend byte_array make_byte_array(const char* value, int size);
        friend byte_array make_byte_array(const std::vector<char>& obj);
        friend byte_array make_byte_array(std::vector<char>&& obj);

    private:

        std::vector<char> array_;

    private: // CONSTRUCTORS

        /// Create empty array.
        byte_array_impl() = default;

        /// Create array with specific size.
        byte_array_impl(int size);

        /// Construct from byte array using copy semantics.
        byte_array_impl(const char* value, int size);

        /// Construct from vector using deep copy semantics.
        byte_array_impl(const std::vector<char>& obj);

        /// Construct from vector using move semantics.
        byte_array_impl(std::vector<char>&& obj);

    public: // METHODS

        /// The number of items contained in the list.
        int get_length();

        /// Gets raw byte array.
        char* get_data();

        /// Gets or sets the element at the specified index.
        char& operator[](int i);

    public: // REFLECTION

        static type typeof();
        type get_type() override;
    };

    inline byte_array make_byte_array() { return new byte_array_impl(); }

    inline byte_array make_byte_array(int size) { return new byte_array_impl(size); }

    inline byte_array make_byte_array(const char* value, int size) { return new byte_array_impl(value, size); }

    inline byte_array make_byte_array(const std::vector<char>& obj) { return new byte_array_impl(obj); }

    inline byte_array make_byte_array(std::vector<char>&& obj) { return new byte_array_impl(std::forward<std::vector<char>>(obj)); }
}
