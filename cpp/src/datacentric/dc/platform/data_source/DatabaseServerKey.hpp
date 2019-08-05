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
#include <dot/system/Ptr.hpp>
#include <dc/types/record/RecordFor.hpp>

namespace dc
{
    class db_server_key_impl; using db_server_key = dot::ptr<db_server_key_impl>;
    class db_server_data_impl; using db_server_data = dot::ptr<db_server_data_impl>;

    inline db_server_key make_db_server_key();

    /// <summary>
    /// Provides a standard way to identify a database server.
    ///
    /// This record is stored in root dataset.
    /// </summary>
    class DC_CLASS db_server_key_impl : public root_key_for_impl<db_server_key_impl, db_server_data_impl>
    {
        typedef db_server_key_impl self;

    public: // PROPERTIES

        /// <summary>
        /// Unique database server identifier string.
        ///
        /// This field is the user friendly name used to
        /// identify the server. It is not the server URI.
        /// </summary>
        dot::string db_server_id;

    public: // STATIC

        /// <summary>
        /// By convention, Default is the Mongo server running on the default port of localhost.
        /// </summary>
        static db_server_key default;

    public: // CONSTRUCTORS

        /// <summary>Default constructor.</summary>
        db_server_key_impl() = default;

        /// <summary>Keys in which string ID is the only element support implicit conversion from value.</summary>
        db_server_key_impl(dot::string value);

        DOT_TYPE_BEGIN(".Runtime.Main", "DbServerKey")
            DOT_TYPE_PROP(DbServerID)
            DOT_TYPE_CTOR(make_db_server_key)
        DOT_TYPE_END()
    };

    inline db_server_key make_db_server_key()
    {
        return new db_server_key_impl;
    }
}
