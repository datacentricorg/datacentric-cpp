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
    byte_array_impl::byte_array_impl(int size)
        : array_(size)
    {}

    byte_array_impl::byte_array_impl(const char* value, int size)
        : array_(value, value + size)
    {}

    byte_array_impl::byte_array_impl(const std::vector<char>& obj)
        : array_(obj)
    {}

    byte_array_impl::byte_array_impl(std::vector<char>&& obj)
        : array_(std::move(obj))
    {}

    int byte_array_impl::get_length()
    {
        return array_.size();
    }

    char* byte_array_impl::get_data()
    {
        return array_.data();
    }

    char& byte_array_impl::operator[](int i)
    {
        return array_[i];
    }

    type byte_array_impl::typeof()
    {
        static type result = []()->type
        {
            type t = make_type_builder<attribute_impl>("dot", "byte_array")
                ->build();
            return t;
        }();
        return result;
    }

    type byte_array_impl::get_type()
    {
        return typeof();
    }
}
