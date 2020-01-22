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
#include <dot/serialization/data_writer.hpp>
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
#include <dot/serialization/deserialize_attribute.hpp>

namespace dot
{
    data_writer_impl::data_writer_impl(object obj)
        : current_dict_(obj)
        , current_state_(tree_writer_state::empty) {}

    void data_writer_impl::write_start_document(dot::string root_element_name)
    {
        if (current_state_ == tree_writer_state::empty && element_stack_.size() == 0)
        {
            current_state_ = tree_writer_state::document_started;
        }
        else
        {
            throw dot::exception("A call to write_start_document(...) must be the first call to the tree writer.");
        }

        dot::string root_name = current_dict_->get_type()->name();
        // Check that the name matches
        if (root_element_name != root_name) throw dot::exception(
            dot::string::format("Attempting to deserialize data for type {0} into type {1}.", root_element_name, root_name));

        root_element_name_ = root_element_name;
        current_element_name_ = root_element_name;
        auto current_dict_info_list = current_dict_->get_type()->get_fields();
        current_dict_elements_ = dot::make_dictionary<dot::string, dot::field_info>();
        for(auto element_info : current_dict_info_list) current_dict_elements_->add(element_info->name, element_info);
        current_array_ = nullptr;
        current_array_item_type_ = nullptr;
    }

    void data_writer_impl::write_end_document(dot::string root_element_name)
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::document_started && element_stack_.size() == 0) current_state_ = tree_writer_state::document_completed;
        else throw dot::exception(
            "A call to write_end_document(...) does not follow  write_end_element(...) at at root level.");

        // Check that the current element name matches the specified name. Writing the actual end tag
        // occurs inside one of write_start_dict, write_start_array_item, or write_start_value calls.
        if (root_element_name != root_element_name_)
            throw dot::exception(dot::string::format(
                "write_end_document({0}) follows write_start_document({1}), root element name mismatch.", root_element_name, root_element_name_));
    }

    void data_writer_impl::write_start_element(dot::string element_name)
    {
        if (current_state_ == tree_writer_state::document_started) current_state_ = tree_writer_state::element_started;
        else if (current_state_ == tree_writer_state::element_completed) current_state_ = tree_writer_state::element_started;
        else if (current_state_ == tree_writer_state::dict_started) current_state_ = tree_writer_state::element_started;
        else if (current_state_ == tree_writer_state::dict_array_item_started) current_state_ = tree_writer_state::element_started;
        else throw dot::exception(
            "A call to write_start_element(...) must be the first call or follow write_end_element(prev_name).");

        current_element_name_ = element_name;
        current_element_info_ = current_dict_elements_[element_name];
    }

    void data_writer_impl::write_end_element(dot::string element_name)
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::element_started) current_state_ = tree_writer_state::element_completed;
        else if (current_state_ == tree_writer_state::dict_completed) current_state_ = tree_writer_state::element_completed;
        else if (current_state_ == tree_writer_state::value_completed) current_state_ = tree_writer_state::element_completed;
        else if (current_state_ == tree_writer_state::array_completed) current_state_ = tree_writer_state::element_completed;
        else throw dot::exception(
            "A call to write_end_element(...) does not follow a matching write_start_element(...) at the same indent level.");

        // Check that the current element name matches the specified name. Writing the actual end tag
        // occurs inside one of write_start_dict, write_start_array_item, or write_start_value calls.
        if (element_name != current_element_name_)
            throw dot::exception(dot::string::format(
                "end_complex_element({0}) follows start_complex_element({1}), element name mismatch.", element_name, current_element_name_));
    }

    void data_writer_impl::write_start_dict(dot::string type_name)
    {
        // Push state before defining dictionary state
        push_state();

        // Check state transition matrix
        if (current_state_ == tree_writer_state::document_started)
        {
            current_state_ = tree_writer_state::dict_started;

            // Return if this call follows start_document, all setup is done in start_document
            return;
        }
        else if (current_state_ == tree_writer_state::element_started) current_state_ = tree_writer_state::dict_started;
        else if (current_state_ == tree_writer_state::array_item_started) current_state_ = tree_writer_state::dict_array_item_started;
        else throw dot::exception(
            "A call to write_start_dict() must follow write_start_element(...) or write_start_array_item().");

        dot::object created_dict;

        if (type_name != nullptr && !type_name->empty())
        {
            created_dict = dot::activator::create_instance("", type_name);
        }
        else
        {
            // Set dictionary info
            dot::type created_dict_type = nullptr;
            if (current_array_ != nullptr) created_dict_type = current_array_item_type_;
            else if (current_dict_ != nullptr) created_dict_type = current_element_info_->field_type;
            else throw dot::exception("Value can only be added to a dictionary or array.");

            created_dict = dot::activator::create_instance(created_dict_type);
        }

        // Add to array or dictionary, depending on what we are inside of
        if (current_array_ != nullptr) current_array_->add_object(created_dict);
        else if (current_dict_ != nullptr) current_element_info_->set_value(current_dict_, created_dict);
        else throw dot::exception("Value can only be added to a dictionary or array.");

        current_dict_ = created_dict;
        auto current_dict_info_list = created_dict->get_type()->get_fields();
        current_dict_elements_ = dot::make_dictionary<dot::string, dot::field_info>();
        for (auto element_info : current_dict_info_list) current_dict_elements_->add(element_info->name, element_info);
        current_array_ = nullptr;
        current_array_item_type_ = nullptr;
    }

    void data_writer_impl::write_end_dict(dot::string type_name)
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::dict_started) current_state_ = tree_writer_state::dict_completed;
        else if (current_state_ == tree_writer_state::dict_array_item_started) current_state_ = tree_writer_state::dict_array_item_completed;
        else if (current_state_ == tree_writer_state::element_completed) current_state_ = tree_writer_state::dict_completed;
        else throw dot::exception(
            "A call to write_end_dict(...) does not follow a matching write_start_dict(...) at the same indent level.");

        // Restore previous state
        pop_state();
    }

    void data_writer_impl::write_start_array()
    {
        // Push state
        push_state();

        // Check state transition matrix
        if (current_state_ == tree_writer_state::element_started) current_state_ = tree_writer_state::array_started;
        else
            throw dot::exception(
                "A call to write_start_array() must follow write_start_element(...).");

        // Create the array
        dot::object created_array_obj = dot::activator::create_instance(current_element_info_->field_type);
        if (created_array_obj.is<dot::list_base>()) // TODO Also support native arrays
        {
            // Add to array or dictionary, depending on what we are inside of
            if (current_array_ != nullptr) current_array_->add_object(created_array_obj);
            else if (current_dict_ != nullptr) current_element_info_->set_value(current_dict_, created_array_obj);
            else throw dot::exception("Value can only be added to a dictionary or array.");

            current_array_ = (dot::list_base) created_array_obj;

            // Get array item type from array type using reflection
            dot::type list_type = current_element_info_->field_type;      // TODO fix
//            if (!list_type->is_generic_type) throw dot::exception(dot::string::format(
//                "Type {0} cannot be serialized because it implements only list_base but not list_base<T>.", list_type));
//            list<type> generic_parameter_types = list_type->generic_type_arguments;
            dot::list<dot::type> generic_parameter_types = list_type->get_generic_arguments();
            if (generic_parameter_types->count() != 1) throw dot::exception(
                dot::string::format("Generic parameter type list {0} has more than ", generic_parameter_types) +
                "one element creating an ambiguity for deserialization code.");
            current_array_item_type_ = generic_parameter_types[0];

            current_dict_ = nullptr;
            current_element_info_ = nullptr;
            current_dict_elements_ = nullptr;
        }
        else
        {
            dot::string class_name = current_element_info_->field_type->full_name();
            throw dot::exception(dot::string::format(
                "Element {0} of type {1} does not implement collection_base.", current_element_info_->name, class_name));
        }
    }

    void data_writer_impl::write_end_array()
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::array_item_completed) current_state_ = tree_writer_state::array_completed;
        else throw dot::exception(
            "A call to write_end_array(...) does not follow write_end_array_item(...).");

        // Pop state
        pop_state();
    }

    void data_writer_impl::write_start_array_item()
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::array_started) current_state_ = tree_writer_state::array_item_started;
        else if (current_state_ == tree_writer_state::array_item_completed) current_state_ = tree_writer_state::array_item_started;
        else throw dot::exception(
            "A call to write_start_array_item() must follow write_start_element(...) or write_end_array_item().");

        //dot::object added_item = nullptr;
        //if (current_array_item_type_ == dot::typeof<string>()) added_item = nullptr;
        //else if (current_array_item_type_ == dot::typeof<double>()) added_item = double();
        //else if (current_array_item_type_ == dot::typeof<nullable<double>>()) added_item = nullptr;
        //else if (current_array_item_type_ == dot::typeof<bool>()) added_item = bool();
        //else if (current_array_item_type_ == dot::typeof<nullable<bool>>()) added_item = nullptr;
        //else if (current_array_item_type_ == dot::typeof<int>()) added_item = int();
        //else if (current_array_item_type_ == dot::typeof<nullable<int>>()) added_item = nullptr;
        //else if (current_array_item_type_ == dot::typeof<int64_t>()) added_item = int64_t();
        //else if (current_array_item_type_ == dot::typeof<nullable<int64_t>>()) added_item = nullptr;
        //else if (current_array_item_type_ == dot::typeof<local_date>()) added_item = dot::local_date();
        //else if (current_array_item_type_ == dot::typeof<nullable<local_date>>()) added_item = nullptr;
        //else if (current_array_item_type_ == dot::typeof<local_time>()) added_item = dot::local_time();
        //else if (current_array_item_type_ == dot::typeof<nullable<local_time>>()) added_item = nullptr;
        //else if (current_array_item_type_ == dot::typeof<local_date_time>()) added_item = dot::local_date_time();
        //else if (current_array_item_type_ == dot::typeof<nullable<local_date_time>>()) added_item = nullptr;
        //else if (current_array_item_type_->is_class) added_item = nullptr;
        //else throw dot::exception(dot::string::format("Value type {0} is not supported for serialization.", current_array_item_type_->name));

        //current_array_->add_object(added_item);
        //current_array_item_ = added_item;
    }

    void data_writer_impl::write_end_array_item()
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::array_item_started) current_state_ = tree_writer_state::array_item_completed;
        else if (current_state_ == tree_writer_state::dict_array_item_completed) current_state_ = tree_writer_state::array_item_completed;
        else if (current_state_ == tree_writer_state::value_array_item_completed) current_state_ = tree_writer_state::array_item_completed;
        else throw dot::exception(
            "A call to write_end_array_item(...) does not follow a matching write_start_array_item(...) at the same indent level.");

        // Do nothing here
    }

    void data_writer_impl::write_start_value()
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::element_started) current_state_ = tree_writer_state::value_started;
        else if (current_state_ == tree_writer_state::array_item_started) current_state_ = tree_writer_state::value_array_item_started;
        else throw dot::exception(
            "A call to write_start_value() must follow write_start_element(...) or write_start_array_item().");
    }

    void data_writer_impl::write_end_value()
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::value_written) current_state_ = tree_writer_state::value_completed;
        else if (current_state_ == tree_writer_state::value_array_item_written) current_state_ = tree_writer_state::value_array_item_completed;
        else throw dot::exception(
            "A call to write_end_value(...) does not follow a matching write_value(...) at the same indent level.");

        // Nothing to write here
    }

    void data_writer_impl::write_value(dot::object value)
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::value_started) current_state_ = tree_writer_state::value_written;
        else if (current_state_ == tree_writer_state::value_array_item_started) current_state_ = tree_writer_state::value_array_item_written;
        else throw dot::exception(
            "A call to write_end_value(...) does not follow a matching write_value(...) at the same indent level.");

        // Check if dict contains _id field
        // skip it if not.
        if (current_array_item_type_ == nullptr
            && current_element_info_ == nullptr
            && current_element_name_ == "_id")
        {
            return;
        }

        // Check that we are either inside dictionary or array
        dot::type element_type = nullptr;
        if (current_array_ != nullptr) element_type = current_array_item_type_;
        else if (current_dict_ != nullptr) element_type = current_element_info_->field_type;
        else throw dot::exception(
            dot::string::format("Cannot write_value(...)for element {0} ", current_element_name_) +
            "is called outside dictionary or array.");

        // Check for deserialize_field_attribute
        // Points to custom field deserializer
        if (current_dict_ != nullptr && current_element_info_ != nullptr)
        {
            list<attribute> attrs = current_element_info_->get_custom_attributes(dot::typeof<deserialize_field_attribute>(), true);
            if (attrs->size())
            {
                deserialize_field_attribute(attrs[0])->deserialize(value, current_element_info_, current_dict_);
                return;
            }
        }

        if (value.is_empty())  // TODO is_empty method should be implemented according to c# extension
        {
            // Do not record null or empty value into dictionary, but add it to an array
            // Add to dictionary or array, depending on what we are inside of
            if (current_array_ != nullptr) current_array_->add_object(nullptr);
            return;
        }

        dot::type value_type = value->get_type();
        if (element_type->equals(dot::typeof<dot::string>()) ||
            element_type->equals(dot::typeof<double>()) || element_type->equals(dot::typeof<dot::nullable<double>>()) ||
            element_type->equals(dot::typeof<bool>()) || element_type->equals(dot::typeof<dot::nullable<bool>>()) ||
            element_type->equals(dot::typeof<int>()) || element_type->equals(dot::typeof<dot::nullable<int>>()) ||
            element_type->equals(dot::typeof<int64_t>()) || element_type->equals(dot::typeof<dot::nullable<int64_t>>())
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

            // Add to array or dictionary, depending on what we are inside of
            if (current_array_ != nullptr) current_array_->add_object(converted_value);
            else if (current_dict_ != nullptr) current_element_info_->set_value(current_dict_, converted_value);
            else throw dot::exception("Value can only be added to a dictionary or array.");
        }
        else if (element_type->equals(dot::typeof<dot::local_date>()) || element_type->equals(dot::typeof<dot::nullable<dot::local_date>>()))
        {
            dot::local_date date_value;

            // Check type match
            if (value_type->equals(dot::typeof<int>()))
            {
                // Deserialize local_date as ISO int in yyyymmdd format
                date_value = dot::local_date_util::parse_iso_int((int) value);
            }
            else if (value_type->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_date as ISO int in yyyymmdd format
                date_value = dot::local_date_util::parse_iso_int((int) value);
            }
            else throw dot::exception(
                    dot::string::format("Attempting to deserialize value of type {0} ", value_type->name()) +
                    "into local_date; type should be int32.");

            // Add to array or dictionary, depending on what we are inside of
            if (current_array_ != nullptr) current_array_->add_object(date_value);
            else if (current_dict_ != nullptr) current_element_info_->set_value(current_dict_, date_value);
            else throw dot::exception("Value can only be added to a dictionary or array.");
        }
        else if (element_type->equals(dot::typeof<dot::local_time>()) || element_type->equals(dot::typeof<dot::nullable<dot::local_time>>()))
        {
            dot::local_time time_value;

            // Check type match
            if (value_type->equals(dot::typeof<int>()))
            {
                // Deserialize local_time as ISO int in hhmmssfff format
                time_value = dot::local_time_util::parse_iso_int((int) value);
            }
            else if (value_type->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_time as ISO int in hhmmssfff format
                time_value = dot::local_time_util::parse_iso_int((int) value);
            }
            else throw dot::exception(
                    dot::string::format("Attempting to deserialize value of type {0} ", value_type->name()) +
                    "into local_time; type should be int32.");

            // Add to array or dictionary, depending on what we are inside of
            if (current_array_ != nullptr) current_array_->add_object(time_value);
            else if (current_dict_ != nullptr) current_element_info_->set_value(current_dict_, time_value);
            else throw dot::exception("Value can only be added to a dictionary or array.");
        }
        else if (element_type->equals(dot::typeof<dot::local_minute>()) || element_type->equals(dot::typeof<dot::nullable<dot::local_minute>>()))
        {
            dot::local_minute minute_value;

            // Check type match
            if (value_type->equals(dot::typeof<int>()))
            {
                // Deserialize local_minute as ISO int in hhmmssfff format
                minute_value = dot::local_minute_util::parse_iso_int((int) value);
            }
            else if (value_type->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_minute as ISO int in hhmmssfff format
                minute_value = dot::local_minute_util::parse_iso_int((int) value);
            }
            else throw dot::exception(
                dot::string::format("Attempting to deserialize value of type {0} ", value_type->name()) +
                "into local_minute; type should be int32.");

            // Add to array or dictionary, depending on what we are inside of
            if (current_array_ != nullptr) current_array_->add_object(minute_value);
            else if (current_dict_ != nullptr) current_element_info_->set_value(current_dict_, minute_value);
            else throw dot::exception("Value can only be added to a dictionary or array.");
        }
        else if (element_type->equals(dot::typeof<dot::local_date_time>()) || element_type->equals(dot::typeof<dot::nullable<dot::local_date_time>>()))
        {
        dot::local_date_time date_time_value;

            // Check type match
            if (value_type->equals(dot::typeof<dot::local_date_time>()))
            {
                date_time_value = (dot::local_date_time) value;
            }
            else if (value_type->equals(dot::typeof<int64_t>()))
            {
                // Deserialize local_date_time as ISO long in yyyymmddhhmmssfff format
                date_time_value = dot::local_date_time_util::parse_iso_long((int64_t) value);
            }
            else if (value_type->equals(dot::typeof<int>()))
            {
                // Deserialize local_date_time as ISO long in yyyymmddhhmmssfff format
                date_time_value = dot::local_date_time_util::parse_iso_long((int) value);
            }
            else if (value_type->equals(dot::typeof<dot::string>()))
            {
                // Deserialize local_date_time as ISO string
                date_time_value = dot::local_date_time_util::parse((dot::string) value);
            }
            else throw dot::exception(
                    dot::string::format("Attempting to deserialize value of type {0} ", value_type->name()) +
                    "into local_date_time; type should be local_date_time.");

            // Add to array or dictionary, depending on what we are inside of
            if (current_array_ != nullptr) current_array_->add_object(date_time_value);
            else if (current_dict_ != nullptr) current_element_info_->set_value(current_dict_, date_time_value);
            else throw dot::exception("Value can only be added to a dictionary or array.");
        }
        else if (element_type->is_enum())
        {
            // Check type match
            if (!value_type->equals(dot::typeof<dot::string>()))
                throw dot::exception(
                    dot::string::format("Attempting to deserialize value of type {0} ", value_type->name()) +
                    dot::string::format("into enum {0}; type should be string.", element_type->name()));

            // Deserialize enum as string
            dot::object enum_value = element_type->get_method("parse")->invoke(nullptr, dot::make_list<dot::object>({ value }));


            // Add to array or dictionary, depending on what we are inside of
            if (current_array_ != nullptr) current_array_->add_object(enum_value);
            else if (current_dict_ != nullptr) current_element_info_->set_value(current_dict_, enum_value);
            else throw dot::exception("Value can only be added to a dictionary or array.");
        }
        // Check for custom deserializer for element type
        else if (element_type->get_custom_attributes(dot::typeof<deserialize_class_attribute>(), true)->size())
        {
            deserialize_class_attribute attr = (deserialize_class_attribute)element_type->get_custom_attributes(dot::typeof<deserialize_class_attribute>(), true)[0];

            dot::object obj = attr->deserialize(value, element_type);

            if (current_array_ != nullptr) current_array_->add_object(obj);
            else if (current_dict_ != nullptr) current_element_info_->set_value(current_dict_, obj);
            else throw dot::exception("Value can only be added to a dictionary or array.");
        }
        else
        {
            // Argument type is unsupported, error message
            throw dot::exception(dot::string::format("Element type {0} is not supported for serialization.", value->get_type()));
        }
    }

    dot::string data_writer_impl::to_string()
    {
        if (current_array_ != nullptr) return current_array_->to_string();
        else if (current_dict_ != nullptr) return current_dict_->to_string();
        else return get_type()->name();
    }

    void data_writer_impl::push_state()
    {
        element_stack_.push(
            data_writer_position
            {
                current_element_name_,
                current_state_,
                current_dict_,
                current_dict_elements_,
                current_element_info_,
                current_array_,
                current_array_item_type_
            });
    }

    void data_writer_impl::pop_state()
    {
        // Pop the outer element name and state from the element stack
        auto stack_item = element_stack_.top();
        element_stack_.pop();

        current_element_name_ = stack_item.current_element_name;
        current_state_ = stack_item.current_state;
        current_dict_ = stack_item.current_dict;
        current_dict_elements_ = stack_item.current_dict_elements;
        current_element_info_ = stack_item.current_element_info;
        current_array_ = stack_item.current_array;
        current_array_item_type_ = stack_item.current_array_item_type;
    }
}
