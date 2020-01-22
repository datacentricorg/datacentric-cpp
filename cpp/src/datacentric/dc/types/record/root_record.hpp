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
    template <typename TKey, typename TRecord> class RootRecordImpl;
    template <typename TKey, typename TRecord> using RootRecord = dot::Ptr<RootRecordImpl<TKey, TRecord>>;
    template <typename TKey, typename TRecord> class TypedRecordImpl;
    template <typename TKey, typename TRecord> using TypedRecord = dot::Ptr<TypedRecordImpl<TKey, TRecord>>;

    /// Base class of records stored in root dataset of the data store.
    ///
    /// Init(...) method of this class sets DataSet to TemporalId.Empty.
    template <typename TKey, typename TRecord>
    class RootRecordImpl : public virtual TypedRecordImpl<TKey, TRecord>
    {
        typedef RootRecordImpl<TKey, TRecord> self;
        typedef TypedRecordImpl<TKey, TRecord> base;

    public: // METHODS

        /// Set Context property and perform validation of the record's data,
        /// then initialize any fields or properties that depend on that data.
        ///
        /// This method must work when called multiple times for the same instance,
        /// possibly with a different context parameter for each subsequent call.
        ///
        /// All overrides of this method must call base.Init(context) first, then
        /// execute the rest of the code in the override.
        void init(ContextBase context) override;

    public: // REFLECTION

        DOT_TYPE_BEGIN("dc", "RootRecord")
            DOT_TYPE_BASE(TypedRecord<TKey, TRecord>)
        DOT_TYPE_END()
    };
}
