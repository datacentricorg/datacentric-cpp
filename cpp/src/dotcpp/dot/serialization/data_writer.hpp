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

#pragma once

#include <dot/declare.hpp>
#include <dot/serialization/tree_writer_base.hpp>
#include <dot/system/ptr.hpp>
#include <dot/system/type.hpp>
#include <dot/system/collections/generic/list.hpp>
#include <dot/system/collections/generic/dictionary.hpp>
#include <stack>

namespace dot
{
    class data_writer_impl; using data_writer = dot::ptr<data_writer_impl>;
    class tuple_writer_impl; using tuple_writer = dot::ptr<tuple_writer_impl>;

    /// Implementation of tree_writer_base for data.
    class DOT_CLASS data_writer_impl : public tree_writer_base_impl
    {
        friend data_writer make_data_writer(object data_obj);
        friend tuple_writer_impl;

    private:
        struct data_writer_position
        {
            dot::string current_element_name;
            tree_writer_state current_state;
            object current_dict;
            dot::dictionary<dot::string, dot::field_info> current_dict_elements;
            dot::field_info current_element_info;
            dot::list_base current_array;
            dot::type current_array_item_type;
        };

    private: // FIELDS

        std::stack<data_writer_position> element_stack_; // TODO make dot::stack
        dot::string root_element_name_;
        dot::string current_element_name_;
        tree_writer_state current_state_;
        object current_dict_;
        dot::dictionary<dot::string, dot::field_info> current_dict_elements_;
        dot::field_info current_element_info_;
        dot::list_base current_array_;
        dot::type current_array_item_type_;

    private: // CONSTRUCTORS

        data_writer_impl(object data_obj);

    public: //  METHODS

        /// Write start document tags. This method
        /// should be called only once for the entire document.
        void write_start_document(dot::string root_element_name) override;

        /// Write end document tag. This method
        /// should be called only once for the entire document.
        /// The root element name passed to this method must match the root element
        /// name passed to the preceding call to write_start_document(...).
        void write_end_document(dot::string root_element_name) override;

        /// Write element start tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.
        void write_start_element(dot::string element_name) override;

        /// Write element end tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.
        /// The element name passed to this method must match the element name passed
        /// to the matching write_start_element(...) call at the same indent level.
        void write_end_element(dot::string element_name) override;

        /// Write dictionary start tag. A call to this method
        /// must follow write_start_element(...) or write_start_array_item().
        void write_start_dict(dot::string type_name) override;

        /// Write dictionary end tag. A call to this method
        /// must be followed by write_end_element(...) or write_end_array_item().
        void write_end_dict(dot::string type_name) override;

        /// Write start tag for an array. A call to this method
        /// must follow write_start_element(name).
        void write_start_array() override;

        /// Write end tag for an array. A call to this method
        /// must be followed by write_end_element(name).
        void write_end_array() override;

        /// Write start tag for an array item. A call to this method
        /// must follow either write_start_array() or write_end_array_item().
        void write_start_array_item() override;

        /// Write end tag for an array item. A call to this method
        /// must be followed by either write_end_array() or write_start_array_item().
        void write_end_array_item() override;

        /// Write value start tag. A call to this method
        /// must follow write_start_element(...) or write_start_array_item().
        void write_start_value() override;

        /// Write value end tag. A call to this method
        /// must be followed by write_end_element(...) or write_end_array_item().
        void write_end_value() override;

        /// Write atomic value. Value type
        /// will be inferred from object.get_type().
        void write_value(dot::object value) override;

        /// Convert to BSON string without checking that BSON document is complete.
        /// This permits the use of this method to inspect the BSON content during creation.
        dot::string to_string() override;

    private:
        /// Push state to the stack.
        void push_state();

        /// Pop state from the stack.
        void pop_state();
    };

    inline data_writer make_data_writer(object data_obj) { return new data_writer_impl(data_obj); }
}
