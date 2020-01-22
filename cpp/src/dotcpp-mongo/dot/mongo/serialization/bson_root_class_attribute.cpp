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
#include <dot/mongo/serialization/bson_root_class_attribute.hpp>

namespace dot
{
    type bson_root_class_attribute_impl::typeof()
    {
        static type result = []()->type
        {
            type t = make_type_builder<bson_root_class_attribute_impl>("dot", "bson_root_class_attribute")
                ->build();
            return t;
        }();
        return result;
    }

    type bson_root_class_attribute_impl::get_type()
    {
        return typeof();
    }
}
