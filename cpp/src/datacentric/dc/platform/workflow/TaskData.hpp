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
#include <dc/platform/workflow/TaskKey.hpp>

namespace dc
{
    class TaskDataImpl; using TaskData = dot::Ptr<TaskDataImpl>;
    class TaskKeyImpl; using TaskKey = dot::Ptr<TaskKeyImpl>;
    class WorkflowKeyImpl; using WorkflowKey = dot::Ptr<WorkflowKeyImpl>;
    class JobKeyImpl; using JobKey = dot::Ptr<JobKeyImpl>;
    class PhaseKeyImpl; using PhaseKey = dot::Ptr<PhaseKeyImpl>;

    /// <summary>Task record. Tasks are created by jobs and executed in parallel within each phase.</summary>
    class DC_CLASS TaskDataImpl : public RecordForImpl<TaskKeyImpl, TaskDataImpl>
    {
    public: // PROPERTIES

        /// <summary>Workflow under which the task is running.</summary>
        WorkflowKey Workflow;

        /// <summary>Job which created the task.</summary>
        JobKey Job;

        /// <summary>Phase to which the task is assigned.</summary>
        PhaseKey Phase;
    };
}
