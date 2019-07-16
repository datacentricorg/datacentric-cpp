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
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <stack>

namespace dc
{
    class BsonWriterImpl; using BsonWriter = dot::Ptr<BsonWriterImpl>;

    /// <summary>Implementation of IBsonWriter using MongoDB IBsonWriter.</summary>
    class DC_CLASS BsonWriterImpl : public ITreeWriterImpl
    {

        friend BsonWriter new_BsonWriter();

    private:

        bsoncxx::builder::core bsonWriter_;
        std::stack<std::pair<dot::String, TreeWriterState>> elementStack_; // TODO make dot::Stack
        TreeWriterState currentState_;

    public:


        /// <summary>Write start document tags. This method
        /// should be called only once for the entire document.</summary>
        void WriteStartDocument(dot::String rootElementName);

        /// <summary>Write end document tag. This method
        /// should be called only once for the entire document.
        /// The root element name passed to this method must match the root element
        /// name passed to the preceding call to WriteStartDocument(...).</summary>
        void WriteEndDocument(dot::String rootElementName);

        /// <summary>Write element start tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.</summary>
        void WriteStartElement(dot::String elementName);

        /// <summary>Write element end tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.
        /// The element name passed to this method must match the element name passed
        /// to the matching WriteStartElement(...) call at the same indent level.</summary>
        void WriteEndElement(dot::String elementName);

        /// <summary>Write dictionary start tag. A call to this method
        /// must follow WriteStartElement(...) or WriteStartArrayItem().</summary>
        void WriteStartDict();

        /// <summary>Write dictionary end tag. A call to this method
        /// must be followed by WriteEndElement(...) or WriteEndArrayItem().</summary>
        void WriteEndDict();

        /// <summary>Write start tag for an array. A call to this method
        /// must follow WriteStartElement(name).</summary>
        void WriteStartArray();

        // <summary>Write end tag for an array. A call to this method
        /// must be followed by WriteEndElement(name).</summary>
        void WriteEndArray();

        /// <summary>Write start tag for an array item. A call to this method
        /// must follow either WriteStartArray() or WriteEndArrayItem().</summary>
        void WriteStartArrayItem();

        /// <summary>Write end tag for an array item. A call to this method
        /// must be followed by either WriteEndArray() or WriteStartArrayItem().</summary>
        void WriteEndArrayItem();

        /// <summary>Write value start tag. A call to this method
        /// must follow WriteStartElement(...) or WriteStartArrayItem().</summary>
        void WriteStartValue();

        /// <summary>Write value end tag. A call to this method
        /// must be followed by WriteEndElement(...) or WriteEndArrayItem().</summary>
        void WriteEndValue();

        /// <summary>Write atomic value. Value type
        /// will be inferred from object.GetType().</summary>
        void WriteValue(dot::Object value);

        /// <summary>Convert to BSON string without checking that BSON document is complete.
        /// This permits the use of this method to inspect the BSON content during creation.</summary>
        dot::String ToString() override;

        bsoncxx::document::view view();

    private:
        BsonWriterImpl()
            : bsonWriter_(true)
            , currentState_(TreeWriterState::Empty) {}
    };

    inline BsonWriter new_BsonWriter() { return new BsonWriterImpl; }
}