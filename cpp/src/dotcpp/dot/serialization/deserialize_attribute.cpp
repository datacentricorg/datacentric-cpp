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
#include <dot/serialization/deserialize_attribute.hpp>

namespace dot
{
    Object DeserializeClassAttributeImpl::deserialize(Object value, dot::Type type)
    {
        return deserializer_(value, type);
    }

    Type DeserializeClassAttributeImpl::typeof()
    {
        static Type result = []()->Type
        {
            Type t = make_type_builder<DeserializeClassAttributeImpl>("dot", "DeserializeClassAttribute")
                ->build();
            return t;
        }();
        return result;
    }

    Type DeserializeClassAttributeImpl::get_type()
    {
        return typeof();
    }

    void DeserializeFieldAttributeImpl::deserialize(Object value, FieldInfo field, Object obj)
    {
        deserializer_(value, field, obj);
    }

    Type DeserializeFieldAttributeImpl::typeof()
    {
        static Type result = []()->Type
        {
            Type t = make_type_builder<DeserializeFieldAttributeImpl>("dot", "DeserializeFieldAttribute")
                ->build();
            return t;
        }();
        return result;
    }

    Type DeserializeFieldAttributeImpl::get_type()
    {
        return typeof();
    }
}
