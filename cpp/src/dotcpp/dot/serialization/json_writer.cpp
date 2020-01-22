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
#define RAPIDJSON_HAS_STDSTRING 1
#include <dot/implement.hpp>
#include <dot/serialization/json_writer.hpp>
#include <dot/system/enum.hpp>
#include <dot/system/string.hpp>
#include <dot/system/object.hpp>
#include <dot/system/type.hpp>
#include <dot/noda_time/local_date.hpp>
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_minute.hpp>
#include <dot/noda_time/local_date_time.hpp>
//#include <dot/mongo/mongo_db/bson/object_id.hpp>
#include <dot/noda_time/local_date_util.hpp>
#include <dot/noda_time/local_time_util.hpp>
#include <dot/noda_time/local_minute_util.hpp>
#include <dot/noda_time/local_date_time_util.hpp>

namespace dot
{
    json_writer_impl::json_writer_impl()
        : json_writer_(buffer_)
        , current_state_(tree_writer_state::empty)
    {}

    void json_writer_impl::write_start_document(dot::string root_element_name)
    {
        // Push state and name into the element stack. Writing the actual start tag occurs inside
        // one of write_start_dict, write_start_array_item, or write_start_value calls.
        element_stack_.push({ root_element_name, current_state_ });

        if (current_state_ == tree_writer_state::empty && element_stack_.size() == 1)
        {
            current_state_ = tree_writer_state::document_started;
        }
        else
            throw dot::exception(
                "A call to write_start_document(...) must be the first call to the tree writer.");
    }

    void json_writer_impl::write_end_document(dot::string root_element_name)
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::dict_completed && element_stack_.size() == 1)
        {
            current_state_ = tree_writer_state::document_completed;
        }
        else
            throw dot::exception(
                "A call to write_end_document(...) does not follow  write_end_element(...) at at root level.");

        // Pop the outer element name and state from the element stack
        dot::string current_element_name;
        std::pair<dot::string, tree_writer_state> top = element_stack_.top();
        element_stack_.pop();
        current_element_name = top.first;
        current_state_ = top.second;

        // Check that the current element name matches the specified name. Writing the actual end tag
        // occurs inside one of write_start_dict, write_start_array_item, or write_start_value calls.
        if (root_element_name != current_element_name)
            throw dot::exception(dot::string::format(
                "write_end_document({0}) follows write_start_document({1}), root element name mismatch.", root_element_name, current_element_name));
    }

    void json_writer_impl::write_start_element(dot::string element_name)
    {
        // Push state and name into the element stack. Writing the actual start tag occurs inside
        // one of write_start_dict, write_start_array_item, or write_start_value calls.
        element_stack_.push({ element_name, current_state_ });

        if (current_state_ == tree_writer_state::document_started) current_state_ = tree_writer_state::element_started;
        else if (current_state_ == tree_writer_state::element_completed) current_state_ = tree_writer_state::element_started;
        else if (current_state_ == tree_writer_state::dict_started) current_state_ = tree_writer_state::element_started;
        else if (current_state_ == tree_writer_state::dict_array_item_started) current_state_ = tree_writer_state::element_started;
        else
            throw dot::exception(
                "A call to write_start_element(...) must be the first call or follow write_end_element(prev_name).");

        // Write "element_name" :
        json_writer_.Key(*element_stack_.top().first);
    }

    void json_writer_impl::write_end_element(dot::string element_name)
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::element_started) current_state_ = tree_writer_state::element_completed;
        else if (current_state_ == tree_writer_state::dict_completed) current_state_ = tree_writer_state::element_completed;
        else if (current_state_ == tree_writer_state::value_completed) current_state_ = tree_writer_state::element_completed;
        else if (current_state_ == tree_writer_state::array_completed) current_state_ = tree_writer_state::element_completed;
        else throw dot::exception(
            "A call to write_end_element(...) does not follow a matching write_start_element(...) at the same indent level.");

        // Pop the outer element name and state from the element stack
        //(current_element_name, current_state_) = element_stack_.pop();
        dot::string current_element_name;
        std::pair<dot::string, tree_writer_state> top = element_stack_.top();
        element_stack_.pop();
        current_element_name = top.first;
        current_state_ = top.second;

        // Check that the current element name matches the specified name. Writing the actual end tag
        // occurs inside one of write_start_dict, write_start_array_item, or write_start_value calls.
        if (element_name != current_element_name)
            throw dot::exception(
                dot::string::format("end_complex_element({0}) follows start_complex_element({1}), element name mismatch.", element_name, current_element_name));

        // Nothing to write here but array closing bracket was written above
    }

    void json_writer_impl::write_start_dict(dot::string type_name)
    {
        // Save initial state to be used below
        tree_writer_state prev_state = current_state_;

        // Check state transition matrix
        if (current_state_ == tree_writer_state::document_started) current_state_ = tree_writer_state::dict_started;
        else if (current_state_ == tree_writer_state::element_started) current_state_ = tree_writer_state::dict_started;
        else if (current_state_ == tree_writer_state::array_item_started) current_state_ = tree_writer_state::dict_array_item_started;
        else
            throw dot::exception(
                "A call to write_start_dict() must follow write_start_element(...) or write_start_array_item().");

        // Write {
        json_writer_.StartObject();

        json_writer_.Key("_t");
        json_writer_.String(*type_name);

        // If prev state is document_started, write _t tag
        //if (prev_state == tree_writer_state::document_started)
        //{
        //    dot::string root_element_name = element_stack_.top().first;
        //    if (!root_element_name->ends_with("key"))  // TODO remove it
        //        this->write_value_element("_t", root_element_name);
        //}
    }

    void json_writer_impl::write_end_dict(dot::string type_name)
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::dict_started) current_state_ = tree_writer_state::dict_completed;
        else if (current_state_ == tree_writer_state::dict_array_item_started) current_state_ = tree_writer_state::dict_array_item_completed;
        else if (current_state_ == tree_writer_state::element_completed) current_state_ = tree_writer_state::dict_completed;
        else
            throw dot::exception(
                "A call to write_end_dict(...) does not follow a matching write_start_dict(...) at the same indent level.");

        // Write }
        json_writer_.EndObject();
    }

    void json_writer_impl::write_start_array()
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::element_started) current_state_ = tree_writer_state::array_started;
        else
            throw dot::exception(
                "A call to write_start_array() must follow write_start_element(...).");

        // Write [
        json_writer_.StartArray();
    }

    void json_writer_impl::write_end_array()
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::array_started) current_state_ = tree_writer_state::array_completed;
        else if (current_state_ == tree_writer_state::array_item_completed) current_state_ = tree_writer_state::array_completed;
        else
            throw dot::exception(
                "A call to write_end_array(...) does not follow write_end_array_item(...).");

        // Write ]
        json_writer_.EndArray();
    }

    void json_writer_impl::write_start_array_item()
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::array_started) current_state_ = tree_writer_state::array_item_started;
        else if (current_state_ == tree_writer_state::array_item_completed) current_state_ = tree_writer_state::array_item_started;
        else throw dot::exception(
            "A call to write_start_array_item() must follow write_start_element(...) or write_end_array_item().");

        // Nothing to write here
    }

    void json_writer_impl::write_end_array_item()
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::array_item_started) current_state_ = tree_writer_state::array_item_completed;
        else if (current_state_ == tree_writer_state::dict_array_item_completed) current_state_ = tree_writer_state::array_item_completed;
        else if (current_state_ == tree_writer_state::value_array_item_completed) current_state_ = tree_writer_state::array_item_completed;
        else
            throw dot::exception(
                "A call to write_end_array_item(...) does not follow a matching write_start_array_item(...) at the same indent level.");

        // Nothing to write here
    }

    void json_writer_impl::write_start_value()
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::element_started) current_state_ = tree_writer_state::value_started;
        else if (current_state_ == tree_writer_state::array_item_started) current_state_ = tree_writer_state::value_array_item_started;
        else
            throw dot::exception(
                "A call to write_start_value() must follow write_start_element(...) or write_start_array_item().");

        // Nothing to write here
    }

    void json_writer_impl::write_end_value()
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::value_written) current_state_ = tree_writer_state::value_completed;
        else if (current_state_ == tree_writer_state::value_array_item_written) current_state_ = tree_writer_state::value_array_item_completed;
        else
            throw dot::exception(
                "A call to write_end_value(...) does not follow a matching write_value(...) at the same indent level.");

        // Nothing to write here
    }

    void json_writer_impl::write_value(dot::object value)
    {
        // Check state transition matrix
        if (current_state_ == tree_writer_state::value_started) current_state_ = tree_writer_state::value_written;
        else if (current_state_ == tree_writer_state::value_array_item_started) current_state_ = tree_writer_state::value_array_item_written;
        else
            throw dot::exception(
                "A call to write_end_value(...) does not follow a matching write_value(...) at the same indent level.");

        if (value.is_empty())
        {
            // Null or empty value is serialized as null JSON value.
            // We should only get her for an array as for dictionaries
            // null values should be skipped
            json_writer_.Null();
            return;
        }

        // Serialize based on value type
        dot::type value_type = value->get_type();

        if (value_type->equals(dot::typeof<dot::string>()))
            json_writer_.String(*(dot::string)value);
        else
        if (value_type->equals(dot::typeof<double>())) // ? TODO check dot::typeof<double>() dot::typeof<nullable_double>()
            json_writer_.Double((double)value);
        else
        if (value_type->equals(dot::typeof<bool>()))
            json_writer_.Bool((bool)value);
        else
        if (value_type->equals(dot::typeof<int>()))
            json_writer_.Int((int)value);
        else
        if (value_type->equals(dot::typeof<int64_t>()))
            json_writer_.Int64((int64_t)value);
        else
        if (value_type->equals(dot::typeof<dot::local_date>()))
            json_writer_.Int(dot::local_date_util::to_iso_int((dot::local_date)value));
        else
        if (value_type->equals(dot::typeof<dot::local_time>()))
            json_writer_.Int(dot::local_time_util::to_iso_int((dot::local_time)value));
        else
        if (value_type->equals(dot::typeof<dot::local_minute>()))
            json_writer_.Int(dot::local_minute_util::to_iso_int((dot::local_minute) value));
        else
        if (value_type->equals(dot::typeof<dot::local_date_time>()))
            json_writer_.Int64(dot::local_date_time_util::to_iso_long((dot::local_date_time)value));
        //else
        //if (value_type->equals(dot::typeof<dot::object_id>()))
        //    json_writer_.String(*value->to_string());
        else
        if (value_type->is_enum())
            json_writer_.String(*value->to_string());
        else
            throw dot::exception(dot::string::format("Element type {0} is not supported for JSON serialization.", value_type));
    }

    dot::string json_writer_impl::to_string()
    {
        return buffer_.GetString();
    }
}
