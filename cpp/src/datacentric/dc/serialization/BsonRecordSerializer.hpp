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
#include <dot/system/Ptr.hpp>
#include <dc/serialization/ITreeWriter.hpp>
#include <dot/system/collections/generic/List.hpp>
#include <dc/serialization/BsonWriter.hpp>


namespace dc
{
    class BsonRecordSerializerImpl; using BsonRecordSerializer = dot::Ptr<BsonRecordSerializerImpl>;
    class DataImpl; using Data = dot::Ptr<DataImpl>;

    /// <summary>Implementation of IBsonWriter using MongoDB IBsonWriter.</summary>
    class DC_CLASS BsonRecordSerializerImpl : public virtual dot::ObjectImpl
    {
        friend BsonRecordSerializer new_BsonRecordSerializer();

    public:

        /// <summary>Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.</summary>
        Data Deserialize(bsoncxx::document::view doc);

        /// <summary>Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.</summary>
        dot::Object DeserializeTuple(bsoncxx::document::view doc, dot::List<dot::PropertyInfo> props, dot::Type tupleType);

        /// <summary>Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.</summary>
        void DeserializeDocument(const bsoncxx::document::view & doc, ITreeWriter writer);

        /// <summary>Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.</summary>
        void DeserializeArray(const bsoncxx::array::view & arr, ITreeWriter writer);

        /// <summary>Null value is handled via [BsonIgnoreIfNull] attribute and is not expected here.</summary>
        void Serialize(ITreeWriter writer, Data value);

    private:
        BsonRecordSerializerImpl() = default;
    };

    inline BsonRecordSerializer new_BsonRecordSerializer() { return new BsonRecordSerializerImpl; }
}