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

#include <dc/precompiled.hpp>
#include <dc/implement.hpp>
#include <dc/serialization/tree_writer_base.hpp>
#include <dot/system/string.hpp>
#include <dot/system/object.hpp>

namespace dc
{
    void tree_writer_base_impl::write_start_dict_element(dot::string element_name)
    {
        this->write_start_element(element_name);
        this->write_start_dict();
    }

    void tree_writer_base_impl::write_end_dict_element(dot::string element_name)
    {
        this->write_end_dict();
        this->write_end_element(element_name);
    }

    void tree_writer_base_impl::write_start_array_element(dot::string element_name)
    {
        this->write_start_element(element_name);
        this->write_start_array();
    }

    void tree_writer_base_impl::write_end_array_element(dot::string element_name)
    {
        this->write_end_array();
        this->write_end_element(element_name);
    }

    void tree_writer_base_impl::write_start_dict_array_item()
    {
        this->write_start_array_item();
        this->write_start_dict();
    }

    void tree_writer_base_impl::write_end_dict_array_item()
    {
        this->write_end_dict();
        this->write_end_array_item();
    }

    void tree_writer_base_impl::write_value_element(dot::string element_name, dot::object value)
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

    void tree_writer_base_impl::write_value_array_item(dot::object value)
    {
        // Writes null or empty value as BSON null
        this->write_start_array_item();
        this->write_start_value();
        this->write_value(value);
        this->write_end_value();
        this->write_end_array_item();
    }

    void tree_writer_base_impl::write_array_item(dot::object value)
    {
        // Will serialize null or empty value
        this->write_start_array_item();
        this->write_start_value();
        this->write_value(value);
        this->write_end_value();
        this->write_end_array_item();
    }
}
