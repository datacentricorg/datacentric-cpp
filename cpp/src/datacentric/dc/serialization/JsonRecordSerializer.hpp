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
#include <dc/serialization/ITreeWriter.hpp>
#include <dot/system/collections/generic/list.hpp>
#include <dc/serialization/BsonWriter.hpp>

#include <rapidjson/document.h>

namespace dc
{
    class JsonRecordSerializerImpl; using JsonRecordSerializer = dot::ptr<JsonRecordSerializerImpl>;
    class data_impl; using data = dot::ptr<data_impl>;

    /// Implementation of IBsonWriter using MongoDB IBsonWriter.
    class DC_CLASS JsonRecordSerializerImpl : public virtual dot::object_impl
    {
        friend JsonRecordSerializer make_JsonRecordSerializer();

    public:

        /// Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.
        data Deserialize(const rapidjson::Document& doc);

        /// Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.
        dot::object DeserializeTuple(rapidjson::Document::ConstObject doc, dot::list<dot::field_info> props, dot::type_t tupleType);

        /// Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.
        void DeserializeDocument(rapidjson::Document::ConstObject doc, ITreeWriter writer);

        /// Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.
        void DeserializeArray(rapidjson::Document::ConstArray arr, ITreeWriter writer);

        /// Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.
        void Serialize(ITreeWriter writer, data value);

    private:
        JsonRecordSerializerImpl() = default;
    };

    inline JsonRecordSerializer make_JsonRecordSerializer() { return new JsonRecordSerializerImpl; }
}
