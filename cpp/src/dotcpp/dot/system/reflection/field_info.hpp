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
    class field_info_base_impl; using field_info = ptr<field_info_base_impl>;

    /// Discovers the attributes of a field and provides access to field metadata.
    class field_info_base_impl : public member_info_impl
    {
        typedef field_info_base_impl self;

    public: // METHODS

        /// Gets the type of this field.
        type field_type;

        /// A string representing the name of the current type.
        virtual string to_string() override { return "field_info"; }

        /// Returns the field value of a specified object.
        virtual object get_value(object obj) = 0;

        /// Sets the field value of a specified object.
        virtual void set_value(object obj, object value) = 0;

    protected: // CONSTRUCTORS

        /// Create from field name, declaring type, field type,
        /// and base class for the pointer to field.
        ///
        /// This constructor is protected. It is used by derived classes only.
        field_info_base_impl(string name, type declaring_type, type field_type)
            : member_info_impl(name, declaring_type)
        {
            this->field_type = field_type;
        }
    };

    /// Implementation of field_info for field defined as a field (member variable).
    template <class field_type_t, class class_>
    class field_info_impl : public field_info_base_impl
    {
        typedef field_type_t class_::* field_ptr_type;

        template <class field_type_, class class__>
        friend field_info make_field_info(string, type, type, field_type_ class__::*);

    public: // FIELDS

        /// Pointer to field defined as a field.
        field_ptr_type field_;

    private: // CONSTRUCTORS

        /// Create from field name, declaring type, field type,
        /// and pointer to field defined as a field (member variable).
        ///
        /// This constructor is private. Use make_field_info(...)
        /// function with matching signature instead.
        field_info_impl(string name, type declaring_type, type field_type, field_ptr_type field)
            : field_info_base_impl(name, declaring_type, field_type)
            , field_(field)
        {}

    private: // METHODS

        /// Returns the field value of a specified object.
        virtual object get_value(object obj) override
        {
            return (*ptr<class_>(obj)).*field_;
        }

        /// Sets the field value of a specified object.
        virtual void set_value(object obj, object value) override
        {
            (*ptr<class_>(obj)).*field_ = (field_type_t)value;
        }
    };

    /// Create from field name, declaring type, field type,
    /// and pointer to field defined as a field (member variable).
    template <class field_type_t, class class_>
    field_info make_field_info(string name, type declaring_type, type field_type, field_type_t class_::* field)
    {
        return new field_info_impl<field_type_t, class_>(name, declaring_type, field_type, field);
    }

    class type_builder_impl;

}