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

#pragma once

#include <dc/declare.hpp>
#include <dc/types/record/data.hpp>
#include <dot/serialization/deserialize_attribute.hpp>

namespace dc
{
    class key_base_impl; using key_base = dot::ptr<key_base_impl>;
    class data_impl; using data = dot::ptr<data_impl>;

    /// Key objects must derive from this type.
    class DC_CLASS key_base_impl : public data_impl
    {
        typedef key_base_impl self;

    public:

        dot::string get_value()
        {
            return to_string();
        }

    public: // METHODS

        dot::string to_string();

        void assign_string(dot::string value);

        dot::object_id _id;

    private:

        void assign_string(std::stringstream & value);

        static dot::object deserialize(dot::object value, dot::type type);
        static void serialize(dot::tree_writer_base writer, dot::object obj);

    public: // REFLECTION
        virtual dot::type get_type() { return typeof(); }
        static dot::type typeof()
        {
            static dot::type result = []()-> dot::type
            {
                dot::type t = dot::make_type_builder<self>("dc", "key_base", { dot::make_deserialize_class_attribute(&key_base_impl::deserialize)
                    , dot::make_serialize_class_attribute(&key_base_impl::serialize) })
                          ->with_method("assign_string", static_cast<void (key_base_impl::*)(dot::string)>(&key_base_impl::assign_string), {"value"})
                          ->build();
                return t;
            }();
            return result;
        }
    };
}
