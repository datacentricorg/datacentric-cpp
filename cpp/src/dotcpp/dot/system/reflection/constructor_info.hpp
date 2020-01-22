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
    class constructor_info_impl; using constructor_info = Ptr<constructor_info_impl>;
    class type_impl; using type = Ptr<type_impl>;

    /// Obtains information about the attributes of a constructor and provides access to constructor metadata.
    class constructor_info_impl : public member_info_impl
    {
        friend class type_builder_impl;

    public: // METHODS

        /// A String representing the name of the current type.
        inline virtual String to_string() override;

        /// Gets the parameters of this constructor.
        inline virtual list<parameter_info> get_parameters();

        /// Invokes specified constructor with given parameters.
        virtual Object invoke(list<Object>) = 0;

    protected: // CONSTRUCTORS

        list<parameter_info> parameters_;

        /// Create from declaring type
        ///
        /// This constructor is protected. It is used by derived classes only.
        inline constructor_info_impl(type declaring_type, list<Attribute> custom_attributes);
    };

    /// Obtains information about the attributes of a constructor and provides access to constructor metadata.
    template <class class_, class ... args>
    class member_constructor_info_impl : public constructor_info_impl
    {

        friend class type_builder_impl;
        typedef class_(*ctor_type)(args...);

    private: // FIELDS

        ctor_type ptr_;

    public: // METHODS

        /// A String representing the name of the current type.
        inline virtual String to_string() override;

        /// Invokes the constructor reflected by this constructor_info instance.
        template <int ... I>
        inline Object invoke_impl(list<Object> params, detail::IndexSequence<I...>);

        /// Invokes the constructor reflected by this constructor_info instance.
        inline virtual Object invoke(list<Object> params);

    private: // CONSTRUCTORS

        /// Create from declaring type, and pointer to constructor.
        ///
        /// This constructor is private. Use make_constructor_info(...)
        /// function with matching signature instead.
        inline member_constructor_info_impl(type declaring_type, ctor_type p, list<Attribute> custom_attributes);
    };
}
