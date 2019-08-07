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
#include <dc/platform/dataset/data_set_data.hpp>
#include <dc/platform/context/context_base.hpp>

namespace dc
{
    void data_set_data_impl::Init(context_base context)
    {
        record_impl<data_set_key_impl, data_set_data_impl>::Init(context);

        if (dot::string::is_null_or_empty(data_set_id)) throw dot::exception("data_set_id has not been set.");

        if (!dot::list<dot::object_id>(Parents).is_empty())
        for (dot::object_id parent : Parents)
        {
            if (ID <= parent)
            {
                if (ID == parent)
                {
                    throw dot::exception(dot::string::format(
                        "Dataset {0} has a parent with the same dot::object_id={1} "
                        "as its own dot::object_id. Each dot::object_id must be unique.", data_set_id, parent.to_string()));
                }
                else
                {
                    throw dot::exception(dot::string::format(
                        "Dataset {0} has a parent whose dot::object_id={1} is greater "
                        "than its own dot::object_id={2}. The dot::object_id of each parent must be strictly "
                        "less than the dot::object_id of the dataset itself.", data_set_id, parent.to_string(), dot::object_id(ID).to_string()));
                }
            }
        }
    }

}