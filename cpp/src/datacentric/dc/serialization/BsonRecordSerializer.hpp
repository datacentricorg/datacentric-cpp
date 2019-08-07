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


namespace dc
{
    class BsonRecordSerializerImpl; using BsonRecordSerializer = dot::ptr<BsonRecordSerializerImpl>;
    class data_impl; using data = dot::ptr<data_impl>;

    /// Implementation of IBsonWriter using MongoDB IBsonWriter.
    class DC_CLASS BsonRecordSerializerImpl : public virtual dot::object_impl
    {
        friend BsonRecordSerializer make_BsonRecordSerializer();

    public:

        /// Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.
        data Deserialize(bsoncxx::document::view doc);

        /// Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.
        dot::object DeserializeTuple(bsoncxx::document::view doc, dot::list<dot::field_info> props, dot::type_t tupleType);

        /// Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.
        void DeserializeDocument(const bsoncxx::document::view & doc, ITreeWriter writer);

        /// Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.
        void DeserializeArray(const bsoncxx::array::view & arr, ITreeWriter writer);

        /// Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.
        void Serialize(ITreeWriter writer, data value);

    private:
        BsonRecordSerializerImpl() = default;
    };

    inline BsonRecordSerializer make_BsonRecordSerializer() { return new BsonRecordSerializerImpl; }
}