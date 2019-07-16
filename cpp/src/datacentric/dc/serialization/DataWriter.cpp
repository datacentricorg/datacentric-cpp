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

#include <dc/implement.hpp>
#include <dc/serialization/DataWriter.hpp>
#include <dc/types/local_time/LocalTime.hpp>
#include <dc/types/local_minute/LocalMinute.hpp>
#include <dc/types/local_date/LocalDate.hpp>
#include <dc/types/local_date_time/LocalDateTime.hpp>
#include <dc/types/record/KeyType.hpp>
#include <dot/system/Enum.hpp>
#include <dot/system/reflection/Activator.hpp>
#include <dot/noda_time/LocalTime.hpp>
#include <dot/noda_time/LocalMinute.hpp>
#include <dot/noda_time/LocalDate.hpp>
#include <dot/noda_time/LocalDateTime.hpp>

namespace dc
{
    DataWriterImpl::DataWriterImpl(Data data)
        : currentDict_(data)
        , currentState_(TreeWriterState::Empty) {}

    void DataWriterImpl::WriteStartDocument(dot::String rootElementName)
    {
        if (currentState_ == TreeWriterState::Empty && elementStack_.size() == 0)
        {
            currentState_ = TreeWriterState::DocumentStarted;
        }
        else
        {
            throw dot::new_Exception("A call to WriteStartDocument(...) must be the first call to the tree writer.");
        }

        dot::String rootName = currentDict_->GetType()->Name;
        // Check that the name matches
        if (rootElementName != rootName) throw dot::new_Exception(
            dot::String::Format("Attempting to deserialize data for type {0} into type {1}.", rootElementName, rootName));

        rootElementName_ = rootElementName;
        currentElementName_ = rootElementName;
        auto currentDictInfoList = currentDict_->GetType()->GetProperties();
        currentDictElements_ = dot::new_Dictionary<dot::String, dot::PropertyInfo>();
        for(auto elementInfo : currentDictInfoList) currentDictElements_->Add(elementInfo->Name, elementInfo);
        currentArray_ = nullptr;
        currentArrayItemType_ = nullptr;
    }

    void DataWriterImpl::WriteEndDocument(dot::String rootElementName)
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::DocumentStarted && elementStack_.size() == 0) currentState_ = TreeWriterState::DocumentCompleted;
        else throw dot::new_Exception(
            "A call to WriteEndDocument(...) does not follow  WriteEndElement(...) at at root level.");

        // Check that the current element name matches the specified name. Writing the actual end tag
        // occurs inside one of WriteStartDict, WriteStartArrayItem, or WriteStartValue calls.
        if (rootElementName != rootElementName_)
            throw dot::new_Exception(dot::String::Format(
                "WriteEndDocument({0}) follows WriteStartDocument({1}), root element name mismatch.", rootElementName, rootElementName_));
    }

    void DataWriterImpl::WriteStartElement(dot::String elementName)
    {
        if (currentState_ == TreeWriterState::DocumentStarted) currentState_ = TreeWriterState::ElementStarted;
        else if (currentState_ == TreeWriterState::ElementCompleted) currentState_ = TreeWriterState::ElementStarted;
        else if (currentState_ == TreeWriterState::DictStarted) currentState_ = TreeWriterState::ElementStarted;
        else if (currentState_ == TreeWriterState::DictArrayItemStarted) currentState_ = TreeWriterState::ElementStarted;
        else throw dot::new_Exception(
            "A call to WriteStartElement(...) must be the first call or follow WriteEndElement(prevName).");

        currentElementName_ = elementName;
        currentElementInfo_ = currentDictElements_[elementName];
    }

    void DataWriterImpl::WriteEndElement(dot::String elementName)
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ElementStarted) currentState_ = TreeWriterState::ElementCompleted;
        else if (currentState_ == TreeWriterState::DictCompleted) currentState_ = TreeWriterState::ElementCompleted;
        else if (currentState_ == TreeWriterState::ValueCompleted) currentState_ = TreeWriterState::ElementCompleted;
        else if (currentState_ == TreeWriterState::ArrayCompleted) currentState_ = TreeWriterState::ElementCompleted;
        else throw dot::new_Exception(
            "A call to WriteEndElement(...) does not follow a matching WriteStartElement(...) at the same indent level.");

        // Check that the current element name matches the specified name. Writing the actual end tag
        // occurs inside one of WriteStartDict, WriteStartArrayItem, or WriteStartValue calls.
        if (elementName != currentElementName_)
            throw dot::new_Exception(dot::String::Format(
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
        else throw dot::new_Exception(
            "A call to WriteStartDict() must follow WriteStartElement(...) or WriteStartArrayItem().");

        // Set dictionary info
        dot::Type createdDictType = nullptr;
        if (currentArray_ != nullptr) createdDictType = currentArrayItemType_;
        else if (currentDict_ != nullptr) createdDictType = currentElementInfo_->PropertyType;
        else throw dot::new_Exception("Value can only be added to a dictionary or array.");

        dot::Object createdDictObj = dot::Activator::CreateInstance(createdDictType);
        if (!(createdDictObj.is<Data>())) // TODO Also support native dictionaries
        {
            dot::String mappedClassName = currentElementInfo_->PropertyType->FullName;
            throw dot::new_Exception(dot::String::Format(
                "Element {0} of type {1} does not implement Data.", currentElementInfo_->Name, mappedClassName));
        }

        auto createdDict = (Data) createdDictObj;

        // Add to array or dictionary, depending on what we are inside of
        if (currentArray_ != nullptr) currentArray_->ObjectAdd(createdDict);
        else if (currentDict_ != nullptr) currentElementInfo_->SetValue(currentDict_, createdDict);
        else throw dot::new_Exception("Value can only be added to a dictionary or array.");

        currentDict_ = (Data) createdDict;
        auto currentDictInfoList = createdDictType->GetProperties();
        currentDictElements_ = dot::new_Dictionary<dot::String, dot::PropertyInfo>();
        for (auto elementInfo : currentDictInfoList) currentDictElements_->Add(elementInfo->Name, elementInfo);
        currentArray_ = nullptr;
        currentArrayItemType_ = nullptr;
    }

    void DataWriterImpl::WriteEndDict()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::DictStarted) currentState_ = TreeWriterState::DictCompleted;
        else if (currentState_ == TreeWriterState::DictArrayItemStarted) currentState_ = TreeWriterState::DictArrayItemCompleted;
        else if (currentState_ == TreeWriterState::ElementCompleted) currentState_ = TreeWriterState::DictCompleted;
        else throw dot::new_Exception(
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
            throw dot::new_Exception(
                "A call to WriteStartArray() must follow WriteStartElement(...).");

        // Create the array
        dot::Object createdArrayObj = dot::Activator::CreateInstance(currentElementInfo_->PropertyType);
        if (createdArrayObj.is<dot::IObjectCollection>()) // TODO Also support native arrays
        {
            dot::IObjectCollection createdArray = (dot::IObjectCollection) createdArrayObj;

            // Add to array or dictionary, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->ObjectAdd(createdArray);
            else if (currentDict_ != nullptr) currentElementInfo_->SetValue(currentDict_, createdArray);
            else throw dot::new_Exception("Value can only be added to a dictionary or array.");

            currentArray_ = createdArray;

            // Get array item type from array type using reflection
            dot::Type listType = currentElementInfo_->PropertyType;      // TODO fix
//            if (!listType->IsGenericType) throw dot::new_Exception(dot::String::Format(
//                "Type {0} cannot be serialized because it implements only IList but not IList<T>.", listType));
//            Array1D<Type> genericParameterTypes = listType->GenericTypeArguments;
            dot::Array1D<dot::Type> genericParameterTypes = listType->GetGenericArguments();
            if (genericParameterTypes->Count != 1) throw dot::new_Exception(
                dot::String::Format("Generic parameter type list {0} has more than ", genericParameterTypes) +
                "one element creating an ambiguity for deserialization code.");
            currentArrayItemType_ = genericParameterTypes[0];

            currentDict_ = nullptr;
            currentElementInfo_ = nullptr;
            currentDictElements_ = nullptr;
        }
        else {
            dot::String mappedClassName = currentElementInfo_->PropertyType->FullName;
            throw dot::new_Exception(dot::String::Format(
                "Element {0} of type {1} does not implement ICollection.", currentElementInfo_->Name, mappedClassName));
        }
    }

    void DataWriterImpl::WriteEndArray()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ArrayItemCompleted) currentState_ = TreeWriterState::ArrayCompleted;
        else throw dot::new_Exception(
            "A call to WriteEndArray(...) does not follow WriteEndArrayItem(...).");

        // Pop state
        PopState();
    }

    void DataWriterImpl::WriteStartArrayItem()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ArrayStarted) currentState_ = TreeWriterState::ArrayItemStarted;
        else if (currentState_ == TreeWriterState::ArrayItemCompleted) currentState_ = TreeWriterState::ArrayItemStarted;
        else throw dot::new_Exception(
            "A call to WriteStartArrayItem() must follow WriteStartElement(...) or WriteEndArrayItem().");

        //Object addedItem = nullptr;
        //if (currentArrayItemType_ == dot::typeof<String>()) addedItem = nullptr;
        //else if (currentArrayItemType_ == dot::typeof<double>()) addedItem = double();
        //else if (currentArrayItemType_ == dot::typeof<Nullable<double>>()) addedItem = nullptr;
        //else if (currentArrayItemType_ == dot::typeof<bool>()) addedItem = bool();
        //else if (currentArrayItemType_ == dot::typeof<Nullable<bool>>()) addedItem = nullptr;
        //else if (currentArrayItemType_ == dot::typeof<int>()) addedItem = int();
        //else if (currentArrayItemType_ == dot::typeof<Nullable<int>>()) addedItem = nullptr;
        //else if (currentArrayItemType_ == dot::typeof<int64_t>()) addedItem = int64_t();
        //else if (currentArrayItemType_ == dot::typeof<Nullable<int64_t>>()) addedItem = nullptr;
        //else if (currentArrayItemType_ == dot::typeof<LocalDate>()) addedItem = LocalDate();
        //else if (currentArrayItemType_ == dot::typeof<Nullable<LocalDate>>()) addedItem = nullptr;
        //else if (currentArrayItemType_ == dot::typeof<LocalTime>()) addedItem = LocalTime();
        //else if (currentArrayItemType_ == dot::typeof<Nullable<LocalTime>>()) addedItem = nullptr;
        //else if (currentArrayItemType_ == dot::typeof<LocalDateTime>()) addedItem = LocalDateTime();
        //else if (currentArrayItemType_ == dot::typeof<Nullable<LocalDateTime>>()) addedItem = nullptr;
        //else if (currentArrayItemType_->IsClass) addedItem = nullptr;
        //else throw dot::new_Exception(dot::String::Format("Value type {0} is not supported for serialization.", currentArrayItemType_->Name));

        //currentArray_->ObjectAdd(addedItem);
        //currentArrayItem_ = addedItem;
    }

    void DataWriterImpl::WriteEndArrayItem()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ArrayItemStarted) currentState_ = TreeWriterState::ArrayItemCompleted;
        else if (currentState_ == TreeWriterState::DictArrayItemCompleted) currentState_ = TreeWriterState::ArrayItemCompleted;
        else if (currentState_ == TreeWriterState::ValueArrayItemCompleted) currentState_ = TreeWriterState::ArrayItemCompleted;
        else throw dot::new_Exception(
            "A call to WriteEndArrayItem(...) does not follow a matching WriteStartArrayItem(...) at the same indent level.");

        // Do nothing here
    }

    void DataWriterImpl::WriteStartValue()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ElementStarted) currentState_ = TreeWriterState::ValueStarted;
        else if (currentState_ == TreeWriterState::ArrayItemStarted) currentState_ = TreeWriterState::ValueArrayItemStarted;
        else throw dot::new_Exception(
            "A call to WriteStartValue() must follow WriteStartElement(...) or WriteStartArrayItem().");
    }

    void DataWriterImpl::WriteEndValue()
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ValueWritten) currentState_ = TreeWriterState::ValueCompleted;
        else if (currentState_ == TreeWriterState::ValueArrayItemWritten) currentState_ = TreeWriterState::ValueArrayItemCompleted;
        else throw dot::new_Exception(
            "A call to WriteEndValue(...) does not follow a matching WriteValue(...) at the same indent level.");

        // Nothing to write here
    }

    void DataWriterImpl::WriteValue(dot::Object value)
    {
        // Check state transition matrix
        if (currentState_ == TreeWriterState::ValueStarted) currentState_ = TreeWriterState::ValueWritten;
        else if (currentState_ == TreeWriterState::ValueArrayItemStarted) currentState_ = TreeWriterState::ValueArrayItemWritten;
        else throw dot::new_Exception(
            "A call to WriteEndValue(...) does not follow a matching WriteValue(...) at the same indent level.");

        // Check that we are either inside dictionary or array
        dot::Type elementType = nullptr;
        if (currentArray_ != nullptr) elementType = currentArrayItemType_;
        else if (currentDict_ != nullptr) elementType = currentElementInfo_->PropertyType;
        else throw dot::new_Exception(
            dot::String::Format("Cannot WriteValue(...)for element {0} ", currentElementName_) +
            "is called outside dictionary or array.");

        if (value.IsEmpty())  // TODO IsEmpty method should be implemented according to c# extension
        {
            // Do not record null or empty value into dictionary, but add it to an array
            // Add to dictionary or array, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->ObjectAdd(nullptr);
            return;
        }

        // Write based on element type
        dot::Type valueType = value->GetType();
        if (elementType->Equals(dot::typeof<dot::String>()) ||
            elementType->Equals(dot::typeof<double>()) || elementType->Equals(dot::typeof<dot::Nullable<double>>()) ||
            elementType->Equals(dot::typeof<bool>()) || elementType->Equals(dot::typeof<dot::Nullable<bool>>()) ||
            elementType->Equals(dot::typeof<int>()) || elementType->Equals(dot::typeof<dot::Nullable<int>>()) ||
            elementType->Equals(dot::typeof<int64_t>()) || elementType->Equals(dot::typeof<dot::Nullable<int64_t>>()) ||
            elementType->Equals(dot::typeof<ObjectId>())
            )
        {
            // Check type match
            //if (!elementType->Equals(valueType)) // TODO change to !elementType->IsAssignableFrom(valueType)
            //    throw dot::new_Exception(
            //        dot::String::Format("Attempting to deserialize value of type {0} ", valueType->Name) +
            //        dot::String::Format("into element of type {0}.", elementType->Name));

            dot::Object convertedValue = value;
            if (elementType->Equals(dot::typeof<double>()))
            {
                if (valueType->Equals(dot::typeof<int>())) convertedValue = static_cast<double>((int) value);
                if (valueType->Equals(dot::typeof<int64_t>())) convertedValue = static_cast<double>((int64_t) value);
            }
            else if (elementType->Equals(dot::typeof<int64_t>()) && valueType->Equals(dot::typeof<int>()))
            {
                convertedValue = static_cast<int64_t>((int) value);
            }
            else if (elementType->Equals(dot::typeof<int>()) && valueType->Equals(dot::typeof<int64_t>()))
            {
                convertedValue = static_cast<int>((int64_t) value);
            }
            else if (elementType->Equals(dot::typeof<ObjectId>()) && valueType->Equals(dot::typeof<dot::String>()))
            {
                convertedValue = ObjectId((dot::String) value);
            }

            // Add to array or dictionary, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->ObjectAdd(convertedValue);
            else if (currentDict_ != nullptr) currentElementInfo_->SetValue(currentDict_, convertedValue);
            else throw dot::new_Exception("Value can only be added to a dictionary or array.");
        }
        else if (elementType->Equals(dot::typeof<dot::LocalDate>()) || elementType->Equals(dot::typeof<dot::Nullable<dot::LocalDate>>()))
        {
            dot::LocalDate dateValue;

            // Check type match
            if (valueType->Equals(dot::typeof<int>()))
            {
                // Deserialize LocalDate as ISO int in yyyymmdd format
                dateValue = LocalDateHelper::ParseIsoInt((int) value);
            }
            else if (valueType->Equals(dot::typeof<int64_t>()))
            {
                // Deserialize LocalDate as ISO int in yyyymmdd format
                dateValue = LocalDateHelper::ParseIsoInt((int64_t) value);
            }
            else throw dot::new_Exception(
                    dot::String::Format("Attempting to deserialize value of type {0} ", valueType->Name) +
                    "into LocalDate; type should be int32.");

            // Add to array or dictionary, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->ObjectAdd(dateValue);
            else if (currentDict_ != nullptr) currentElementInfo_->SetValue(currentDict_, dateValue);
            else throw dot::new_Exception("Value can only be added to a dictionary or array.");
        }
        else if (elementType->Equals(dot::typeof<dot::LocalTime>()) || elementType->Equals(dot::typeof<dot::Nullable<dot::LocalTime>>()))
        {
            dot::LocalTime timeValue;

            // Check type match
            if (valueType->Equals(dot::typeof<int>()))
            {
                // Deserialize LocalTime as ISO int in hhmmssfff format
                timeValue = LocalTimeHelper::ParseIsoInt((int) value);
            }
            else if (valueType->Equals(dot::typeof<int64_t>()))
            {
                // Deserialize LocalTime as ISO int in hhmmssfff format
                timeValue = LocalTimeHelper::ParseIsoInt((int64_t) value);
            }
            else throw dot::new_Exception(
                    dot::String::Format("Attempting to deserialize value of type {0} ", valueType->Name) +
                    "into LocalTime; type should be int32.");

            // Add to array or dictionary, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->ObjectAdd(timeValue);
            else if (currentDict_ != nullptr) currentElementInfo_->SetValue(currentDict_, timeValue);
            else throw dot::new_Exception("Value can only be added to a dictionary or array.");
        }
        else if (elementType->Equals(dot::typeof<dot::LocalMinute>()) || elementType->Equals(dot::typeof<dot::Nullable<dot::LocalMinute>>()))
        {
            dot::LocalMinute minuteValue;

            // Check type match
            if (valueType->Equals(dot::typeof<int>()))
            {
                // Deserialize LocalMinute as ISO int in hhmmssfff format
                minuteValue = LocalMinuteHelper::ParseIsoInt((int) value);
            }
            else if (valueType->Equals(dot::typeof<int64_t>()))
            {
                // Deserialize LocalMinute as ISO int in hhmmssfff format
                minuteValue = LocalMinuteHelper::ParseIsoInt((int64_t) value);
            }
            else throw dot::new_Exception(
                dot::String::Format("Attempting to deserialize value of type {0} ", valueType->Name) +
                "into LocalMinute; type should be int32.");

            // Add to array or dictionary, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->ObjectAdd(minuteValue);
            else if (currentDict_ != nullptr) currentElementInfo_->SetValue(currentDict_, minuteValue);
            else throw dot::new_Exception("Value can only be added to a dictionary or array.");
        }
        else if (elementType->Equals(dot::typeof<dot::LocalDateTime>()) || elementType->Equals(dot::typeof<dot::Nullable<dot::LocalDateTime>>()))
        {
        dot::LocalDateTime dateTimeValue;

            // Check type match
            if (valueType->Equals(dot::typeof<dot::LocalDateTime>()))
            {
                dateTimeValue = (dot::LocalDateTime) value;
            }
            else if (valueType->Equals(dot::typeof<int64_t>()))
            {
                // Deserialize LocalDateTime as ISO long in yyyymmddhhmmssfff format
                dateTimeValue = LocalDateTimeHelper::ParseIsoLong((int64_t) value);
            }
            else if (valueType->Equals(dot::typeof<int>()))
            {
                // Deserialize LocalDateTime as ISO long in yyyymmddhhmmssfff format
                dateTimeValue = LocalDateTimeHelper::ParseIsoLong((int) value);
            }
            else if (valueType->Equals(dot::typeof<dot::String>()))
            {
                // Deserialize LocalDateTime as ISO string
                dateTimeValue = LocalDateTimeHelper::Parse((dot::String) value);
            }
            else throw dot::new_Exception(
                    dot::String::Format("Attempting to deserialize value of type {0} ", valueType->Name) +
                    "into LocalDateTime; type should be LocalDateTime.");

            // Add to array or dictionary, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->ObjectAdd(dateTimeValue);
            else if (currentDict_ != nullptr) currentElementInfo_->SetValue(currentDict_, dateTimeValue);
            else throw dot::new_Exception("Value can only be added to a dictionary or array.");
        }
        else if (elementType->IsEnum)
        {
            // Check type match
            if (!valueType->Equals(dot::typeof<dot::String>()))
                throw dot::new_Exception(
                    dot::String::Format("Attempting to deserialize value of type {0} ", valueType->Name) +
                    dot::String::Format("into enum {0}; type should be string.", elementType->Name));

            // Deserialize enum as string
            dot::String enumString = (dot::String) value;
            dot::Object enumValue = dot::Enum::Parse(elementType, enumString);

            // Add to array or dictionary, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->ObjectAdd(enumValue);
            else if (currentDict_ != nullptr) currentElementInfo_->SetValue(currentDict_, enumValue);
            else throw dot::new_Exception("Value can only be added to a dictionary or array.");
        }
        else
        {
            // We run out of value types at this point, now we can create
            // a reference type and check that it implements KeyType
            dot::Object keyObj = (KeyType)dot::Activator::CreateInstance(elementType);
            if (keyObj.is<KeyType>())
            {
                KeyType key = (KeyType) keyObj;

                // Check type match
                if (!valueType->Equals(dot::typeof<dot::String>()) && !valueType->Equals(elementType))
                    throw dot::new_Exception(
                        dot::String::Format("Attempting to deserialize value of type {0} ", valueType->Name) +
                        dot::String::Format("into key type {0}; keys should be serialized into semicolon delimited string.", elementType->Name));

                // Populate from semicolon delimited string
                dot::String stringValue = value->ToString();
                key->AssignString(stringValue);

                // Add to array or dictionary, depending on what we are inside of
                if (currentArray_ != nullptr) currentArray_->ObjectAdd(key);
                else if (currentDict_ != nullptr) currentElementInfo_->SetValue(currentDict_, key);
                else throw dot::new_Exception("Value can only be added to a dictionary or array.");
            }
            else
            {
                // Argument type is unsupported, error message
                throw dot::new_Exception(dot::String::Format("Element type {0} is not supported for serialization.", value->GetType()));
            }
        }
    }

    dot::String DataWriterImpl::ToString()
    {
        if (currentArray_ != nullptr) return currentArray_->ToString();
        else if (currentDict_ != nullptr) return currentDict_->ToString();
        else return GetType()->Name;
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
