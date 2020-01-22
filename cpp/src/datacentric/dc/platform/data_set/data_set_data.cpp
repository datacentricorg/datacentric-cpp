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

#include <dc/precompiled.hpp>
#include <dc/implement.hpp>
#include <dc/platform/data_set/data_set_data.hpp>
#include <dc/platform/context/context_base.hpp>

namespace dc
{
    void data_set_data_impl::init(context_base context)
    {
        typed_record_impl<data_set_key_impl, data_set_data_impl>::init(context);

        if (dot::String::is_null_or_empty(data_set_id)) throw dot::Exception("data_set_id has not been set.");

        if (!dot::List<temporal_id>(parents).is_empty())
        for (temporal_id parent : parents)
        {
            if (id <= parent)
            {
                if (id == parent)
                {
                    throw dot::Exception(dot::String::format(
                        "Dataset {0} has a parent with the same temporal_id={1} "
                        "as its own temporal_id. Each temporal_id must be unique.", data_set_id, parent.to_string()));
                }
                else
                {
                    throw dot::Exception(dot::String::format(
                        "Dataset {0} has a parent whose temporal_id={1} is greater "
                        "than its own temporal_id={2}. The temporal_id of each parent must be strictly "
                        "less than the temporal_id of the dataset itself.", data_set_id, parent.to_string(), temporal_id(id).to_string()));
                }
            }
        }
    }
}
