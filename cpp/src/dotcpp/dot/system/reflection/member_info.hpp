﻿/*
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

#include <dot/system/attribute.hpp>
#include <dot/system/object.hpp>
#include <dot/system/collections/generic/list.hpp>

namespace dot
{
    class member_info_impl; using member_info = ptr<member_info_impl>;
    class member_info_data_impl; using member_info_data = ptr<member_info_data_impl>;
    class type_impl; using type = ptr<type_impl>;

    /// Data for member_info.
    class DOT_CLASS member_info_data_impl : public virtual object_impl
    {
        typedef member_info_data_impl self;

    public: // PROPERTIES

        /// Gets the name of the current member.
        string name; // TODO - replace by method

    protected: // CONSTRUCTORS

        /// Create an empty instance of member_info_data.
        ///
        /// This constructor is protected. It is used by derived classes only.
        member_info_data_impl() = default;
    };

    /// Obtains information about the attributes of a member and provides access to member metadata.
    class member_info_impl : public virtual object_impl
    {
        typedef member_info_impl self;

    private: // FIELDS

        list<attribute> custom_attributes_;

    public: // METHODS

        /// Gets the name of the current member.
        string name; // TODO - convert to method

        /// Gets the class that declares this member.
        type declaring_type; // TODO - convert to method

        /// Gets a collection that contains this member's custom attributes.
        list<attribute> get_custom_attributes(bool) { return custom_attributes_; }

        list<attribute> get_custom_attributes(dot::type attr_type, bool);

        /// A string representing the name of the current type.
        virtual string to_string() override { return "member_info"; }

    protected: // CONSTRUCTORS

        /// Create from property name and declaring type.
        ///
        /// This constructor is protected. It is used by derived classes only.
        member_info_impl(const string& name, type declaring_type, list<attribute> custom_attributes)
            : custom_attributes_(custom_attributes)
        {
            this->name = name;
            this->declaring_type = declaring_type;
        }
    };
}
