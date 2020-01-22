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
#include <dc/serialization/data_writer.hpp>
#include <dot/noda_time/local_time_util.hpp>
#include <dot/noda_time/local_minute_util.hpp>
#include <dot/noda_time/local_date_util.hpp>
#include <dot/noda_time/local_date_time_util.hpp>
#include <dc/types/record/key_base.hpp>
#include <dot/system/enum.hpp>
#include <dot/system/reflection/activator.hpp>
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_minute.hpp>
#include <dot/noda_time/local_date.hpp>
#include <dot/noda_time/local_date_time.hpp>

namespace dc
{
    data_writer_impl::data_writer_impl(data data_obj)
        : currentDict_(data_obj)
        , currentState_(tree_writer_state::empty) {}

    void data_writer_impl::write_start_document(dot::string rootElementName)
    {
        if (currentState_ == tree_writer_state::empty && elementStack_.size() == 0)
        {
            currentState_ = tree_writer_state::DocumentStarted;
        }
        else
        {
            throw dot::exception("A call to write_start_document(...) must be the first call to the tree writer.");
        }

        dot::string rootName = currentDict_->get_type()->name;
        // Check that the name matches
        if (rootElementName != rootName) throw dot::exception(
            dot::string::format("Attempting to deserialize data for type {0} into type {1}.", rootElementName, rootName));

        rootElementName_ = rootElementName;
        currentElementName_ = rootElementName;
        auto currentDictInfoList = currentDict_->get_type()->get_fields();
        currentDictElements_ = dot::make_dictionary<dot::string, dot::field_info>();
        for(auto elementInfo : currentDictInfoList) currentDictElements_->add(elementInfo->name, elementInfo);
        currentArray_ = nullptr;
        currentArrayItemType_ = nullptr;
    }

    void data_writer_impl::write_end_document(dot::string rootElementName)
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::DocumentStarted && elementStack_.size() == 0) currentState_ = tree_writer_state::DocumentCompleted;
        else throw dot::exception(
            "A call to write_end_document(...) does not follow  write_end_element(...) at at root level.");

        // Check that the current element name matches the specified name. Writing the actual end tag
        // occurs inside one of write_start_dict, write_start_array_item, or write_start_value calls.
        if (rootElementName != rootElementName_)
            throw dot::exception(dot::string::format(
                "write_end_document({0}) follows write_start_document({1}), root element name mismatch.", rootElementName, rootElementName_));
    }

    void data_writer_impl::write_start_element(dot::string elementName)
    {
        if (currentState_ == tree_writer_state::DocumentStarted) currentState_ = tree_writer_state::ElementStarted;
        else if (currentState_ == tree_writer_state::ElementCompleted) currentState_ = tree_writer_state::ElementStarted;
        else if (currentState_ == tree_writer_state::DictStarted) currentState_ = tree_writer_state::ElementStarted;
        else if (currentState_ == tree_writer_state::DictArrayItemStarted) currentState_ = tree_writer_state::ElementStarted;
        else throw dot::exception(
            "A call to write_start_element(...) must be the first call or follow write_end_element(prevName).");

        currentElementName_ = elementName;
        currentElementInfo_ = currentDictElements_[elementName];
    }

    void data_writer_impl::write_end_element(dot::string elementName)
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::ElementStarted) currentState_ = tree_writer_state::ElementCompleted;
        else if (currentState_ == tree_writer_state::DictCompleted) currentState_ = tree_writer_state::ElementCompleted;
        else if (currentState_ == tree_writer_state::ValueCompleted) currentState_ = tree_writer_state::ElementCompleted;
        else if (currentState_ == tree_writer_state::ArrayCompleted) currentState_ = tree_writer_state::ElementCompleted;
        else throw dot::exception(
            "A call to write_end_element(...) does not follow a matching write_start_element(...) at the same indent level.");

        // Check that the current element name matches the specified name. Writing the actual end tag
        // occurs inside one of write_start_dict, write_start_array_item, or write_start_value calls.
        if (elementName != currentElementName_)
            throw dot::exception(dot::string::format(
                "EndComplexElement({0}) follows StartComplexElement({1}), element name mismatch.", elementName, currentElementName_));
    }

    void data_writer_impl::write_start_dict()
    {
        // Push state before defining dictionary state
        PushState();

        // Check state transition matrix
        if (currentState_ == tree_writer_state::DocumentStarted)
        {
            currentState_ = tree_writer_state::DictStarted;

            // Return if this call follows StartDocument, all setup is done in StartDocument
            return;
        }
        else if (currentState_ == tree_writer_state::ElementStarted) currentState_ = tree_writer_state::DictStarted;
        else if (currentState_ == tree_writer_state::ArrayItemStarted) currentState_ = tree_writer_state::DictArrayItemStarted;
        else throw dot::exception(
            "A call to write_start_dict() must follow write_start_element(...) or write_start_array_item().");

        // Set dictionary info
        dot::type createdDictType = nullptr;
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

    void data_writer_impl::write_end_dict()
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::DictStarted) currentState_ = tree_writer_state::DictCompleted;
        else if (currentState_ == tree_writer_state::DictArrayItemStarted) currentState_ = tree_writer_state::DictArrayItemCompleted;
        else if (currentState_ == tree_writer_state::ElementCompleted) currentState_ = tree_writer_state::DictCompleted;
        else throw dot::exception(
            "A call to write_end_dict(...) does not follow a matching write_start_dict(...) at the same indent level.");

        // Restore previous state
        PopState();
    }

    void data_writer_impl::write_start_array()
    {
        // Push state
        PushState();

        // Check state transition matrix
        if (currentState_ == tree_writer_state::ElementStarted) currentState_ = tree_writer_state::ArrayStarted;
        else
            throw dot::exception(
                "A call to write_start_array() must follow write_start_element(...).");

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
            dot::type listType = currentElementInfo_->field_type;      // TODO fix
//            if (!listType->IsGenericType) throw dot::exception(dot::string::format(
//                "Type {0} cannot be serialized because it implements only IList but not IList<T>.", listType));
//            list<Type> genericParameterTypes = listType->GenericTypeArguments;
            dot::list<dot::type> genericParameterTypes = listType->get_generic_arguments();
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

    void data_writer_impl::write_end_array()
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::ArrayItemCompleted) currentState_ = tree_writer_state::ArrayCompleted;
        else throw dot::exception(
            "A call to write_end_array(...) does not follow write_end_array_item(...).");

        // Pop state
        PopState();
    }

    void data_writer_impl::write_start_array_item()
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::ArrayStarted) currentState_ = tree_writer_state::ArrayItemStarted;
        else if (currentState_ == tree_writer_state::ArrayItemCompleted) currentState_ = tree_writer_state::ArrayItemStarted;
        else throw dot::exception(
            "A call to write_start_array_item() must follow write_start_element(...) or write_end_array_item().");

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

    void data_writer_impl::write_end_array_item()
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::ArrayItemStarted) currentState_ = tree_writer_state::ArrayItemCompleted;
        else if (currentState_ == tree_writer_state::DictArrayItemCompleted) currentState_ = tree_writer_state::ArrayItemCompleted;
        else if (currentState_ == tree_writer_state::ValueArrayItemCompleted) currentState_ = tree_writer_state::ArrayItemCompleted;
        else throw dot::exception(
            "A call to write_end_array_item(...) does not follow a matching write_start_array_item(...) at the same indent level.");

        // Do nothing here
    }

    void data_writer_impl::write_start_value()
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::ElementStarted) currentState_ = tree_writer_state::ValueStarted;
        else if (currentState_ == tree_writer_state::ArrayItemStarted) currentState_ = tree_writer_state::ValueArrayItemStarted;
        else throw dot::exception(
            "A call to write_start_value() must follow write_start_element(...) or write_start_array_item().");
    }

    void data_writer_impl::write_end_value()
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::ValueWritten) currentState_ = tree_writer_state::ValueCompleted;
        else if (currentState_ == tree_writer_state::ValueArrayItemWritten) currentState_ = tree_writer_state::ValueArrayItemCompleted;
        else throw dot::exception(
            "A call to write_end_value(...) does not follow a matching write_value(...) at the same indent level.");

        // Nothing to write here
    }

    void data_writer_impl::write_value(dot::object value)
    {
        // Check state transition matrix
        if (currentState_ == tree_writer_state::ValueStarted) currentState_ = tree_writer_state::ValueWritten;
        else if (currentState_ == tree_writer_state::ValueArrayItemStarted) currentState_ = tree_writer_state::ValueArrayItemWritten;
        else throw dot::exception(
            "A call to write_end_value(...) does not follow a matching write_value(...) at the same indent level.");

        // Check that we are either inside dictionary or array
        dot::type elementType = nullptr;
        if (currentArray_ != nullptr) elementType = currentArrayItemType_;
        else if (currentDict_ != nullptr) elementType = currentElementInfo_->field_type;
        else throw dot::exception(
            dot::string::format("Cannot write_value(...)for element {0} ", currentElementName_) +
            "is called outside dictionary or array.");

        if (value.is_empty())  // TODO is_empty method should be implemented according to c# extension
        {
            // Do not record null or empty value into dictionary, but add it to an array
            // Add to dictionary or array, depending on what we are inside of
            if (currentArray_ != nullptr) currentArray_->add_object(nullptr);
            return;
        }

        // Write based on element type
        dot::type valueType = value->get_type();
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
            dot::object enumValue = elementType->get_method("parse")->invoke(nullptr, dot::make_list<dot::object>({ value }));


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
                throw dot::exception(dot::string::format("Element type {0} is not supported for serialization.", value->get_type()));
            }
        }
    }

    dot::string data_writer_impl::to_string()
    {
        if (currentArray_ != nullptr) return currentArray_->to_string();
        else if (currentDict_ != nullptr) return currentDict_->to_string();
        else return get_type()->name;
    }

    void data_writer_impl::PushState()
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

    void data_writer_impl::PopState()
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
