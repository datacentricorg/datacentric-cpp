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
#include <dot/system/object.hpp>
#include <dc/types/record/key.hpp>

namespace dc
{
    template <typename TKey, typename TRecord> class typed_key_impl;
    template <typename TKey, typename TRecord> using typed_key = dot::ptr<typed_key_impl<TKey, TRecord>>;

    class context_base_impl; using context_base = dot::ptr<context_base_impl>;

    /// Base class of a foreign key.
    ///
    /// Generic parameters TKey, TRecord represent a variation on
    /// the curiously recurring template pattern (CRTP). They make
    /// it possible to get key type from the record and vice versa.
    ///
    /// Any elements of defined in the class derived from this one
    /// become key tokens. Property Value and method ToString() of
    /// the key consists of key tokens with semicolon delimiter.
    template <typename TKey, typename TRecord>
    class typed_key_impl : public virtual key_impl
    {
        typedef typed_key_impl<TKey, TRecord> self;

    public: // METHODS

        /// Assign key elements from record to key.
        void populate_from(typed_record<TKey, TRecord> record);

        DOT_TYPE_BEGIN("dc", "typed_key")
            DOT_TYPE_BASE(key)
            DOT_TYPE_GENERIC_ARGUMENT(dot::ptr<TKey>)
            DOT_TYPE_GENERIC_ARGUMENT(dot::ptr<TRecord>)
        DOT_TYPE_END()
    };
}
