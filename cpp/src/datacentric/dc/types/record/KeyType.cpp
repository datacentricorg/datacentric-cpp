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

#include <dc/implement.hpp>
#include <dc/types/record/KeyType.hpp>
#include <dot/system/reflection/Activator.hpp>

namespace dc
{

    dot::string KeyTypeImpl::ToString()
    {
        dot::Array1D<dot::field_info> props = GetType()->GetProperties();

        std::stringstream ss;

        for (int i = 0; i < props->getCount(); ++i)
        {
            dot::field_info prop = props[i];

            dot::object value = prop->GetValue(this);

            if (i) ss << separator;

            if (!value.IsEmpty())
            {
                ss << *value->ToString();
            }
            else
            {
                if (prop->PropertyType->Name->EndsWith("Key")) // TODO check using parents list
                {
                    dot::object emptyKey = dot::Activator::CreateInstance(prop->PropertyType);
                    ss << *emptyKey->ToString();
                }
            }

        }

        return ss.str();
    }

    void KeyTypeImpl::AssignString(std::stringstream & value)
    {
        dot::Array1D<dot::field_info> props = GetType()->GetProperties();

        for (dot::field_info prop : props)
        {
            if (prop->PropertyType->Name->EndsWith("Key")) // TODO check using parents list
            {
                KeyType subKey = (KeyType)dot::Activator::CreateInstance(prop->PropertyType);
                subKey->AssignString(value);

                prop->SetValue(this, subKey);
            }
            else
            {
                std::string token;
                std::getline(value, token, separator);

                if (token.empty())
                    continue;

                if (prop->PropertyType->Equals(dot::typeof<int>()) || prop->PropertyType->Equals(dot::typeof<dot::Nullable<int>>()))
                {
                    prop->SetValue(this, std::stoi(token));
                }
                else if (prop->PropertyType->Equals(dot::typeof<dot::string>()))
                {
                    prop->SetValue(this, dot::string(token));
                }
                else if (prop->PropertyType->Equals(dot::typeof<ObjectId>()))
                {
                    prop->SetValue(this, ObjectId(token));
                }
                else
                {
                    throw dot::exception("Unknown type in KeyType.AssignString");
                }

            }
        }

    }

    void KeyTypeImpl::AssignString(dot::string value)
    {
        std::stringstream ss;
        ss.str(*value);

        AssignString(ss);
    }
}
