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
#include <dot/system/ptr.hpp>
#include <dot/system/type.hpp>

namespace dot
{

    class deserialize_class_attribute_impl; using deserialize_class_attribute = ptr<deserialize_class_attribute_impl>;

    /// Attribute sets custom deserealizator
    class DOT_CLASS deserialize_class_attribute_impl : public attribute_impl
    {

    public:
        typedef object(*deserializer_func_type)(object, dot::type);

        friend deserialize_class_attribute make_deserialize_class_attribute(deserializer_func_type);

        object deserialize(object value, dot::type);

    public: // REFLECTION

        static type typeof();
        type get_type() override;

    private:

        deserialize_class_attribute_impl(deserializer_func_type deserializer)
            : deserializer_(deserializer)
        {}

        deserializer_func_type deserializer_;
    };

    inline deserialize_class_attribute make_deserialize_class_attribute(deserialize_class_attribute_impl::deserializer_func_type deserializer)
    {
        return new deserialize_class_attribute_impl(deserializer);
    }


    class deserialize_field_attribute_impl; using deserialize_field_attribute = ptr<deserialize_field_attribute_impl>;

    /// Attribute sets custom deserealizator
    class DOT_CLASS deserialize_field_attribute_impl : public attribute_impl
    {

    public:
        typedef void(*deserializer_func_type)(object, field_info, object);

        friend deserialize_field_attribute make_deserialize_field_attribute(deserializer_func_type);

        void deserialize(object value, field_info field, object obj);

    public: // REFLECTION

        static type typeof();
        type get_type() override;

    private:

        deserialize_field_attribute_impl(deserializer_func_type deserializer)
            : deserializer_(deserializer)
        {}

        deserializer_func_type deserializer_;
    };

    inline void ignore_field_deserealization(object, field_info, object)
    {
    }

    inline deserialize_field_attribute make_deserialize_field_attribute(deserialize_field_attribute_impl::deserializer_func_type deserializer)
    {
        return new deserialize_field_attribute_impl(deserializer);
    }
}