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
#include <dc/platform/settings/ISettings.hpp>
#include <dc/platform/reflection/DefaultClassMapSettings.hpp>

namespace dc
{
    class DefaultSettingsImpl; using DefaultSettings = dot::ptr<DefaultSettingsImpl>;

    /// <summary>Create temporary context with logging to the current output.</summary>
    class DC_CLASS DefaultSettingsImpl : public ISettingsImpl
    {
        typedef DefaultSettingsImpl self;

        friend DefaultSettings new_DefaultSettings();

    public: // PROPERTIES

        /// <summary>Class map settings.</summary>
        virtual IClassMapSettings getClassMap()
        {
            return ClassMap;
        }

        /// <summary>activator settings.</summary>
        //IactivatorSettings activator;

        /// <summary>Locale settings.</summary>
        //ILocaleSettings Locale;
    private:
        IClassMapSettings ClassMap;

    private: // CONSTRUCTORS

        DefaultSettingsImpl()
        {
            ClassMap = new_DefaultClassMapSettings();
        }
    };

    inline DefaultSettings new_DefaultSettings() { return new DefaultSettingsImpl(); }
}
