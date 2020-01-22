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
#include <dc/types/record/typed_record.hpp>

namespace dc
{
    template <typename TKey, typename TRecord> class root_record_for_impl;
    template <typename TKey, typename TRecord> using root_record_for = dot::Ptr<root_record_for_impl<TKey, TRecord>>;
    template <typename TKey, typename TRecord> class typed_record_impl;
    template <typename TKey, typename TRecord> using typed_record = dot::Ptr<typed_record_impl<TKey, TRecord>>;

    /// Root record is recorded without a dataset.
    template <typename TKey, typename TRecord>
    class root_record_for_impl : public virtual typed_record_impl<TKey, TRecord>
    {
        typedef root_record_for_impl<TKey, TRecord> self;
    public:

        DOT_TYPE_BEGIN("dc", "root_record")
            DOT_TYPE_BASE(typed_record<TKey, TRecord>)
        DOT_TYPE_END()

    };
}
