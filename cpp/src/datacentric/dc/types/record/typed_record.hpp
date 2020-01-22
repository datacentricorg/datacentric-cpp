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
#include <dc/types/record/record.hpp>
#include <dot/system/reflection/activator.hpp>

namespace dc
{
    class record_impl; using record = dot::Ptr<record_impl>;

    template <typename TKey, typename TRecord> class typed_record_impl;
    template <typename TKey, typename TRecord> using typed_record = dot::Ptr<typed_record_impl<TKey, TRecord>>;

    /// Base class of records stored in data source.
    template <typename TKey, typename TRecord>
    class typed_record_impl : public virtual record_impl
    {
        typedef typed_record_impl<TKey, TRecord> self;

    public:

        /// String key consists of semicolon delimited primary key elements:
        ///
        /// KeyElement1;KeyElement2
        ///
        /// To avoid serialization format uncertainty, key elements
        /// can have any atomic type except Double.
        dot::String get_key() override
        {
            dot::list<dot::FieldInfo> props =  dot::typeof<dot::Ptr<TKey>>()->get_fields();
            dot::Type type_ = get_type();

            std::stringstream ss;

            for (int i = 0; i < props->count(); ++i)
            {
                dot::FieldInfo key_prop = props[i];

                dot::FieldInfo prop = type_->get_field(key_prop->name());

                dot::Object value = prop->get_value(this);

                if (i) ss << separator;

                if (!value.is_empty())
                {
                    ss << *value->to_string();
                }
                else
                {
                    if (prop->field_type()->is_subclass_of(dot::typeof<key>()))
                    {
                        dot::Object empty_key = dot::Activator::create_instance(prop->field_type());
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
        /// to bypass saving the Object to the data store and reading it back when
        /// record A has property that is a key for record B, and both records are
        /// created in-memory without any need to save them to storage.
        dot::Ptr<TKey> to_key()
        {
            dot::Type key_type = dot::typeof<dot::Ptr<TKey>>();
            dot::Ptr<TKey> result = (dot::Ptr<TKey>)dot::Activator::create_instance(key_type);

            // The cached value will be used only for lookup in the dataset
            // passed to this method, but not for lookup in another dataset
            // for which the current dataset is a parent
            result->populate_from(this);

            return result;
        }

        /// This conversion operator creates a new key from record and then caches itself
        ///
        /// The purpose of this operator is to bypass saving the Object to the data store
        /// and reading it back when record A has property that is a key for record B,
        /// and both records are created in-memory without any need to save them to storage.
        operator dot::Ptr<TKey>() { return to_key(); }

        DOT_TYPE_BEGIN("dc", "typed_record")
            DOT_TYPE_BASE(record)
            DOT_TYPE_GENERIC_ARGUMENT(dot::Ptr<TKey>)
            DOT_TYPE_GENERIC_ARGUMENT(dot::Ptr<TRecord>)
        DOT_TYPE_END()
    };
}

#include <dc/types/record/root_record.hpp>
#include <dc/types/record/typed_key.hpp>
#include <dc/types/record/root_key.hpp>

#include <dc/platform/data_source/data_source_data.hpp>
#include <dc/platform/data_source/data_source_key.hpp>

#include <dc/platform/data_source/database_server_data.hpp>
#include <dc/platform/data_source/database_server_key.hpp>
