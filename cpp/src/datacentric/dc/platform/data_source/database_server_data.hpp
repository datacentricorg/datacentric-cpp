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
    class db_server_key_impl; using db_server_key = dot::Ptr<db_server_key_impl>;
    class db_server_data_impl; using db_server_data = dot::Ptr<db_server_data_impl>;
    template <typename t_key, typename t_record> class root_record_for_impl;

    inline db_server_data make_db_server_data();

    /// Provides a standard way to identify a database server.
    ///
    /// This record is stored in root dataset.
    class DC_CLASS db_server_data_impl : public root_record_for_impl<db_server_key_impl, db_server_data_impl>
    {
        typedef db_server_data_impl self;

    public: // PROPERTIES

        /// Mongo server URI.
        ///
        /// Server URI specified here must refer to the entire server, not
        /// an individual database.
        dot::String db_server_uri;

        DOT_TYPE_BEGIN("dc", "db_server_data")
            DOT_TYPE_PROP(db_server_uri)
            DOT_TYPE_CTOR(make_db_server_data)
        DOT_TYPE_END()
    };

    inline db_server_data make_db_server_data()
    {
        return new db_server_data_impl;
    }
}
