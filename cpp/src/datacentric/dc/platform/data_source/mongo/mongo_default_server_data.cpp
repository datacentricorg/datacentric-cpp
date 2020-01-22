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

#include <dc/precompiled.hpp>
#include <dc/implement.hpp>
#include <dc/platform/data_source/mongo/mongo_default_server_data.hpp>
#include <dc/platform/data_source/database_server_key.hpp>
#include <dc/platform/context/context_base.hpp>

namespace dc
{
    mongo_default_server_data_impl::mongo_default_server_data_impl()
    {
        db_server_uri = db_server_key_impl::default_key->db_server_uri;
    }

    dot::string mongo_default_server_data_impl::get_mongo_server_uri()
    {
        return "mongodb://localhost/";
    }
}
