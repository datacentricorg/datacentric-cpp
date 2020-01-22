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
    class DatabaseImpl; using Database = Ptr<DatabaseImpl>;

    /// Class representing a MongoDB database.
    class DOT_MONGO_CLASS DatabaseImpl : public ObjectImpl
    {
    private:

        friend class DatabaseInner;
        friend class ClientInner;

        /// Base class for database implementation classes.
        /// Derived database impl class is hidden to cpp.
        class DOT_MONGO_CLASS DatabaseInnerBase
        {
            friend class DatabaseImpl;

        public:
            virtual ~DatabaseInnerBase() = default;

        protected:

            /// Returns the collection with given name.
            virtual Collection get_collection(dot::String name) = 0;
        };

    public:

        /// Returns the collection with given name.
        Collection get_collection(dot::String name);

    private:

        DatabaseImpl(std::unique_ptr<DatabaseInnerBase> && impl);

        std::unique_ptr<DatabaseInnerBase> impl_;
    };
}
