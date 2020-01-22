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
#include <dc/types/record/key.hpp>
#include <dot/system/reflection/activator.hpp>

namespace dc
{
    dot::String key_impl::to_string()
    {
        dot::List<dot::FieldInfo> props = get_type()->get_fields();

        std::stringstream ss;

        for (int i = 0; i < props->count(); ++i)
        {
            dot::FieldInfo prop = props[i];

            dot::Object value = prop->get_value(this);

            if (i) ss << separator;

            if (!value.is_empty())
            {
                ss << *value->to_string();
            }
            else
            {
                if (prop->field_type()->is_subclass_of(dot::typeof<key>()))
                {
                    dot::Object empty_key = dot::Activator::create_instance(prop->field_type());
                    ss << *empty_key->to_string();
                }
            }

        }

        return ss.str();
    }

    void key_impl::populate_from(std::stringstream & value)
    {
        dot::List<dot::FieldInfo> props = get_type()->get_fields();

        for (dot::FieldInfo prop : props)
        {
            if (prop->field_type()->is_subclass_of(dot::typeof<key>()))
            {
                key sub_key = (key)dot::Activator::create_instance(prop->field_type());
                sub_key->populate_from(value);

                prop->set_value(this, sub_key);
            }
            else
            {
                std::string token;
                std::getline(value, token, separator);

                if (token.empty())
                    continue;

                if (prop->field_type()->equals(dot::typeof<int>()) || prop->field_type()->equals(dot::typeof<dot::Nullable<int>>()))
                {
                    prop->set_value(this, std::stoi(token));
                }
                else if (prop->field_type()->equals(dot::typeof<dot::String>()))
                {
                    prop->set_value(this, dot::String(token));
                }
                else if (prop->field_type()->equals(dot::typeof<temporal_id>()))
                {
                    prop->set_value(this, temporal_id(token));
                }
                else
                {
                    throw dot::Exception("Unknown type in key.assign_string(...)");
                }

            }
        }

    }

    dot::Object key_impl::deserialize(dot::Object value, dot::Type type)
    {
        dot::Type value_type = value->get_type();

        if (value.is<key>())
        {
            return value;
        }
        if (value_type->equals(dot::typeof<dot::String>()))
        {
            key key = dot::Activator::create_instance(type);
            key->populate_from((dot::String)value);
            return key;
        }

        throw dot::Exception(dot::String::format("Couldn't construct {0}  from {1}", type->name(), value_type->name()));
        return dot::Object();

    }

    void key_impl::serialize(dot::tree_writer_base writer, dot::Object obj)
    {
        writer->write_start_value();
        writer->write_value(((key)obj)->get_value());
        writer->write_end_value();
    }

    void key_impl::populate_from(dot::String value)
    {
        std::stringstream ss;
        ss.str(*value);

        populate_from(ss);
    }
}
