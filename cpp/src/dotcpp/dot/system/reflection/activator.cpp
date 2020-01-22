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
#include <dot/system/reflection/activator.hpp>
#include <dot/system/object_impl.hpp>
#include <dot/system/exception.hpp>
#include <dot/system/string.hpp>
#include <dot/system/collections/generic/list.hpp>
#include <dot/system/reflection/method_info.hpp>
#include <dot/system/reflection/constructor_info.hpp>

namespace dot
{
    Object Activator::create_instance(Type t)
    {
        return create_instance(t, nullptr);
    }

    Object Activator::create_instance(Type t, List<Object> params)
    {
        List<ConstructorInfo> ctors = t->get_constructors();

        // If no constructors
        if (ctors.is_empty() || ctors->count() == 0)
        {
            throw Exception(String::format("Type {0}::{1} does not have registered constructors", t->name_space(), t->name()));
        }

        // Search for best matched constructor
        ConstructorInfo best_ctor = nullptr;
        int params_count = 0;
        if (!params.is_empty())
        {
            params_count = params->count();
        }

        for (auto ctor : ctors)
        {
            auto ctor_params = ctor->get_parameters();
            bool matches = true;

            // Continue if different parameters count
            if (ctor_params->count() != params_count)
                continue;

            // Compare all parameters types
            for (int i = 0; i < params_count; ++i)
            {
                if ((String)ctor_params[i]->parameter_type()->name() != params[i]->get_type()->name())
                {
                    matches = false;
                    break;
                }
            }

            // Break if found
            if (matches)
            {
                best_ctor = ctor;
                break;
            }
        }

        // If not found
        if (best_ctor == nullptr)
        {
            throw Exception("No matching public constructor was found.");
        }

        return best_ctor->invoke(params);
    }

    Object Activator::create_instance(String assembly_name, String type_name)
    {
        return create_instance(TypeImpl::get_type_of(type_name), nullptr);
    }

    Object Activator::create_instance(String assembly_name, String type_name, List<Object> params)
    {
        return create_instance(TypeImpl::get_type_of(type_name), params);
    }
}
