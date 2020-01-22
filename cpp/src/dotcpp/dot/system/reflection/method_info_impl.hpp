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
    inline String method_info_impl::to_string() { return "method_info"; }

    inline list<parameter_info> method_info_impl::get_parameters()
    {
        return parameters_;
    }

    inline type method_info_impl::return_type()
    {
        return return_type_;
    }

    inline method_info_impl::method_info_impl(const String& name, type declaring_type, type return_type, list<Attribute> custom_attributes)
        : member_info_impl(name, declaring_type, custom_attributes)
        , return_type_(return_type)
    {}

    template <class class_, class return_t, class ... args>
    inline String member_method_info_impl<class_, return_t, args...>::to_string() { return "member_method_info"; }

    template <class class_, class return_t, class ... args>
    template <int ... I>
    inline Object member_method_info_impl<class_, return_t, args...>::invoke_impl(Object obj, list<Object> params, detail::IndexSequence<I...>, std::false_type)
    {
        return ((*Ptr<class_>(obj)).*ptr_)(params[I]...);
    }

    template <class class_, class return_t, class ... args>
    template <int ... I>
    inline Object member_method_info_impl<class_, return_t, args...>::invoke_impl(Object obj, list<Object> params, detail::IndexSequence<I...>, std::true_type)
    {
        ((*Ptr<class_>(obj)).*ptr_)(params[I]...);
        return Object();
    }

    template <class class_, class return_t, class ... args>
    inline Object member_method_info_impl<class_, return_t, args...>::invoke(Object obj, list<Object> params)
    {
        if (params->count() != parameters_->count())
            throw Exception("Wrong number of parameters for method " + this->declaring_type()->name() + "." + this->name());

        return invoke_impl(obj, params, typename detail::MakeIndexSequence<sizeof...(args)>::index_type(), typename std::is_same<return_t, void>::type());
    }

    template <class class_, class return_t, class ... args>
    inline member_method_info_impl<class_, return_t, args...>::member_method_info_impl(const String& name, type declaring_type, type return_type, method_type p, list<Attribute> custom_attributes)
            : method_info_impl(name, declaring_type, return_type, custom_attributes)
            , ptr_(p)
    {}

    template <class return_t, class ... args>
    inline String static_method_info_impl<return_t, args...>::to_string() { return "static_method_info"; }

    template <class return_t, class ... args>
    template <int ... I>
    inline Object static_method_info_impl<return_t, args...>::invoke_impl(Object obj, list<Object> params, detail::IndexSequence<I...>, std::false_type)
    {
        return (*ptr_)(params[I]...);
    }

    template <class return_t, class ... args>
    template <int ... I>
    inline Object static_method_info_impl<return_t, args...>::invoke_impl(Object obj, list<Object> params, detail::IndexSequence<I...>, std::true_type)
    {
        (*ptr_)(params[I]...);
        return Object();
    }

    template <class return_t, class ... args>
    inline Object static_method_info_impl<return_t, args...>::invoke(Object obj, list<Object> params)
    {
        if (params->count() != parameters_->count())
            throw Exception("Wrong number of parameters for method " + this->declaring_type()->name() + "." + this->name());

        return invoke_impl(obj, params, typename detail::MakeIndexSequence<sizeof...(args)>::index_type(), typename std::is_same<return_t, void>::type());
    }

    template <class return_t, class ... args>
    inline static_method_info_impl<return_t, args...>::static_method_info_impl(const String& name, type declaring_type, type return_type, method_type p, list<Attribute> custom_attributes)
            : method_info_impl(name, declaring_type, return_type, custom_attributes)
            , ptr_(p)
    {}
}
