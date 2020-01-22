/*
Copyright (C) 2015-present The DotCpp Authors.

This file is part of .C++, a native C++ implementation of
popular .NET class library APIs developed to facilitate
code reuse between C# and C++.

    http://github.com/dotcpp/dotcpp (source)
    http://dotcpp.org (documentation)

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

#include <dot/precompiled.hpp>
#include <dot/implement.hpp>
#include <dot/serialization/tuple_writer.hpp>
#include <dot/noda_time/local_time_util.hpp>
#include <dot/noda_time/local_minute_util.hpp>
#include <dot/noda_time/local_date_util.hpp>
#include <dot/noda_time/local_date_time_util.hpp>
#include <dot/system/enum.hpp>
#include <dot/system/reflection/activator.hpp>
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_minute.hpp>
#include <dot/noda_time/local_date.hpp>
#include <dot/noda_time/local_date_time.hpp>
#include <dot/serialization/data_writer.hpp>
#include <dot/serialization/deserialize_attribute.hpp>

namespace dot
{
    void TupleWriterImpl::write_start_document(dot::String root_element_name)
    {
    }

    void TupleWriterImpl::write_end_document(dot::String root_element_name)
    {
    }

    void TupleWriterImpl::write_start_element(dot::String element_name)
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_start_element(element_name);
        }
        else
        {
            for (int i = 0; i < props_->count(); ++i)
            {
                if (props_[i]->name() == element_name)
                {
                    index_of_current_ = i;

                    if (dot::typeof<dot::ListBase>()->is_assignable_from(props_[i]->field_type()))
                    {
                        data_writer_ = make_data_writer(nullptr);
                        data_writer_->current_element_info_ = props_[i];
                        data_writer_->current_element_name_ = props_[i]->name();
                        data_writer_->current_state_ = TreeWriterState::element_started;

                        data_writer_->current_array_ = dot::make_list<dot::List<dot::Object>>();

                        //data_writer_->write_start_element(element_name);
                        //deserialize_document(doc, writer);
                        //writer->write_end_document(type_name);
                    }

                    if (props_[i]->field_type()->is_class() && props_[i]->field_type()->get_fields()->size())
                    {
                        Object result = dot::Activator::create_instance(props_[i]->field_type());
                        data_writer_ = make_data_writer(result);
                        data_writer_->write_start_document(props_[i]->field_type()->name());

                        tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::Object>({ tuple_, index_of_current_, result }));

                        //data_writer_->write_start_element(element_name);
                        //deserialize_document(doc, writer);
                        //writer->write_end_document(type_name);
                    }

                    return;
                }
            }
            throw dot::Exception(dot::String::format("Unknown element {0} in tuple writer.", element_name));
        }
    }

    void TupleWriterImpl::write_end_element(dot::String element_name)
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_end_element(element_name);
        }
    }

    void TupleWriterImpl::write_start_dict(dot::String type_name)
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_start_dict(type_name);
        }
    }

    void TupleWriterImpl::write_end_dict(dot::String type_name)
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_end_dict(type_name);
            if (data_writer_->current_state_ == TreeWriterState::document_started)
                data_writer_ = nullptr;
        }
    }

    void TupleWriterImpl::write_start_array()
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_start_array();
        }
    }

    void TupleWriterImpl::write_end_array()
    {
        if (data_writer_ != nullptr)
        {
            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::Object>({ tuple_, index_of_current_, data_writer_->current_array_ }));
            data_writer_->write_end_array();
            data_writer_ = nullptr;
        }
    }

    void TupleWriterImpl::write_start_array_item()
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_start_array_item();
        }
    }

    void TupleWriterImpl::write_end_array_item()
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_end_array_item();
        }
    }

    void TupleWriterImpl::write_start_value()
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_start_value();
        }
    }

    void TupleWriterImpl::write_end_value()
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_end_value();
        }
    }

    void TupleWriterImpl::write_value(dot::Object value)
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_value(value);
            return;
        }

        if (index_of_current_ == -1)
            return;

        // Check that we are either inside dictionary or array
        dot::Type element_type = tuple_->get_type()->get_generic_arguments()[index_of_current_]; // TODO - cache array instead of getting every time?

        if (value.is_empty())  // TODO is_empty method should be implemented according to c# extension
        {
            // Do not record null or empty value into dictionary, but add it to an array
            // Add to dictionary or array, depending on what we are inside of
            return;
        }

        // Write based on element Type
        dot::Type value_type = value->get_type();
        if (element_type->equals(dot::typeof<dot::String>()) ||
            element_type->equals(dot::typeof<double>()) || element_type->equals(dot::typeof<dot::Nullable<double>>()) ||
            element_type->equals(dot::typeof<bool>()) || element_type->equals(dot::typeof<dot::Nullable<bool>>()) ||
            element_type->equals(dot::typeof<int>()) || element_type->equals(dot::typeof<dot::Nullable<int>>()) ||
            element_type->equals(dot::typeof<int64_t>()) || element_type->equals(dot::typeof<dot::Nullable<int64_t>>())
            )
        {
            // Check Type match
            //if (!element_type->equals(value_type)) // TODO change to !element_type->is_assignable_from(value_type)
            //    throw dot::Exception(
            //        dot::String::format("Attempting to deserialize value of Type {0} ", value_type->name()) +
            //        dot::String::format("into element of Type {0}.", element_type->name()));

            dot::Object converted_value = value;
            if (element_type->equals(dot::typeof<double>()))
            {
                if (value_type->equals(dot::typeof<int>())) converted_value = static_cast<double>((int) value);
                if (value_type->equals(dot::typeof<int64_t>())) converted_value = static_cast<double>((int64_t) value);
            }
            else if (element_type->equals(dot::typeof<int64_t>()) && value_type->equals(dot::typeof<int>()))
            {
                converted_value = static_cast<int64_t>((int) value);
            }
            else if (element_type->equals(dot::typeof<int>()) && value_type->equals(dot::typeof<int64_t>()))
            {
                converted_value = static_cast<int>((int64_t) value);
            }
            //else if (element_type->equals(dot::typeof<dot::ObjectId>()) && value_type->equals(dot::typeof<dot::String>()))
            //{
            //    converted_value = dot::ObjectId((dot::String) value);
            //}

            // Add to array or dictionary, depending on what we are inside of
            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::Object>({ tuple_, index_of_current_, converted_value }));
        }
        else if (element_type->equals(dot::typeof<dot::LocalDate>()) || element_type->equals(dot::typeof<dot::Nullable<dot::LocalDate>>()))
        {
            dot::LocalDate date_value;

            // Check Type match
            if (value_type->equals(dot::typeof<int>()))
            {
                // Deserialize LocalDate as ISO int in yyyymmdd format
                date_value = dot::LocalDateUtil::parse_iso_int((int)value);
            }
            else if (value_type->equals(dot::typeof<int64_t>()))
            {
                // Deserialize LocalDate as ISO int in yyyymmdd format
                date_value = dot::LocalDateUtil::parse_iso_int((int)value);
            }
            else throw dot::Exception(
                    dot::String::format("Attempting to deserialize value of type {0} ", value_type->name()) +
                    "into LocalDate; type should be int32.");

            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::Object>({ tuple_, index_of_current_, date_value }));
        }
        else if (element_type->equals(dot::typeof<dot::LocalTime>()) || element_type->equals(dot::typeof<dot::Nullable<dot::LocalTime>>()))
        {
            dot::LocalTime time_value;

            // Check Type match
            if (value_type->equals(dot::typeof<int>()))
            {
                // Deserialize LocalTime as ISO int in hhmmssfff format
                time_value = dot::LocalTimeUtil::parse_iso_int((int)value);
            }
            else if (value_type->equals(dot::typeof<int64_t>()))
            {
                // Deserialize LocalTime as ISO int in hhmmssfff format
                time_value = dot::LocalTimeUtil::parse_iso_int((int)value);
            }
            else throw dot::Exception(
                    dot::String::format("Attempting to deserialize value of type {0} ", value_type->name()) +
                    "into LocalTime; type should be int32.");

            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::Object>({ tuple_, index_of_current_, time_value }));
        }
        else if (element_type->equals(dot::typeof<dot::LocalMinute>()) || element_type->equals(dot::typeof<dot::Nullable<dot::LocalMinute>>()))
        {
            dot::LocalMinute minute_value;

            // Check Type match
            if (value_type->equals(dot::typeof<int>()))
            {
                // Deserialize LocalMinute as ISO int in hhmmssfff format
                minute_value = dot::LocalMinuteUtil::parse_iso_int((int)value);
            }
            else if (value_type->equals(dot::typeof<int64_t>()))
            {
                // Deserialize LocalMinute as ISO int in hhmmssfff format
                minute_value = dot::LocalMinuteUtil::parse_iso_int((int)value);
            }
            else throw dot::Exception(
                dot::String::format("Attempting to deserialize value of type {0} ", value_type->name()) +
                "into LocalMinute; type should be int32.");

            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::Object>({ tuple_, index_of_current_, minute_value }));
        }
        else if (element_type->equals(dot::typeof<dot::LocalDateTime>()) || element_type->equals(dot::typeof<dot::Nullable<dot::LocalDateTime>>()))
        {
        dot::LocalDateTime date_time_value;

            // Check Type match
            if (value_type->equals(dot::typeof<dot::LocalDateTime>()))
            {
                date_time_value = (dot::LocalDateTime)value;
            }
            else if (value_type->equals(dot::typeof<int64_t>()))
            {
                // Deserialize LocalDateTime as ISO long in yyyymmddhhmmssfff format
                date_time_value = dot::LocalDateTimeUtil::parse_iso_long((int64_t)value);
            }
            else if (value_type->equals(dot::typeof<int>()))
            {
                // Deserialize LocalDateTime as ISO long in yyyymmddhhmmssfff format
                date_time_value = dot::LocalDateTimeUtil::parse_iso_long((int)value);
            }
            else if (value_type->equals(dot::typeof<dot::String>()))
            {
                // Deserialize LocalDateTime as ISO String
                date_time_value = dot::LocalDateTimeUtil::parse((dot::String)value);
            }
            else throw dot::Exception(
                    dot::String::format("Attempting to deserialize value of type {0} ", value_type->name()) +
                    "into LocalDateTime; type should be LocalDateTime.");

            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::Object>({ tuple_, index_of_current_, date_time_value }));
        }
        else if (element_type->is_enum())
        {
            // Check Type match
            if (!value_type->equals(dot::typeof<dot::String>()))
                throw dot::Exception(
                    dot::String::format("Attempting to deserialize value of type {0} ", value_type->name()) +
                    dot::String::format("into enum {0}; type should be String.", element_type->name()));

            // Deserialize enum as String
            dot::String enum_string = (dot::String) value;
            dot::Object enum_value = dot::EnumBase::parse(element_type, enum_string);

            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::Object>({ tuple_, index_of_current_, enum_value }));
        }
        else if (element_type->get_custom_attributes(dot::typeof<DeserializeClassAttribute>(), true)->size())
        {
            DeserializeClassAttribute attr = (DeserializeClassAttribute)element_type->get_custom_attributes(dot::typeof<DeserializeClassAttribute>(), true)[0];

            dot::Object obj = attr->deserialize(value, element_type);

            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::Object>({ tuple_, index_of_current_, obj }));
        }
        else
        {
            // Argument Type is unsupported, error message
            throw dot::Exception(dot::String::format("Element type {0} is not supported for serialization.", value->get_type()));
        }
    }

    dot::String TupleWriterImpl::to_string()
    {
        return tuple_->to_string();
    }

    TupleWriterImpl::TupleWriterImpl(dot::Object tuple, dot::List<dot::FieldInfo> props)
        : tuple_(tuple)
        , props_(props)
    {
    }
}
