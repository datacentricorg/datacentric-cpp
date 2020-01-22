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
#include <dc/platform/data_source/mongo/mongo_standard_format_server_data.hpp>
#include <dc/platform/context/context_base.hpp>

namespace dc
{
    dot::string mongo_standard_format_server_data_impl::get_mongo_server_uri()
    {
        if (hosts == nullptr || hosts->count() == 0) throw dot::exception(
            dot::string::format("The list of hosts provided for MongoDB server {0} is null or empty.", db_server_uri));

        // TODO uncoment when implemented
        //dot::string host_names = dot::string::join(",", hosts);
        //dot::string result = dot::string::concat("mongodb://", host_names, "/");
        //return result;
        throw dot::exception("Not implemented.");
    }
}
