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

#pragma once

#include <dot/mongo/declare.hpp>
#include <dot/system/object_impl.hpp>
#include <dot/system/ptr.hpp>
#include <dot/mongo/mongo_db/mongo/database.hpp>

namespace dot
{
    class client_impl; using client = ptr<client_impl>;

    client make_client(string uri);

    class DOT_MONGO_CLASS client_impl : public object_impl
    {
    private:

        friend class client_inner;
        friend client make_client(string uri);

        class DOT_MONGO_CLASS client_inner_base
        {
            friend class client_impl;

        protected:

            virtual database get_database(dot::string name) = 0;

            virtual void drop_database(dot::string name) = 0;
        };

    public:

        database get_database(dot::string name);

        void drop_database(dot::string name);

    private:

        client_impl(string uri);

        std::unique_ptr<client_inner_base> impl_;
    };
}