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
#include <dc/platform/data_source/mongo/mongo_server_data.hpp>

namespace dc
{
    class mongo_default_server_data_impl; using mongo_default_server_data = dot::ptr<mongo_default_server_data_impl>;

    /// Returns MongoDB URI for the server running
    /// on the default port 27017 of localhost:
    ///
    /// mongodb://localhost/
    class DC_CLASS mongo_default_server_data_impl final : public mongo_server_data_impl
    {
        typedef mongo_default_server_data_impl self;

        friend mongo_default_server_data make_mongo_default_server_data();

    protected: // CONSTRUCTORS

        /// Assign the value of DbServerID that
        /// the default Mongo server uses by convention.
        mongo_default_server_data_impl();

    public: // METHODS

        /// Get Mongo server URI without database name.
        dot::string get_mongo_server_uri() override;
    };

    inline mongo_default_server_data make_mongo_default_server_data() { return new mongo_default_server_data_impl; }
}
