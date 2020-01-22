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
    class database_inner : public database_impl::database_inner_base
    {
    public:

        /// Constructs from mongocxx::database.
        database_inner(mongocxx::database const& database)
            : database_(database)
        {
        }

        /// Returns collection from database by specified name.
        virtual collection get_collection(dot::string name) override
        {
            return new collection_impl(std::make_unique<collection_inner>(database_[*name]));
        }

    private:

        mongocxx::database database_;
    };

    collection database_impl::get_collection(dot::string name)
    {
        return impl_->get_collection(name);
    }

    database_impl::database_impl(std::unique_ptr<database_inner_base> && impl)
        : impl_(std::move(impl))
    {
    }

}