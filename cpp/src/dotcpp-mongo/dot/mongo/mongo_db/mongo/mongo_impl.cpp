/*
Copyright (C) 2015-present The DotCpp Authors.

This file is part of .C++, a native C++ implementation of
popular .NET class library APIs developed to facilitate
code reuse between C# and C++.

    http://github.com/dotcpp/dotcpp (source)
    http://dotcpp.org (documentation)

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

#include <dot/mongo/precompiled.hpp>
#include <dot/mongo/implement.hpp>
#include <dot/mongo/mongo_db/mongo/client.hpp>
#include <dot/mongo/mongo_db/mongo/collection.hpp>
#include <dot/mongo/mongo_db/mongo/database.hpp>

#include <mongocxx/database.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <dot/mongo/serialization/bson_writer.hpp>
#include <dot/mongo/serialization/bson_record_serializer.hpp>

#include <dot/mongo/mongo_db/cursor/cursor_wrapper.hpp>
#include <dot/mongo/mongo_db/query/query_builder.hpp>

#include <dot/mongo/mongo_db/query/query.hpp>

#include <mongocxx/pipeline.hpp>
#include <bsoncxx/builder/basic/document.hpp>

//#include <bsoncxx/json.hpp>

#include <dot/mongo/serialization/filter_token_serialization.hpp>
#include <dot/mongo/mongo_db/mongo/collection_impl.hpp>
#include <dot/mongo/mongo_db/mongo/database_impl.hpp>
#include <dot/mongo/mongo_db/mongo/client_impl.hpp>
#include <dot/mongo/mongo_db/cursor/cursor_impl.hpp>
#include <dot/mongo/mongo_db/query/query_impl.hpp>

namespace dot
{
}
