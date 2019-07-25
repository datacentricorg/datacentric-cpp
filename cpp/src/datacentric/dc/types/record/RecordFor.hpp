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
#include <dc/types/record/RecordType.hpp>
#include <dot/system/reflection/Activator.hpp>

namespace dc
{
    class RecordTypeImpl; using RecordType = dot::Ptr<RecordTypeImpl>;

    template <typename TKey, typename TRecord> class RecordForImpl;
    template <typename TKey, typename TRecord> using RecordFor = dot::Ptr<RecordForImpl<TKey, TRecord>>;


    /// <summary>
    /// Base class of records stored in data source.
    /// </summary>
    template <typename TKey, typename TRecord>
    class RecordForImpl : public virtual RecordTypeImpl
    {
        typedef RecordForImpl<TKey, TRecord> self;

    public:

        dot::String getKey() override
        {
            dot::Array1D<dot::PropertyInfo> props =  dot::typeof<dot::Ptr<TKey>>()->GetProperties();
            dot::Type type = GetType();

            std::stringstream ss;

            for (int i = 0; i < props->Count; ++i)
            {
                dot::PropertyInfo key_prop = props[i];

                dot::PropertyInfo prop = type->GetProperty(key_prop->Name);

                dot::Object value = prop->GetValue(this);

                if (i) ss << separator;

                if (!value.IsEmpty())
                {
                    ss << *value->ToString();
                }
                else
                {
                    if (prop->PropertyType->Name->EndsWith("Key")) // TODO check using parents list
                    {
                        dot::Object emptyKey = dot::Activator::CreateInstance(prop->PropertyType);
                        ss << *emptyKey->ToString();
                    }
                }

            }

            return ss.str();
        }

    public:

        /// <summary>
        /// This conversion method creates a new key, populates key elements of the
        /// created key by the values taken from the record, and then caches the
        /// record inside the key using record.DataSet. The cached value will be
        /// used only for lookup in the same dataset but not for lookup in another
        /// dataset for which the current dataset is a parent.
        ///
        /// The purpose of caching is to increase the speed of repeated loading, and
        /// to bypass saving the object to the data store and reading it back when
        /// record A has property that is a key for record B, and both records are
        /// created in-memory without any need to save them to storage.
        /// </summary>
        dot::Ptr<TKey> ToKey()
        {
            dot::Type keyType = dot::typeof<dot::Ptr<TKey>>();
            dot::Ptr<TKey> result = (dot::Ptr<TKey>)dot::Activator::CreateInstance(keyType);

            // The cached value will be used only for lookup in the dataset
            // passed to this method, but not for lookup in another dataset
            // for which the current dataset is a parent
            result->SetCachedRecord(this, DataSet);

            return result;
        }

        /// <summary>
        /// This conversion operator creates a new key from record and then caches itself
        ///
        /// The purpose of this operator is to bypass saving the object to the data store
        /// and reading it back when record A has property that is a key for record B,
        /// and both records are created in-memory without any need to save them to storage.
        /// </summary>
        operator dot::Ptr<TKey>() { return ToKey(); }

        DOT_TYPE_BEGIN(".Runtime.Main", "RecordFor")
            DOT_TYPE_BASE(RecordType)
            DOT_TYPE_GENERIC_ARGUMENT(dot::Ptr<TKey>)
            DOT_TYPE_GENERIC_ARGUMENT(dot::Ptr<TRecord>)
        DOT_TYPE_END()
    };


}


#include <dc/types/record/RootRecordFor.hpp>
#include <dc/types/record/KeyFor.hpp>
#include <dc/types/record/RootKeyFor.hpp>

#include <dc/platform/data_source/DataSourceData.hpp>
#include <dc/platform/data_source/DataSourceKey.hpp>

#include <dc/platform/data_source/DatabaseServerData.hpp>
#include <dc/platform/data_source/DatabaseServerKey.hpp>
