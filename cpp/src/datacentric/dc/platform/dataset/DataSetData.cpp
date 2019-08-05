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

#include <dc/implement.hpp>
#include <dc/platform/dataset/DataSetData.hpp>
#include <dc/platform/context/IContext.hpp>

namespace dc
{
    void DataSetDataImpl::Init(IContext context)
    {
        record_for_impl<DataSetKeyImpl, DataSetDataImpl>::Init(context);

        if (dot::string::is_null_or_empty(DataSetID)) throw dot::exception("DataSetID has not been set.");

        if (!dot::list<ObjectId>(Parents).is_empty())
        for (ObjectId parent : Parents)
        {
            if (ID <= parent)
            {
                if (ID == parent)
                {
                    throw dot::exception(dot::string::format(
                        "Dataset {0} has a parent with the same ObjectId={1} "
                        "as its own ObjectId. Each ObjectId must be unique.", DataSetID, parent.to_string()));
                }
                else
                {
                    throw dot::exception(dot::string::format(
                        "Dataset {0} has a parent whose ObjectId={1} is greater "
                        "than its own ObjectId={2}. The ObjectId of each parent must be strictly "
                        "less than the ObjectId of the dataset itself.", DataSetID, parent.to_string(), ObjectId(ID).to_string()));
                }
            }
        }
    }

}