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
#include <dot/mongo/mongo_db/mongo/collection.hpp>

namespace dot
{
    class database_impl; using database = Ptr<database_impl>;

    /// Class representing a MongoDB database.
    class DOT_MONGO_CLASS database_impl : public ObjectImpl
    {
    private:

        friend class database_inner;
        friend class client_inner;

        /// Base class for database implementation classes.
        /// Derived database impl class is hidden to cpp.
        class DOT_MONGO_CLASS database_inner_base
        {
            friend class database_impl;

        public:
            virtual ~database_inner_base() = default;

        protected:

            /// Returns the collection with given name.
            virtual collection get_collection(dot::String name) = 0;
        };

    public:

        /// Returns the collection with given name.
        collection get_collection(dot::String name);

    private:

        database_impl(std::unique_ptr<database_inner_base> && impl);

        std::unique_ptr<database_inner_base> impl_;
    };
}