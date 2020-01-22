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
#include <dot/detail/traits.hpp>

namespace dot
{
    class method_info_impl; using method_info = Ptr<method_info_impl>;
    class type_impl; using type = Ptr<type_impl>;

    /// Obtains information about the attributes of a method and provides access to method metadata.
    class DOT_CLASS method_info_impl : public member_info_impl
    {
        friend class type_builder_impl;

        typedef method_info_impl self;

    public: // METHODS

        /// A String representing the name of the current type.
        inline virtual String to_string() override;

        /// Gets the parameters of this method.
        inline virtual list<parameter_info> get_parameters();

        /// Invokes specified method with given parameters.
        virtual Object invoke(Object, list<Object>) = 0;

        /// Gets the return type of this method.
        type return_type();

    protected: // FIELDS

        type return_type_;
        list<parameter_info> parameters_;

    protected: // CONSTRUCTORS

        /// Create from method name, declaring type, return type.
        ///
        /// This constructor is protected. It is used by derived classes only.
        inline method_info_impl(const String& name, type declaring_type, type return_type, list<Attribute> custom_attributes);
    };

    /// Obtains information about the attributes of a non-static method and provides access to method metadata.
    template <class class_, class return_t, class ... args>
    class member_method_info_impl : public method_info_impl
    {
        friend class type_builder_impl;
        typedef return_t (class_::*method_type)(args...);

    private: // FIELDS

        /// C++ function pointer type for the method.
        method_type ptr_;

    public: // METHODS

        /// A String representing the name of the current type.
        inline virtual String to_string() override;

        /// Invokes the method reflected by this method_info instance.
        template <int ... I>
        inline Object invoke_impl(Object obj, list<Object> params, detail::IndexSequence<I...>, std::false_type);

        /// Invokes the method reflected by this method_info instance.
        template <int ... I>
        inline Object invoke_impl(Object obj, list<Object> params, detail::IndexSequence<I...>, std::true_type);

        /// Invokes the method reflected by this method_info instance.
        inline virtual Object invoke(Object obj, list<Object> params);

    private: // CONSTRUCTORS

        /// Create from method name, declaring type, return type, and pointer to method.
        ///
        /// This constructor is private. Use make_method_info(...)
        /// function with matching signature instead.
        inline member_method_info_impl(const String& name, type declaring_type, type return_type, method_type p, list<Attribute> custom_attributes);
    };

    /// Obtains information about the attributes of a static method and provides access to method metadata.
    template <class return_t, class ... args>
    class static_method_info_impl : public method_info_impl
    {
        friend class type_builder_impl;
        typedef return_t (*method_type)(args...);

    private: // FIELDS

        method_type ptr_;

    public: // METHODS

        /// A String representing the name of the current type.
        virtual String to_string() override;

        /// Invokes the method reflected by this method_info instance.
        template <int ... I>
        inline Object invoke_impl(Object obj, list<Object> params, detail::IndexSequence<I...>, std::false_type);

        /// Invokes the method reflected by this method_info instance.
        template <int ... I>
        inline Object invoke_impl(Object obj, list<Object> params, detail::IndexSequence<I...>, std::true_type);

        /// Invokes the method reflected by this method_info instance.
        inline virtual Object invoke(Object obj, list<Object> params);

    private: // CONSTRUCTORS

        /// Create from method name, declaring type, return type, and pointer to method.
        ///
        /// This constructor is private. Use make_method_info(...)
        /// function with matching signature instead.
        inline static_method_info_impl(const String& name, type declaring_type, type return_type, method_type p, list<Attribute> custom_attributes);
    };
}
