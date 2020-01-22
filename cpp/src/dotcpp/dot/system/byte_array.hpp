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

        /// Compares byte arrays. Size of arrays should be equal.
        int compare(byte_array other);

        /// Compares byte arrays. Size of arrays should be equal.
        int compare(char* other);

        /// Copy length bytes of src to this.
        void copy(byte_array src, int length);

        /// Copy length bytes of src with src_offset to this.
        void copy(byte_array src, int src_offset, int length);

        /// Copy length bytes of src to this with offset.
        void copy(int offset, byte_array src, int length);

        /// Copy length bytes of src with src_offset to this with offset.
        void copy(int offset, byte_array src, int src_offset, int length);

        /// Copy length bytes of src to this.
        void copy(const char* src, int length);

        /// Copy length bytes of src to this with offset.
        void copy(int offset, const char* src, int length);

        /// Copy primitive type value to this.
        template <typename T>
        void copy_value(T value)
        {
            copy_value(0, value);
        }

        /// Copy primitive type value to this with offset.
        template <typename T>
        void copy_value(int offset, T value)
        {
            const int value_size = sizeof(T);
            if (offset + value_size > get_length())
                throw dot::exception("Not enough byte_array size to copy.");

            copy_value(get_data() + offset, value);
        }

        /// Copy primitive type value to dist.
        template <typename T>
        static void copy_value(char* dist, T value)
        {
            const int value_size = sizeof(T);
            for (int i = 0; i < value_size; ++i)
            {
                dist[i] = (char) (value >> (8 * (value_size - i - 1)) & 0xff);
            }
        }

        /// Convert byte array to primitive type.
        template <typename T>
        T to_primitive()
        {
            return to_primitive<T>(0);
        }

        /// Convert byte array with offset to primitive type.
        template <typename T>
        T to_primitive(int offset)
        {
            const int value_size = sizeof(T);
            if (offset + value_size > get_length())
                throw dot::exception("Not enough byte_array size to convert.");

            return to_primitive<T>(get_data() + offset);
        }

        /// Convert byte array to primitive type.
        template <typename T>
        static T to_primitive(char* src)
        {
            const int value_size = sizeof(T);
            T res = 0;

            for (int i = 0; i < value_size; ++i)
            {
                res <<= 8;
                res |= src[i];
            }

            return res;
        }

    public: // OPERATORS

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
