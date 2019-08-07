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
#include <dc/platform/data_source/DatabaseServerData.hpp>

namespace dc
{
    class MongoServerDataImpl; using MongoServerData = dot::ptr<MongoServerDataImpl>;

    /// Database server definition for MongoDB. This class is responsible
    /// for assembling MongoDB URI from hostname, port, and other parameters.
    ///
    /// Derived classes implement the API for the standard (``mongodb''),
    /// and seedlist (``mongodb+srv'') connection formats as well as for
    /// the default server running on localhost.
    class DC_CLASS MongoServerDataImpl : public db_server_data_impl
    {
        typedef MongoServerDataImpl self;

    public: // METHODS

        /// Get Mongo server URI without database name.
        virtual dot::string GetMongoServerUri() = 0;
    };
}
