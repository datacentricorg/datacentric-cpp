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
#include <dot/system/object_impl.hpp>
#include <dot/system/ptr.hpp>

namespace dot
{
    class index_options_impl; using index_options = ptr<index_options_impl>;

    /// Options for creating an index.
    class DOT_MONGO_CLASS index_options_impl : public object_impl
    {
        friend index_options make_index_options();

    private: // CONSTRUCTORS

        index_options_impl() = default;

    public: // FIELDS

        nullable<bool> unique;
        nullable<int> text_index_version;
        nullable<int> sphere_index_version;
        nullable<bool> sparse;
        string name;
        nullable<double> min;
        nullable<double> max;
        string language_override;
        string default_language;
        nullable<double> bucket_size;
        nullable<int> bits;
        nullable<bool> background;
        nullable<int> version;
    };

    inline index_options make_index_options() { return new index_options_impl(); }
}