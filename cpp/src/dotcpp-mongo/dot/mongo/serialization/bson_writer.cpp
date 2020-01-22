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

#include <dot/mongo/precompiled.hpp>
#include <dot/mongo/implement.hpp>
#include <dot/system/enum.hpp>
#include <dot/system/string.hpp>
#include <dot/mongo/serialization/bson_writer.hpp>
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

#include <dot/mongo/serialization/bson_root_class_attribute.hpp>
#include <dot/mongo/mongo_db/bson/object_id.hpp>
#include <dot/mongo/mongo_db/mongo/settings.hpp>

#include <bsoncxx/json.hpp>

namespace dot
{
    void bson_writer_impl::write_start_document(dot::string root_element_name)
    {
        // Push state and name into the element stack. Writing the actual start tag occurs inside
        // one of write_start_dict, write_start_array_item, or write_start_value calls.
        element_stack_.push({ root_element_name, current_state_ });

        if (current_state_ == TreeWriterState::empty && element_stack_.size() == 1)
        {
            current_state_ = TreeWriterState::document_started;
        }
        else
            throw dot::exception(
                "A call to write_start_document(...) must be the first call to the tree writer.");

        bson_writer_.open_document();
        bson_writer_.key_owned("_t");

        if (dot::mongo_client_settings::get_discriminator_convention() == dot::discriminator_convention::scalar)
        {
            bson_writer_.append(*root_element_name);
        }
        else if (dot::mongo_client_settings::get_discriminator_convention() == dot::discriminator_convention::hierarchical)
        {
            // Get type parrents
            type root_element_type = dot::type_impl::get_type_of(root_element_name);
            list<type> root_element_base_types = get_parents_list(root_element_type);

            // And wirte them to array
            bson_writer_.open_array();
            for (auto i = root_element_base_types->rbegin(); i != root_element_base_types->rend(); ++i)
                bson_writer_.append(*(*i)->name());
            bson_writer_.append(*root_element_name);
            bson_writer_.close_array();
        }
        else
        {
            throw dot::exception("Unknown discriminator_convention.");
        }

    }

    void bson_writer_impl::write_end_document(dot::string root_element_name)
    {
        // Check state transition matrix
        if (current_state_ == TreeWriterState::dict_completed && element_stack_.size() == 1)
        {
            current_state_ = TreeWriterState::document_completed;
        }
        else
            throw dot::exception(
                "A call to write_end_document(...) does not follow  write_end_element(...) at at root level.");

        // Pop the outer element name and state from the element stack
        dot::string current_element_name;
        std::pair<dot::string, TreeWriterState> top = element_stack_.top();
        element_stack_.pop();
        current_element_name = top.first;
        current_state_ = top.second;

        // Check that the current element name matches the specified name. Writing the actual end tag
        // occurs inside one of write_start_dict, write_start_array_item, or write_start_value calls.
        if (root_element_name != current_element_name)
            throw dot::exception(dot::string::format(
                "write_end_document({0}) follows write_start_document({1}), root element name mismatch.", root_element_name, current_element_name));
    }

    void bson_writer_impl::write_start_element(dot::string element_name)
    {
        // Push state and name into the element stack. Writing the actual start tag occurs inside
        // one of write_start_dict, write_start_array_item, or write_start_value calls.
        element_stack_.push({ element_name, current_state_ });

        if (current_state_ == TreeWriterState::document_started) current_state_ = TreeWriterState::element_started;
        else if (current_state_ == TreeWriterState::element_completed) current_state_ = TreeWriterState::element_started;
        else if (current_state_ == TreeWriterState::dict_started) current_state_ = TreeWriterState::element_started;
        else if (current_state_ == TreeWriterState::dict_array_item_started) current_state_ = TreeWriterState::element_started;
        else
            throw dot::exception(
                "A call to write_start_element(...) must be the first call or follow write_end_element(prev_name).");

        // Write "element_name" :
        bson_writer_.key_owned(*(element_stack_.top().first));
    }

    void bson_writer_impl::write_end_element(dot::string element_name)
    {
        // Check state transition matrix
        if (current_state_ == TreeWriterState::element_started) current_state_ = TreeWriterState::element_completed;
        else if (current_state_ == TreeWriterState::dict_completed) current_state_ = TreeWriterState::element_completed;
        else if (current_state_ == TreeWriterState::value_completed) current_state_ = TreeWriterState::element_completed;
        else if (current_state_ == TreeWriterState::array_completed) current_state_ = TreeWriterState::element_completed;
        else throw dot::exception(
            "A call to write_end_element(...) does not follow a matching write_start_element(...) at the same indent level.");

        // Pop the outer element name and state from the element stack
        //(current_element_name, current_state_) = element_stack_.pop();
        dot::string current_element_name;
        std::pair<dot::string, TreeWriterState> top = element_stack_.top();
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

    void bson_writer_impl::write_start_dict(dot::string type_name)
    {
        // Save initial state to be used below
        TreeWriterState prev_state = current_state_;

        // Check state transition matrix
        if (current_state_ == TreeWriterState::document_started) current_state_ = TreeWriterState::dict_started;
        else if (current_state_ == TreeWriterState::element_started) current_state_ = TreeWriterState::dict_started;
        else if (current_state_ == TreeWriterState::array_item_started) current_state_ = TreeWriterState::dict_array_item_started;
        else
            throw dot::exception(
                "A call to write_start_dict() must follow write_start_element(...) or write_start_array_item().");

        if (prev_state == TreeWriterState::document_started)
            return;

        // Write {
        bson_writer_.open_document();
        bson_writer_.key_owned("_t");
        bson_writer_.append(*type_name);

        // If prev state is document_started, write _t tag
        //if (prev_state == TreeWriterState::document_started)
        //{
        //    dot::string root_element_name = element_stack_.top().first;
        //    if (!root_element_name->ends_with("key"))  // TODO remove it
        //        this->write_value_element("_t", root_element_name);
        //}
    }

    void bson_writer_impl::write_end_dict(dot::string type_name)
    {
        // Check state transition matrix
        if (current_state_ == TreeWriterState::dict_started) current_state_ = TreeWriterState::dict_completed;
        else if (current_state_ == TreeWriterState::dict_array_item_started) current_state_ = TreeWriterState::dict_array_item_completed;
        else if (current_state_ == TreeWriterState::element_completed) current_state_ = TreeWriterState::dict_completed;
        else
            throw dot::exception(
                "A call to write_end_dict(...) does not follow a matching write_start_dict(...) at the same indent level.");

        // Write }
        bson_writer_.close_document();
    }

    void bson_writer_impl::write_start_array()
    {
        // Check state transition matrix
        if (current_state_ == TreeWriterState::element_started) current_state_ = TreeWriterState::array_started;
        else
            throw dot::exception(
                "A call to write_start_array() must follow write_start_element(...).");

        // Write [
        bson_writer_.open_array();
    }

    void bson_writer_impl::write_end_array()
    {
        // Check state transition matrix
        if (current_state_ == TreeWriterState::array_started) current_state_ = TreeWriterState::array_completed;
        else if (current_state_ == TreeWriterState::array_item_completed) current_state_ = TreeWriterState::array_completed;
        else
            throw dot::exception(
                "A call to write_end_array(...) does not follow write_end_array_item(...).");

        // Write ]
        bson_writer_.close_array();
    }

    void bson_writer_impl::write_start_array_item()
    {
        // Check state transition matrix
        if (current_state_ == TreeWriterState::array_started) current_state_ = TreeWriterState::array_item_started;
        else if (current_state_ == TreeWriterState::array_item_completed) current_state_ = TreeWriterState::array_item_started;
        else throw dot::exception(
            "A call to write_start_array_item() must follow write_start_element(...) or write_end_array_item().");

        // Nothing to write here
    }

    void bson_writer_impl::write_end_array_item()
    {
        // Check state transition matrix
        if (current_state_ == TreeWriterState::array_item_started) current_state_ = TreeWriterState::array_item_completed;
        else if (current_state_ == TreeWriterState::dict_array_item_completed) current_state_ = TreeWriterState::array_item_completed;
        else if (current_state_ == TreeWriterState::value_array_item_completed) current_state_ = TreeWriterState::array_item_completed;
        else
            throw dot::exception(
                "A call to write_end_array_item(...) does not follow a matching write_start_array_item(...) at the same indent level.");

        // Nothing to write here
    }

    void bson_writer_impl::write_start_value()
    {
        // Check state transition matrix
        if (current_state_ == TreeWriterState::element_started) current_state_ = TreeWriterState::value_started;
        else if (current_state_ == TreeWriterState::array_item_started) current_state_ = TreeWriterState::value_array_item_started;
        else
            throw dot::exception(
                "A call to write_start_value() must follow write_start_element(...) or write_start_array_item().");

        // Nothing to write here
    }

    void bson_writer_impl::write_end_value()
    {
        // Check state transition matrix
        if (current_state_ == TreeWriterState::value_written) current_state_ = TreeWriterState::value_completed;
        else if (current_state_ == TreeWriterState::value_array_item_written) current_state_ = TreeWriterState::value_array_item_completed;
        else
            throw dot::exception(
                "A call to write_end_value(...) does not follow a matching write_value(...) at the same indent level.");

        // Nothing to write here
    }

    void bson_writer_impl::write_value(dot::object value)
    {
        // Check state transition matrix
        if (current_state_ == TreeWriterState::value_started) current_state_ = TreeWriterState::value_written;
        else if (current_state_ == TreeWriterState::value_array_item_started) current_state_ = TreeWriterState::value_array_item_written;
        else
            throw dot::exception(
                "A call to write_end_value(...) does not follow a matching write_value(...) at the same indent level.");

        if (value.is_empty())
        {
            // Null or empty value is serialized as null BSON value.
            // We should only get her for an array as for dictionaries
            // null values should be skipped
            bson_writer_.append(bsoncxx::types::b_null{});
            return;
        }

        // Serialize based on value type
        dot::type value_type = value->get_type();

        if (value_type->equals(dot::typeof<dot::string>()))
            bson_writer_.append(*(dot::string)value);
        else
        if (value_type->equals(dot::typeof<double>())) // ? TODO check dot::typeof<double>() dot::typeof<nullable_double>()
            bson_writer_.append((double)value);
        else
        if (value_type->equals(dot::typeof<bool>()))
            bson_writer_.append((bool)value);
        else
        if (value_type->equals(dot::typeof<int>()))
            bson_writer_.append((int)value);
        else
        if (value_type->equals(dot::typeof<int64_t>()))
            bson_writer_.append((int64_t)value);
        else
        if (value_type->equals(dot::typeof<dot::LocalDate>()))
            bson_writer_.append(dot::LocalDateUtil::to_iso_int((dot::LocalDate)value));
        else
        if (value_type->equals(dot::typeof<dot::LocalTime>()))
            bson_writer_.append(dot::LocalTimeUtil::to_iso_int((dot::LocalTime)value));
        else
        if (value_type->equals(dot::typeof<dot::LocalMinute>()))
            bson_writer_.append(dot::LocalMinuteUtil::to_iso_int((dot::LocalMinute) value));
        else
        if (value_type->equals(dot::typeof<dot::LocalDateTime>()))
            bson_writer_.append(bsoncxx::types::b_date{ dot::LocalDateTimeUtil::to_std_chrono((dot::LocalDateTime)value) });
        else
        if (value_type->equals(dot::typeof<dot::object_id>()))
            bson_writer_.append(((dot::struct_wrapper<dot::object_id>)value)->oid());
        else
        if (value_type->equals(dot::typeof<dot::byte_array>()))
            bson_writer_.append(to_bson_binary((byte_array) value));
        else
        if (value_type->is_enum())
            bson_writer_.append(*value->to_string());
        else
            throw dot::exception(dot::string::format("Element type {0} is not supported for BSON serialization.", value_type));
    }

    dot::string bson_writer_impl::to_string()
    {
        return bsoncxx::to_json(bson_writer_.view_array()[0].get_document().view());
    }

    bsoncxx::document::view bson_writer_impl::view()
    {
        return bson_writer_.view_array()[0].get_document().view();
    }

    bsoncxx::types::b_binary bson_writer_impl::to_bson_binary(byte_array obj)
    {
        return bsoncxx::types::b_binary
        {
            bsoncxx::v_noabi::binary_sub_type::k_binary,
            (uint32_t)obj->get_length(),
            (uint8_t*)obj->get_data()
        };
    }

    list<type> bson_writer_impl::get_parents_list(type from_type)
    {
        list<type> parents_list = make_list<type>();

        // Appending base types to list
        type base = from_type->get_base_type();
        while (base != nullptr)
        {
            parents_list->add(base);

            // Break on root class
            if (base->get_custom_attributes(::dot::typeof<bson_root_class_attribute>(), false)->get_length() != 0)
                break;

            base = base->get_base_type();
        }

        return parents_list;
    }
}
