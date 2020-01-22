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

namespace dc
{
    template <typename TKey, typename TRecord>
    void RootRecordImpl<TKey, TRecord>::init(ContextBase context)
    {
        // Initialize base before executing the rest of the code in this method
        base::init(context);

        // For this base type of records stored in root dataset,
        // set DataSet element to the value designated for the
        // root dataset: TemporalId.Empty.
        data_set = TemporalId::empty;
    }
}
