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

#include <dot/declare.hpp>
#include <dot/system/ptr.hpp>
#include <dot/system/object.hpp>

namespace dot
{
    class list_base_impl; using list_base = Ptr<list_base_impl>;

    class DOT_CLASS list_base_impl : virtual public ObjectImpl
    {
    public:

        /// Add Object to end of collection.
        virtual void add_object(Object item) = 0;

        /// Get Object from collection by index.
        virtual Object get_item(int index) = 0;

        /// Set Object from collection by index.
        virtual void set_item(int index, Object value) = 0;

        /// Get length of collection.
        virtual int get_length() = 0;

        /// Gets the type of the current instance.
        type get_type() override;

        /// Gets the type of the collection_base_impl.
        static type typeof();
    };
}
