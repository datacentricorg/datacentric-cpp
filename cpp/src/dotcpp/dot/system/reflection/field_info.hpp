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

#include <dot/system/exception.hpp>
#include <dot/system/reflection/member_info.hpp>

namespace dot
{
    class FieldInfoBaseImpl; using FieldInfo = Ptr<FieldInfoBaseImpl>;

    /// Discovers the attributes of a field and provides access to field metadata.
    class DOT_CLASS FieldInfoBaseImpl : public MemberInfoImpl
    {
        typedef FieldInfoBaseImpl self;

    private: // FIELDS

        Type field_type_;

    public: // METHODS

        /// Gets the Type of this field.
        Type field_type() const { return field_type_; }

        /// A string representing the name of the current Type.
        virtual String to_string() override { return "FieldInfo"; }

        /// Returns the field value of a specified Object.
        virtual Object get_value(Object obj) = 0;

        /// Sets the field value of a specified Object.
        virtual void set_value(Object obj, Object value) = 0;

    protected: // CONSTRUCTORS

        /// Create from field name, declaring Type, field Type,
        /// and base class for the pointer to field.
        ///
        /// This constructor is protected. It is used by derived classes only.
        FieldInfoBaseImpl(String name, Type declaring_type, Type field_type, list<Attribute> custom_attributes)
            : MemberInfoImpl(name, declaring_type, custom_attributes)
            , field_type_(field_type)
        {}
    };

    /// Implementation of FieldInfo for field defined as a field (member variable).
    template <class FieldType, class Class>
    class FieldInfoImpl : public FieldInfoBaseImpl
    {
        typedef FieldType Class::* FieldPtrType;

        template <class FieldType_, class Class_>
        friend FieldInfo make_field_info(String, Type, Type, FieldType_ Class_::*, list<Attribute>);

    public: // FIELDS

        /// Pointer to field defined as a field.
        FieldPtrType field_;

    private: // CONSTRUCTORS

        /// Create from field name, declaring Type, field Type,
        /// and pointer to field defined as a field (member variable).
        ///
        /// This constructor is private. Use make_field_info(...)
        /// function with matching signature instead.
        FieldInfoImpl(String name, Type declaring_type, Type field_type, FieldPtrType field, list<Attribute> custom_attributes)
            : FieldInfoBaseImpl(name, declaring_type, field_type, custom_attributes)
            , field_(field)
        {}

    private: // METHODS

        /// Returns the field value of a specified Object.
        virtual Object get_value(Object obj) override
        {
            return (*Ptr<Class>(obj)).*field_;
        }

        /// Sets the field value of a specified Object.
        virtual void set_value(Object obj, Object value) override
        {
            (*Ptr<Class>(obj)).*field_ = (FieldType)value;
        }
    };

    /// Create from field name, declaring Type, field Type,
    /// and pointer to field defined as a field (member variable).
    template <class FieldType, class Class>
    FieldInfo make_field_info(String name, Type declaring_type, Type field_type, FieldType Class::* field, list<Attribute> custom_attributes)
    {
        return new FieldInfoImpl<FieldType, Class>(name, declaring_type, field_type, field, custom_attributes);
    }

    class TypeBuilderImpl;
}
