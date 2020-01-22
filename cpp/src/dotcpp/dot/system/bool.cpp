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
#include <dot/system/bool.hpp>
#include <dot/system/string.hpp>
#include <dot/system/type.hpp>

namespace dot
{
    const String BoolImpl::false_string = "False";

    const String BoolImpl::true_string = "True";

    bool BoolImpl::equals(Object obj)
    {
        if (this == &(*obj)) return true;
        if (obj.is<Ptr<BoolImpl>>())
        {
            return value_ == obj.as<Ptr<BoolImpl>>()->value_;
        }

        return false;
    }

    size_t BoolImpl::hash_code()
    {
        return std::hash<bool>()(value_);
    }

    String BoolImpl::to_string()
    {
        return value_ ? BoolImpl::true_string : BoolImpl::false_string;
    }

    type BoolImpl::typeof()
    {
        return dot::typeof<bool>();
    }

    type BoolImpl::get_type()
    {
        return typeof();
    }

    bool BoolImpl::parse(String s)
    {
        if (s == BoolImpl::false_string) return false;
        if (s == BoolImpl::true_string) return true;

        throw Exception("value is not equivalent to true_string or false_string.");
    }
}
