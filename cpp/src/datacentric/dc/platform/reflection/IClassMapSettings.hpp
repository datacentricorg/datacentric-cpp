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
#include <dot/system/Object.hpp>
#include <dot/system/String.hpp>
#include <dot/system/collections/generic/ICollection.hpp>

namespace dc
{
    class IClassMapSettingsImpl; using IClassMapSettings = dot::Ptr<IClassMapSettingsImpl>;

    /// <summary>Provides settings for mapping namespaces, class names, and method names to storage and CLI.</summary>
    class DC_CLASS IClassMapSettingsImpl : public virtual dot::ObjectImpl
    {
        typedef IClassMapSettingsImpl self;

    public: // PROPERTIES

        /// <summary>Provides access to other settings of the same root settings object.</summary>
        //DOT_DECL_GET(ISettings, Settings) // pointer loop

        /// <summary>Namespace prefixes including dot separators ignored by class mapping.</summary>
        DOT_DECL_GET(dot::ICollection<dot::String>, IgnoredNamespacePrefixes)

        /// <summary>Namespace suffixes including dot separators ignored by class mapping.</summary>
        DOT_DECL_GET(dot::ICollection<dot::String>, IgnoredNamespaceSuffixes)

        /// <summary>Class name prefixes ignored by class mapping.</summary>
        DOT_DECL_GET(dot::ICollection<dot::String>, IgnoredClassNamePrefixes)

        /// <summary>Class name prefixes ignored by class mapping.</summary>
        DOT_DECL_GET(dot::ICollection<dot::String>, IgnoredClassNameSuffixes)

        /// <summary>Method name prefixes ignored by class mapping.</summary>
        DOT_DECL_GET(dot::ICollection<dot::String>, IgnoredMethodNamePrefixes)

        /// <summary>Method name prefixes ignored by class mapping.</summary>
        DOT_DECL_GET(dot::ICollection<dot::String>, IgnoredMethodNameSuffixes)
    };
}
