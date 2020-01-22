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
    /// Class implements dot::collection methods.
    /// Holds mongocxx::collection object.
    class collection_inner : public collection_impl::collection_inner_base
    {
        friend class query_inner_impl;

    public:

        /// Constructs from mongocxx::collection
        collection_inner(mongocxx::collection const& collection)
            : collection_(collection)
        {
        }

        /// Serialize object and pass it to mongo collection.
        virtual void insert_one(object obj) override
        {
            bson_record_serializer serializer = make_bson_record_serializer();
            bson_writer writer = make_bson_writer();
            serializer->serialize(writer, obj);

            collection_.insert_one(writer->view());
        }

        /// Delete one document according to filter.
        virtual void delete_one(filter_token_base filter) override
        {
            collection_.delete_one(serialize_tokens(filter));
        }

        /// Delete many document according to filter.
        virtual void delete_many(filter_token_base filter) override
        {
            collection_.delete_many(serialize_tokens(filter));
        }

    private:

        mongocxx::collection collection_;
    };

    collection_impl::collection_impl(std::unique_ptr<collection_inner_base> && impl)
        : impl_(std::move(impl))
    {
    }

    void collection_impl::insert_one(object obj)
    {
        impl_->insert_one(obj);
    }

    void collection_impl::delete_one(filter_token_base filter)
    {
        impl_->delete_one(filter);
    }

    void collection_impl::delete_many(filter_token_base filter)
    {
        impl_->delete_many(filter);
    }

}