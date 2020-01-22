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

    /// Class implements dot::client methods.
    /// Holds mongocxx::client object.
    class client_inner : public client_impl::client_inner_base
    {
    public:

        /// Constructs from string with uri to mongo database.
        client_inner(string uri)
        {
            static mongocxx::instance instance{};

            client_ = mongocxx::client(mongocxx::uri(*uri));
        }

    protected:

        /// Returns database from client by specified name.
        virtual database get_database(dot::string name) override
        {
            return new database_impl(std::make_unique<database_inner>(client_[*name]));
        }

        /// Drops database from client by specified name.
        virtual void drop_database(dot::string name) override
        {
            client_[*name].drop();
        }

    private:

        mongocxx::client client_;

    };

    client_impl::client_impl(string uri)
    {
        impl_ = std::make_unique<client_inner>(uri);
    }

    database client_impl::get_database(dot::string name)
    {
        return impl_->get_database(name);
    }

    void client_impl::drop_database(dot::string name)
    {
        impl_->drop_database(name);
    }

}