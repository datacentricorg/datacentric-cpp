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
#include <dot/mongo/mongo_db/bson/object_id.hpp>
#include <dot/system/type.hpp>
#include <dot/serialization/tree_writer_base.hpp>
#include <dot/serialization/serialize_attribute.hpp>

namespace dc
{
    class data_impl; using data = dot::ptr<data_impl>;
    class key_base_impl; using key_base = dot::ptr<key_base_impl>;

    /// Data objects must derive from this type.
    class DC_CLASS data_impl : public virtual dot::object_impl
    {
        typedef data_impl self;

    public: // METHODS

        /// Creates dictionary at current writer level.
        void serialize_to(dot::tree_writer_base writer);

    private:

        static void serialize_data(dot::tree_writer_base writer, dot::object data_obj)
        {
            ((data)data_obj)->serialize_to(writer);
        }

    public:

        virtual dot::type get_type() { return typeof(); }
        static dot::type typeof()
        {
            static dot::type result = []()-> dot::type
            {
                dot::type t = dot::make_type_builder<data_impl>("dc", "data", {dot::make_serialize_class_attribute(&data_impl::serialize_data)})
                    ->with_field("_t", static_cast<dot::string data_impl::*>(nullptr), { dot::make_deserialize_field_attribute(&dot::ignore_field_deserialization) })
                    ->with_method("serialize_to", &data_impl::serialize_to, {"writer"})
                    ->build();
                return t;
            }();
            return result;
        }

    protected:

        static const char separator = ';';
    };
}
