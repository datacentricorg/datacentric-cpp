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

    class SerializeClassAttributeImpl; using SerializeClassAttribute = Ptr<SerializeClassAttributeImpl>;

    /// Attribute sets custom serializator for type
    /// Constructs from method that accepts tree writer base and Object
    class DOT_CLASS SerializeClassAttributeImpl : public AttributeImpl
    {

    public:
        typedef void(*serializer_func_type)(dot::tree_writer_base, dot::Object);

        friend SerializeClassAttribute make_serialize_class_attribute(serializer_func_type);

        void serialize(tree_writer_base writer, dot::Object obj);

    public: // REFLECTION

        static Type typeof();
        Type get_type() override;

    private:

        SerializeClassAttributeImpl(serializer_func_type serializer)
            : serializer_(serializer)
        {}

        serializer_func_type serializer_;
    };

    inline SerializeClassAttribute make_serialize_class_attribute(SerializeClassAttributeImpl::serializer_func_type serializer)
    {
        return new SerializeClassAttributeImpl(serializer);
    }


    class SerializeFieldAttributeImpl; using SerializeFieldAttribute = Ptr<SerializeFieldAttributeImpl>;

    /// Attribute sets custom serializator for field
    /// Constructs from method that accepts tree writer base and Object
    class DOT_CLASS SerializeFieldAttributeImpl : public AttributeImpl
    {

    public:
        typedef void(*serializer_func_type)(dot::tree_writer_base, dot::Object);

        friend SerializeFieldAttribute make_serialize_field_attribute(serializer_func_type);

        void serialize(tree_writer_base writer, dot::Object obj);

    public: // REFLECTION

        static Type typeof();
        Type get_type() override;

    private:

        SerializeFieldAttributeImpl(serializer_func_type serializer)
            : serializer_(serializer)
        {}

        serializer_func_type serializer_;
    };

    inline SerializeFieldAttribute make_serialize_field_attribute(SerializeFieldAttributeImpl::serializer_func_type serializer)
    {
        return new SerializeFieldAttributeImpl(serializer);
    }
}