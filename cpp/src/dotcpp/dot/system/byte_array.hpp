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
    class ByteArrayImpl; using ByteArray = Ptr<ByteArrayImpl>;

    /// Represents array of bytes.
    class DOT_CLASS ByteArrayImpl : public ObjectImpl
    {
        friend ByteArray make_byte_array();
        friend ByteArray make_byte_array(int size);
        friend ByteArray make_byte_array(const char* value, int size);
        friend ByteArray make_byte_array(const std::vector<char>& obj);
        friend ByteArray make_byte_array(std::vector<char>&& obj);
        friend ByteArray make_byte_array(const std::initializer_list<uint8_t>& obj);

    private:

        std::vector<char> array_;

    private: // CONSTRUCTORS

        /// Create empty array.
        ByteArrayImpl() = default;

        /// Create array with specific size.
        ByteArrayImpl(int size);

        /// Construct from byte array using copy semantics.
        ByteArrayImpl(const char* value, int size);

        /// Construct from vector using deep copy semantics.
        ByteArrayImpl(const std::vector<char>& obj);

        /// Construct from vector using move semantics.
        ByteArrayImpl(std::vector<char>&& obj);

        /// Construct from initializer_list.
        ByteArrayImpl(const std::initializer_list<uint8_t>& obj);

    public: // METHODS

        /// The number of items contained in the list.
        int get_length();

        /// Gets raw byte array.
        char* get_data();

        /// Gets element at the specified index.
        uint8_t get(int i);

        /// Compares byte arrays. Size of arrays should be equal.
        int compare(ByteArray other);

        /// Compares byte arrays. Size of arrays should be equal.
        int compare(char* other);

        /// Copy length bytes of src to this with offset.
        void copy(int offset, ByteArray src);

        /// Copy length bytes of src with src_offset to this with offset.
        void copy(int offset, ByteArray src, int src_offset, int length);

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
                throw dot::Exception("Not enough ByteArray size to copy.");

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
                throw dot::Exception("Not enough ByteArray size to convert.");

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
                res |= (uint8_t) src[i];
            }

            return res;
        }

    public: // OPERATORS

        /// Gets or sets the element at the specified index.
        char& operator[](int i);

    public: // REFLECTION

        static Type typeof();
        Type get_type() override;
    };

    inline ByteArray make_byte_array() { return new ByteArrayImpl(); }

    inline ByteArray make_byte_array(int size) { return new ByteArrayImpl(size); }

    inline ByteArray make_byte_array(const char* value, int size) { return new ByteArrayImpl(value, size); }

    inline ByteArray make_byte_array(const std::vector<char>& obj) { return new ByteArrayImpl(obj); }

    inline ByteArray make_byte_array(std::vector<char>&& obj) { return new ByteArrayImpl(std::forward<std::vector<char>>(obj)); }

    inline ByteArray make_byte_array(const std::initializer_list<uint8_t>& obj) { return new ByteArrayImpl(obj); }
}
