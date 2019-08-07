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
#include <dc/types/record/record.hpp>

namespace dc
{
    class db_server_key_impl; using db_server_key = dot::ptr<db_server_key_impl>;
    class db_server_data_impl; using db_server_data = dot::ptr<db_server_data_impl>;

    inline db_server_key make_db_server_key();

    /// Provides a standard way to identify a database server.
    ///
    /// This record is stored in root dataset.
    class DC_CLASS db_server_key_impl : public root_key_impl<db_server_key_impl, db_server_data_impl>
    {
        typedef db_server_key_impl self;

    public: // PROPERTIES

        /// Unique database server identifier string.
        ///
        /// This field is the user friendly name used to
        /// identify the server. It is not the server URI.
        dot::string db_server_id;

    public: // STATIC

        /// By convention, Default is the Mongo server running on the default port of localhost.
        static db_server_key default;

    public: // CONSTRUCTORS

        /// Default constructor.
        db_server_key_impl() = default;

        /// Keys in which string ID is the only element support implicit conversion from value.
        db_server_key_impl(dot::string value);

        DOT_TYPE_BEGIN("dc", "DbServerKey")
            DOT_TYPE_PROP(db_server_id)
            DOT_TYPE_CTOR(make_db_server_key)
        DOT_TYPE_END()
    };

    inline db_server_key make_db_server_key()
    {
        return new db_server_key_impl;
    }
}
