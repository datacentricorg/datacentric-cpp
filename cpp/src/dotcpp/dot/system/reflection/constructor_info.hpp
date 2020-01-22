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

#include <dot/system/reflection/member_info.hpp>
#include <dot/system/reflection/parameter_info.hpp>
#include <dot/system/exception.hpp>

namespace dot
{
    class ConstructorInfoImpl; using ConstructorInfo = Ptr<ConstructorInfoImpl>;
    class TypeImpl; using Type = Ptr<TypeImpl>;

    /// Obtains information about the attributes of a constructor and provides access to constructor metadata.
    class ConstructorInfoImpl : public MemberInfoImpl
    {
        friend class TypeBuilderImpl;

    public: // METHODS

        /// A String representing the name of the current Type.
        inline virtual String to_string() override;

        /// Gets the parameters of this constructor.
        inline virtual list<ParameterInfo> get_parameters();

        /// Invokes specified constructor with given parameters.
        virtual Object invoke(list<Object>) = 0;

    protected: // CONSTRUCTORS

        list<ParameterInfo> parameters_;

        /// Create from declaring Type
        ///
        /// This constructor is protected. It is used by derived classes only.
        inline ConstructorInfoImpl(Type declaring_type, list<Attribute> custom_attributes);
    };

    /// Obtains information about the attributes of a constructor and provides access to constructor metadata.
    template <class Class, class ... Args>
    class MemberConstructorInfoImpl : public ConstructorInfoImpl
    {

        friend class TypeBuilderImpl;
        typedef Class(*CtorType)(Args...);

    private: // FIELDS

        CtorType ptr_;

    public: // METHODS

        /// A String representing the name of the current Type.
        inline virtual String to_string() override;

        /// Invokes the constructor reflected by this ConstructorInfo instance.
        template <int ... I>
        inline Object invoke_impl(list<Object> params, detail::IndexSequence<I...>);

        /// Invokes the constructor reflected by this ConstructorInfo instance.
        inline virtual Object invoke(list<Object> params);

    private: // CONSTRUCTORS

        /// Create from declaring Type, and pointer to constructor.
        ///
        /// This constructor is private. Use make_constructor_info(...)
        /// function with matching signature instead.
        inline MemberConstructorInfoImpl(Type declaring_type, CtorType p, list<Attribute> custom_attributes);
    };
}
