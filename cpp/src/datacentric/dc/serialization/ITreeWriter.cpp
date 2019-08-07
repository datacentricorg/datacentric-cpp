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

#include <dc/precompiled.hpp>
#include <dc/implement.hpp>
#include <dc/serialization/ITreeWriter.hpp>
#include <dot/system/string.hpp>
#include <dot/system/object.hpp>

namespace dc
{

    /// WriteStartElement(...) followed by WriteStartDict().
    void ITreeWriterImpl::WriteStartDictElement(dot::string elementName)
    {
        this->WriteStartElement(elementName);
        this->WriteStartDict();
    }

    /// WriteEndDict(...) followed by WriteEndElement(...).
    void ITreeWriterImpl::WriteEndDictElement(dot::string elementName)
    {
        this->WriteEndDict();
        this->WriteEndElement(elementName);
    }

    /// WriteStartElement(...) followed by WriteStartArray().
    void ITreeWriterImpl::WriteStartArrayElement(dot::string elementName)
    {
        this->WriteStartElement(elementName);
        this->WriteStartArray();
    }

    /// WriteEndArray(...) followed by WriteEndElement(...).
    void ITreeWriterImpl::WriteEndArrayElement(dot::string elementName)
    {
        this->WriteEndArray();
        this->WriteEndElement(elementName);
    }

    /// WriteStartArrayItem(...) followed by WriteStartDict().
    void ITreeWriterImpl::WriteStartDictArrayItem()
    {
        this->WriteStartArrayItem();
        this->WriteStartDict();
    }

    /// WriteEndDict(...) followed by WriteEndArrayItem(...).
    void ITreeWriterImpl::WriteEndDictArrayItem()
    {
        this->WriteEndDict();
        this->WriteEndArrayItem();
    }

    /// Write an element with no inner nodes.
    /// Element type is inferred by calling obj.type().
    void ITreeWriterImpl::WriteValueElement(dot::string elementName, dot::object value)
    {
        // Do not serialize null or empty value
        if (!value.is_empty())
        {
            this->WriteStartElement(elementName);
            this->WriteStartValue();
            this->WriteValue(value);
            this->WriteEndValue();
            this->WriteEndElement(elementName);
        }
    }

    /// Write an array item with no inner nodes.
    /// Element type is inferred by calling obj.type().
    void ITreeWriterImpl::WriteValueArrayItem(dot::object value)
    {
        // Writes null or empty value as BSON null
        this->WriteStartArrayItem();
        this->WriteStartValue();
        this->WriteValue(value);
        this->WriteEndValue();
        this->WriteEndArrayItem();
    }

    /// Write a single array item.
    void ITreeWriterImpl::WriteArrayItem(dot::object value)
    {
        // Will serialize null or empty value
        this->WriteStartArrayItem();
        this->WriteStartValue();
        this->WriteValue(value);
        this->WriteEndValue();
        this->WriteEndArrayItem();
    }

}
