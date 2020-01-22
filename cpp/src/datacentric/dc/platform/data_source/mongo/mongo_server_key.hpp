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

#include <dc/declare.hpp>
#include <dot/system/ptr.hpp>
#include <dc/types/record/typed_record.hpp>

namespace dc
{
    class MongoServerKeyImpl; using MongoServerKey = dot::Ptr<MongoServerKeyImpl>;
    class MongoServerImpl; using MongoServer = dot::Ptr<MongoServerImpl>;

    /// Provides Mongo server URI.
    ///
    /// Server URI specified here must refer to the entire server, not
    /// an individual database.
    class DC_CLASS MongoServerKeyImpl : public TypedKeyImpl<MongoServerKeyImpl, MongoServerImpl>
    {
        friend MongoServerKey make_mongo_server_key();
        friend MongoServerKey make_mongo_server_key(dot::String);

    public: // PROPERTIES

        /// Mongo server URI.
        ///
        /// Server URI specified here must refer to the entire server, not
        /// an individual database.
        dot::String mongo_server_uri;

    public: // STATIC

        /// Mongo server running on default port of localhost.
        static MongoServerKey default_key;

    private: // CONSTRUCTORS

        MongoServerKeyImpl() = default;

        MongoServerKeyImpl(dot::String uri);
    };

    inline MongoServerKey make_mongo_server_key() { return new MongoServerKeyImpl(); }
    inline MongoServerKey make_mongo_server_key(dot::String uri) { return new MongoServerKeyImpl(uri); }
}
