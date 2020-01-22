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

#include <dot/system/attribute.hpp>
#include <dot/system/object.hpp>
#include <dot/system/collections/generic/list.hpp>

namespace dot
{
    class MemberInfoImpl; using MemberInfo = Ptr<MemberInfoImpl>;
    class TypeImpl; using Type = Ptr<TypeImpl>;

    /// Obtains information about the attributes of a member and provides access to member metadata.
    class DOT_CLASS MemberInfoImpl : public virtual ObjectImpl
    {
        typedef MemberInfoImpl self;

    private: // FIELDS

        String name_;
        Type declaring_type_;
        List<Attribute> custom_attributes_;

    public: // METHODS

        /// Gets the name of the current member.
        String name() const { return name_; }

        /// Gets the class that declares this member.
        Type declaring_type() const { return declaring_type_; }

        /// Gets a collection that contains this member's custom attributes.
        List<Attribute> get_custom_attributes(bool) { return custom_attributes_; }

        List<Attribute> get_custom_attributes(dot::Type attr_type, bool);

        /// A String representing the name of the current Type.
        virtual String to_string() override { return "MemberInfo"; }

    protected: // CONSTRUCTORS

        /// Create from property name and declaring Type.
        ///
        /// This constructor is protected. It is used by derived classes only.
        MemberInfoImpl(const String& name, Type declaring_type, List<Attribute> custom_attributes)
            : name_(name)
            , declaring_type_(declaring_type)
            , custom_attributes_(custom_attributes)
        {}
    };
}
