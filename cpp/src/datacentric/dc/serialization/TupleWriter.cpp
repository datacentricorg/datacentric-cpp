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
#include <dc/serialization/TupleWriter.hpp>
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
#include <dc/serialization/DataWriter.hpp>

namespace dc
{
    void TupleWriterImpl::WriteStartDocument(dot::String rootElementName)
    {

    }

    void TupleWriterImpl::WriteEndDocument(dot::String rootElementName)
    {

    }

    void TupleWriterImpl::WriteStartElement(dot::String elementName)
    {
        if (dataWriter_ != nullptr)
        {
            dataWriter_->WriteStartElement(elementName);
        }
        else
        {
            for (int i = 0; i < props_->getCount(); ++i)
            {
                if (elementName == "_key" || elementName == "_id")
                {
                    indexOfCurrent_ = -1;
                    return;
                }

                if (props_[i]->Name == elementName)
                {
                    indexOfCurrent_ = i;
                    if (props_[i]->PropertyType->Name->EndsWith("Data")) //! TODO change EndsWith
                    {
                        Data result = (Data)dot::Activator::CreateInstance(props_[i]->PropertyType);
                        dataWriter_ = new_DataWriter(result);
                        dataWriter_->WriteStartDocument(props_[i]->PropertyType->Name);

                        tuple_->GetType()->GetMethod("SetItem")->Invoke(tuple_, dot::new_Array1D<dot::Object>({ tuple_, indexOfCurrent_, result }));

                        //dataWriter_->WriteStartElement(elementName);
                        //DeserializeDocument(doc, writer);
                        //writer->WriteEndDocument(typeName);
                    }
                    if (! props_[i]->PropertyType->GetInterface("IObjectEnumerable").IsEmpty())
                    {
                        dataWriter_ = new_DataWriter(nullptr);
                        dataWriter_->currentElementInfo_ = props_[i];
                        dataWriter_->currentElementName_ = props_[i]->Name;
                        dataWriter_->currentState_ = TreeWriterState::ElementStarted;

                        dataWriter_->currentArray_ = dot::new_List<dot::IObjectCollection>();

                        //dataWriter_->WriteStartElement(elementName);
                        //DeserializeDocument(doc, writer);
                        //writer->WriteEndDocument(typeName);
                    }

                    return;
                }
            }
            throw dot::new_Exception(dot::String::Format("Unknown element {0} in tuple writer.", elementName));
        }
    }

    void TupleWriterImpl::WriteEndElement(dot::String elementName)
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
            tuple_->GetType()->GetMethod("SetItem")->Invoke(tuple_, dot::new_Array1D<dot::Object>({ tuple_, indexOfCurrent_, dataWriter_->currentArray_ }));
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

    void TupleWriterImpl::WriteValue(dot::Object value)
    {
        if (dataWriter_ != nullptr)
        {
            dataWriter_->WriteValue(value);
            return;
        }

        if (indexOfCurrent_ == -1)
            return;

        // Check that we are either inside dictionary or array
        dot::Type elementType = tuple_->GetType()->GetGenericArguments()[indexOfCurrent_];

        if (value.IsEmpty())  // TODO IsEmpty method should be implemented according to c# extension
        {
            // Do not record null or empty value into dictionary, but add it to an array
            // Add to dictionary or array, depending on what we are inside of
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
            tuple_->GetType()->GetMethod("SetItem")->Invoke(tuple_, dot::new_Array1D<dot::Object>({ tuple_, indexOfCurrent_, convertedValue }));
        }
        else if (elementType->Equals(dot::typeof<dot::LocalDate>()) || elementType->Equals(dot::typeof<dot::Nullable<dot::LocalDate>>()))
        {
            dot::LocalDate dateValue;

            // Check type match
            if (valueType->Equals(dot::typeof<int>()))
            {
                // Deserialize LocalDate as ISO int in yyyymmdd format
                dateValue = LocalDateHelper::ParseIsoInt((int)value);
            }
            else if (valueType->Equals(dot::typeof<int64_t>()))
            {
                // Deserialize LocalDate as ISO int in yyyymmdd format
                dateValue = LocalDateHelper::ParseIsoInt((int64_t)value);
            }
            else throw dot::new_Exception(
                    dot::String::Format("Attempting to deserialize value of type {0} ", valueType->Name) +
                    "into LocalDate; type should be int32.");

            tuple_->GetType()->GetMethod("SetItem")->Invoke(tuple_, dot::new_Array1D<dot::Object>({ tuple_, indexOfCurrent_, dateValue }));
        }
        else if (elementType->Equals(dot::typeof<dot::LocalTime>()) || elementType->Equals(dot::typeof<dot::Nullable<dot::LocalTime>>()))
        {
            dot::LocalTime timeValue;

            // Check type match
            if (valueType->Equals(dot::typeof<int>()))
            {
                // Deserialize LocalTime as ISO int in hhmmssfff format
                timeValue = LocalTimeHelper::ParseIsoInt((int)value);
            }
            else if (valueType->Equals(dot::typeof<int64_t>()))
            {
                // Deserialize LocalTime as ISO int in hhmmssfff format
                timeValue = LocalTimeHelper::ParseIsoInt((int64_t)value);
            }
            else throw dot::new_Exception(
                    dot::String::Format("Attempting to deserialize value of type {0} ", valueType->Name) +
                    "into LocalTime; type should be int32.");

            tuple_->GetType()->GetMethod("SetItem")->Invoke(tuple_, dot::new_Array1D<dot::Object>({ tuple_, indexOfCurrent_, timeValue }));
        }
        else if (elementType->Equals(dot::typeof<dot::LocalMinute>()) || elementType->Equals(dot::typeof<dot::Nullable<dot::LocalMinute>>()))
        {
            dot::LocalMinute minuteValue;

            // Check type match
            if (valueType->Equals(dot::typeof<int>()))
            {
                // Deserialize LocalMinute as ISO int in hhmmssfff format
                minuteValue = LocalMinuteHelper::ParseIsoInt((int)value);
            }
            else if (valueType->Equals(dot::typeof<int64_t>()))
            {
                // Deserialize LocalMinute as ISO int in hhmmssfff format
                minuteValue = LocalMinuteHelper::ParseIsoInt((int64_t)value);
            }
            else throw dot::new_Exception(
                dot::String::Format("Attempting to deserialize value of type {0} ", valueType->Name) +
                "into LocalMinute; type should be int32.");

            tuple_->GetType()->GetMethod("SetItem")->Invoke(tuple_, dot::new_Array1D<dot::Object>({ tuple_, indexOfCurrent_, minuteValue }));
        }
        else if (elementType->Equals(dot::typeof<dot::LocalDateTime>()) || elementType->Equals(dot::typeof<dot::Nullable<dot::LocalDateTime>>()))
        {
        dot::LocalDateTime dateTimeValue;

            // Check type match
            if (valueType->Equals(dot::typeof<dot::LocalDateTime>()))
            {
                dateTimeValue = (dot::LocalDateTime)value;
            }
            else if (valueType->Equals(dot::typeof<int64_t>()))
            {
                // Deserialize LocalDateTime as ISO long in yyyymmddhhmmssfff format
                dateTimeValue = LocalDateTimeHelper::ParseIsoLong((int64_t)value);
            }
            else if (valueType->Equals(dot::typeof<int>()))
            {
                // Deserialize LocalDateTime as ISO long in yyyymmddhhmmssfff format
                dateTimeValue = LocalDateTimeHelper::ParseIsoLong((int)value);
            }
            else if (valueType->Equals(dot::typeof<dot::String>()))
            {
                // Deserialize LocalDateTime as ISO string
                dateTimeValue = LocalDateTimeHelper::Parse((dot::String)value);
            }
            else throw dot::new_Exception(
                    dot::String::Format("Attempting to deserialize value of type {0} ", valueType->Name) +
                    "into LocalDateTime; type should be LocalDateTime.");

            tuple_->GetType()->GetMethod("SetItem")->Invoke(tuple_, dot::new_Array1D<dot::Object>({ tuple_, indexOfCurrent_, dateTimeValue }));
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

            tuple_->GetType()->GetMethod("SetItem")->Invoke(tuple_, dot::new_Array1D<dot::Object>({ tuple_, indexOfCurrent_, enumValue }));
        }
        else
        {
            // We run out of value types at this point, now we can create
            // a reference type and check that it implements KeyType
            dot::Object keyObj = (KeyType)dot::Activator::CreateInstance(elementType);
            if (keyObj.is<KeyType>())
            {
                KeyType key = (KeyType)keyObj;

                // Check type match
                if (!valueType->Equals(dot::typeof<dot::String>()) && !valueType->Equals(elementType))
                    throw dot::new_Exception(
                        dot::String::Format("Attempting to deserialize value of type {0} ", valueType->Name) +
                        dot::String::Format("into key type {0}; keys should be serialized into semicolon delimited string.", elementType->Name));

                // Populate from semicolon delimited string
                dot::String stringValue = value->ToString();
                key->AssignString(stringValue);

                // Add to array or dictionary, depending on what we are inside of
                tuple_->GetType()->GetMethod("SetItem")->Invoke(tuple_, dot::new_Array1D<dot::Object>({ tuple_, indexOfCurrent_, key }));

            }
            else
            {
                // Argument type is unsupported, error message
                throw dot::new_Exception(dot::String::Format("Element type {0} is not supported for serialization.", value->GetType()));
            }
        }
    }

    dot::String TupleWriterImpl::ToString()
    {
        return tuple_->ToString();
    }


    TupleWriterImpl::TupleWriterImpl(dot::Object tuple, dot::List<dot::PropertyInfo> props)
        : tuple_(tuple)
        , props_(props)
    {

    }


}