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

#include <dot/declare.hpp>
#include <dot/system/ptr.hpp>
#include <dot/system/type.hpp>
#include <dot/serialization/tree_writer_base.hpp>
#include <dot/system/collections/generic/list.hpp>

#include <rapidjson/document.h>

namespace dot
{
    class JsonRecordSerializerImpl; using JsonRecordSerializer = dot::Ptr<JsonRecordSerializerImpl>;

    /// Implementation of bson_writer_base using MongoDB bson_writer_base.
    class DOT_CLASS JsonRecordSerializerImpl : public virtual dot::ObjectImpl
    {
        friend JsonRecordSerializer make_json_record_serializer();

    public:

        /// Null value is handled via [bson_ignore_if_null] attribute and is not expected here.
        Object deserialize(const rapidjson::Document& doc);

        /// Null value is handled via [bson_ignore_if_null] attribute and is not expected here.
        dot::Object deserialize_tuple(rapidjson::Document::ConstObject doc, dot::List<dot::FieldInfo> props, dot::Type tuple_type);

        /// Null value is handled via [bson_ignore_if_null] attribute and is not expected here.
        void deserialize_document(rapidjson::Document::ConstObject doc, tree_writer_base writer);

        /// Null value is handled via [bson_ignore_if_null] attribute and is not expected here.
        void deserialize_array(rapidjson::Document::ConstArray arr, tree_writer_base writer);

        /// Null value is handled via [bson_ignore_if_null] attribute and is not expected here.
        void serialize(tree_writer_base writer, Object value);

    private:

        void standard_serialize(tree_writer_base writer, dot::Object value);

        void standard_serialize(dot::ListBase obj, dot::String element_name, dot::tree_writer_base writer);

    private:
        JsonRecordSerializerImpl() = default;
    };

    inline JsonRecordSerializer make_json_record_serializer() { return new JsonRecordSerializerImpl; }
}
