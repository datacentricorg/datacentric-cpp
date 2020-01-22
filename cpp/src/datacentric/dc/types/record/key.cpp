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
    dot::string key_impl::to_string()
    {
        dot::list<dot::field_info> props = get_type()->get_fields();

        std::stringstream ss;

        for (int i = 0; i < props->count(); ++i)
        {
            dot::field_info prop = props[i];

            dot::object value = prop->get_value(this);

            if (i) ss << separator;

            if (!value.is_empty())
            {
                ss << *value->to_string();
            }
            else
            {
                if (prop->field_type()->is_subclass_of(dot::typeof<key>()))
                {
                    dot::object empty_key = dot::activator::create_instance(prop->field_type());
                    ss << *empty_key->to_string();
                }
            }

        }

        return ss.str();
    }

    void key_impl::populate_from(std::stringstream & value)
    {
        dot::list<dot::field_info> props = get_type()->get_fields();

        for (dot::field_info prop : props)
        {
            if (prop->field_type()->is_subclass_of(dot::typeof<key>()))
            {
                key sub_key = (key)dot::activator::create_instance(prop->field_type());
                sub_key->populate_from(value);

                prop->set_value(this, sub_key);
            }
            else
            {
                std::string token;
                std::getline(value, token, separator);

                if (token.empty())
                    continue;

                if (prop->field_type()->equals(dot::typeof<int>()) || prop->field_type()->equals(dot::typeof<dot::nullable<int>>()))
                {
                    prop->set_value(this, std::stoi(token));
                }
                else if (prop->field_type()->equals(dot::typeof<dot::string>()))
                {
                    prop->set_value(this, dot::string(token));
                }
                else if (prop->field_type()->equals(dot::typeof<dot::object_id>()))
                {
                    prop->set_value(this, dot::object_id(token));
                }
                else
                {
                    throw dot::exception("Unknown type in key.assign_string(...)");
                }

            }
        }

    }

    dot::object key_impl::deserialize(dot::object value, dot::type type)
    {
        dot::type value_type = value->get_type();

        if (value.is<key>())
        {
            return value;
        }
        if (value_type->equals(dot::typeof<dot::string>()))
        {
            key key = dot::activator::create_instance(type);
            key->populate_from((dot::string)value);
            return key;
        }

        throw dot::exception(dot::string::format("Couldn't construct {0}  from {1}", type->name(), value_type->name()));
        return dot::object();

    }

    void key_impl::serialize(dot::tree_writer_base writer, dot::object obj)
    {
        writer->write_start_value();
        writer->write_value(((key)obj)->get_value());
        writer->write_end_value();
    }

    void key_impl::populate_from(dot::string value)
    {
        std::stringstream ss;
        ss.str(*value);

        populate_from(ss);
    }
}
