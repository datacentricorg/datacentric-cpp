/*
Copyright (C) 2013-present The DataCentric Authors.

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

#include <dot/mongo/precompiled.hpp>
#include <dot/mongo/implement.hpp>
#include <dot/mongo/mongo_db/mongo/client.hpp>
#include <dot/mongo/mongo_db/mongo/collection.hpp>
#include <dot/mongo/mongo_db/mongo/database.hpp>

#include <mongocxx/database.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>


namespace dot
{

    class collection_inner : public collection_impl::collection_inner_base
    {
    public:

        collection_inner(mongocxx::collection const& collection)
            : collection_(collection)
        {
        }

        virtual void insert_one(object obj) override
        {

        }

    private:

        mongocxx::collection collection_;
    };

    class database_inner : public database_impl::database_inner_base
    {
    public:

        database_inner(mongocxx::database const& database)
            : database_(database)
        {
        }

        virtual collection get_collection(dot::string name) override
        {
            return new collection_impl( std::make_unique<collection_inner>(database_[*name]) );
        }

    private:

        mongocxx::database database_;
    };



    class client_inner : public client_impl::client_inner_base
    {
    public:

        client_inner(string uri)
        {
            static mongocxx::instance instance{};

            client_ = mongocxx::client(mongocxx::uri(*uri));
        }

    protected:

        virtual database get_database(dot::string name) override
        {
            return new database_impl(std::make_unique<database_inner>(client_[*name]) );
        }

        virtual void drop_database(dot::string name) override
        {
            client_[*name].drop();
        }

    private:

        mongocxx::client client_;

    };

    collection_impl::collection_impl(std::unique_ptr<collection_inner_base> && impl)
        : impl_(std::move(impl))
    {
    }

    void collection_impl::insert_one(object obj)
    {
        impl_->insert_one(obj);
    }

    collection database_impl::get_collection(dot::string name)
    {
        return impl_->get_collection(name);
    }

    database_impl::database_impl(std::unique_ptr<database_inner_base> && impl)
        : impl_(std::move(impl))
    {
    }

    client make_client(string uri)
    {
        return new client_impl(uri);
    }

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