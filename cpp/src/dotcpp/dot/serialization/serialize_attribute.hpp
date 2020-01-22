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
#include <dot/serialization/tree_writer_base.hpp>

namespace dot
{

    class serialize_class_attribute_impl; using serialize_class_attribute = ptr<serialize_class_attribute_impl>;

    /// Attribute sets custom serializator for type
    /// Constructs from method that accepts tree writer base and object
    class DOT_CLASS serialize_class_attribute_impl : public attribute_impl
    {

    public:
        typedef void(*serializer_func_type)(dot::tree_writer_base, dot::object);

        friend serialize_class_attribute make_serialize_class_attribute(serializer_func_type);

        void serialize(tree_writer_base writer, dot::object obj);

    public: // REFLECTION

        static type typeof();
        type get_type() override;

    private:

        serialize_class_attribute_impl(serializer_func_type serializer)
            : serializer_(serializer)
        {}

        serializer_func_type serializer_;
    };

    inline serialize_class_attribute make_serialize_class_attribute(serialize_class_attribute_impl::serializer_func_type serializer)
    {
        return new serialize_class_attribute_impl(serializer);
    }
}