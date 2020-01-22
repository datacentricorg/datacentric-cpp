#include "collection.hpp"
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

        /// Serialize Object and pass it to mongo collection.
        virtual void insert_one(Object obj) override
        {
            bson_record_serializer serializer = make_bson_record_serializer();
            bson_writer writer = make_bson_writer();
            serializer->serialize(writer, obj);

            collection_.insert_one(writer->view());
        }

        /// Serialize Object and pass it to mongo collection.
        virtual void insert_many(ListBase objs) override
        {
            if (!objs->get_length())
                return;

            mongocxx::bulk_write bulk = collection_.create_bulk_write();

            for (int i = 0; i < objs->get_length(); ++i)
            {
                bson_record_serializer serializer = make_bson_record_serializer();
                bson_writer writer = make_bson_writer();
                serializer->serialize(writer, objs->get_item(i));
                bulk.append(mongocxx::model::insert_one(writer->view()));
            }

            bulk.execute();
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

        /// Creates an index over the collection for the provided keys with the provided options.
        virtual void create_index(List<std::tuple<String, int>> indexes, index_options options) override
        {
            namespace bsonb = bsoncxx::builder::basic;
            bsoncxx::builder::core index_builder(false);
            bsonb::document options_builder;

            // Write indexes to bson
            for (std::tuple<String, int>& index : indexes)
            {
                index_builder.key_view(*std::get<0>(index));
                int order = std::get<1>(index);
                if (order == 1 || order == -1)
                    index_builder.append(order);
                else throw Exception("Index sort order must be 1 or -1.");
            }

            // Write options to bson
            if (options != nullptr)
            {
                if (options->unique != nullptr)
                    options_builder.append(bsonb::kvp("unique", options->unique.value()));
                if (options->text_index_version != nullptr)
                    options_builder.append(bsonb::kvp("textIndexVersion", options->text_index_version.value()));
                if (options->sphere_index_version != nullptr)
                    options_builder.append(bsonb::kvp("sphereIndexVersion", options->sphere_index_version.value()));
                if (options->sparse != nullptr)
                    options_builder.append(bsonb::kvp("sparse", options->sparse.value()));
                if (options->name != nullptr)
                    options_builder.append(bsonb::kvp("name", *options->name));
                if (options->min != nullptr)
                    options_builder.append(bsonb::kvp("min", options->min.value()));
                if (options->max != nullptr)
                    options_builder.append(bsonb::kvp("max", options->max.value()));
                if (options->language_override != nullptr)
                    options_builder.append(bsonb::kvp("languageOverride", *options->language_override));
                if (options->default_language != nullptr)
                    options_builder.append(bsonb::kvp("defaultLanguage", *options->default_language));
                if (options->bucket_size != nullptr)
                    options_builder.append(bsonb::kvp("bucketSize", options->bucket_size.value()));
                if (options->bits != nullptr)
                    options_builder.append(bsonb::kvp("bits", options->bits.value()));
                if (options->background != nullptr)
                    options_builder.append(bsonb::kvp("background", options->background.value()));
                if (options->version != nullptr)
                    options_builder.append(bsonb::kvp("version", options->version.value()));
            }

            // Create index
            collection_.create_index(index_builder.view_document(), options_builder.view());
        }

    private:

        mongocxx::collection collection_;
    };

    collection_impl::collection_impl(std::unique_ptr<collection_inner_base> && impl)
        : impl_(std::move(impl))
    {
    }

    void collection_impl::insert_one(Object obj)
    {
        impl_->insert_one(obj);
    }

    void collection_impl::insert_many(dot::ListBase objs)
    {
        impl_->insert_many(objs);
    }

    void collection_impl::delete_one(filter_token_base filter)
    {
        impl_->delete_one(filter);
    }

    void collection_impl::delete_many(filter_token_base filter)
    {
        impl_->delete_many(filter);
    }

    void collection_impl::create_index(List<std::tuple<String, int>> indexes, index_options options)
    {
        impl_->create_index(indexes, options);
    }
}