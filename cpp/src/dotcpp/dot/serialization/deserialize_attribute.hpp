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

    class DeserializeClassAttributeImpl; using DeserializeClassAttribute = Ptr<DeserializeClassAttributeImpl>;

    /// Attribute sets custom deserializator for type
    /// Constructs from method that accepts Object value, required type and returns deserialized Object
    class DOT_CLASS DeserializeClassAttributeImpl : public AttributeImpl
    {

    public:
        typedef Object(*deserializer_func_type)(Object, dot::Type);

        friend DeserializeClassAttribute make_deserialize_class_attribute(deserializer_func_type);

        Object deserialize(Object value, dot::Type);

    public: // REFLECTION

        static Type typeof();
        Type get_type() override;

    private:

        DeserializeClassAttributeImpl(deserializer_func_type deserializer)
            : deserializer_(deserializer)
        {}

        deserializer_func_type deserializer_;
    };

    inline DeserializeClassAttribute make_deserialize_class_attribute(DeserializeClassAttributeImpl::deserializer_func_type deserializer)
    {
        return new DeserializeClassAttributeImpl(deserializer);
    }


    class DeserializeFieldAttributeImpl; using DeserializeFieldAttribute = Ptr<DeserializeFieldAttributeImpl>;

    /// Attribute sets custom deserializator for field
    /// Constructs from method that accepts field value, field info, and Data Object
    class DOT_CLASS DeserializeFieldAttributeImpl : public AttributeImpl
    {

    public:
        typedef void(*deserializer_func_type)(Object, FieldInfo, Object);

        friend DeserializeFieldAttribute make_deserialize_field_attribute(deserializer_func_type);

        void deserialize(Object value, FieldInfo field, Object obj);

    public: // REFLECTION

        static Type typeof();
        Type get_type() override;

    private:

        DeserializeFieldAttributeImpl(deserializer_func_type deserializer)
            : deserializer_(deserializer)
        {}

        deserializer_func_type deserializer_;
    };

    inline void ignore_field_deserialization(Object, FieldInfo, Object)
    {
    }

    inline DeserializeFieldAttribute make_deserialize_field_attribute(DeserializeFieldAttributeImpl::deserializer_func_type deserializer)
    {
        return new DeserializeFieldAttributeImpl(deserializer);
    }
}