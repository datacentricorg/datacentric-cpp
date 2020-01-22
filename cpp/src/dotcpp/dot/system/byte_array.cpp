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

#include <dot/precompiled.hpp>
#include <dot/implement.hpp>
#include <dot/system/byte_array.hpp>
#include <dot/system/string.hpp>
#include <dot/system/type.hpp>

namespace dot
{
    ByteArrayImpl::ByteArrayImpl(int size)
        : array_(size)
    {}

    ByteArrayImpl::ByteArrayImpl(const char* value, int size)
        : array_(value, value + size)
    {}

    ByteArrayImpl::ByteArrayImpl(const std::vector<char>& obj)
        : array_(obj)
    {}

    ByteArrayImpl::ByteArrayImpl(std::vector<char>&& obj)
        : array_(std::move(obj))
    {}

    ByteArrayImpl::ByteArrayImpl(const std::initializer_list<uint8_t>& obj)
        : array_(obj.begin(), obj.end())
    {}

    int ByteArrayImpl::get_length()
    {
        return array_.size();
    }

    char* ByteArrayImpl::get_data()
    {
        return array_.data();
    }

    uint8_t ByteArrayImpl::get(int i)
    {
        return array_[i];
    }

    int ByteArrayImpl::compare(ByteArray other)
    {
        if (get_length() != other->get_length())
            throw dot::Exception("Size of byte arrays should be equal.");

        return compare(other->get_data());
    }

    int ByteArrayImpl::compare(char* other)
    {
        return memcmp(get_data(), other, get_length());
    }

    void ByteArrayImpl::copy(int offset, ByteArray src)
    {
        // Call copy(int offset, const char* src, int length)
        copy(offset, src->get_data(), src->get_length());
    }

    void ByteArrayImpl::copy(int offset, ByteArray src, int src_offset, int length)
    {
        // Check sizes
        if (src_offset + length > src->get_length())
            throw dot::Exception("Not enough ByteArray size to copy.");

        // Call copy(int offset, const char* src, int length)
        copy(offset, src->get_data() + src_offset, length);
    }

    void ByteArrayImpl::copy(const char* src, int length)
    {
        // Call copy(int offset, const char* src, int length)
        copy(0, src, length);
    }

    void ByteArrayImpl::copy(int offset, const char* src, int length)
    {
        // Check sizes
        if (offset + length > get_length())
            throw dot::Exception("Not enough ByteArray size to copy.");

        // Native bytes copy
        memcpy(get_data() + offset, src, length);
    }

    char& ByteArrayImpl::operator[](int i)
    {
        return array_[i];
    }

    Type ByteArrayImpl::typeof()
    {
        static Type result = []()->Type
        {
            Type t = make_type_builder<ByteArrayImpl>("dot", "ByteArray")
                ->build();
            return t;
        }();
        return result;
    }

    Type ByteArrayImpl::get_type()
    {
        return typeof();
    }
}
