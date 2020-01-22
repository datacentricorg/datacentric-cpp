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
//#include <dot/mongo/mongo_db/mongo/database.hpp>

namespace dot
{

    class collection_impl; using collection = ptr<collection_impl>;

    class DOT_MONGO_CLASS collection_impl : public object_impl
    {
    private:

        friend class collection_inner;
        friend class database_inner;
        friend class query_inner_impl;

        class DOT_MONGO_CLASS collection_inner_base
        {
            friend class collection_impl;

        protected:

            virtual void insert_one(object obj) = 0;
        };

    public:

        void insert_one(object obj);

    private:

        collection_impl(std::unique_ptr<collection_inner_base> && impl);

        std::unique_ptr<collection_inner_base> impl_;
    };

}