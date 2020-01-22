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
#include <dc/types/record/record.hpp>
#include <dc/platform/context/context_base.hpp>

namespace dc
{
    void RecordImpl::init(ContextBase context)
    {
        // The line below is an example of calling init(...) method for base class.
        // It should be uncommented for all classes derived from this class.
        //
        // base.init(context);

        this->context = context.to_weak();
        if (context == nullptr) throw dot::Exception(
            dot::String::format("Null context is passed to the init(...) method for {0}.", get_type()->name()));
    }

    void RecordImpl::serialize_key(dot::tree_writer_base writer, dot::Object obj)
    {
        writer->write_value_element("_key", ((Record)obj)->get_key());
    }

    dot::Type RecordImpl::typeof()
    {
        static dot::Type result = []()-> dot::Type
        {
            dot::Type t = dot::make_type_builder<RecordImpl>("dc", "Record", { dot::make_bson_root_class_attribute() })
                ->with_field("_id", &self::id)
                ->with_field("_dataset", &self::data_set)
                ->with_field("_key", static_cast<dot::String RecordImpl::*>(nullptr), { dot::make_deserialize_field_attribute(&dot::ignore_field_deserialization)
                    , dot::make_serialize_field_attribute(&RecordImpl::serialize_key) })
                ->template with_base<Data>()
                ->build();
            return t;
        }();
        return result;
    }

    dot::Type RecordImpl::get_type()
    {
        return typeof();
    }
}
