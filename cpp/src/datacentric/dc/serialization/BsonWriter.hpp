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
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <stack>

namespace dc
{
    class BsonWriterImpl; using BsonWriter = dot::ptr<BsonWriterImpl>;

    /// Implementation of IBsonWriter using MongoDB IBsonWriter.
    class DC_CLASS BsonWriterImpl : public ITreeWriterImpl
    {

        friend BsonWriter make_BsonWriter();

    private:

        bsoncxx::builder::core bsonWriter_;
        std::stack<std::pair<dot::string, TreeWriterState>> elementStack_; // TODO make dot::Stack
        TreeWriterState currentState_;

    public:


        /// Write start document tags. This method
        /// should be called only once for the entire document.
        void WriteStartDocument(dot::string rootElementName);

        /// Write end document tag. This method
        /// should be called only once for the entire document.
        /// The root element name passed to this method must match the root element
        /// name passed to the preceding call to WriteStartDocument(...).
        void WriteEndDocument(dot::string rootElementName);

        /// Write element start tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.
        void WriteStartElement(dot::string elementName);

        /// Write element end tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.
        /// The element name passed to this method must match the element name passed
        /// to the matching WriteStartElement(...) call at the same indent level.
        void WriteEndElement(dot::string elementName);

        /// Write dictionary start tag. A call to this method
        /// must follow WriteStartElement(...) or WriteStartArrayItem().
        void WriteStartDict();

        /// Write dictionary end tag. A call to this method
        /// must be followed by WriteEndElement(...) or WriteEndArrayItem().
        void WriteEndDict();

        /// Write start tag for an array. A call to this method
        /// must follow WriteStartElement(name).
        void WriteStartArray();

        /// Write end tag for an array. A call to this method
        /// must be followed by WriteEndElement(name).
        void WriteEndArray();

        /// Write start tag for an array item. A call to this method
        /// must follow either WriteStartArray() or WriteEndArrayItem().
        void WriteStartArrayItem();

        /// Write end tag for an array item. A call to this method
        /// must be followed by either WriteEndArray() or WriteStartArrayItem().
        void WriteEndArrayItem();

        /// Write value start tag. A call to this method
        /// must follow WriteStartElement(...) or WriteStartArrayItem().
        void WriteStartValue();

        /// Write value end tag. A call to this method
        /// must be followed by WriteEndElement(...) or WriteEndArrayItem().
        void WriteEndValue();

        /// Write atomic value. Value type
        /// will be inferred from object.type().
        void WriteValue(dot::object value);

        /// Convert to BSON string without checking that BSON document is complete.
        /// This permits the use of this method to inspect the BSON content during creation.
        dot::string to_string() override;

        bsoncxx::document::view view();

    private:
        BsonWriterImpl()
            : bsonWriter_(true)
            , currentState_(TreeWriterState::empty) {}
    };

    inline BsonWriter make_BsonWriter() { return new BsonWriterImpl; }
}