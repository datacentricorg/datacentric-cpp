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
#include <dc/types/record/DataType.hpp>
#include <dot/system/Ptr.hpp>
#include <dot/system/Type.hpp>
#include <dot/system/collections/generic/List.hpp>
#include <dot/system/collections/generic/Dictionary.hpp>
#include <dot/system/reflection/field_info.hpp>
#include <dc/serialization/DataWriter.hpp>
#include <stack>

namespace dc
{

    class TupleWriterImpl; using TupleWriter = dot::ptr<TupleWriterImpl>;

    /// <summary>Implementation of ITreeWriter for Data.</summary>
    class DC_CLASS TupleWriterImpl : public ITreeWriterImpl
    {
        friend TupleWriter new_TupleWriter(dot::object tuple, dot::List<dot::field_info> props);

    public:

        /// <summary>Write start document tags. This method
        /// should be called only once for the entire document.</summary>
        void WriteStartDocument(dot::string rootElementName) override;

        /// <summary>Write end document tag. This method
        /// should be called only once for the entire document.
        /// The root element name passed to this method must match the root element
        /// name passed to the preceding call to WriteStartDocument(...).</summary>
        void WriteEndDocument(dot::string rootElementName) override;

        /// <summary>Write element start tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.</summary>
        void WriteStartElement(dot::string elementName) override;

        /// <summary>Write element end tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.
        /// The element name passed to this method must match the element name passed
        /// to the matching WriteStartElement(...) call at the same indent level.</summary>
        void WriteEndElement(dot::string elementName) override;

        /// <summary>Write dictionary start tag. A call to this method
        /// must follow WriteStartElement(...) or WriteStartArrayItem().</summary>
        void WriteStartDict() override;

        /// <summary>Write dictionary end tag. A call to this method
        /// must be followed by WriteEndElement(...) or WriteEndArrayItem().</summary>
        void WriteEndDict() override;

        /// <summary>Write start tag for an array. A call to this method
        /// must follow WriteStartElement(name).</summary>
        void WriteStartArray() override;

        /// <summary>Write end tag for an array. A call to this method
        /// must be followed by WriteEndElement(name).</summary>
        void WriteEndArray() override;

        /// <summary>Write start tag for an array item. A call to this method
        /// must follow either WriteStartArray() or WriteEndArrayItem().</summary>
        void WriteStartArrayItem() override;

        /// <summary>Write end tag for an array item. A call to this method
        /// must be followed by either WriteEndArray() or WriteStartArrayItem().</summary>
        void WriteEndArrayItem() override;

        /// <summary>Write value start tag. A call to this method
        /// must follow WriteStartElement(...) or WriteStartArrayItem().</summary>
        void WriteStartValue() override;

        /// <summary>Write value end tag. A call to this method
        /// must be followed by WriteEndElement(...) or WriteEndArrayItem().</summary>
        void WriteEndValue() override;

        /// <summary>Write atomic value. Value type
        /// will be inferred from object.GetType().</summary>
        void WriteValue(dot::object value) override;

        /// <summary>Convert to BSON string without checking that BSON document is complete.
        /// This permits the use of this method to inspect the BSON content during creation.</summary>
        dot::string ToString() override;

    private:

        TupleWriterImpl(dot::object tuple, dot::List<dot::field_info> props);

    private:

        dot::object tuple_;
        dot::List<dot::field_info> props_;
        int indexOfCurrent_;
        DataWriter dataWriter_;
        Data data_;

    };

    inline TupleWriter new_TupleWriter(dot::object tuple, dot::List<dot::field_info> props) { return new TupleWriterImpl(tuple, props); }
}
