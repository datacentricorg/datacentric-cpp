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
    inline string constructor_info_impl::to_string() { return "constructor_info"; }

    /// Gets the parameters of this constructor.
    inline list<parameter_info> constructor_info_impl::get_parameters()
    {
        return parameters_;
    }

    inline constructor_info_impl::constructor_info_impl(type declaring_type, list<attribute> custom_attributes)
            : member_info_impl(".ctor", declaring_type, custom_attributes)
    {}

    template <class class_, class ... args>
    inline string member_constructor_info_impl<class_, args...>::to_string() { return "member_constructor_info"; }

    template <class class_, class ... args>
    template <int ... I>
    inline object member_constructor_info_impl<class_, args...>::invoke_impl(list<object> params, detail::IndexSequence<I...>)
    {
        return (*ptr_)(params[I]...);
    }

    template <class class_, class ... args>
    inline object member_constructor_info_impl<class_, args...>::invoke(list<object> params)
    {
        if ((params.is_empty() && parameters_->count() != 0) || (!params.is_empty() && (params->count() != parameters_->count())))
            throw exception("Wrong number of parameters for constructor " + this->declaring_type()->name() + "." + this->name());

        return invoke_impl(params, typename detail::MakeIndexSequence<sizeof...(args)>::index_type());
    }

    template <class class_, class ... args>
    inline member_constructor_info_impl<class_, args...>::member_constructor_info_impl(type declaring_type, ctor_type p, list<attribute> custom_attributes)
            : constructor_info_impl(declaring_type, custom_attributes)
            , ptr_(p)
    {}
}
