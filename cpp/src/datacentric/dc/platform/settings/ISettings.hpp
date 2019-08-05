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
#include <dot/system/object.hpp>

namespace dc
{
    class ISettingsImpl; using ISettings = dot::ptr<ISettingsImpl>;

    /// <summary>Provides access component settings via the root settings object.</summary>
    class DC_CLASS ISettingsImpl : public virtual dot::object_impl
    {
        typedef ISettingsImpl self;

    public: // PROPERTIES

        /// <summary>Class map settings.</summary>
        virtual IClassMapSettings getClassMap() = 0;

        /// <summary>activator settings.</summary>
        //DOT_DECL_PROP(IactivatorSettings, activator)

        /// <summary>Locale settings.</summary>
        //DOT_DECL_PROP(ILocaleSettings, Locale)
    };
}
