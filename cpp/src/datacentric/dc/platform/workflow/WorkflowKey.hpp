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
#include <dc/platform/workflow/WorkflowData.hpp>

namespace dc
{
    class WorkflowKeyImpl; using WorkflowKey = dot::Ptr<WorkflowKeyImpl>;
    class WorkflowDataImpl; using WorkflowData = dot::Ptr<WorkflowDataImpl>;

    inline WorkflowKey new_WorkflowKey();

    /// <summary>Workflow is a collection of jobs executed in the order of phases.</summary>
    class DC_CLASS WorkflowKeyImpl : public KeyForImpl<WorkflowKeyImpl, WorkflowDataImpl>
    {
  		typedef WorkflowKeyImpl self;
        friend WorkflowKey new_WorkflowKey();

    public: // PROPERTIES

        DOT_TYPE_BEGIN(".Analyst", "WorkflowKey")
            DOT_TYPE_PROP(WorkflowID)
            DOT_TYPE_CTOR(new_WorkflowKey)
        DOT_TYPE_END()

        /// <summary>Unique workflow identifier.</summary>
        DOT_AUTO_PROP(dot::String, WorkflowID)
    };

    inline WorkflowKey new_WorkflowKey() { return new WorkflowKeyImpl; }


}


