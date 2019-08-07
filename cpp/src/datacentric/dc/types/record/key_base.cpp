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
#include <dc/types/record/key_base.hpp>
#include <dot/system/reflection/activator.hpp>

namespace dc
{

    dot::string key_base_impl::to_string()
    {
        dot::list<dot::field_info> props = type()->get_fields();

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
                if (prop->field_type->name->ends_with("Key")) // TODO check using parents list
                {
                    dot::object empty_key = dot::activator::create_instance(prop->field_type);
                    ss << *empty_key->to_string();
                }
            }

        }

        return ss.str();
    }

    void key_base_impl::AssignString(std::stringstream & value)
    {
        dot::list<dot::field_info> props = type()->get_fields();

        for (dot::field_info prop : props)
        {
            if (prop->field_type->name->ends_with("Key")) // TODO check using parents list
            {
                key_base subKey = (key_base)dot::activator::create_instance(prop->field_type);
                subKey->AssignString(value);

                prop->set_value(this, subKey);
            }
            else
            {
                std::string token;
                std::getline(value, token, separator);

                if (token.empty())
                    continue;

                if (prop->field_type->equals(dot::typeof<int>()) || prop->field_type->equals(dot::typeof<dot::nullable<int>>()))
                {
                    prop->set_value(this, std::stoi(token));
                }
                else if (prop->field_type->equals(dot::typeof<dot::string>()))
                {
                    prop->set_value(this, dot::string(token));
                }
                else if (prop->field_type->equals(dot::typeof<dot::object_id>()))
                {
                    prop->set_value(this, dot::object_id(token));
                }
                else
                {
                    throw dot::exception("Unknown type in key_base.assign_string(...)");
                }

            }
        }

    }

    void key_base_impl::AssignString(dot::string value)
    {
        std::stringstream ss;
        ss.str(*value);

        AssignString(ss);
    }
}
