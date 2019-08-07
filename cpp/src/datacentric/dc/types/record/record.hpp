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
#include <dc/types/record/record_base.hpp>
#include <dot/system/reflection/activator.hpp>

namespace dc
{
    class record_base_impl; using record_base = dot::ptr<record_base_impl>;

    template <typename TKey, typename TRecord> class record_impl;
    template <typename TKey, typename TRecord> using record = dot::ptr<record_impl<TKey, TRecord>>;

    /// Base class of records stored in data source.
    template <typename TKey, typename TRecord>
    class record_impl : public virtual record_base_impl
    {
        typedef record_impl<TKey, TRecord> self;

    public:

        dot::string get_key() override
        {
            dot::list<dot::field_info> props =  dot::typeof<dot::ptr<TKey>>()->get_fields();
            dot::type_t type_ = type();

            std::stringstream ss;

            for (int i = 0; i < props->count(); ++i)
            {
                dot::field_info key_prop = props[i];

                dot::field_info prop = type_->get_field(key_prop->name);

                dot::object value = prop->get_value(this);

                if (i) ss << separator;

                if (!value.is_empty())
                {
                    ss << *value->to_string();
                }
                else
                {
                    if (prop->field_type->name->ends_with("Key")) // TODO check using parents list
                    {
                        dot::object empty_key = dot::activator::create_instance(prop->field_type);
                        ss << *empty_key->to_string();
                    }
                }

            }

            return ss.str();
        }

    public:

        /// This conversion method creates a new key, populates key elements of the
        /// created key by the values taken from the record, and then caches the
        /// record inside the key using record.data_set. The cached value will be
        /// used only for lookup in the same dataset but not for lookup in another
        /// dataset for which the current dataset is a parent.
        ///
        /// The purpose of caching is to increase the speed of repeated loading, and
        /// to bypass saving the object to the data store and reading it back when
        /// record A has property that is a key for record B, and both records are
        /// created in-memory without any need to save them to storage.
        dot::ptr<TKey> to_key()
        {
            dot::type_t key_type = dot::typeof<dot::ptr<TKey>>();
            dot::ptr<TKey> result = (dot::ptr<TKey>)dot::activator::create_instance(key_type);

            // The cached value will be used only for lookup in the dataset
            // passed to this method, but not for lookup in another dataset
            // for which the current dataset is a parent
            result->SetCachedRecord(this, data_set);

            return result;
        }

        /// This conversion operator creates a new key from record and then caches itself
        ///
        /// The purpose of this operator is to bypass saving the object to the data store
        /// and reading it back when record A has property that is a key for record B,
        /// and both records are created in-memory without any need to save them to storage.
        operator dot::ptr<TKey>() { return to_key(); }

        DOT_TYPE_BEGIN("dc", "Record")
            DOT_TYPE_BASE(record_base)
            DOT_TYPE_GENERIC_ARGUMENT(dot::ptr<TKey>)
            DOT_TYPE_GENERIC_ARGUMENT(dot::ptr<TRecord>)
        DOT_TYPE_END()
    };
}

#include <dc/types/record/root_record.hpp>
#include <dc/types/record/key.hpp>
#include <dc/types/record/root_key.hpp>

#include <dc/platform/data_source/data_source_data.hpp>
#include <dc/platform/data_source/data_source_key.hpp>

#include <dc/platform/data_source/DatabaseServerData.hpp>
#include <dc/platform/data_source/DatabaseServerKey.hpp>
