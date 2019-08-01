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
#include <dc/types/record/RecordFor.hpp>
#include <dc/platform/workflow/LinkedTypeKey.hpp>

namespace dc
{
    class LinkedTypeDataImpl; using LinkedTypeData = dot::ptr<LinkedTypeDataImpl>;
    class LinkedTypeKeyImpl; using LinkedTypeKey = dot::ptr<LinkedTypeKeyImpl>;

    /// <summary>Linked record type.</summary>
    class DC_CLASS LinkedTypeDataImpl : public RecordForImpl<LinkedTypeKeyImpl, LinkedTypeDataImpl>
    {
    public: // PROPERTIES

        /// <summary>Unique record type identifier is the default table or folder name in storage.</summary>
        dot::string TypeID;
    };
}
