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
#include <dot/system/ptr.hpp>
#include <dot/system/object_impl.hpp>
#include <dot/system/string.hpp>

namespace dot
{
    enum class tree_writer_state
    {
        /// Empty
        empty,

        /// State following write_start_document.
        document_started,

        /// State following write_end_document.
        document_completed,

        /// State following write_start_element.
        element_started,

        /// State following write_end_element.
        element_completed,

        /// State following write_start_array.
        array_started,

        /// State following write_end_array.
        array_completed,

        /// State following write_start_array_item.
        array_item_started,

        /// State following write_end_array_item.
        array_item_completed,

        /// State following write_start_dict that is not inside an array.
        dict_started,

        /// State following write_end_dict that is not inside an array.
        dict_completed,

        /// State following write_start_dict inside an array.
        dict_array_item_started,

        /// State following write_end_dict inside an array.
        dict_array_item_completed,

        /// State following write_start_value that is not inside an array.
        value_started,

        /// State following write_value that is not inside an array.
        value_written,

        /// State following write_end_value that is not inside an array.
        value_completed,

        /// State following write_start_value inside an array.
        value_array_item_started,

        /// State following write_value inside an array.
        value_array_item_written,

        /// State following write_end_value inside an array.
        value_array_item_completed
    };

    class tree_writer_base_impl; using tree_writer_base = dot::ptr<tree_writer_base_impl>;

    /// Interface for writing tree data.
    class DOT_CLASS tree_writer_base_impl : public dot::object_impl
    {
    public:

        /// Write start document tags. This method
        /// should be called only once for the entire document.
        virtual void write_start_document(dot::string root_element_name) = 0;

        /// Write end document tag. This method
        /// should be called only once for the entire document.
        /// The root element name passed to this method must match the root element
        /// name passed to the preceding call to write_start_document(...).
        virtual void write_end_document(dot::string root_element_name) = 0;

        /// Write element start tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.
        virtual void write_start_element(dot::string element_name) = 0;

        /// Write element end tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.
        /// The element name passed to this method must match the element name passed
        /// to the matching write_start_element(name) call at the same indent level.
        virtual void write_end_element(dot::string element_name) = 0;

        /// Write dictionary start tag. A call to this method
        /// must follow write_start_element(name).
        virtual void write_start_dict(dot::string type_name) = 0;

        /// Write dictionary end tag. A call to this method
        /// must be followed by write_end_element(name).
        virtual void write_end_dict(dot::string type_name) = 0;

        /// Write start tag for an array. A call to this method
        /// must follow write_start_element(name).
        virtual void write_start_array() = 0;

        /// Write end tag for an array. A call to this method
        /// must be followed by write_end_element(name).
        virtual void write_end_array() = 0;

        /// Write start tag for an array item. A call to this method
        /// must follow either write_start_array() or write_end_array_item().
        virtual void write_start_array_item() = 0;

        /// Write end tag for an array item. A call to this method
        /// must be followed by either write_end_array() or write_start_array_item().
        virtual void write_end_array_item() = 0;

        /// Write value start tag. A call to this method
        /// must follow write_start_element(name).
        virtual void write_start_value() = 0;

        /// Write value end tag. A call to this method
        /// must be followed by write_end_element(name).
        virtual void write_end_value() = 0;

        /// Write atomic value. Value type
        /// will be inferred from object.get_type().
        virtual void write_value(dot::object value) = 0;

        /// write_start_element(...) followed by write_start_array().
        void write_start_array_element(dot::string element_name);

        /// write_end_array(...) followed by write_end_element(...).
        void write_end_array_element(dot::string element_name);

        /// Write an element with no inner nodes.
        /// Element type is inferred by calling obj.get_type().
        void write_value_element(dot::string element_name, dot::object value);

        /// Write an array item with no inner nodes.
        /// Element type is inferred by calling obj.get_type().
        void write_value_array_item(dot::object value);

        /// Write a single array item.
        void write_array_item(dot::object value);

        /// Write an array of elements with no inner nodes.
        /// Element type is inferred by calling obj.get_type().
        template <class container>
        void write_value_array(dot::string element_name, container values)
        {
            this->write_start_array_element(element_name);
            for (dot::object value : values)
            {
                this->write_array_item(value);
            }
            this->write_end_array_element(element_name);
        }
    };
}
