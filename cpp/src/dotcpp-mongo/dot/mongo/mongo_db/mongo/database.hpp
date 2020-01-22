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
#include <dot/mongo/mongo_db/mongo/collection.hpp>

namespace dot
{
    class database_impl; using database = ptr<database_impl>;

    class DOT_MONGO_CLASS database_impl : public object_impl
    {
    private:

        friend class database_inner;
        friend class client_inner;

        class DOT_MONGO_CLASS database_inner_base
        {
            friend class database_impl;

        protected:

            virtual collection get_collection(dot::string name) = 0;
        };

    public:

        collection get_collection(dot::string name);

    private:

        database_impl(std::unique_ptr<database_inner_base> && impl);

        std::unique_ptr<database_inner_base> impl_;
    };
}