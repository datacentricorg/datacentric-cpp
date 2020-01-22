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

#include <dot/mongo/declare.hpp>
#include <dot/system/ptr.hpp>
#include <dot/serialization/tree_writer_base.hpp>
#include <dot/system/byte_array.hpp>
#include <dot/system/collections/generic/list.hpp>
#include <dot/mongo/serialization/bson_writer.hpp>

namespace dot
{
    class BsonRecordSerializerImpl; using BsonRecordSerializer = dot::Ptr<BsonRecordSerializerImpl>;

    /// Implementation of bson_writer_base using MongoDB bson_writer_base.
    class DOT_MONGO_CLASS BsonRecordSerializerImpl : public virtual dot::ObjectImpl
    {
        friend BsonRecordSerializer make_bson_record_serializer();

    public:

        /// Null value is handled via [bson_ignore_if_null] attribute and is not expected here.
        dot::Object deserialize(bsoncxx::document::view doc);

        /// Null value is handled via [bson_ignore_if_null] attribute and is not expected here.
        dot::Object deserialize_tuple(bsoncxx::document::view doc, dot::List<dot::FieldInfo> props, dot::Type tuple_type);

        /// Null value is handled via [bson_ignore_if_null] attribute and is not expected here.
        void deserialize_document(const bsoncxx::document::view & doc, tree_writer_base writer);

        /// Null value is handled via [bson_ignore_if_null] attribute and is not expected here.
        void deserialize_array(const bsoncxx::array::view & arr, tree_writer_base writer);

        /// Null value is handled via [bson_ignore_if_null] attribute and is not expected here.
        void serialize(tree_writer_base writer, dot::Object value);

    private:

        void standard_serialize(tree_writer_base writer, dot::Object value);

        void standard_serialize(dot::ListBase obj, dot::String element_name, dot::tree_writer_base writer);

        /// Converts bson b_binary to ByteArray.
        static ByteArray to_byte_array(const bsoncxx::types::b_binary& bin_array);

    private:
        BsonRecordSerializerImpl() = default;
    };

    inline BsonRecordSerializer make_bson_record_serializer() { return new BsonRecordSerializerImpl; }
}
