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
#include <dc/platform/reflection/ClassInfo.hpp>
#include <dc/platform/settings/class_map_settings.hpp>

namespace dc
{
    dot::string ClassInfoImpl::to_string()
    {
        return MappedFullName;
    }

    ClassInfo ClassInfoImpl::GetOrCreate(dot::object value)
    {
        return GetOrCreate(value->type());
    }

    ClassInfo ClassInfoImpl::GetOrCreate(Type_ type)
    {
        dot::dictionary<Type_, ClassInfo> dict_ = ClassInfoImpl::GetTypeDict();
        ClassInfo result;

        // Check if a cached instance exists in dictionary
        if (dict_->try_get_value(type, result))
        {
            // Return if found
            return result;
        }
        else
        {
            // Otherwise create and add to dictionary
            result = new ClassInfoImpl(type);
            dict_->add(type, result);
            return result;
        }
    }

    ClassInfoImpl::ClassInfoImpl(Type_ type)
    {
        // Set type, raw full name, class name, and namespace
        Type = type;
        RawFullName = type->full_name();
        RawClassName = type->name;
        RawNamespace = type->name_space;

        // Remove ignored class name prefix
        MappedClassName = RawClassName;
        for (dot::string ignoredTypeNamePrefix : class_map_settings::ignored_class_name_prefixes())
        {
            if (MappedClassName->starts_with(ignoredTypeNamePrefix))
            {
                MappedClassName = MappedClassName->remove(0, ignoredTypeNamePrefix->length());

                // Break to prevent more than one prefix removed
                break;
            }
        }

        // Remove ignored class name suffix
        for (dot::string ignoredTypeNameSuffix : class_map_settings::ignored_class_name_suffixes())
        {
            if (MappedClassName->ends_with(ignoredTypeNameSuffix))
            {
                MappedClassName = MappedClassName->substring(0, MappedClassName->length() - ignoredTypeNameSuffix->length());

                // Break to prevent more than one prefix removed
                break;
            }
        }

        // Remove ignored namespace prefix
        MappedNamespace = RawNamespace;
        for (dot::string ignoredModuleNamePrefix : class_map_settings::ignored_namespace_prefixes())
        {
            if (MappedNamespace->starts_with(ignoredModuleNamePrefix))
            {
                MappedNamespace = MappedNamespace->remove(0, ignoredModuleNamePrefix->length());

                // Break to prevent more than one prefix removed
                break;
            }
        }

        // Remove ignored namespace suffix
        for (dot::string ignoredModuleNameSuffix : class_map_settings::ignored_namespace_suffixes())
        {
            if (MappedNamespace->ends_with(ignoredModuleNameSuffix))
            {
                MappedNamespace = MappedNamespace->substring(0, MappedNamespace->length() - ignoredModuleNameSuffix->length());

                // Break to prevent more than one prefix removed
                break;
            }
        }

        // Create mapped full name by combining mapped namespace and mapped class name
        MappedFullName = MappedNamespace + dot::string(".") + MappedClassName;
    }

    dot::dictionary<dot::type_t, ClassInfo>& ClassInfoImpl::GetTypeDict()
    {
        static dot::dictionary<Type_, ClassInfo> dict_ = dot::make_dictionary<Type_, ClassInfo>();
        return dict_;
    }
}
