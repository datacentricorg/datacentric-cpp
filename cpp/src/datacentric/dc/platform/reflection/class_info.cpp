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
#include <dc/platform/reflection/class_info.hpp>
#include <dc/platform/settings/class_map_settings.hpp>

namespace dc
{
    dot::string class_info_impl::to_string()
    {
        return mapped_full_name;
    }

    class_info class_info_impl::get_or_create(dot::object value)
    {
        return get_or_create(value->get_type());
    }

    class_info class_info_impl::get_or_create(dot::type type)
    {
        dot::dictionary<dot::type, class_info> dict_ = class_info_impl::get_type_dict();
        class_info result;

        // Check if a cached instance exists in dictionary
        if (dict_->try_get_value(type, result))
        {
            // Return if found
            return result;
        }
        else
        {
            // Otherwise create and add to dictionary
            result = new class_info_impl(type);
            dict_->add(type, result);
            return result;
        }
    }

    class_info_impl::class_info_impl(dot::type type)
    {
        // Set type, raw full name, class name, and namespace
        this->type = type;
        raw_full_name = type->full_name();
        raw_class_name = type->name;
        raw_namespace = type->name_space;

        // Remove ignored class name prefix
        mapped_class_name = raw_class_name;
        for (dot::string ignored_type_name_prefix : class_map_settings::ignored_class_name_prefixes())
        {
            if (mapped_class_name->starts_with(ignored_type_name_prefix))
            {
                mapped_class_name = mapped_class_name->remove(0, ignored_type_name_prefix->length());

                // Break to prevent more than one prefix removed
                break;
            }
        }

        // Remove ignored class name suffix
        for (dot::string ignored_type_name_suffix : class_map_settings::ignored_class_name_suffixes())
        {
            if (mapped_class_name->ends_with(ignored_type_name_suffix))
            {
                mapped_class_name = mapped_class_name->substring(0, mapped_class_name->length() - ignored_type_name_suffix->length());

                // Break to prevent more than one prefix removed
                break;
            }
        }

        // Remove ignored namespace prefix
        mapped_namespace = raw_namespace;
        for (dot::string ignored_module_name_prefix : class_map_settings::ignored_namespace_prefixes())
        {
            if (mapped_namespace->starts_with(ignored_module_name_prefix))
            {
                mapped_namespace = mapped_namespace->remove(0, ignored_module_name_prefix->length());

                // Break to prevent more than one prefix removed
                break;
            }
        }

        // Remove ignored namespace suffix
        for (dot::string ignored_module_name_suffix : class_map_settings::ignored_namespace_suffixes())
        {
            if (mapped_namespace->ends_with(ignored_module_name_suffix))
            {
                mapped_namespace = mapped_namespace->substring(0, mapped_namespace->length() - ignored_module_name_suffix->length());

                // Break to prevent more than one prefix removed
                break;
            }
        }

        // Create mapped full name by combining mapped namespace and mapped class name
        mapped_full_name = mapped_namespace + dot::string(".") + mapped_class_name;
    }

    dot::dictionary<dot::type, class_info>& class_info_impl::get_type_dict()
    {
        static dot::dictionary<dot::type, class_info> dict_ = dot::make_dictionary<dot::type, class_info>();
        return dict_;
    }
}
