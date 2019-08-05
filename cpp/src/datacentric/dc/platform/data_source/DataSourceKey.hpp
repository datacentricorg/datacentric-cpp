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
#include <dc/types/record/RecordFor.hpp>
#include <dc/platform/data_source/DatabaseKey.hpp>
#include <dc/platform/data_source/DatabaseData.hpp>

namespace dc
{
    class data_source_key_impl; using data_source_key = dot::ptr<data_source_key_impl>;
    class data_source_data_impl; using data_source_data = dot::ptr<data_source_data_impl>;

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
    class DC_CLASS data_source_key_impl : public root_key_for_impl<data_source_key_impl, data_source_data_impl>
    {
        typedef data_source_key_impl self;

    public: // PROPERTIES

        /// <summary>Unique data source identifier.</summary>
        dot::string data_source_id;

    public: // STATIC

        /// <summary>
        /// By convention, Cache is the name of the Operational Data Store (ODS).
        /// </summary>
        static data_source_key cache;

        /// <summary>
        /// By convention, Master is the name of the Master Data Store (MDS).
        /// </summary>
        static data_source_key master;

    public: // CONSTRUCTORS

        data_source_key_impl() = default;

        data_source_key_impl(dot::string value);

        DOT_TYPE_BEGIN(".Runtime.Main", "DataSourceKey")
            DOT_TYPE_PROP(data_source_id)
            DOT_TYPE_BASE(root_key_for<data_source_key_impl, data_source_data_impl>)
        DOT_TYPE_END()
    };
}
