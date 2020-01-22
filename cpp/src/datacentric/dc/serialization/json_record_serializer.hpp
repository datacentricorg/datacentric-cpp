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
#include <dc/serialization/tree_writer_base.hpp>
#include <dot/system/collections/generic/list.hpp>
#include <dc/serialization/bson_writer.hpp>

#include <rapidjson/document.h>

namespace dc
{
    class json_record_serializer_impl; using json_record_serializer = dot::ptr<json_record_serializer_impl>;
    class data_impl; using data = dot::ptr<data_impl>;

    /// Implementation of IBsonWriter using MongoDB IBsonWriter.
    class DC_CLASS json_record_serializer_impl : public virtual dot::object_impl
    {
        friend json_record_serializer make_json_record_serializer();

    public:

        /// Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.
        data deserialize(const rapidjson::Document& doc);

        /// Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.
        dot::object deserialize_tuple(rapidjson::Document::ConstObject doc, dot::list<dot::field_info> props, dot::type tuple_type);

        /// Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.
        void deserialize_document(rapidjson::Document::ConstObject doc, tree_writer_base writer);

        /// Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.
        void deserialize_array(rapidjson::Document::ConstArray arr, tree_writer_base writer);

        /// Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.
        void serialize(tree_writer_base writer, data value);

    private:
        json_record_serializer_impl() = default;
    };

    inline json_record_serializer make_json_record_serializer() { return new json_record_serializer_impl; }
}
