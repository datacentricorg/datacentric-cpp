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
#include <dc/platform/data_source/mongo/mongo_server_key.hpp>

namespace dc
{
    class MongoServerImpl; using MongoServer = dot::Ptr<MongoServerImpl>;

    /// Database server definition for MongoDB. This class is responsible
    /// for assembling MongoDB URI from hostname, port, and other parameters.
    ///
    /// Derived classes implement the API for the standard (``mongodb''),
    /// and seedlist (``mongodb+srv'') connection formats as well as for
    /// the default server running on localhost.
    class DC_CLASS MongoServerImpl : public TypedRecordImpl<MongoServerKeyImpl, MongoServerImpl>
    {
        typedef MongoServerImpl self;
        friend MongoServer make_mongo_server();

    public: // PROPERTIES

        /// Mongo server URI.
        ///
        /// Server URI specified here must refer to the entire server, not
        /// an individual database.
        dot::String mongo_server_uri;

    private: // CONSTRUCTORS

        MongoServerImpl() = default;
    };

    inline MongoServer make_mongo_server() { return new MongoServerImpl(); }
}
