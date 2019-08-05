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
#include <dot/system/collections/generic/list.hpp>

namespace dc
{
    class DefaultClassMapSettingsImpl; using DefaultClassMapSettings = dot::ptr<DefaultClassMapSettingsImpl>;

    /// <summary>Settings for mapping namespaces, class names, and method names from .Runtime module.
    /// Use 'Settings.ClassMapSettings = newSettings' to use alternative mapping settings,
    /// or add individual settings using e.g. 'IgnoredNamespacePrefixes.Add(newPrefix)'.</summary>
    class DC_CLASS DefaultClassMapSettingsImpl : public IClassMapSettingsImpl
    {
        typedef DefaultClassMapSettingsImpl self;

        friend DefaultClassMapSettings new_DefaultClassMapSettings();

    public:
        /// <summary>Namespace prefixes including dot separators ignored by class mapping.</summary>
        virtual dot::list<dot::string> getIgnoredNamespacePrefixes()
        {
            return IgnoredNamespacePrefixes;
        }

        /// <summary>Namespace suffixes including dot separators ignored by class mapping.</summary>
        virtual dot::list<dot::string> getIgnoredNamespaceSuffixes()
        {
            return IgnoredNamespaceSuffixes;
        }

        /// <summary>Class name prefixes ignored by class mapping.</summary>
        virtual dot::list<dot::string> getIgnoredClassNamePrefixes()
        {
            return IgnoredClassNamePrefixes;
        }

        /// <summary>Class name prefixes ignored by class mapping.</summary>
        virtual dot::list<dot::string> getIgnoredClassNameSuffixes()
        {
            return IgnoredClassNameSuffixes;
        }

        /// <summary>Method name prefixes ignored by class mapping.</summary>
        virtual dot::list<dot::string> getIgnoredMethodNamePrefixes()
        {
            return IgnoredMethodNamePrefixes;
        }

        /// <summary>Method name prefixes ignored by class mapping.</summary>
        virtual dot::list<dot::string> getIgnoredMethodNameSuffixes()
        {
            return IgnoredMethodNameSuffixes;
        }

    private: // PROPERTIES

        /// <summary>Namespace prefixes including dot separators ignored by class mapping.</summary>
        dot::list<dot::string> IgnoredNamespacePrefixes;

        /// <summary>Namespace suffixes including dot separators ignored by class mapping.</summary>
        dot::list<dot::string> IgnoredNamespaceSuffixes;

        /// <summary>Class name prefixes ignored by class mapping.</summary>
        dot::list<dot::string> IgnoredClassNamePrefixes;

        /// <summary>Class name prefixes ignored by class mapping.</summary>
        dot::list<dot::string> IgnoredClassNameSuffixes;

        /// <summary>Method name prefixes ignored by class mapping.</summary>
        dot::list<dot::string> IgnoredMethodNamePrefixes;

        /// <summary>Method name prefixes ignored by class mapping.</summary>
        dot::list<dot::string> IgnoredMethodNameSuffixes;

    private: // CONSTRUCTORS

        DefaultClassMapSettingsImpl()
        {
            IgnoredNamespacePrefixes =  dot::make_list<dot::string>({ "." });
            IgnoredNamespaceSuffixes =  dot::make_list<dot::string>({ ".Api", ".Impl" });
            IgnoredClassNamePrefixes =  dot::make_list<dot::string>({ });
            IgnoredClassNameSuffixes =  dot::make_list<dot::string>({ "Data", "Key" });
            IgnoredMethodNamePrefixes = dot::make_list<dot::string>();
            IgnoredMethodNameSuffixes = dot::make_list<dot::string>();
        }
    };

    inline DefaultClassMapSettings new_DefaultClassMapSettings() { return new DefaultClassMapSettingsImpl(); }
}
