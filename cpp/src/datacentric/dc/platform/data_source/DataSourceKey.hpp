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
#include <dot/system/Ptr.hpp>
#include <dc/types/record/RecordFor.hpp>
#include <dc/platform/data_source/DatabaseKey.hpp>
#include <dc/platform/data_source/DatabaseData.hpp>

namespace dc
{
    class DataSourceKeyImpl; using DataSourceKey = dot::Ptr<DataSourceKeyImpl>;
    class DataSourceDataImpl; using DataSourceData = dot::Ptr<DataSourceDataImpl>;

    /// <summary>
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
    /// </summary>
    class DC_CLASS DataSourceKeyImpl : public RootKeyForImpl<DataSourceKeyImpl, DataSourceDataImpl>
    {
        typedef DataSourceKeyImpl self;

    public: // PROPERTIES

        /// <summary>Unique data source identifier.</summary>
        DOT_AUTO_PROP(dot::String, DataSourceID)

    public: // STATIC

        /// <summary>
        /// By convention, Cache is the name of the Operational Data Store (ODS).
        /// </summary>
        static DataSourceKey Cache;

        /// <summary>
        /// By convention, Master is the name of the Master Data Store (MDS).
        /// </summary>
        static DataSourceKey Master;

    public: // CONSTRUCTORS

        DataSourceKeyImpl() = default;

        DataSourceKeyImpl(dot::String value);

        DOT_TYPE_BEGIN(".Runtime.Main", "DataSourceKey")
            DOT_TYPE_PROP(DataSourceID)
            DOT_TYPE_BASE(RootKeyFor<DataSourceKeyImpl, DataSourceDataImpl>)
        DOT_TYPE_END()
    };
}
