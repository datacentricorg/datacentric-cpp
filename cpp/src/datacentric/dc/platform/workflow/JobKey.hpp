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
#include <dc/platform/workflow/JobData.hpp>

namespace dc
{
    class JobKeyImpl; using JobKey = dot::Ptr<JobKeyImpl>;
    class JobTypeKeyImpl; using JobTypeKey = dot::Ptr<JobTypeKeyImpl>;
    class LinkedTypeKeyImpl; using LinkedTypeKey = dot::Ptr<LinkedTypeKeyImpl>;
    class JobDataImpl; using JobData = dot::Ptr<JobDataImpl>;


    JobKey new_JobKey();

    /// <summary>Job.</summary>
    class DC_CLASS JobKeyImpl : public KeyForImpl<JobKeyImpl, JobDataImpl>
    {
        typedef JobKeyImpl self;

    public: // PROPERTIES

        ObjectId ID;

        DOT_TYPE_BEGIN(".Analyst", "JobKey")
            ->WithProperty("_id", &self::ID)
            DOT_TYPE_BASE(KeyFor<JobKeyImpl, JobDataImpl>)
            DOT_TYPE_CTOR(new_JobKey)
        DOT_TYPE_END()

    };

    inline JobKey new_JobKey() { return new JobKeyImpl; }
}
