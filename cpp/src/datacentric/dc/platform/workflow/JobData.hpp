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
#include <dc/platform/workflow/JobKey.hpp>


namespace dc
{
    class JobDataImpl; using JobData = dot::Ptr<JobDataImpl>;
    class JobQueueKeyImpl; using JobQueueKey = dot::Ptr<JobQueueKeyImpl>;
    class JobTypeKeyImpl; using JobTypeKey = dot::Ptr<JobTypeKeyImpl>;
    class LinkedTypeKeyImpl; using LinkedTypeKey = dot::Ptr<LinkedTypeKeyImpl>;
    class JobKeyImpl; using JobKey = dot::Ptr<JobKeyImpl>;

    JobData new_JobData();

    /// <summary>Job.</summary>
    class DC_CLASS JobDataImpl : public RecordForImpl<JobKeyImpl, JobDataImpl>
    {
        typedef JobDataImpl self;

    public: // PROPERTIES

        /// <summary>Queue to which the job is submitted.</summary>
        JobQueueKey Queue;

        //DOT_IMPL_PROP(ObjectId, ID, { return id_; }, { id_ = value; });

        DOT_TYPE_BEGIN(".Analyst", "JobData")
            DOT_TYPE_PROP(Queue)
            DOT_TYPE_BASE(RecordFor<JobKeyImpl, JobDataImpl>)
            DOT_TYPE_CTOR(new_JobData)
        DOT_TYPE_END()
    };

    inline JobData new_JobData() { return new JobDataImpl; }
}
