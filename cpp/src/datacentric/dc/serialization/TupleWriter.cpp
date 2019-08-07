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
#include <dc/serialization/TupleWriter.hpp>
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
#include <dc/serialization/DataWriter.hpp>

namespace dc
{
    void TupleWriterImpl::WriteStartDocument(dot::string rootElementName)
    {

    }

    void TupleWriterImpl::WriteEndDocument(dot::string rootElementName)
    {

    }

    void TupleWriterImpl::WriteStartElement(dot::string elementName)
    {
        if (dataWriter_ != nullptr)
        {
            dataWriter_->WriteStartElement(elementName);
        }
        else
        {
            for (int i = 0; i < props_->count(); ++i)
            {
                if (elementName == "_key" || elementName == "_id")
                {
                    indexOfCurrent_ = -1;
                    return;
                }

                if (props_[i]->name == elementName)
                {
                    indexOfCurrent_ = i;
                    if (props_[i]->field_type->name->ends_with("Data")) //! TODO change ends_with
                    {
                        data result = (data)dot::activator::create_instance(props_[i]->field_type);
                        dataWriter_ = make_DataWriter(result);
                        dataWriter_->WriteStartDocument(props_[i]->field_type->name);

                        tuple_->type()->get_method("SetItem")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, indexOfCurrent_, result }));

                        //dataWriter_->WriteStartElement(elementName);
                        //DeserializeDocument(doc, writer);
                        //writer->WriteEndDocument(typeName);
                    }

                    if (! props_[i]->field_type->get_interface("ListBase").is_empty()) // TODO - refactor after removing the interface
                    {
                        dataWriter_ = make_DataWriter(nullptr);
                        dataWriter_->currentElementInfo_ = props_[i];
                        dataWriter_->currentElementName_ = props_[i]->name;
                        dataWriter_->currentState_ = TreeWriterState::ElementStarted;

                        dataWriter_->currentArray_ = dot::make_list<dot::list<dot::object>>();

                        //dataWriter_->WriteStartElement(elementName);
                        //DeserializeDocument(doc, writer);
                        //writer->WriteEndDocument(typeName);
                    }

                    return;
                }
            }
            throw dot::exception(dot::string::format("Unknown element {0} in tuple writer.", elementName));
        }
    }

    void TupleWriterImpl::WriteEndElement(dot::string elementName)
    {
        if (dataWriter_ != nullptr)
        {
            dataWriter_->WriteEndElement(elementName);
        }
    }

    void TupleWriterImpl::WriteStartDict()
    {
        if (dataWriter_ != nullptr)
        {
            dataWriter_->WriteStartDict();
        }

    }

    void TupleWriterImpl::WriteEndDict()
    {
        if (dataWriter_ != nullptr)
        {
            dataWriter_->WriteEndDict();
            if (dataWriter_->currentState_ == TreeWriterState::DocumentStarted)
                dataWriter_ = nullptr;
        }
    }

    void TupleWriterImpl::WriteStartArray()
    {
        if (dataWriter_ != nullptr)
        {
            dataWriter_->WriteStartArray();
        }

    }

    void TupleWriterImpl::WriteEndArray()
    {
        if (dataWriter_ != nullptr)
        {
            tuple_->type()->get_method("SetItem")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, indexOfCurrent_, dataWriter_->currentArray_ }));
            dataWriter_->WriteEndArray();
            dataWriter_ = nullptr;
        }

    }

    void TupleWriterImpl::WriteStartArrayItem()
    {
        if (dataWriter_ != nullptr)
        {
            dataWriter_->WriteStartArrayItem();
        }

    }

    void TupleWriterImpl::WriteEndArrayItem()
    {
        if (dataWriter_ != nullptr)
        {
            dataWriter_->WriteEndArrayItem();
        }

    }

    void TupleWriterImpl::WriteStartValue()
    {
        if (dataWriter_ != nullptr)
        {
            dataWriter_->WriteStartValue();
        }

    }

    void TupleWriterImpl::WriteEndValue()
    {
        if (dataWriter_ != nullptr)
        {
            dataWriter_->WriteEndValue();
        }

    }

    void TupleWriterImpl::WriteValue(dot::object value)
    {
        if (dataWriter_ != nullptr)
        {
            dataWriter_->WriteValue(value);
            return;
        }

        if (indexOfCurrent_ == -1)
            return;

        // Check that we are either inside dictionary or array
        dot::type_t elementType = tuple_->type()->get_generic_arguments()[indexOfCurrent_]; // TODO - cache array instead of getting every time?

        if (value.is_empty())  // TODO is_empty method should be implemented according to c# extension
        {
            // Do not record null or empty value into dictionary, but add it to an array
            // Add to dictionary or array, depending on what we are inside of
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
            tuple_->type()->get_method("SetItem")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, indexOfCurrent_, convertedValue }));
        }
        else if (elementType->equals(dot::typeof<dot::local_date>()) || elementType->equals(dot::typeof<dot::nullable<dot::local_date>>()))
        {
            dot::local_date dateValue;

            // Check type match
            if (valueType->equals(dot::typeof<int>()))
            {
                // Deserialize local_date as ISO int in yyyymmdd format
                dateValue = dot::local_date_util::parse_iso_int((int)value);
            }
            else if (valueType->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_date as ISO int in yyyymmdd format
                dateValue = dot::local_date_util::parse_iso_int((int)value);
            }
            else throw dot::exception(
                    dot::string::format("Attempting to deserialize value of type {0} ", valueType->name) +
                    "into local_date; type should be int32.");

            tuple_->type()->get_method("SetItem")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, indexOfCurrent_, dateValue }));
        }
        else if (elementType->equals(dot::typeof<dot::local_time>()) || elementType->equals(dot::typeof<dot::nullable<dot::local_time>>()))
        {
            dot::local_time timeValue;

            // Check type match
            if (valueType->equals(dot::typeof<int>()))
            {
                // Deserialize local_time as ISO int in hhmmssfff format
                timeValue = dot::local_time_util::parse_iso_int((int)value);
            }
            else if (valueType->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_time as ISO int in hhmmssfff format
                timeValue = dot::local_time_util::parse_iso_int((int)value);
            }
            else throw dot::exception(
                    dot::string::format("Attempting to deserialize value of type {0} ", valueType->name) +
                    "into local_time; type should be int32.");

            tuple_->type()->get_method("SetItem")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, indexOfCurrent_, timeValue }));
        }
        else if (elementType->equals(dot::typeof<dot::local_minute>()) || elementType->equals(dot::typeof<dot::nullable<dot::local_minute>>()))
        {
            dot::local_minute minuteValue;

            // Check type match
            if (valueType->equals(dot::typeof<int>()))
            {
                // Deserialize local_minute as ISO int in hhmmssfff format
                minuteValue = dot::local_minute_util::parse_iso_int((int)value);
            }
            else if (valueType->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_minute as ISO int in hhmmssfff format
                minuteValue = dot::local_minute_util::parse_iso_int((int)value);
            }
            else throw dot::exception(
                dot::string::format("Attempting to deserialize value of type {0} ", valueType->name) +
                "into local_minute; type should be int32.");

            tuple_->type()->get_method("SetItem")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, indexOfCurrent_, minuteValue }));
        }
        else if (elementType->equals(dot::typeof<dot::local_date_time>()) || elementType->equals(dot::typeof<dot::nullable<dot::local_date_time>>()))
        {
        dot::local_date_time dateTimeValue;

            // Check type match
            if (valueType->equals(dot::typeof<dot::local_date_time>()))
            {
                dateTimeValue = (dot::local_date_time)value;
            }
            else if (valueType->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_date_time as ISO long in yyyymmddhhmmssfff format
                dateTimeValue = dot::local_date_time_util::parse_iso_long((int64_t)value);
            }
            else if (valueType->equals(dot::typeof<int>()))
            {
                // Deserialize local_date_time as ISO long in yyyymmddhhmmssfff format
                dateTimeValue = dot::local_date_time_util::parse_iso_long((int)value);
            }
            else if (valueType->equals(dot::typeof<dot::string>()))
            {
                // Deserialize local_date_time as ISO string
                dateTimeValue = dot::local_date_time_util::parse((dot::string)value);
            }
            else throw dot::exception(
                    dot::string::format("Attempting to deserialize value of type {0} ", valueType->name) +
                    "into local_date_time; type should be local_date_time.");

            tuple_->type()->get_method("SetItem")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, indexOfCurrent_, dateTimeValue }));
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

            tuple_->type()->get_method("SetItem")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, indexOfCurrent_, enumValue }));
        }
        else
        {
            // We run out of value types at this point, now we can create
            // a reference type and check that it is derived from key_base
            dot::object keyObj = (key_base)dot::activator::create_instance(elementType);
            if (keyObj.is<key_base>())
            {
                key_base key = (key_base)keyObj;

                // Check type match
                if (!valueType->equals(dot::typeof<dot::string>()) && !valueType->equals(elementType))
                    throw dot::exception(
                        dot::string::format("Attempting to deserialize value of type {0} ", valueType->name) +
                        dot::string::format("into key type {0}; keys should be serialized into semicolon delimited string.", elementType->name));

                // Populate from semicolon delimited string
                dot::string stringValue = value->to_string();
                key->AssignString(stringValue);

                // Add to array or dictionary, depending on what we are inside of
                tuple_->type()->get_method("SetItem")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, indexOfCurrent_, key }));

            }
            else
            {
                // Argument type is unsupported, error message
                throw dot::exception(dot::string::format("Element type {0} is not supported for serialization.", value->type()));
            }
        }
    }

    dot::string TupleWriterImpl::to_string()
    {
        return tuple_->to_string();
    }


    TupleWriterImpl::TupleWriterImpl(dot::object tuple, dot::list<dot::field_info> props)
        : tuple_(tuple)
        , props_(props)
    {

    }


}