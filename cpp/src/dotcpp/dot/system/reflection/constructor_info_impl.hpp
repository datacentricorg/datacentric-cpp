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
    inline String ConstructorInfoImpl::to_string() { return "ConstructorInfo"; }

    /// Gets the parameters of this constructor.
    inline List<ParameterInfo> ConstructorInfoImpl::get_parameters()
    {
        return parameters_;
    }

    inline ConstructorInfoImpl::ConstructorInfoImpl(Type declaring_type, List<Attribute> custom_attributes)
            : MemberInfoImpl(".ctor", declaring_type, custom_attributes)
    {}

    template <class Class, class ... Args>
    inline String MemberConstructorInfoImpl<Class, Args...>::to_string() { return "member_constructor_info"; }

    template <class Class, class ... Args>
    template <int ... I>
    inline Object MemberConstructorInfoImpl<Class, Args...>::invoke_impl(List<Object> params, detail::IndexSequence<I...>)
    {
        return (*ptr_)(params[I]...);
    }

    template <class Class, class ... Args>
    inline Object MemberConstructorInfoImpl<Class, Args...>::invoke(List<Object> params)
    {
        if ((params.is_empty() && parameters_->count() != 0) || (!params.is_empty() && (params->count() != parameters_->count())))
            throw Exception("Wrong number of parameters for constructor " + this->declaring_type()->name() + "." + this->name());

        return invoke_impl(params, typename detail::MakeIndexSequence<sizeof...(Args)>::index_type());
    }

    template <class Class, class ... Args>
    inline MemberConstructorInfoImpl<Class, Args...>::MemberConstructorInfoImpl(Type declaring_type, CtorType p, List<Attribute> custom_attributes)
            : ConstructorInfoImpl(declaring_type, custom_attributes)
            , ptr_(p)
    {}
}
