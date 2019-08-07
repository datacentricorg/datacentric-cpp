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
#include <dot/system/Enum.hpp>
#include <dot/system/string.hpp>
#include <dc/serialization/BsonWriter.hpp>
#include <dot/system/object.hpp>
#include <dot/system/type.hpp>
#include <dot/noda_time/local_date.hpp>
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_minute.hpp>
#include <dot/noda_time/local_date_time.hpp>
#include <dot/noda_time/local_date_util.hpp>
#include <dot/noda_time/local_time_util.hpp>
#include <dot/noda_time/local_minute_util.hpp>
#include <dot/noda_time/local_date_time_util.hpp>
#include <dot/mongo/mongo_db/bson/object_id.hpp>

#include <bsoncxx/json.hpp>

namespace dc
{

    void BsonWriterImpl::WriteStartDocument(dot::string rootElementName)
    {
        // Push state and name into the element stack. Writing the actual start tag occurs inside
        // one of WriteStartDict, WriteStartArrayItem, or WriteStartValue calls.
        elementStack_.push({ rootElementName, currentState_ });

        if (currentState_ == TreeWriterState::empty && elementStack_.size() == 1)
        {
            currentState_ = TreeWriterState::DocumentStarted;
        }
        else
            throw dot::exception(
                "A call to WriteStartDocument(...) must be the first call to the tree writer.");

    }

    void BsonWriterImpl::WriteEndDocument(dot::string rootElementName)
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::DictCompleted && elementStack_.size() == 1)
        {
            currentState_ = TreeWriterState::DocumentCompleted;
        }
        else
            throw dot::exception(
                "A call to WriteEndDocument(...) does not follow  WriteEndElement(...) at at root level.");

        // Pop the outer element name and state from the element stack
        dot::string currentElementName;
        std::pair<dot::string, TreeWriterState> top = elementStack_.top();
        elementStack_.pop();
        currentElementName = top.first;
        currentState_ = top.second;

        // Check that the current element name matches the specified name. Writing the actual end tag
        // occurs inside one of WriteStartDict, WriteStartArrayItem, or WriteStartValue calls.
        if (rootElementName != currentElementName)
            throw dot::exception(dot::string::format(
                "WriteEndDocument({0}) follows WriteStartDocument({1}), root element name mismatch.", rootElementName, currentElementName));
    }

    void BsonWriterImpl::WriteStartElement(dot::string elementName)
    {
        // Push state and name into the element stack. Writing the actual start tag occurs inside
        // one of WriteStartDict, WriteStartArrayItem, or WriteStartValue calls.
        elementStack_.push({ elementName, currentState_ });

        if (currentState_ == TreeWriterState::DocumentStarted) currentState_ = TreeWriterState::ElementStarted;
        else if (currentState_ == TreeWriterState::ElementCompleted) currentState_ = TreeWriterState::ElementStarted;
        else if (currentState_ == TreeWriterState::DictStarted) currentState_ = TreeWriterState::ElementStarted;
        else if (currentState_ == TreeWriterState::DictArrayItemStarted) currentState_ = TreeWriterState::ElementStarted;
        else
            throw dot::exception(
                "A call to WriteStartElement(...) must be the first call or follow WriteEndElement(prevName).");

        // Write "elementName" :
        bsonWriter_.key_owned(*(elementStack_.top().first));
    }

    void BsonWriterImpl::WriteEndElement(dot::string elementName)
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ElementStarted) currentState_ = TreeWriterState::ElementCompleted;
        else if (currentState_ == TreeWriterState::DictCompleted) currentState_ = TreeWriterState::ElementCompleted;
        else if (currentState_ == TreeWriterState::ValueCompleted) currentState_ = TreeWriterState::ElementCompleted;
        else if (currentState_ == TreeWriterState::ArrayCompleted) currentState_ = TreeWriterState::ElementCompleted;
        else throw dot::exception(
            "A call to WriteEndElement(...) does not follow a matching WriteStartElement(...) at the same indent level.");

        // Pop the outer element name and state from the element stack
        //(currentElementName, currentState_) = elementStack_.Pop();
        dot::string currentElementName;
        std::pair<dot::string, TreeWriterState> top = elementStack_.top();
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

    /// Write dictionary start tag. A call to this method
    /// must follow WriteStartElement(...) or WriteStartArrayItem().
    void BsonWriterImpl::WriteStartDict()
    {
        // Save initial state to be used below
        TreeWriterState prevState = currentState_;

        // Check state transition matrix
        if (currentState_ == TreeWriterState::DocumentStarted) currentState_ = TreeWriterState::DictStarted;
        else if (currentState_ == TreeWriterState::ElementStarted) currentState_ = TreeWriterState::DictStarted;
        else if (currentState_ == TreeWriterState::ArrayItemStarted) currentState_ = TreeWriterState::DictArrayItemStarted;
        else
            throw dot::exception(
                "A call to WriteStartDict() must follow WriteStartElement(...) or WriteStartArrayItem().");

        // Write {
        bsonWriter_.open_document();

        // If prev state is DocumentStarted, write _t tag
        //if (prevState == TreeWriterState::DocumentStarted)
        //{
        //    dot::string rootElementName = elementStack_.top().first;
        //    if (!rootElementName->ends_with("Key"))  // TODO remove it
        //        this->WriteValueElement("_t", rootElementName);
        //}
    }

    void BsonWriterImpl::WriteEndDict()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::DictStarted) currentState_ = TreeWriterState::DictCompleted;
        else if (currentState_ == TreeWriterState::DictArrayItemStarted) currentState_ = TreeWriterState::DictArrayItemCompleted;
        else if (currentState_ == TreeWriterState::ElementCompleted) currentState_ = TreeWriterState::DictCompleted;
        else
            throw dot::exception(
                "A call to WriteEndDict(...) does not follow a matching WriteStartDict(...) at the same indent level.");

        // Write }
        bsonWriter_.close_document();
    }

    void BsonWriterImpl::WriteStartArray()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ElementStarted) currentState_ = TreeWriterState::ArrayStarted;
        else
            throw dot::exception(
                "A call to WriteStartArray() must follow WriteStartElement(...).");

        // Write [
        bsonWriter_.open_array();
    }

    void BsonWriterImpl::WriteEndArray()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ArrayStarted) currentState_ = TreeWriterState::ArrayCompleted;
        else if (currentState_ == TreeWriterState::ArrayItemCompleted) currentState_ = TreeWriterState::ArrayCompleted;
        else
            throw dot::exception(
                "A call to WriteEndArray(...) does not follow WriteEndArrayItem(...).");

        // Write ]
        bsonWriter_.close_array();
    }

    void BsonWriterImpl::WriteStartArrayItem()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ArrayStarted) currentState_ = TreeWriterState::ArrayItemStarted;
        else if (currentState_ == TreeWriterState::ArrayItemCompleted) currentState_ = TreeWriterState::ArrayItemStarted;
        else throw dot::exception(
            "A call to WriteStartArrayItem() must follow WriteStartElement(...) or WriteEndArrayItem().");

        // Nothing to write here
    }

    void BsonWriterImpl::WriteEndArrayItem()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ArrayItemStarted) currentState_ = TreeWriterState::ArrayItemCompleted;
        else if (currentState_ == TreeWriterState::DictArrayItemCompleted) currentState_ = TreeWriterState::ArrayItemCompleted;
        else if (currentState_ == TreeWriterState::ValueArrayItemCompleted) currentState_ = TreeWriterState::ArrayItemCompleted;
        else
            throw dot::exception(
                "A call to WriteEndArrayItem(...) does not follow a matching WriteStartArrayItem(...) at the same indent level.");

        // Nothing to write here
    }

    void BsonWriterImpl::WriteStartValue()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ElementStarted) currentState_ = TreeWriterState::ValueStarted;
        else if (currentState_ == TreeWriterState::ArrayItemStarted) currentState_ = TreeWriterState::ValueArrayItemStarted;
        else
            throw dot::exception(
                "A call to WriteStartValue() must follow WriteStartElement(...) or WriteStartArrayItem().");

        // Nothing to write here
    }

    void BsonWriterImpl::WriteEndValue()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ValueWritten) currentState_ = TreeWriterState::ValueCompleted;
        else if (currentState_ == TreeWriterState::ValueArrayItemWritten) currentState_ = TreeWriterState::ValueArrayItemCompleted;
        else
            throw dot::exception(
                "A call to WriteEndValue(...) does not follow a matching WriteValue(...) at the same indent level.");

        // Nothing to write here
    }

    void BsonWriterImpl::WriteValue(dot::object value)
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ValueStarted) currentState_ = TreeWriterState::ValueWritten;
        else if (currentState_ == TreeWriterState::ValueArrayItemStarted) currentState_ = TreeWriterState::ValueArrayItemWritten;
        else
            throw dot::exception(
                "A call to WriteEndValue(...) does not follow a matching WriteValue(...) at the same indent level.");

        if (value.is_empty())
        {
            // Null or empty value is serialized as null BSON value.
            // We should only get her for an array as for dictionaries
            // null values should be skipped
            bsonWriter_.append(bsoncxx::types::b_null{});
            return;
        }

        // Serialize based on value type
        dot::type_t valueType = value->type();

        if (valueType->equals(dot::typeof<dot::string>()))
            bsonWriter_.append(*(dot::string)value);
        else
        if (valueType->equals(dot::typeof<double>())) // ? TODO check dot::typeof<Double>() dot::typeof<NullableDouble>()
            bsonWriter_.append((double)value);
        else
        if (valueType->equals(dot::typeof<bool>()))
            bsonWriter_.append((bool)value);
        else
        if (valueType->equals(dot::typeof<int>()))
            bsonWriter_.append((int)value);
        else
        if (valueType->equals(dot::typeof<int64_t>()))
            bsonWriter_.append((int64_t)value);
        else
        if (valueType->equals(dot::typeof<dot::local_date>()))
            bsonWriter_.append(dot::local_date_util::to_iso_int((dot::local_date)value));
        else
        if (valueType->equals(dot::typeof<dot::local_time>()))
            bsonWriter_.append(dot::local_time_util::to_iso_int((dot::local_time)value));
        else
        if (valueType->equals(dot::typeof<dot::local_minute>()))
            bsonWriter_.append(dot::local_minute_util::to_iso_int((dot::local_minute) value));
        else
        if (valueType->equals(dot::typeof<dot::local_date_time>()))
            bsonWriter_.append(bsoncxx::types::b_date{ dot::local_date_time_util::to_std_chrono((dot::local_date_time)value) });
        else
        if (valueType->equals(dot::typeof<dot::object_id>()))
            bsonWriter_.append(((dot::struct_wrapper<dot::object_id>)value)->oid());
        else
        if (valueType->is_enum)
            bsonWriter_.append(*value->to_string());
        else
            throw dot::exception(dot::string::format("Element type {0} is not supported for BSON serialization.", valueType));
    }

    dot::string BsonWriterImpl::to_string()
    {
        return bsoncxx::to_json(bsonWriter_.view_array()[0].get_document().view());
    }

    bsoncxx::document::view BsonWriterImpl::view()
    {
        return bsonWriter_.view_array()[0].get_document().view();
    }

}