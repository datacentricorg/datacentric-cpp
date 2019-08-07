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
#include <dot/system/collections/generic/list.hpp>
#include <dc/platform/data_source/mongo/MongoServerData.hpp>

namespace dc
{
    class MongoStandardFormatServerDataImpl; using MongoStandardFormatServerData = dot::ptr<MongoStandardFormatServerDataImpl>;

    /// Assembles MongoDB URI using the standard (``mongodb'') connection
    /// string format for a single server or a cluster.
    class DC_CLASS MongoStandardFormatServerDataImpl final : public MongoServerDataImpl
    {
        typedef MongoStandardFormatServerDataImpl self;

    public: // PROPERTIES

        /// MongoDB server hostname or the list of MongoDB cluster
        /// hostnames with optional port in ``host'' or ``host::port''
        /// format.
        dot::list<dot::string> Hosts;

    public: // METHODS

        /// Get Mongo server URI without database name.
        dot::string GetMongoServerUri() override;
    };
}
