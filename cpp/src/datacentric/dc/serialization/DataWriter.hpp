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
#include <dc/serialization/ITreeWriter.hpp>
#include <dc/types/record/data.hpp>
#include <dot/system/ptr.hpp>
#include <dot/system/type.hpp>
#include <dot/system/collections/generic/list.hpp>
#include <dot/system/collections/generic/Dictionary.hpp>
#include <dot/system/reflection/field_info.hpp>
#include <stack>

namespace dc
{
    class DataWriterImpl; using DataWriter = dot::ptr<DataWriterImpl>;
    class TupleWriterImpl; using TupleWriter = dot::ptr<TupleWriterImpl>;

    /// Implementation of ITreeWriter for Data.
    class DC_CLASS DataWriterImpl : public ITreeWriterImpl
    {
        friend DataWriter make_DataWriter(data data_obj);
        friend TupleWriterImpl;

    private:
        struct DataWriterPosition
        {
            dot::string CurrentElementName;
            TreeWriterState CurrentState;
            data CurrentDict;
            dot::dictionary<dot::string, dot::field_info> CurrentDictElements;
            dot::field_info CurrentElementInfo;
            dot::list_base CurrentArray;
            dot::type_t CurrentArrayItemType;
        };

    private: // FIELDS

        std::stack<DataWriterPosition> elementStack_; // TODO make dot::Stack
        dot::string rootElementName_;
        dot::string currentElementName_;
        TreeWriterState currentState_;
        data currentDict_;
        dot::dictionary<dot::string, dot::field_info> currentDictElements_;
        dot::field_info currentElementInfo_;
        dot::list_base currentArray_;
        dot::type_t currentArrayItemType_;

    private: // CONSTRUCTORS

        DataWriterImpl(data data_obj);

    public: //  METHODS

        /// Write start document tags. This method
        /// should be called only once for the entire document.
        void WriteStartDocument(dot::string rootElementName) override;

        /// Write end document tag. This method
        /// should be called only once for the entire document.
        /// The root element name passed to this method must match the root element
        /// name passed to the preceding call to WriteStartDocument(...).
        void WriteEndDocument(dot::string rootElementName) override;

        /// Write element start tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.
        void WriteStartElement(dot::string elementName) override;

        /// Write element end tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.
        /// The element name passed to this method must match the element name passed
        /// to the matching WriteStartElement(...) call at the same indent level.
        void WriteEndElement(dot::string elementName) override;

        /// Write dictionary start tag. A call to this method
        /// must follow WriteStartElement(...) or WriteStartArrayItem().
        void WriteStartDict() override;

        /// Write dictionary end tag. A call to this method
        /// must be followed by WriteEndElement(...) or WriteEndArrayItem().
        void WriteEndDict() override;

        /// Write start tag for an array. A call to this method
        /// must follow WriteStartElement(name).
        void WriteStartArray() override;

        /// Write end tag for an array. A call to this method
        /// must be followed by WriteEndElement(name).
        void WriteEndArray() override;

        /// Write start tag for an array item. A call to this method
        /// must follow either WriteStartArray() or WriteEndArrayItem().
        void WriteStartArrayItem() override;

        /// Write end tag for an array item. A call to this method
        /// must be followed by either WriteEndArray() or WriteStartArrayItem().
        void WriteEndArrayItem() override;

        /// Write value start tag. A call to this method
        /// must follow WriteStartElement(...) or WriteStartArrayItem().
        void WriteStartValue() override;

        /// Write value end tag. A call to this method
        /// must be followed by WriteEndElement(...) or WriteEndArrayItem().
        void WriteEndValue() override;

        /// Write atomic value. Value type
        /// will be inferred from object.type().
        void WriteValue(dot::object value) override;

        /// Convert to BSON string without checking that BSON document is complete.
        /// This permits the use of this method to inspect the BSON content during creation.
        dot::string to_string() override;

    private:
        /// Push state to the stack.
        void PushState();

        /// Pop state from the stack.
        void PopState();

    };

    inline DataWriter make_DataWriter(data data_obj) { return new DataWriterImpl(data_obj); }
}

