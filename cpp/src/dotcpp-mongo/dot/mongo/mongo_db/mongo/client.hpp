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
#include <dot/mongo/mongo_db/mongo/database.hpp>
#include <dot/mongo/mongo_db/mongo/settings.hpp>

namespace dot
{
    class ClientImpl; using Client = Ptr<ClientImpl>;

    /// Class representing a client connection to MongoDB.
    ///
    /// Acts as a logical gateway for working with databases contained within a MongoDB server.
    ///
    /// Example:
    /// @code
    ///   dot::Client client = make_client("mongodb://localhost:27017");
    /// @endcode
    ///
    /// Note that client is not thread-safe.
    class DOT_MONGO_CLASS ClientImpl : public ObjectImpl
    {
    private:

        friend class ClientInner;
        friend Client make_client(String uri);

        /// Base class for client implementation classes.
        /// Derived client impl class is hidden to cpp.
        class DOT_MONGO_CLASS ClientInnerBase
        {
            friend class ClientImpl;

        public:
            virtual ~ClientInnerBase() = default;

        protected:

            /// Returns Client side representation of a server side database.
            virtual Database get_database(dot::String name) = 0;

            /// Drops the database and all its collections.
            virtual void drop_database(dot::String name) = 0;
        };

    public:

        /// Returns Client side representation of a server side database.
        Database get_database(dot::String name);

        /// Drops the database and all its collections.
        void drop_database(dot::String name);

    private:

        ClientImpl(String uri);

        std::unique_ptr<ClientInnerBase> impl_;
    };

    // Returns dot::client consturcted from given db uri.
    inline Client make_client(String uri)
    {
        return new ClientImpl(uri);
    }
}
