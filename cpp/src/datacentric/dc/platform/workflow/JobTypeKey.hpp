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
#include <dc/platform/workflow/JobTypeData.hpp>

namespace dc
{
    class JobTypeKeyImpl; using JobTypeKey = dot::ptr<JobTypeKeyImpl>;
    class JobTypeDataImpl; using JobTypeData = dot::ptr<JobTypeDataImpl>;

    JobTypeKey new_JobTypeKey();

    /// <summary>Job type.</summary>
    class DC_CLASS JobTypeKeyImpl : public KeyForImpl<JobTypeKeyImpl, JobTypeDataImpl>
    {
        typedef JobTypeKeyImpl self;

    public: // PROPERTIES

        /// <summary>Unique job type identifier.</summary>
        //String JobTypeID;

        dot::string JobTypeID;

        DOT_TYPE_BEGIN(".Analyst", "JobTypeKey")
            DOT_TYPE_PROP(JobTypeID)
            DOT_TYPE_CTOR(new_JobTypeKey)
        DOT_TYPE_END()
    };

    inline JobTypeKey new_JobTypeKey() { return new JobTypeKeyImpl; }
}
