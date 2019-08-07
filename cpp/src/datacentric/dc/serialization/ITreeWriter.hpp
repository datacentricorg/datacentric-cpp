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
#include <dot/system/object_impl.hpp>

namespace dc
{
    enum class TreeWriterState
    {
        /// Empty
        empty,

        /// State following WriteStartDocument.
        DocumentStarted,

        /// State following WriteEndDocument.
        DocumentCompleted,

        /// State following WriteStartElement.
        ElementStarted,

        /// State following WriteEndElement.
        ElementCompleted,

        /// State following WriteStartArray.
        ArrayStarted,

        /// State following WriteEndArray.
        ArrayCompleted,

        /// State following WriteStartArrayItem.
        ArrayItemStarted,

        /// State following WriteEndArrayItem.
        ArrayItemCompleted,

        /// State following WriteStartDict that is not inside an array.
        DictStarted,

        /// State following WriteEndDict that is not inside an array.
        DictCompleted,

        /// State following WriteStartDict inside an array.
        DictArrayItemStarted,

        /// State following WriteEndDict inside an array.
        DictArrayItemCompleted,

        /// State following WriteStartValue that is not inside an array.
        ValueStarted,

        /// State following WriteValue that is not inside an array.
        ValueWritten,

        /// State following WriteEndValue that is not inside an array.
        ValueCompleted,

        /// State following WriteStartValue inside an array.
        ValueArrayItemStarted,

        /// State following WriteValue inside an array.
        ValueArrayItemWritten,

        /// State following WriteEndValue inside an array.
        ValueArrayItemCompleted
    };

    class ITreeWriterImpl; using ITreeWriter = dot::ptr<ITreeWriterImpl>;
    class dot::object;
    class dot::string;

    /// Interface for writing tree data.
    class DC_CLASS ITreeWriterImpl : public dot::object_impl
    {
    public:

        /// Write start document tags. This method
        /// should be called only once for the entire document.
        virtual void WriteStartDocument(dot::string rootElementName) = 0;

        /// Write end document tag. This method
        /// should be called only once for the entire document.
        /// The root element name passed to this method must match the root element
        /// name passed to the preceding call to WriteStartDocument(...).
        virtual void WriteEndDocument(dot::string rootElementName) = 0;

        /// Write element start tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.
        virtual void WriteStartElement(dot::string elementName) = 0;

        /// Write element end tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.
        /// The element name passed to this method must match the element name passed
        /// to the matching WriteStartElement(name) call at the same indent level.
        virtual void WriteEndElement(dot::string elementName) = 0;

        /// Write dictionary start tag. A call to this method
        /// must follow WriteStartElement(name).
        virtual void WriteStartDict() = 0;

        /// Write dictionary end tag. A call to this method
        /// must be followed by WriteEndElement(name).
        virtual void WriteEndDict() = 0;

        /// Write start tag for an array. A call to this method
        /// must follow WriteStartElement(name).
        virtual void WriteStartArray() = 0;

        /// Write end tag for an array. A call to this method
        /// must be followed by WriteEndElement(name).
        virtual void WriteEndArray() = 0;

        /// Write start tag for an array item. A call to this method
        /// must follow either WriteStartArray() or WriteEndArrayItem().
        virtual void WriteStartArrayItem() = 0;

        /// Write end tag for an array item. A call to this method
        /// must be followed by either WriteEndArray() or WriteStartArrayItem().
        virtual void WriteEndArrayItem() = 0;

        /// Write value start tag. A call to this method
        /// must follow WriteStartElement(name).
        virtual void WriteStartValue() = 0;

        /// Write value end tag. A call to this method
        /// must be followed by WriteEndElement(name).
        virtual void WriteEndValue() = 0;

        /// Write atomic value. Value type
        /// will be inferred from object.type().
        virtual void WriteValue(dot::object value) = 0;

        /// Cast to IXmlWriter (throws if the cast fails).
        //IXmlWriter AsXmlWriter() { return (IXmlWriter)obj; }

        /// Cast to IBsonWriter (throws if the cast fails).
        //IBsonWriter AsBsonWriter() { return (IBsonWriter)obj; }

        /// WriteStartElement(...) followed by WriteStartDict().
        void WriteStartDictElement(dot::string elementName);

        /// WriteEndDict(...) followed by WriteEndElement(...).
        void WriteEndDictElement(dot::string elementName);

        /// WriteStartElement(...) followed by WriteStartArray().
        void WriteStartArrayElement(dot::string elementName);

        /// WriteEndArray(...) followed by WriteEndElement(...).
        void WriteEndArrayElement(dot::string elementName);

        /// WriteStartArrayItem(...) followed by WriteStartDict().
        void WriteStartDictArrayItem();

        /// WriteEndDict(...) followed by WriteEndArrayItem(...).
        void WriteEndDictArrayItem();

        /// Write an element with no inner nodes.
        /// Element type is inferred by calling obj.type().
        void WriteValueElement(dot::string elementName, dot::object value);

        /// Write an array item with no inner nodes.
        /// Element type is inferred by calling obj.type().
        void WriteValueArrayItem(dot::object value);

        /// Write a single array item.
        void WriteArrayItem(dot::object value);

        /// Write an array of elements with no inner nodes.
        /// Element type is inferred by calling obj.type().
        template <class container>
        void WriteValueArray(dot::string elementName, container values)
        {
            this->WriteStartArrayElement(elementName);
            for (dot::object value : values)
            {
                this->WriteArrayItem(value);
            }
            this->WriteEndArrayElement(elementName);
        }
    };

}