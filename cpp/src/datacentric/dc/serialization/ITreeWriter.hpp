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
#include <dot/system/ObjectImpl.hpp>

namespace dc
{
    enum class TreeWriterState
    {
        /// <summary>Empty</summary>
        Empty,

        /// <summary>State following WriteStartDocument.</summary>
        DocumentStarted,

        /// <summary>State following WriteEndDocument.</summary>
        DocumentCompleted,

        /// <summary>State following WriteStartElement.</summary>
        ElementStarted,

        /// <summary>State following WriteEndElement.</summary>
        ElementCompleted,

        /// <summary>State following WriteStartArray.</summary>
        ArrayStarted,

        /// <summary>State following WriteEndArray.</summary>
        ArrayCompleted,

        /// <summary>State following WriteStartArrayItem.</summary>
        ArrayItemStarted,

        /// <summary>State following WriteEndArrayItem.</summary>
        ArrayItemCompleted,

        /// <summary>State following WriteStartDict that is not inside an array.</summary>
        DictStarted,

        /// <summary>State following WriteEndDict that is not inside an array.</summary>
        DictCompleted,

        /// <summary>State following WriteStartDict inside an array.</summary>
        DictArrayItemStarted,

        /// <summary>State following WriteEndDict inside an array.</summary>
        DictArrayItemCompleted,

        /// <summary>State following WriteStartValue that is not inside an array.</summary>
        ValueStarted,

        /// <summary>State following WriteValue that is not inside an array.</summary>
        ValueWritten,

        /// <summary>State following WriteEndValue that is not inside an array.</summary>
        ValueCompleted,

        /// <summary>State following WriteStartValue inside an array.</summary>
        ValueArrayItemStarted,

        /// <summary>State following WriteValue inside an array.</summary>
        ValueArrayItemWritten,

        /// <summary>State following WriteEndValue inside an array.</summary>
        ValueArrayItemCompleted
    };

    class ITreeWriterImpl; using ITreeWriter = dot::ptr<ITreeWriterImpl>;
    class dot::object;
    class dot::string;

    /// <summary>Interface for writing tree data.</summary>
    class DC_CLASS ITreeWriterImpl : public dot::object_impl
    {
    public:

        /// <summary>Write start document tags. This method
        /// should be called only once for the entire document.</summary>
        virtual void WriteStartDocument(dot::string rootElementName) = 0;

        /// <summary>Write end document tag. This method
        /// should be called only once for the entire document.
        /// The root element name passed to this method must match the root element
        /// name passed to the preceding call to WriteStartDocument(...).</summary>
        virtual void WriteEndDocument(dot::string rootElementName) = 0;

        /// <summary>Write element start tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.</summary>
        virtual void WriteStartElement(dot::string elementName) = 0;

        /// <summary>Write element end tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.
        /// The element name passed to this method must match the element name passed
        /// to the matching WriteStartElement(name) call at the same indent level.</summary>
        virtual void WriteEndElement(dot::string elementName) = 0;

        /// <summary>Write dictionary start tag. A call to this method
        /// must follow WriteStartElement(name).</summary>
        virtual void WriteStartDict() = 0;

        /// <summary>Write dictionary end tag. A call to this method
        /// must be followed by WriteEndElement(name).</summary>
        virtual void WriteEndDict() = 0;

        /// <summary>Write start tag for an array. A call to this method
        /// must follow WriteStartElement(name).</summary>
        virtual void WriteStartArray() = 0;

        /// <summary>Write end tag for an array. A call to this method
        /// must be followed by WriteEndElement(name).</summary>
        virtual void WriteEndArray() = 0;

        /// <summary>Write start tag for an array item. A call to this method
        /// must follow either WriteStartArray() or WriteEndArrayItem().</summary>
        virtual void WriteStartArrayItem() = 0;

        /// <summary>Write end tag for an array item. A call to this method
        /// must be followed by either WriteEndArray() or WriteStartArrayItem().</summary>
        virtual void WriteEndArrayItem() = 0;

        /// <summary>Write value start tag. A call to this method
        /// must follow WriteStartElement(name).</summary>
        virtual void WriteStartValue() = 0;

        /// <summary>Write value end tag. A call to this method
        /// must be followed by WriteEndElement(name).</summary>
        virtual void WriteEndValue() = 0;

        /// <summary>Write atomic value. Value type
        /// will be inferred from object.GetType().</summary>
        virtual void WriteValue(dot::object value) = 0;

        /// <summary>Cast to IXmlWriter (throws if the cast fails).</summary>
        //IXmlWriter AsXmlWriter() { return (IXmlWriter)obj; }

        /// <summary>Cast to IBsonWriter (throws if the cast fails).</summary>
        //IBsonWriter AsBsonWriter() { return (IBsonWriter)obj; }

        /// <summary>WriteStartElement(...) followed by WriteStartDict().</summary>
        void WriteStartDictElement(dot::string elementName);

        /// <summary>WriteEndDict(...) followed by WriteEndElement(...).</summary>
        void WriteEndDictElement(dot::string elementName);

        /// <summary>WriteStartElement(...) followed by WriteStartArray().</summary>
        void WriteStartArrayElement(dot::string elementName);

        /// <summary>WriteEndArray(...) followed by WriteEndElement(...).</summary>
        void WriteEndArrayElement(dot::string elementName);

        /// <summary>WriteStartArrayItem(...) followed by WriteStartDict().</summary>
        void WriteStartDictArrayItem();

        /// <summary>WriteEndDict(...) followed by WriteEndArrayItem(...).</summary>
        void WriteEndDictArrayItem();

        /// <summary>Write an element with no inner nodes.
        /// Element type is inferred by calling obj.GetType().</summary>
        void WriteValueElement(dot::string elementName, dot::object value);

        /// <summary>Write an array item with no inner nodes.
        /// Element type is inferred by calling obj.GetType().</summary>
        void WriteValueArrayItem(dot::object value);

        /// <summary>Write a single array item.</summary>
        void WriteArrayItem(dot::object value);

        /// <summary>Write an array of elements with no inner nodes.
        /// Element type is inferred by calling obj.GetType().</summary>
        void WriteValueArray(dot::string elementName, dot::IObjectEnumerable values);
    };

}