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

    /// Class implements dot::Client methods.
    /// Holds mongocxx::client object.
    class ClientInner : public ClientImpl::ClientInnerBase
    {
    public:

        /// Constructs from String with uri to mongo database.
        ClientInner(String uri)
        {
            static mongocxx::instance instance{};

            client_ = mongocxx::client(mongocxx::uri(*uri));
        }

    protected:

        /// Returns database from client by specified name.
        virtual Database get_database(dot::String name) override
        {
            return new DatabaseImpl(std::make_unique<DatabaseInner>(client_[*name]));
        }

        /// Drops database from client by specified name.
        virtual void drop_database(dot::String name) override
        {
            client_[*name].drop();
        }

    private:

        mongocxx::client client_;

    };

    ClientImpl::ClientImpl(String uri)
    {
        impl_ = std::make_unique<ClientInner>(uri);
    }

    Database ClientImpl::get_database(dot::String name)
    {
        return impl_->get_database(name);
    }

    void ClientImpl::drop_database(dot::String name)
    {
        impl_->drop_database(name);
    }
}
