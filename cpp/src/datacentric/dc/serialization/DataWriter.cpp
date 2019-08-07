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
#include <dc/serialization/DataWriter.hpp>
#include <dot/noda_time/local_time_util.hpp>
#include <dot/noda_time/local_minute_util.hpp>
#include <dot/noda_time/local_date_util.hpp>
#include <dot/noda_time/local_date_time_util.hpp>
#include <dc/types/record/key_base.hpp>
#include <dot/system/Enum.hpp>
#include <dot/system/reflection/activator.hpp>
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_minute.hpp>
#include <dot/noda_time/local_date.hpp>
#include <dot/noda_time/local_date_time.hpp>

namespace dc
{
    DataWriterImpl::DataWriterImpl(data data_obj)
        : currentDict_(data_obj)
        , currentState_(TreeWriterState::empty) {}

    void DataWriterImpl::WriteStartDocument(dot::string rootElementName)
    {
        if (currentState_ == TreeWriterState::empty && elementStack_.size() == 0)
        {
            currentState_ = TreeWriterState::DocumentStarted;
        }
        else
        {
            throw dot::exception("A call to WriteStartDocument(...) must be the first call to the tree writer.");
        }

        dot::string rootName = currentDict_->type()->name;
        // Check that the name matches
        if (rootElementName != rootName) throw dot::exception(
            dot::string::format("Attempting to deserialize data for type {0} into type {1}.", rootElementName, rootName));

        rootElementName_ = rootElementName;
        currentElementName_ = rootElementName;
        auto currentDictInfoList = currentDict_->type()->get_fields();
        currentDictElements_ = dot::make_dictionary<dot::string, dot::field_info>();
        for(auto elementInfo : currentDictInfoList) currentDictElements_->add(elementInfo->name, elementInfo);
        currentArray_ = nullptr;
        currentArrayItemType_ = nullptr;
    }

    void DataWriterImpl::WriteEndDocument(dot::string rootElementName)
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::DocumentStarted && elementStack_.size() == 0) currentState_ = TreeWriterState::DocumentCompleted;
        else throw dot::exception(
            "A call to WriteEndDocument(...) does not follow  WriteEndElement(...) at at root level.");

        // Check that the current element name matches the specified name. Writing the actual end tag
        // occurs inside one of WriteStartDict, WriteStartArrayItem, or WriteStartValue calls.
        if (rootElementName != rootElementName_)
            throw dot::exception(dot::string::format(
                "WriteEndDocument({0}) follows WriteStartDocument({1}), root element name mismatch.", rootElementName, rootElementName_));
    }

    void DataWriterImpl::WriteStartElement(dot::string elementName)
    {
        if (currentState_ == TreeWriterState::DocumentStarted) currentState_ = TreeWriterState::ElementStarted;
        else if (currentState_ == TreeWriterState::ElementCompleted) currentState_ = TreeWriterState::ElementStarted;
        else if (currentState_ == TreeWriterState::DictStarted) currentState_ = TreeWriterState::ElementStarted;
        else if (currentState_ == TreeWriterState::DictArrayItemStarted) currentState_ = TreeWriterState::ElementStarted;
        else throw dot::exception(
            "A call to WriteStartElement(...) must be the first call or follow WriteEndElement(prevName).");

        currentElementName_ = elementName;
        currentElementInfo_ = currentDictElements_[elementName];
    }

    void DataWriterImpl::WriteEndElement(dot::string elementName)
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ElementStarted) currentState_ = TreeWriterState::ElementCompleted;
        else if (currentState_ == TreeWriterState::DictCompleted) currentState_ = TreeWriterState::ElementCompleted;
        else if (currentState_ == TreeWriterState::ValueCompleted) currentState_ = TreeWriterState::ElementCompleted;
        else if (currentState_ == TreeWriterState::ArrayCompleted) currentState_ = TreeWriterState::ElementCompleted;
        else throw dot::exception(
            "A call to WriteEndElement(...) does not follow a matching WriteStartElement(...) at the same indent level.");

        // Check that the current element name matches the specified name. Writing the actual end tag
        // occurs inside one of WriteStartDict, WriteStartArrayItem, or WriteStartValue calls.
        if (elementName != currentElementName_)
            throw dot::exception(dot::string::format(
                "EndComplexElement({0}) follows StartComplexElement({1}), element name mismatch.", elementName, currentElementName_));
    }

    void DataWriterImpl::WriteStartDict()
    {
        // Push state before defining dictionary state
        PushState();

        // Check state transition matrix
        if (currentState_ == TreeWriterState::DocumentStarted)
        {
            currentState_ = TreeWriterState::DictStarted;

            // Return if this call follows StartDocument, all setup is done in StartDocument
            return;
        }
        else if (currentState_ == TreeWriterState::ElementStarted) currentState_ = TreeWriterState::DictStarted;
        else if (currentState_ == TreeWriterState::ArrayItemStarted) currentState_ = TreeWriterState::DictArrayItemStarted;
        else throw dot::exception(
            "A call to WriteStartDict() must follow WriteStartElement(...) or WriteStartArrayItem().");

        // Set dictionary info
        dot::type_t createdDictType = nullptr;
        if (currentArray_ != nullptr) createdDictType = currentArrayItemType_;
        else if (currentDict_ != nullptr) createdDictType = currentElementInfo_->field_type;
        else throw dot::exception("Value can only be added to a dictionary or array.");

        dot::object createdDictObj = dot::activator::create_instance(createdDictType);
        if (!(createdDictObj.is<data>())) // TODO Also support native dictionaries
        {
            dot::string mappedClassName = currentElementInfo_->field_type->full_name();
            throw dot::exception(dot::string::format(
                "Element {0} of type {1} does not implement Data.", currentElementInfo_->name, mappedClassName));
        }

        auto createdDict = (data) createdDictObj;

        // Add to array or dictionary, depending on what we are inside of
        if (currentArray_ != nullptr) currentArray_->add_object(createdDict);
        else if (currentDict_ != nullptr) currentElementInfo_->set_value(currentDict_, createdDict);
        else throw dot::exception("Value can only be added to a dictionary or array.");

        currentDict_ = (data) createdDict;
        auto currentDictInfoList = createdDictType->get_fields();
        currentDictElements_ = dot::make_dictionary<dot::string, dot::field_info>();
        for (auto elementInfo : currentDictInfoList) currentDictElements_->add(elementInfo->name, elementInfo);
        currentArray_ = nullptr;
        currentArrayItemType_ = nullptr;
    }

    void DataWriterImpl::WriteEndDict()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::DictStarted) currentState_ = TreeWriterState::DictCompleted;
        else if (currentState_ == TreeWriterState::DictArrayItemStarted) currentState_ = TreeWriterState::DictArrayItemCompleted;
        else if (currentState_ == TreeWriterState::ElementCompleted) currentState_ = TreeWriterState::DictCompleted;
        else throw dot::exception(
            "A call to WriteEndDict(...) does not follow a matching WriteStartDict(...) at the same indent level.");

        // Restore previous state
        PopState();
    }

    void DataWriterImpl::WriteStartArray()
    {
        // Push state
        PushState();

        // Check state transition matrix
        if (currentState_ == TreeWriterState::ElementStarted) currentState_ = TreeWriterState::ArrayStarted;
        else
            throw dot::exception(
                "A call to WriteStartArray() must follow WriteStartElement(...).");

        // Create the array
        dot::object createdArrayObj = dot::activator::create_instance(currentElementInfo_->field_type);
        if (createdArrayObj.is<dot::list_base>()) // TODO Also support native arrays
        {
            // Add to array or dictionary, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->add_object(createdArrayObj);
            else if (currentDict_ != nullptr) currentElementInfo_->set_value(currentDict_, createdArrayObj);
            else throw dot::exception("Value can only be added to a dictionary or array.");

            currentArray_ = (dot::list_base) createdArrayObj;

            // Get array item type from array type using reflection
            dot::type_t listType = currentElementInfo_->field_type;      // TODO fix
//            if (!listType->IsGenericType) throw dot::exception(dot::string::format(
//                "Type {0} cannot be serialized because it implements only IList but not IList<T>.", listType));
//            list<Type> genericParameterTypes = listType->GenericTypeArguments;
            dot::list<dot::type_t> genericParameterTypes = listType->get_generic_arguments();
            if (genericParameterTypes->count() != 1) throw dot::exception(
                dot::string::format("Generic parameter type list {0} has more than ", genericParameterTypes) +
                "one element creating an ambiguity for deserialization code.");
            currentArrayItemType_ = genericParameterTypes[0];

            currentDict_ = nullptr;
            currentElementInfo_ = nullptr;
            currentDictElements_ = nullptr;
        }
        //else {
        //    dot::string mappedClassName = currentElementInfo_->field_type->get_full_name();
        //    throw dot::exception(dot::string::format(
        //        "Element {0} of type {1} does not implement ICollection.", currentElementInfo_->name, mappedClassName));
        //}
    }

    void DataWriterImpl::WriteEndArray()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ArrayItemCompleted) currentState_ = TreeWriterState::ArrayCompleted;
        else throw dot::exception(
            "A call to WriteEndArray(...) does not follow WriteEndArrayItem(...).");

        // Pop state
        PopState();
    }

    void DataWriterImpl::WriteStartArrayItem()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ArrayStarted) currentState_ = TreeWriterState::ArrayItemStarted;
        else if (currentState_ == TreeWriterState::ArrayItemCompleted) currentState_ = TreeWriterState::ArrayItemStarted;
        else throw dot::exception(
            "A call to WriteStartArrayItem() must follow WriteStartElement(...) or WriteEndArrayItem().");

        //Object addedItem = nullptr;
        //if (currentArrayItemType_ == dot::typeof<String>()) addedItem = nullptr;
        //else if (currentArrayItemType_ == dot::typeof<double>()) addedItem = double();
        //else if (currentArrayItemType_ == dot::typeof<nullable<double>>()) addedItem = nullptr;
        //else if (currentArrayItemType_ == dot::typeof<bool>()) addedItem = bool();
        //else if (currentArrayItemType_ == dot::typeof<nullable<bool>>()) addedItem = nullptr;
        //else if (currentArrayItemType_ == dot::typeof<int>()) addedItem = int();
        //else if (currentArrayItemType_ == dot::typeof<nullable<int>>()) addedItem = nullptr;
        //else if (currentArrayItemType_ == dot::typeof<int64_t>()) addedItem = int64_t();
        //else if (currentArrayItemType_ == dot::typeof<nullable<int64_t>>()) addedItem = nullptr;
        //else if (currentArrayItemType_ == dot::typeof<local_date>()) addedItem = dot::local_date();
        //else if (currentArrayItemType_ == dot::typeof<nullable<local_date>>()) addedItem = nullptr;
        //else if (currentArrayItemType_ == dot::typeof<local_time>()) addedItem = dot::local_time();
        //else if (currentArrayItemType_ == dot::typeof<nullable<local_time>>()) addedItem = nullptr;
        //else if (currentArrayItemType_ == dot::typeof<local_date_time>()) addedItem = dot::local_date_time();
        //else if (currentArrayItemType_ == dot::typeof<nullable<local_date_time>>()) addedItem = nullptr;
        //else if (currentArrayItemType_->IsClass) addedItem = nullptr;
        //else throw dot::exception(dot::string::format("Value type {0} is not supported for serialization.", currentArrayItemType_->name));

        //currentArray_->add_object(addedItem);
        //currentArrayItem_ = addedItem;
    }

    void DataWriterImpl::WriteEndArrayItem()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ArrayItemStarted) currentState_ = TreeWriterState::ArrayItemCompleted;
        else if (currentState_ == TreeWriterState::DictArrayItemCompleted) currentState_ = TreeWriterState::ArrayItemCompleted;
        else if (currentState_ == TreeWriterState::ValueArrayItemCompleted) currentState_ = TreeWriterState::ArrayItemCompleted;
        else throw dot::exception(
            "A call to WriteEndArrayItem(...) does not follow a matching WriteStartArrayItem(...) at the same indent level.");

        // Do nothing here
    }

    void DataWriterImpl::WriteStartValue()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ElementStarted) currentState_ = TreeWriterState::ValueStarted;
        else if (currentState_ == TreeWriterState::ArrayItemStarted) currentState_ = TreeWriterState::ValueArrayItemStarted;
        else throw dot::exception(
            "A call to WriteStartValue() must follow WriteStartElement(...) or WriteStartArrayItem().");
    }

    void DataWriterImpl::WriteEndValue()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ValueWritten) currentState_ = TreeWriterState::ValueCompleted;
        else if (currentState_ == TreeWriterState::ValueArrayItemWritten) currentState_ = TreeWriterState::ValueArrayItemCompleted;
        else throw dot::exception(
            "A call to WriteEndValue(...) does not follow a matching WriteValue(...) at the same indent level.");

        // Nothing to write here
    }

    void DataWriterImpl::WriteValue(dot::object value)
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ValueStarted) currentState_ = TreeWriterState::ValueWritten;
        else if (currentState_ == TreeWriterState::ValueArrayItemStarted) currentState_ = TreeWriterState::ValueArrayItemWritten;
        else throw dot::exception(
            "A call to WriteEndValue(...) does not follow a matching WriteValue(...) at the same indent level.");

        // Check that we are either inside dictionary or array
        dot::type_t elementType = nullptr;
        if (currentArray_ != nullptr) elementType = currentArrayItemType_;
        else if (currentDict_ != nullptr) elementType = currentElementInfo_->field_type;
        else throw dot::exception(
            dot::string::format("Cannot WriteValue(...)for element {0} ", currentElementName_) +
            "is called outside dictionary or array.");

        if (value.is_empty())  // TODO is_empty method should be implemented according to c# extension
        {
            // Do not record null or empty value into dictionary, but add it to an array
            // Add to dictionary or array, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->add_object(nullptr);
            return;
        }

        // Write based on element type
        dot::type_t valueType = value->type();
        if (elementType->equals(dot::typeof<dot::string>()) ||
            elementType->equals(dot::typeof<double>()) || elementType->equals(dot::typeof<dot::nullable<double>>()) ||
            elementType->equals(dot::typeof<bool>()) || elementType->equals(dot::typeof<dot::nullable<bool>>()) ||
            elementType->equals(dot::typeof<int>()) || elementType->equals(dot::typeof<dot::nullable<int>>()) ||
            elementType->equals(dot::typeof<int64_t>()) || elementType->equals(dot::typeof<dot::nullable<int64_t>>()) ||
            elementType->equals(dot::typeof<dot::object_id>())
            )
        {
            // Check type match
            //if (!elementType->equals(valueType)) // TODO change to !elementType->IsAssignableFrom(valueType)
            //    throw dot::exception(
            //        dot::string::format("Attempting to deserialize value of type {0} ", valueType->name) +
            //        dot::string::format("into element of type {0}.", elementType->name));

            dot::object convertedValue = value;
            if (elementType->equals(dot::typeof<double>()))
            {
                if (valueType->equals(dot::typeof<int>())) convertedValue = static_cast<double>((int) value);
                if (valueType->equals(dot::typeof<int64_t>())) convertedValue = static_cast<double>((int64_t) value);
            }
            else if (elementType->equals(dot::typeof<int64_t>()) && valueType->equals(dot::typeof<int>()))
            {
                convertedValue = static_cast<int64_t>((int) value);
            }
            else if (elementType->equals(dot::typeof<int>()) && valueType->equals(dot::typeof<int64_t>()))
            {
                convertedValue = static_cast<int>((int64_t) value);
            }
            else if (elementType->equals(dot::typeof<dot::object_id>()) && valueType->equals(dot::typeof<dot::string>()))
            {
                convertedValue = dot::object_id((dot::string) value);
            }

            // Add to array or dictionary, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->add_object(convertedValue);
            else if (currentDict_ != nullptr) currentElementInfo_->set_value(currentDict_, convertedValue);
            else throw dot::exception("Value can only be added to a dictionary or array.");
        }
        else if (elementType->equals(dot::typeof<dot::local_date>()) || elementType->equals(dot::typeof<dot::nullable<dot::local_date>>()))
        {
            dot::local_date dateValue;

            // Check type match
            if (valueType->equals(dot::typeof<int>()))
            {
                // Deserialize local_date as ISO int in yyyymmdd format
                dateValue = dot::local_date_util::parse_iso_int((int) value);
            }
            else if (valueType->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_date as ISO int in yyyymmdd format
                dateValue = dot::local_date_util::parse_iso_int((int) value);
            }
            else throw dot::exception(
                    dot::string::format("Attempting to deserialize value of type {0} ", valueType->name) +
                    "into local_date; type should be int32.");

            // Add to array or dictionary, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->add_object(dateValue);
            else if (currentDict_ != nullptr) currentElementInfo_->set_value(currentDict_, dateValue);
            else throw dot::exception("Value can only be added to a dictionary or array.");
        }
        else if (elementType->equals(dot::typeof<dot::local_time>()) || elementType->equals(dot::typeof<dot::nullable<dot::local_time>>()))
        {
            dot::local_time timeValue;

            // Check type match
            if (valueType->equals(dot::typeof<int>()))
            {
                // Deserialize local_time as ISO int in hhmmssfff format
                timeValue = dot::local_time_util::parse_iso_int((int) value);
            }
            else if (valueType->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_time as ISO int in hhmmssfff format
                timeValue = dot::local_time_util::parse_iso_int((int) value);
            }
            else throw dot::exception(
                    dot::string::format("Attempting to deserialize value of type {0} ", valueType->name) +
                    "into local_time; type should be int32.");

            // Add to array or dictionary, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->add_object(timeValue);
            else if (currentDict_ != nullptr) currentElementInfo_->set_value(currentDict_, timeValue);
            else throw dot::exception("Value can only be added to a dictionary or array.");
        }
        else if (elementType->equals(dot::typeof<dot::local_minute>()) || elementType->equals(dot::typeof<dot::nullable<dot::local_minute>>()))
        {
            dot::local_minute minuteValue;

            // Check type match
            if (valueType->equals(dot::typeof<int>()))
            {
                // Deserialize local_minute as ISO int in hhmmssfff format
                minuteValue = dot::local_minute_util::parse_iso_int((int) value);
            }
            else if (valueType->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_minute as ISO int in hhmmssfff format
                minuteValue = dot::local_minute_util::parse_iso_int((int) value);
            }
            else throw dot::exception(
                dot::string::format("Attempting to deserialize value of type {0} ", valueType->name) +
                "into local_minute; type should be int32.");

            // Add to array or dictionary, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->add_object(minuteValue);
            else if (currentDict_ != nullptr) currentElementInfo_->set_value(currentDict_, minuteValue);
            else throw dot::exception("Value can only be added to a dictionary or array.");
        }
        else if (elementType->equals(dot::typeof<dot::local_date_time>()) || elementType->equals(dot::typeof<dot::nullable<dot::local_date_time>>()))
        {
        dot::local_date_time dateTimeValue;

            // Check type match
            if (valueType->equals(dot::typeof<dot::local_date_time>()))
            {
                dateTimeValue = (dot::local_date_time) value;
            }
            else if (valueType->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_date_time as ISO long in yyyymmddhhmmssfff format
                dateTimeValue = dot::local_date_time_util::parse_iso_long((int64_t) value);
            }
            else if (valueType->equals(dot::typeof<int>()))
            {
                // Deserialize local_date_time as ISO long in yyyymmddhhmmssfff format
                dateTimeValue = dot::local_date_time_util::parse_iso_long((int) value);
            }
            else if (valueType->equals(dot::typeof<dot::string>()))
            {
                // Deserialize local_date_time as ISO string
                dateTimeValue = dot::local_date_time_util::parse((dot::string) value);
            }
            else throw dot::exception(
                    dot::string::format("Attempting to deserialize value of type {0} ", valueType->name) +
                    "into local_date_time; type should be local_date_time.");

            // Add to array or dictionary, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->add_object(dateTimeValue);
            else if (currentDict_ != nullptr) currentElementInfo_->set_value(currentDict_, dateTimeValue);
            else throw dot::exception("Value can only be added to a dictionary or array.");
        }
        else if (elementType->is_enum)
        {
            // Check type match
            if (!valueType->equals(dot::typeof<dot::string>()))
                throw dot::exception(
                    dot::string::format("Attempting to deserialize value of type {0} ", valueType->name) +
                    dot::string::format("into enum {0}; type should be string.", elementType->name));

            // Deserialize enum as string
            dot::string enumString = (dot::string) value;
            dot::object enumValue = dot::enum_base::parse(elementType, enumString);

            // Add to array or dictionary, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->add_object(enumValue);
            else if (currentDict_ != nullptr) currentElementInfo_->set_value(currentDict_, enumValue);
            else throw dot::exception("Value can only be added to a dictionary or array.");
        }
        else
        {
            // We run out of value types at this point, now we can create
            // a reference type and check that it is derived from key_base
            dot::object keyObj = (key_base)dot::activator::create_instance(elementType);
            if (keyObj.is<key_base>())
            {
                key_base key = (key_base) keyObj;

                // Check type match
                if (!valueType->equals(dot::typeof<dot::string>()) && !valueType->equals(elementType))
                    throw dot::exception(
                        dot::string::format("Attempting to deserialize value of type {0} ", valueType->name) +
                        dot::string::format("into key type {0}; keys should be serialized into semicolon delimited string.", elementType->name));

                // Populate from semicolon delimited string
                dot::string stringValue = value->to_string();
                key->AssignString(stringValue);

                // Add to array or dictionary, depending on what we are inside of
                if (currentArray_ != nullptr) currentArray_->add_object(key);
                else if (currentDict_ != nullptr) currentElementInfo_->set_value(currentDict_, key);
                else throw dot::exception("Value can only be added to a dictionary or array.");
            }
            else
            {
                // Argument type is unsupported, error message
                throw dot::exception(dot::string::format("Element type {0} is not supported for serialization.", value->type()));
            }
        }
    }

    dot::string DataWriterImpl::to_string()
    {
        if (currentArray_ != nullptr) return currentArray_->to_string();
        else if (currentDict_ != nullptr) return currentDict_->to_string();
        else return type()->name;
    }

    void DataWriterImpl::PushState()
    {
        elementStack_.push(
            DataWriterPosition
            {
                currentElementName_,
                currentState_,
                currentDict_,
                currentDictElements_,
                currentElementInfo_,
                currentArray_,
                currentArrayItemType_
            });
    }

    void DataWriterImpl::PopState()
    {
        // Pop the outer element name and state from the element stack
        auto stackItem = elementStack_.top();
        elementStack_.pop();

        currentElementName_ = stackItem.CurrentElementName;
        currentState_ = stackItem.CurrentState;
        currentDict_ = stackItem.CurrentDict;
        currentDictElements_ = stackItem.CurrentDictElements;
        currentElementInfo_ = stackItem.CurrentElementInfo;
        currentArray_ = stackItem.CurrentArray;
        currentArrayItemType_ = stackItem.CurrentArrayItemType;
    }

}
