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
#include <dot/system/collections/generic/list.hpp>
#include <dot/serialization/tree_writer_base.hpp>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <stack>

namespace dot
{
    class JsonWriterImpl; using JsonWriter = dot::Ptr<JsonWriterImpl>;

    /// Implementation of TreeWriterBaseImpl using RapidJSON lib.
    class DOT_CLASS JsonWriterImpl : public TreeWriterBaseImpl
    {
        friend JsonWriter make_json_writer();

    private:

        rapidjson::StringBuffer buffer_;
        rapidjson::Writer<rapidjson::StringBuffer> json_writer_;
        std::stack<std::pair<dot::String, TreeWriterState>> element_stack_; // TODO make dot::stack
        TreeWriterState current_state_;

    private:
        JsonWriterImpl();

    public:

        /// Write start document tags. This method
        /// should be called only once for the entire document.
        void write_start_document(dot::String root_element_name) override;

        /// Write end document tag. This method
        /// should be called only once for the entire document.
        /// The root element name passed to this method must match the root element
        /// name passed to the preceding call to write_start_document(...).
        void write_end_document(dot::String root_element_name) override;

        /// Write element start tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.
        void write_start_element(dot::String element_name) override;

        /// Write element end tag. Each element may contain
        /// a single dictionary, a single value, or multiple array items.
        /// The element name passed to this method must match the element name passed
        /// to the matching write_start_element(...) call at the same indent level.
        void write_end_element(dot::String element_name) override;

        /// Write dictionary start tag. A call to this method
        /// must follow write_start_element(...) or write_start_array_item().
        void write_start_dict(dot::String type_name) override;

        /// Write dictionary end tag. A call to this method
        /// must be followed by write_end_element(...) or write_end_array_item().
        void write_end_dict(dot::String type_name) override;

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
        /// will be inferred from Object.get_type().
        void write_value(dot::Object value) override;

        /// Convert to JSON String without checking that JSON document is complete.
        /// This permits the use of this method to inspect the JSON content during creation.
        dot::String to_string() override;
    };

    inline JsonWriter make_json_writer() { return new JsonWriterImpl; }
}
