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
#include <dc/platform/reflection/ClassInfo.hpp>
#include <dc/platform/settings/Settings.hpp>

namespace dc
{
    dot::String ClassInfoImpl::ToString()
    {
        return MappedFullName;
    }

    ClassInfo ClassInfoImpl::GetOrCreate(dot::Object value)
    {
        return GetOrCreate(value->GetType());
    }

    ClassInfo ClassInfoImpl::GetOrCreate(Type_ type)
    {
        dot::Dictionary<Type_, ClassInfo> dict_ = ClassInfoImpl::GetTypeDict();
        ClassInfo result;

        // Check if a cached instance exists in dictionary
        if (dict_->TryGetValue(type, result))
        {
            // Return if found
            return result;
        }
        else
        {
            // Otherwise create and add to dictionary
            result = new ClassInfoImpl(type);
            dict_->Add(type, result);
            return result;
        }
    }

    ClassInfoImpl::ClassInfoImpl(Type_ type)
    {
        // Set type, raw full name, class name, and namespace
        Type = type;
        RawFullName = type->getFullName();
        RawClassName = type->Name;
        RawNamespace = type->Namespace;

        // Remove ignored class name prefix
        MappedClassName = RawClassName;
        for (dot::String ignoredTypeNamePrefix : Settings::Default->getClassMap()->getIgnoredClassNamePrefixes())
        {
            if (MappedClassName->StartsWith(ignoredTypeNamePrefix))
            {
                MappedClassName = MappedClassName->Remove(0, ignoredTypeNamePrefix->getLength());

                // Break to prevent more than one prefix removed
                break;
            }
        }

        // Remove ignored class name suffix
        for (dot::String ignoredTypeNameSuffix : Settings::Default->getClassMap()->getIgnoredClassNameSuffixes())
        {
            if (MappedClassName->EndsWith(ignoredTypeNameSuffix))
            {
                MappedClassName = MappedClassName->SubString(0, MappedClassName->getLength() - ignoredTypeNameSuffix->getLength());

                // Break to prevent more than one prefix removed
                break;
            }
        }

        // Remove ignored namespace prefix
        MappedNamespace = RawNamespace;
        for (dot::String ignoredModuleNamePrefix : Settings::Default->getClassMap()->getIgnoredNamespacePrefixes())
        {
            if (MappedNamespace->StartsWith(ignoredModuleNamePrefix))
            {
                MappedNamespace = MappedNamespace->Remove(0, ignoredModuleNamePrefix->getLength());

                // Break to prevent more than one prefix removed
                break;
            }
        }

        // Remove ignored namespace suffix
        for (dot::String ignoredModuleNameSuffix : Settings::Default->getClassMap()->getIgnoredNamespaceSuffixes())
        {
            if (MappedNamespace->EndsWith(ignoredModuleNameSuffix))
            {
                MappedNamespace = MappedNamespace->SubString(0, MappedNamespace->getLength() - ignoredModuleNameSuffix->getLength());

                // Break to prevent more than one prefix removed
                break;
            }
        }

        // Create mapped full name by combining mapped namespace and mapped class name
        MappedFullName = MappedNamespace + dot::String(".") + MappedClassName;
    }

    dot::Dictionary<dot::Type, ClassInfo>& ClassInfoImpl::GetTypeDict()
    {
        static dot::Dictionary<Type_, ClassInfo> dict_ = dot::new_Dictionary<Type_, ClassInfo>();
        return dict_;
    }
}
