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
    class MethodInfoImpl; using MethodInfo = Ptr<MethodInfoImpl>;
    class TypeImpl; using Type = Ptr<TypeImpl>;

    /// Obtains information about the attributes of a method and provides access to method metadata.
    class DOT_CLASS MethodInfoImpl : public MemberInfoImpl
    {
        friend class TypeBuilderImpl;

        typedef MethodInfoImpl self;

    public: // METHODS

        /// A String representing the name of the current Type.
        inline virtual String to_string() override;

        /// Gets the parameters of this method.
        inline virtual List<ParameterInfo> get_parameters();

        /// Invokes specified method with given parameters.
        virtual Object invoke(Object, List<Object>) = 0;

        /// Gets the return Type of this method.
        Type return_type();

    protected: // FIELDS

        Type return_type_;
        List<ParameterInfo> parameters_;

    protected: // CONSTRUCTORS

        /// Create from method name, declaring Type, return Type.
        ///
        /// This constructor is protected. It is used by derived classes only.
        inline MethodInfoImpl(const String& name, Type declaring_type, Type return_type, List<Attribute> custom_attributes);
    };

    /// Obtains information about the attributes of a non-static method and provides access to method metadata.
    template <class Class, class ReturnType, class ... Args>
    class MemberMethodInfoImpl : public MethodInfoImpl
    {
        friend class TypeBuilderImpl;
        typedef ReturnType (Class::*MethodType)(Args...);

    private: // FIELDS

        /// C++ function pointer Type for the method.
        MethodType ptr_;

    public: // METHODS

        /// A String representing the name of the current Type.
        inline virtual String to_string() override;

        /// Invokes the method reflected by this MethodInfo instance.
        template <int ... I>
        inline Object invoke_impl(Object obj, List<Object> params, detail::IndexSequence<I...>, std::false_type);

        /// Invokes the method reflected by this MethodInfo instance.
        template <int ... I>
        inline Object invoke_impl(Object obj, List<Object> params, detail::IndexSequence<I...>, std::true_type);

        /// Invokes the method reflected by this MethodInfo instance.
        inline virtual Object invoke(Object obj, List<Object> params);

    private: // CONSTRUCTORS

        /// Create from method name, declaring Type, return Type, and pointer to method.
        ///
        /// This constructor is private. Use make_method_info(...)
        /// function with matching signature instead.
        inline MemberMethodInfoImpl(const String& name, Type declaring_type, Type return_type, MethodType p, List<Attribute> custom_attributes);
    };

    /// Obtains information about the attributes of a static method and provides access to method metadata.
    template <class ReturnType, class ... Args>
    class StaticMethodInfoImpl : public MethodInfoImpl
    {
        friend class TypeBuilderImpl;
        typedef ReturnType (*MethodType)(Args...);

    private: // FIELDS

        MethodType ptr_;

    public: // METHODS

        /// A String representing the name of the current Type.
        virtual String to_string() override;

        /// Invokes the method reflected by this MethodInfo instance.
        template <int ... I>
        inline Object invoke_impl(Object obj, List<Object> params, detail::IndexSequence<I...>, std::false_type);

        /// Invokes the method reflected by this MethodInfo instance.
        template <int ... I>
        inline Object invoke_impl(Object obj, List<Object> params, detail::IndexSequence<I...>, std::true_type);

        /// Invokes the method reflected by this MethodInfo instance.
        inline virtual Object invoke(Object obj, List<Object> params);

    private: // CONSTRUCTORS

        /// Create from method name, declaring Type, return Type, and pointer to method.
        ///
        /// This constructor is private. Use make_method_info(...)
        /// function with matching signature instead.
        inline StaticMethodInfoImpl(const String& name, Type declaring_type, Type return_type, MethodType p, List<Attribute> custom_attributes);
    };
}
