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
    class bson_root_class_attribute_impl; using bson_root_class_attribute = Ptr<bson_root_class_attribute_impl>;

    /// Attribute marks class as root class for writing hierarchical discriminator convention.
    class DOT_MONGO_CLASS bson_root_class_attribute_impl : public AttributeImpl
    {
        friend bson_root_class_attribute make_bson_root_class_attribute();

    private:

        bson_root_class_attribute_impl() = default;

    public: // REFLECTION

        static type typeof();
        type get_type() override;
    };

    inline bson_root_class_attribute make_bson_root_class_attribute() { return new bson_root_class_attribute_impl(); }
}
