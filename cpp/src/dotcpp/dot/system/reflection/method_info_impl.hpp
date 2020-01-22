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

namespace dot
{
    inline String MethodInfoImpl::to_string() { return "MethodInfo"; }

    inline List<ParameterInfo> MethodInfoImpl::get_parameters()
    {
        return parameters_;
    }

    inline Type MethodInfoImpl::return_type()
    {
        return return_type_;
    }

    inline MethodInfoImpl::MethodInfoImpl(const String& name, Type declaring_type, Type return_type, List<Attribute> custom_attributes)
        : MemberInfoImpl(name, declaring_type, custom_attributes)
        , return_type_(return_type)
    {}

    template <class Class, class ReturnType, class ... Args>
    inline String MemberMethodInfoImpl<Class, ReturnType, Args...>::to_string() { return "member_method_info"; }

    template <class Class, class ReturnType, class ... Args>
    template <int ... I>
    inline Object MemberMethodInfoImpl<Class, ReturnType, Args...>::invoke_impl(Object obj, List<Object> params, detail::IndexSequence<I...>, std::false_type)
    {
        return ((*Ptr<Class>(obj)).*ptr_)(params[I]...);
    }

    template <class Class, class ReturnType, class ... Args>
    template <int ... I>
    inline Object MemberMethodInfoImpl<Class, ReturnType, Args...>::invoke_impl(Object obj, List<Object> params, detail::IndexSequence<I...>, std::true_type)
    {
        ((*Ptr<Class>(obj)).*ptr_)(params[I]...);
        return Object();
    }

    template <class Class, class ReturnType, class ... Args>
    inline Object MemberMethodInfoImpl<Class, ReturnType, Args...>::invoke(Object obj, List<Object> params)
    {
        if (params->count() != parameters_->count())
            throw Exception("Wrong number of parameters for method " + this->declaring_type()->name() + "." + this->name());

        return invoke_impl(obj, params, typename detail::MakeIndexSequence<sizeof...(Args)>::index_type(), typename std::is_same<ReturnType, void>::type());
    }

    template <class Class, class ReturnType, class ... Args>
    inline MemberMethodInfoImpl<Class, ReturnType, Args...>::MemberMethodInfoImpl(const String& name, Type declaring_type, Type return_type, MethodType p, List<Attribute> custom_attributes)
            : MethodInfoImpl(name, declaring_type, return_type, custom_attributes)
            , ptr_(p)
    {}

    template <class ReturnType, class ... Args>
    inline String StaticMethodInfoImpl<ReturnType, Args...>::to_string() { return "static_method_info"; }

    template <class ReturnType, class ... Args>
    template <int ... I>
    inline Object StaticMethodInfoImpl<ReturnType, Args...>::invoke_impl(Object obj, List<Object> params, detail::IndexSequence<I...>, std::false_type)
    {
        return (*ptr_)(params[I]...);
    }

    template <class ReturnType, class ... Args>
    template <int ... I>
    inline Object StaticMethodInfoImpl<ReturnType, Args...>::invoke_impl(Object obj, List<Object> params, detail::IndexSequence<I...>, std::true_type)
    {
        (*ptr_)(params[I]...);
        return Object();
    }

    template <class ReturnType, class ... Args>
    inline Object StaticMethodInfoImpl<ReturnType, Args...>::invoke(Object obj, List<Object> params)
    {
        if (params->count() != parameters_->count())
            throw Exception("Wrong number of parameters for method " + this->declaring_type()->name() + "." + this->name());

        return invoke_impl(obj, params, typename detail::MakeIndexSequence<sizeof...(Args)>::index_type(), typename std::is_same<ReturnType, void>::type());
    }

    template <class ReturnType, class ... Args>
    inline StaticMethodInfoImpl<ReturnType, Args...>::StaticMethodInfoImpl(const String& name, Type declaring_type, Type return_type, MethodType p, List<Attribute> custom_attributes)
            : MethodInfoImpl(name, declaring_type, return_type, custom_attributes)
            , ptr_(p)
    {}
}
