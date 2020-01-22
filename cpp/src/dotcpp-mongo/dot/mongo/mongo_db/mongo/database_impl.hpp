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

#pragma once

namespace dot
{
    /// Class implements dot::database methods.
    /// Holds mongocxx::database object.
    class DatabaseInner : public DatabaseImpl::DatabaseInnerBase
    {
    public:

        /// Constructs from mongocxx::database.
        DatabaseInner(mongocxx::database const& database)
            : database_(database)
        {
        }

        /// Returns collection from database by specified name.
        virtual Collection get_collection(dot::String name) override
        {
            return new CollectionImpl(std::make_unique<CollectionInner>(database_[*name]));
        }

    private:

        mongocxx::database database_;
    };

    Collection DatabaseImpl::get_collection(dot::String name)
    {
        return impl_->get_collection(name);
    }

    DatabaseImpl::DatabaseImpl(std::unique_ptr<DatabaseInnerBase> && impl)
        : impl_(std::move(impl))
    {
    }
}
