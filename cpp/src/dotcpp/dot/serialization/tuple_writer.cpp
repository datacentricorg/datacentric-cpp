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
    void tuple_writer_impl::write_start_document(dot::string root_element_name)
    {
    }

    void tuple_writer_impl::write_end_document(dot::string root_element_name)
    {
    }

    void tuple_writer_impl::write_start_element(dot::string element_name)
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_start_element(element_name);
        }
        else
        {
            for (int i = 0; i < props_->count(); ++i)
            {
                if (element_name == "_key" || element_name == "_id")
                {
                    index_of_current_ = -1;
                    return;
                }

                if (props_[i]->name == element_name)
                {
                    index_of_current_ = i;

                    if (dot::typeof<dot::list_base>()->is_assignable_from(props_[i]->field_type))
                    {
                        data_writer_ = make_data_writer(nullptr);
                        data_writer_->current_element_info_ = props_[i];
                        data_writer_->current_element_name_ = props_[i]->name;
                        data_writer_->current_state_ = tree_writer_state::element_started;

                        data_writer_->current_array_ = dot::make_list<dot::list<dot::object>>();

                        //data_writer_->write_start_element(element_name);
                        //deserialize_document(doc, writer);
                        //writer->write_end_document(type_name);
                    }

                    if (props_[i]->field_type->is_class && props_[i]->field_type->get_fields()->size())
                    {
                        object result = dot::activator::create_instance(props_[i]->field_type);
                        data_writer_ = make_data_writer(result);
                        data_writer_->write_start_document(props_[i]->field_type->name);

                        tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, index_of_current_, result }));

                        //data_writer_->write_start_element(element_name);
                        //deserialize_document(doc, writer);
                        //writer->write_end_document(type_name);
                    }

                    return;
                }
            }
            throw dot::exception(dot::string::format("Unknown element {0} in tuple writer.", element_name));
        }
    }

    void tuple_writer_impl::write_end_element(dot::string element_name)
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_end_element(element_name);
        }
    }

    void tuple_writer_impl::write_start_dict(dot::string type_name)
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_start_dict(type_name);
        }

    }

    void tuple_writer_impl::write_end_dict(dot::string type_name)
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_end_dict(type_name);
            if (data_writer_->current_state_ == tree_writer_state::document_started)
                data_writer_ = nullptr;
        }
    }

    void tuple_writer_impl::write_start_array()
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_start_array();
        }

    }

    void tuple_writer_impl::write_end_array()
    {
        if (data_writer_ != nullptr)
        {
            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, index_of_current_, data_writer_->current_array_ }));
            data_writer_->write_end_array();
            data_writer_ = nullptr;
        }

    }

    void tuple_writer_impl::write_start_array_item()
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_start_array_item();
        }

    }

    void tuple_writer_impl::write_end_array_item()
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_end_array_item();
        }

    }

    void tuple_writer_impl::write_start_value()
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_start_value();
        }

    }

    void tuple_writer_impl::write_end_value()
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_end_value();
        }

    }

    void tuple_writer_impl::write_value(dot::object value)
    {
        if (data_writer_ != nullptr)
        {
            data_writer_->write_value(value);
            return;
        }

        if (index_of_current_ == -1)
            return;

        // Check that we are either inside dictionary or array
        dot::type element_type = tuple_->get_type()->get_generic_arguments()[index_of_current_]; // TODO - cache array instead of getting every time?

        if (value.is_empty())  // TODO is_empty method should be implemented according to c# extension
        {
            // Do not record null or empty value into dictionary, but add it to an array
            // Add to dictionary or array, depending on what we are inside of
            return;
        }

        // Write based on element type
        dot::type value_type = value->get_type();
        if (element_type->equals(dot::typeof<dot::string>()) ||
            element_type->equals(dot::typeof<double>()) || element_type->equals(dot::typeof<dot::nullable<double>>()) ||
            element_type->equals(dot::typeof<bool>()) || element_type->equals(dot::typeof<dot::nullable<bool>>()) ||
            element_type->equals(dot::typeof<int>()) || element_type->equals(dot::typeof<dot::nullable<int>>()) ||
            element_type->equals(dot::typeof<int64_t>()) || element_type->equals(dot::typeof<dot::nullable<int64_t>>())
            //element_type->equals(dot::typeof<dot::object_id>())
            )
        {
            // Check type match
            //if (!element_type->equals(value_type)) // TODO change to !element_type->is_assignable_from(value_type)
            //    throw dot::exception(
            //        dot::string::format("Attempting to deserialize value of type {0} ", value_type->name) +
            //        dot::string::format("into element of type {0}.", element_type->name));

            dot::object converted_value = value;
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
            //else if (element_type->equals(dot::typeof<dot::object_id>()) && value_type->equals(dot::typeof<dot::string>()))
            //{
            //    converted_value = dot::object_id((dot::string) value);
            //}

            // Add to array or dictionary, depending on what we are inside of
            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, index_of_current_, converted_value }));
        }
        else if (element_type->equals(dot::typeof<dot::local_date>()) || element_type->equals(dot::typeof<dot::nullable<dot::local_date>>()))
        {
            dot::local_date date_value;

            // Check type match
            if (value_type->equals(dot::typeof<int>()))
            {
                // Deserialize local_date as ISO int in yyyymmdd format
                date_value = dot::local_date_util::parse_iso_int((int)value);
            }
            else if (value_type->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_date as ISO int in yyyymmdd format
                date_value = dot::local_date_util::parse_iso_int((int)value);
            }
            else throw dot::exception(
                    dot::string::format("Attempting to deserialize value of type {0} ", value_type->name) +
                    "into local_date; type should be int32.");

            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, index_of_current_, date_value }));
        }
        else if (element_type->equals(dot::typeof<dot::local_time>()) || element_type->equals(dot::typeof<dot::nullable<dot::local_time>>()))
        {
            dot::local_time time_value;

            // Check type match
            if (value_type->equals(dot::typeof<int>()))
            {
                // Deserialize local_time as ISO int in hhmmssfff format
                time_value = dot::local_time_util::parse_iso_int((int)value);
            }
            else if (value_type->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_time as ISO int in hhmmssfff format
                time_value = dot::local_time_util::parse_iso_int((int)value);
            }
            else throw dot::exception(
                    dot::string::format("Attempting to deserialize value of type {0} ", value_type->name) +
                    "into local_time; type should be int32.");

            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, index_of_current_, time_value }));
        }
        else if (element_type->equals(dot::typeof<dot::local_minute>()) || element_type->equals(dot::typeof<dot::nullable<dot::local_minute>>()))
        {
            dot::local_minute minute_value;

            // Check type match
            if (value_type->equals(dot::typeof<int>()))
            {
                // Deserialize local_minute as ISO int in hhmmssfff format
                minute_value = dot::local_minute_util::parse_iso_int((int)value);
            }
            else if (value_type->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_minute as ISO int in hhmmssfff format
                minute_value = dot::local_minute_util::parse_iso_int((int)value);
            }
            else throw dot::exception(
                dot::string::format("Attempting to deserialize value of type {0} ", value_type->name) +
                "into local_minute; type should be int32.");

            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, index_of_current_, minute_value }));
        }
        else if (element_type->equals(dot::typeof<dot::local_date_time>()) || element_type->equals(dot::typeof<dot::nullable<dot::local_date_time>>()))
        {
        dot::local_date_time date_time_value;

            // Check type match
            if (value_type->equals(dot::typeof<dot::local_date_time>()))
            {
                date_time_value = (dot::local_date_time)value;
            }
            else if (value_type->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_date_time as ISO long in yyyymmddhhmmssfff format
                date_time_value = dot::local_date_time_util::parse_iso_long((int64_t)value);
            }
            else if (value_type->equals(dot::typeof<int>()))
            {
                // Deserialize local_date_time as ISO long in yyyymmddhhmmssfff format
                date_time_value = dot::local_date_time_util::parse_iso_long((int)value);
            }
            else if (value_type->equals(dot::typeof<dot::string>()))
            {
                // Deserialize local_date_time as ISO string
                date_time_value = dot::local_date_time_util::parse((dot::string)value);
            }
            else throw dot::exception(
                    dot::string::format("Attempting to deserialize value of type {0} ", value_type->name) +
                    "into local_date_time; type should be local_date_time.");

            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, index_of_current_, date_time_value }));
        }
        else if (element_type->is_enum)
        {
            // Check type match
            if (!value_type->equals(dot::typeof<dot::string>()))
                throw dot::exception(
                    dot::string::format("Attempting to deserialize value of type {0} ", value_type->name) +
                    dot::string::format("into enum {0}; type should be string.", element_type->name));

            // Deserialize enum as string
            dot::string enum_string = (dot::string) value;
            dot::object enum_value = dot::enum_base::parse(element_type, enum_string);

            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, index_of_current_, enum_value }));
        }
        else if (element_type->get_custom_attributes(dot::typeof<deserialize_class_attribute>(), true)->size())
        {
            deserialize_class_attribute attr = (deserialize_class_attribute)element_type->get_custom_attributes(dot::typeof<deserialize_class_attribute>(), true)[0];

            dot::object obj = attr->deserialize(value, element_type);

            tuple_->get_type()->get_method("set_item")->invoke(tuple_, dot::make_list<dot::object>({ tuple_, index_of_current_, obj }));
        }
        else
        {
            // Argument type is unsupported, error message
            throw dot::exception(dot::string::format("Element type {0} is not supported for serialization.", value->get_type()));
        }
    }

    dot::string tuple_writer_impl::to_string()
    {
        return tuple_->to_string();
    }

    tuple_writer_impl::tuple_writer_impl(dot::object tuple, dot::list<dot::field_info> props)
        : tuple_(tuple)
        , props_(props)
    {
    }
}
