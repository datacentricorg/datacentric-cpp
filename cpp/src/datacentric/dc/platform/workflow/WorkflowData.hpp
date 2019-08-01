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
#include <dc/platform/workflow/WorkflowKey.hpp>

namespace dc
{
    class WorkflowDataImpl; using WorkflowData = dot::ptr<WorkflowDataImpl>;
    class JobKeyImpl; using JobKey = dot::ptr<JobKeyImpl>;
    class WorkflowKeyImpl; using WorkflowKey = dot::ptr<WorkflowKeyImpl>;
    class PhaseKeyImpl; using PhaseKey = dot::ptr<PhaseKeyImpl>;

    inline WorkflowData new_WorkflowData();

    /// <summary>Workflow is a collection of jobs executed in the order of phases.</summary>
    class DC_CLASS WorkflowDataImpl : public RecordForImpl<WorkflowKeyImpl, WorkflowDataImpl>
    {
        typedef WorkflowDataImpl self;
        friend WorkflowData new_WorkflowData();

    public: // PROPERTIES

        /// <summary>Unique workflow identifier.</summary>
        dot::string WorkflowID;

        /// <summary>Unique workflow identifier.</summary>
        WorkflowKey Workflow;

        /// <summary>Jobs included in the workflow directly.</summary>
        dot::List<JobKey> Jobs;

        /// <summary>Jobs are taken from the specified workflows except those on the exclude list.</summary>
        dot::List<WorkflowKey> Workflows;

        /// <summary>Jobs on the exclude list are not not imported from the specified workflows.</summary>
        dot::List<JobKey> Exclude;

        /// <summary>Phases are executed in the order of this list. Jobs whose phase is not included in this list are skipped.</summary>
        dot::List<PhaseKey> Phases;

        /// <summary>LocalDate.</summary>
        dot::local_date LDate;

        /// <summary>LocalDate.</summary>
        dot::local_time LTime;

        /// <summary>LocalDate.</summary>
        dot::local_date_time LDateTime;

        dot::Nullable<double> dbl;

        WorkflowData data;

        DOT_TYPE_BEGIN(".Analyst", "WorkflowData")
            DOT_TYPE_PROP(WorkflowID)
            DOT_TYPE_PROP(Workflow)
            DOT_TYPE_PROP(Jobs)
            DOT_TYPE_PROP(Workflows)
            DOT_TYPE_PROP(Exclude)
            DOT_TYPE_PROP(Phases)
            DOT_TYPE_PROP(LDate)
            DOT_TYPE_PROP(LTime)
            DOT_TYPE_PROP(LDateTime)
            DOT_TYPE_PROP(dbl)
            DOT_TYPE_PROP(data)
            DOT_TYPE_CTOR(new_WorkflowData)
            DOT_TYPE_BASE(RecordFor<WorkflowKeyImpl, WorkflowDataImpl>)
        DOT_TYPE_END()

    protected: // CONSTRUCTORS

        WorkflowDataImpl() = default;
    };

    /// <summary>Create an empty instance.</summary>
    inline WorkflowData new_WorkflowData() { return new WorkflowDataImpl(); } // TODO Remove when derived classes are defined
}
