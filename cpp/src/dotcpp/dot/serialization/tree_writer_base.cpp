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
#include <dot/serialization/tree_writer_base.hpp>
#include <dot/system/string.hpp>
#include <dot/system/object.hpp>

namespace dot
{
    void TreeWriterBaseImpl::write_start_array_element(dot::String element_name)
    {
        this->write_start_element(element_name);
        this->write_start_array();
    }

    void TreeWriterBaseImpl::write_end_array_element(dot::String element_name)
    {
        this->write_end_array();
        this->write_end_element(element_name);
    }

    void TreeWriterBaseImpl::write_value_element(dot::String element_name, dot::Object value)
    {
        // Do not serialize null or empty value
        if (!value.is_empty())
        {
            this->write_start_element(element_name);
            this->write_start_value();
            this->write_value(value);
            this->write_end_value();
            this->write_end_element(element_name);
        }
    }

    void TreeWriterBaseImpl::write_value_array_item(dot::Object value)
    {
        // Writes null or empty value as BSON null
        this->write_start_array_item();
        this->write_start_value();
        this->write_value(value);
        this->write_end_value();
        this->write_end_array_item();
    }

    void TreeWriterBaseImpl::write_array_item(dot::Object value)
    {
        // Will serialize null or empty value
        this->write_start_array_item();
        this->write_start_value();
        this->write_value(value);
        this->write_end_value();
        this->write_end_array_item();
    }
}
