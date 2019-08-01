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
#include <dc/platform/workflow/JobQueueKey.hpp>

namespace dc
{
    class JobQueueDataImpl; using JobQueueData = dot::ptr<JobQueueDataImpl>;
    class JobQueueKeyImpl; using JobQueueKey = dot::ptr<JobQueueKeyImpl>;
    class JobTypeKeyImpl; using JobTypeKey = dot::ptr<JobTypeKeyImpl>;
    class LinkedTypeKeyImpl; using LinkedTypeKey = dot::ptr<LinkedTypeKeyImpl>;
    class JobKeyImpl; using JobKey = dot::ptr<JobKeyImpl>;

    JobQueueData new_JobQueueData();

    /// <summary>Job.</summary>
    class DC_CLASS JobQueueDataImpl : public RecordForImpl<JobQueueKeyImpl, JobQueueDataImpl>
    {
        typedef JobQueueDataImpl self;

    public: // PROPERTIES

        DOT_TYPE_BEGIN(".Analyst", "JobQueueData")
            DOT_TYPE_BASE(RecordFor<JobQueueKeyImpl, JobQueueDataImpl>)
            DOT_TYPE_CTOR(new_JobQueueData)
        DOT_TYPE_END()
    };

    inline JobQueueData new_JobQueueData() { return new JobQueueDataImpl; }
}