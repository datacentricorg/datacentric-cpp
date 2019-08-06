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
#include <dc/types/record/record_base.hpp>
#include <dc/settings/SettingsKey.hpp>

namespace dc
{
    class SettingsDataImpl; using SettingsData = dot::ptr<SettingsDataImpl>;
    class SettingsKeyImpl; using SettingsKey = dot::ptr<SettingsKeyImpl>;

    /// <summary>Settings records must derive from this type.</summary>
    class DC_CLASS SettingsDataImpl : public record_impl<SettingsKeyImpl, SettingsDataImpl>
    {
    public: // PROPERTIES

        /// <summary>Unique settings identifier.</summary>
        dot::string SettingsID;
    };
}
