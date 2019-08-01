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
#include <dc/settings/SystemSettingsData.hpp>

namespace dc
{
    class CustomSystemSettingsDataImpl; using CustomSystemSettingsData = dot::ptr<CustomSystemSettingsDataImpl>;

    /// <summary>Custom system settings data.</summary>
    class DC_CLASS CustomSystemSettingsDataImpl : public SystemSettingsDataImpl
    {
    public: // PROPERTIES
    };
}
