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

#include <dot/mongo/declare.hpp>
#include <dot/system/ptr.hpp>
#include <dot/system/type.hpp>

namespace dot
{
    class filter_token_serialization_attribute_impl;
    using filter_token_serialization_attribute = Ptr<filter_token_serialization_attribute_impl>;

    /// Attribute marks class as root class for writing hierarchical discriminator convention.
    class DOT_MONGO_CLASS filter_token_serialization_attribute_impl : public AttributeImpl
    {
    public:

        typedef Object (*serializer_func_type)(Object);

        friend filter_token_serialization_attribute make_filter_token_serialization_attribute(serializer_func_type);

        Object serialize(Object obj);

    public: // REFLECTION

        static Type typeof();
        Type get_type() override;

    private:

        /// Constructs from serializer_func_type.
        filter_token_serialization_attribute_impl(serializer_func_type serializer)
            : serializer_(serializer)
        {}

        serializer_func_type serializer_;
    };

    inline filter_token_serialization_attribute make_filter_token_serialization_attribute(filter_token_serialization_attribute_impl::serializer_func_type serializer) {
        return new filter_token_serialization_attribute_impl(serializer);
    }
}
