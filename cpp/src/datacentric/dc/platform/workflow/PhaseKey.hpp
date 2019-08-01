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
#include <dc/platform/workflow/PhaseData.hpp>

namespace dc
{
    class PhaseKeyImpl; using PhaseKey = dot::ptr<PhaseKeyImpl>;
    class PhaseDataImpl; using PhaseData = dot::ptr<PhaseDataImpl>;

    /// <summary>Workflow phase record. Jobs are executed in the order of phases and in parallel within each phase.</summary>
    class DC_CLASS PhaseKeyImpl : public KeyForImpl<PhaseKeyImpl, PhaseDataImpl>
    {
        typedef PhaseKeyImpl self;

    public: // PROPERTIES

        /// <summary>Unique phase identifier.</summary>
        dot::string PhaseID;

        DOT_TYPE_BEGIN(".Runtime.Main", "PhaseKey")
            DOT_TYPE_BASE(KeyFor<PhaseKeyImpl, PhaseDataImpl>)
            DOT_TYPE_PROP(PhaseID)
        DOT_TYPE_END()

    };
}
