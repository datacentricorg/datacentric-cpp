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
#define RAPIDJSON_HAS_STDSTRING 1
#include <dc/implement.hpp>
#include <dc/serialization/json_writer.hpp>
#include <dot/system/enum.hpp>
#include <dot/system/string.hpp>
#include <dot/system/object.hpp>
#include <dot/system/type.hpp>
#include <dot/noda_time/local_date.hpp>
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_minute.hpp>
#include <dot/noda_time/local_date_time.hpp>
#include <dot/mongo/mongo_db/bson/object_id.hpp>
#include <dot/noda_time/local_date_util.hpp>
#include <dot/noda_time/local_time_util.hpp>
#include <dot/noda_time/local_minute_util.hpp>
#include <dot/noda_time/local_date_time_util.hpp>

namespace dc
{
    json_writer_impl::json_writer_impl()
        : jsonWriter_(buffer_)
        , currentState_(tree_writer_state::empty)
    {}

    void json_writer_impl::WriteStartDocument(dot::string rootElementName)
    {
        // Push state and name into the element stack. Writing the actual start tag occurs inside
        // one of WriteStartDict, WriteStartArrayItem, or WriteStartValue calls.
        elementStack_.push({ rootElementName, currentState_ });

        if (currentState_ == tree_writer_state::empty && elementStack_.size() == 1)
        {
            currentState_ = tree_writer_state::DocumentStarted;
        }
        else
            throw dot::exception(
                "A call to WriteStartDocument(...) must be the first call to the tree writer.");
    }

    void json_writer_impl::WriteEndDocument(dot::string rootElementName)
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::DictCompleted && elementStack_.size() == 1)
        {
            currentState_ = tree_writer_state::DocumentCompleted;
        }
        else
            throw dot::exception(
                "A call to WriteEndDocument(...) does not follow  WriteEndElement(...) at at root level.");

        // Pop the outer element name and state from the element stack
        dot::string currentElementName;
        std::pair<dot::string, tree_writer_state> top = elementStack_.top();
        elementStack_.pop();
        currentElementName = top.first;
        currentState_ = top.second;

        // Check that the current element name matches the specified name. Writing the actual end tag
        // occurs inside one of WriteStartDict, WriteStartArrayItem, or WriteStartValue calls.
        if (rootElementName != currentElementName)
            throw dot::exception(dot::string::format(
                "WriteEndDocument({0}) follows WriteStartDocument({1}), root element name mismatch.", rootElementName, currentElementName));
    }

    void json_writer_impl::WriteStartElement(dot::string elementName)
    {
        // Push state and name into the element stack. Writing the actual start tag occurs inside
        // one of WriteStartDict, WriteStartArrayItem, or WriteStartValue calls.
        elementStack_.push({ elementName, currentState_ });

        if (currentState_ == tree_writer_state::DocumentStarted) currentState_ = tree_writer_state::ElementStarted;
        else if (currentState_ == tree_writer_state::ElementCompleted) currentState_ = tree_writer_state::ElementStarted;
        else if (currentState_ == tree_writer_state::DictStarted) currentState_ = tree_writer_state::ElementStarted;
        else if (currentState_ == tree_writer_state::DictArrayItemStarted) currentState_ = tree_writer_state::ElementStarted;
        else
            throw dot::exception(
                "A call to WriteStartElement(...) must be the first call or follow WriteEndElement(prevName).");

        // Write "elementName" :
        jsonWriter_.Key(*elementStack_.top().first);
    }

    void json_writer_impl::WriteEndElement(dot::string elementName)
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::ElementStarted) currentState_ = tree_writer_state::ElementCompleted;
        else if (currentState_ == tree_writer_state::DictCompleted) currentState_ = tree_writer_state::ElementCompleted;
        else if (currentState_ == tree_writer_state::ValueCompleted) currentState_ = tree_writer_state::ElementCompleted;
        else if (currentState_ == tree_writer_state::ArrayCompleted) currentState_ = tree_writer_state::ElementCompleted;
        else throw dot::exception(
            "A call to WriteEndElement(...) does not follow a matching WriteStartElement(...) at the same indent level.");

        // Pop the outer element name and state from the element stack
        //(currentElementName, currentState_) = elementStack_.Pop();
        dot::string currentElementName;
        std::pair<dot::string, tree_writer_state> top = elementStack_.top();
        elementStack_.pop();
        currentElementName = top.first;
        currentState_ = top.second;

        // Check that the current element name matches the specified name. Writing the actual end tag
        // occurs inside one of WriteStartDict, WriteStartArrayItem, or WriteStartValue calls.
        if (elementName != currentElementName)
            throw dot::exception(
                dot::string::format("EndComplexElement({0}) follows StartComplexElement({1}), element name mismatch.", elementName, currentElementName));

        // Nothing to write here but array closing bracket was written above
    }

    void json_writer_impl::WriteStartDict()
    {
        // Save initial state to be used below
        tree_writer_state prevState = currentState_;

        // Check state transition matrix
        if (currentState_ == tree_writer_state::DocumentStarted) currentState_ = tree_writer_state::DictStarted;
        else if (currentState_ == tree_writer_state::ElementStarted) currentState_ = tree_writer_state::DictStarted;
        else if (currentState_ == tree_writer_state::ArrayItemStarted) currentState_ = tree_writer_state::DictArrayItemStarted;
        else
            throw dot::exception(
                "A call to WriteStartDict() must follow WriteStartElement(...) or WriteStartArrayItem().");

        // Write {
        jsonWriter_.StartObject();

        // If prev state is DocumentStarted, write _t tag
        //if (prevState == tree_writer_state::DocumentStarted)
        //{
        //    dot::string rootElementName = elementStack_.top().first;
        //    if (!rootElementName->ends_with("Key"))  // TODO remove it
        //        this->WriteValueElement("_t", rootElementName);
        //}
    }

    void json_writer_impl::WriteEndDict()
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::DictStarted) currentState_ = tree_writer_state::DictCompleted;
        else if (currentState_ == tree_writer_state::DictArrayItemStarted) currentState_ = tree_writer_state::DictArrayItemCompleted;
        else if (currentState_ == tree_writer_state::ElementCompleted) currentState_ = tree_writer_state::DictCompleted;
        else
            throw dot::exception(
                "A call to WriteEndDict(...) does not follow a matching WriteStartDict(...) at the same indent level.");

        // Write }
        jsonWriter_.EndObject();
    }

    void json_writer_impl::WriteStartArray()
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::ElementStarted) currentState_ = tree_writer_state::ArrayStarted;
        else
            throw dot::exception(
                "A call to WriteStartArray() must follow WriteStartElement(...).");

        // Write [
        jsonWriter_.StartArray();
    }

    void json_writer_impl::WriteEndArray()
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::ArrayStarted) currentState_ = tree_writer_state::ArrayCompleted;
        else if (currentState_ == tree_writer_state::ArrayItemCompleted) currentState_ = tree_writer_state::ArrayCompleted;
        else
            throw dot::exception(
                "A call to WriteEndArray(...) does not follow WriteEndArrayItem(...).");

        // Write ]
        jsonWriter_.EndArray();
    }

    void json_writer_impl::WriteStartArrayItem()
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::ArrayStarted) currentState_ = tree_writer_state::ArrayItemStarted;
        else if (currentState_ == tree_writer_state::ArrayItemCompleted) currentState_ = tree_writer_state::ArrayItemStarted;
        else throw dot::exception(
            "A call to WriteStartArrayItem() must follow WriteStartElement(...) or WriteEndArrayItem().");

        // Nothing to write here
    }

    void json_writer_impl::WriteEndArrayItem()
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::ArrayItemStarted) currentState_ = tree_writer_state::ArrayItemCompleted;
        else if (currentState_ == tree_writer_state::DictArrayItemCompleted) currentState_ = tree_writer_state::ArrayItemCompleted;
        else if (currentState_ == tree_writer_state::ValueArrayItemCompleted) currentState_ = tree_writer_state::ArrayItemCompleted;
        else
            throw dot::exception(
                "A call to WriteEndArrayItem(...) does not follow a matching WriteStartArrayItem(...) at the same indent level.");

        // Nothing to write here
    }

    void json_writer_impl::WriteStartValue()
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::ElementStarted) currentState_ = tree_writer_state::ValueStarted;
        else if (currentState_ == tree_writer_state::ArrayItemStarted) currentState_ = tree_writer_state::ValueArrayItemStarted;
        else
            throw dot::exception(
                "A call to WriteStartValue() must follow WriteStartElement(...) or WriteStartArrayItem().");

        // Nothing to write here
    }

    void json_writer_impl::WriteEndValue()
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::ValueWritten) currentState_ = tree_writer_state::ValueCompleted;
        else if (currentState_ == tree_writer_state::ValueArrayItemWritten) currentState_ = tree_writer_state::ValueArrayItemCompleted;
        else
            throw dot::exception(
                "A call to WriteEndValue(...) does not follow a matching WriteValue(...) at the same indent level.");

        // Nothing to write here
    }

    void json_writer_impl::WriteValue(dot::object value)
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::ValueStarted) currentState_ = tree_writer_state::ValueWritten;
        else if (currentState_ == tree_writer_state::ValueArrayItemStarted) currentState_ = tree_writer_state::ValueArrayItemWritten;
        else
            throw dot::exception(
                "A call to WriteEndValue(...) does not follow a matching WriteValue(...) at the same indent level.");

        if (value.is_empty())
        {
            // Null or empty value is serialized as null JSON value.
            // We should only get her for an array as for dictionaries
            // null values should be skipped
            jsonWriter_.Null();
            return;
        }

        // Serialize based on value type
        dot::type valueType = value->get_type();

        if (valueType->equals(dot::typeof<dot::string>()))
            jsonWriter_.String(*(dot::string)value);
        else
        if (valueType->equals(dot::typeof<double>())) // ? TODO check dot::typeof<Double>() dot::typeof<NullableDouble>()
            jsonWriter_.Double((double)value);
        else
        if (valueType->equals(dot::typeof<bool>()))
            jsonWriter_.Bool((bool)value);
        else
        if (valueType->equals(dot::typeof<int>()))
            jsonWriter_.Int((int)value);
        else
        if (valueType->equals(dot::typeof<int64_t>()))
            jsonWriter_.Int64((int64_t)value);
        else
        if (valueType->equals(dot::typeof<dot::local_date>()))
            jsonWriter_.Int(dot::local_date_util::to_iso_int((dot::local_date)value));
        else
        if (valueType->equals(dot::typeof<dot::local_time>()))
            jsonWriter_.Int(dot::local_time_util::to_iso_int((dot::local_time)value));
        else
        if (valueType->equals(dot::typeof<dot::local_minute>()))
            jsonWriter_.Int(dot::local_minute_util::to_iso_int((dot::local_minute) value));
        else
        if (valueType->equals(dot::typeof<dot::local_date_time>()))
            jsonWriter_.Int64(dot::local_date_time_util::to_iso_long((dot::local_date_time)value));
        else
        if (valueType->equals(dot::typeof<dot::object_id>()))
            jsonWriter_.String(*value->to_string());
        else
        if (valueType->is_enum)
            jsonWriter_.String(*value->to_string());
        else
            throw dot::exception(dot::string::format("Element type {0} is not supported for JSON serialization.", valueType));
    }

    dot::string json_writer_impl::to_string()
    {
        return buffer_.GetString();
    }
}
