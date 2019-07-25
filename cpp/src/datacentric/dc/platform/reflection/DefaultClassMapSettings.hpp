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
#include <dc/platform/reflection/IClassMapSettings.hpp>
#include <dot/system/collections/generic/List.hpp>

namespace dc
{
    class DefaultClassMapSettingsImpl; using DefaultClassMapSettings = dot::Ptr<DefaultClassMapSettingsImpl>;

    /// <summary>Settings for mapping namespaces, class names, and method names from .Runtime module.
    /// Use 'Settings.ClassMapSettings = newSettings' to use alternative mapping settings,
    /// or add individual settings using e.g. 'IgnoredNamespacePrefixes.Add(newPrefix)'.</summary>
    class DC_CLASS DefaultClassMapSettingsImpl : public IClassMapSettingsImpl
    {
        typedef DefaultClassMapSettingsImpl self;

        friend DefaultClassMapSettings new_DefaultClassMapSettings();

    public: // PROPERTIES

        /// <summary>Namespace prefixes including dot separators ignored by class mapping.</summary>
        dot::ICollection<dot::String> IgnoredNamespacePrefixes;

        /// <summary>Namespace suffixes including dot separators ignored by class mapping.</summary>
        dot::ICollection<dot::String> IgnoredNamespaceSuffixes;

        /// <summary>Class name prefixes ignored by class mapping.</summary>
        dot::ICollection<dot::String> IgnoredClassNamePrefixes;

        /// <summary>Class name prefixes ignored by class mapping.</summary>
        dot::ICollection<dot::String> IgnoredClassNameSuffixes;

        /// <summary>Method name prefixes ignored by class mapping.</summary>
        dot::ICollection<dot::String> IgnoredMethodNamePrefixes;

        /// <summary>Method name prefixes ignored by class mapping.</summary>
        dot::ICollection<dot::String> IgnoredMethodNameSuffixes;

    private: // CONSTRUCTORS

        DefaultClassMapSettingsImpl()
        {
            IgnoredNamespacePrefixes = dot::new_List<dot::String>({ "." });
            IgnoredNamespaceSuffixes = dot::new_List<dot::String>({ ".Api", ".Impl" });
            IgnoredClassNamePrefixes = dot::new_List<dot::String>({ });
            IgnoredClassNameSuffixes =  dot::new_List<dot::String>({ "Data", "Key" });
            IgnoredMethodNamePrefixes = dot::new_List<dot::String>();
            IgnoredMethodNameSuffixes = dot::new_List<dot::String>();
        }
    };

    inline DefaultClassMapSettings new_DefaultClassMapSettings() { return new DefaultClassMapSettingsImpl(); }
}
