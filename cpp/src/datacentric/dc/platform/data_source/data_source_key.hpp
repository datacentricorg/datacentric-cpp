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
#include <dot/system/ptr.hpp>
#include <dc/types/record/typed_record.hpp>
#include <dc/types/record/typed_key.hpp>

namespace dc
{
    class DataSourceKeyImpl; using DataSourceKey = dot::Ptr<DataSourceKeyImpl>;
    class DataSourceImpl; using DataSource = dot::Ptr<DataSourceImpl>;

    /// Data source is a logical concept similar to database
    /// that can be implemented for a document DB, relational DB,
    /// key-value store, or filesystem.
    ///
    /// Data source API provides the ability to:
    ///
    /// (a) store and query datasets;
    /// (b) store records in a specific dataset; and
    /// (c) query record across a group of datasets.
    ///
    /// This record is stored in root dataset.
    class DC_CLASS DataSourceKeyImpl : public TypedKeyImpl<DataSourceKeyImpl, DataSourceImpl>
    {
        typedef DataSourceKeyImpl self;

    public: // PROPERTIES

        /// Unique data source name.
        dot::String data_source_name;

    public: // STATIC

        /// By convention, cache is the name of the Operational Data Store (ODS).
        static DataSourceKey cache;

        /// By convention, master is the name of the Master Data Store (MDS).
        static DataSourceKey master;

    public: // CONSTRUCTORS

        DataSourceKeyImpl() = default;

        DataSourceKeyImpl(dot::String value);

        DOT_TYPE_BEGIN("dc", "DataSourceKey")
            DOT_TYPE_PROP(data_source_name)
            DOT_TYPE_BASE(TypedKey<DataSourceKeyImpl, DataSourceImpl>)
        DOT_TYPE_END()
    };
}
