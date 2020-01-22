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

#include <dot/mongo/precompiled.hpp>
#include <dot/mongo/implement.hpp>
#include <dot/mongo/serialization/filter_token_serialization_attribute.hpp>

namespace dot
{
    Object filter_token_serialization_attribute_impl::serialize(Object obj)
    {
        return serializer_(obj);
    }

    type filter_token_serialization_attribute_impl::typeof()
    {
        static type result = []()->type
        {
            type t = make_type_builder<filter_token_serialization_attribute_impl>("dot", "filter_token_serialization_attribute")
                ->build();
            return t;
        }();
        return result;
    }

    type filter_token_serialization_attribute_impl::get_type()
    {
        return typeof();
    }
}
