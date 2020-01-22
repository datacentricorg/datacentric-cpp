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
#include <dc/platform/data_set/data_set_detail_key.hpp>

namespace dc
{
    class data_set_detail_key_impl; using data_set_detail_key = dot::ptr<data_set_detail_key_impl>;
    class data_set_detail_data_impl; using data_set_detail_data = dot::ptr<data_set_detail_data_impl>;

    inline data_set_detail_data make_data_set_detail_data();

    /// Provides the ability to change data associated with the dataset
    /// without changing the dataset record, which is immutable in a
    /// temporal data source.
    ///
    /// The reason dataset record is immutable is that any change to the
    /// the dataset record in a temporal data source results in creation
    /// of a record with new TemporalId, which is treated as a new dataset.
    ///
    /// The DataSetDetail record uses TemporalId of the referenced dataset
    /// as its primary key. It is located in the parent of the dataset
    /// record to which it applies, rather than inside that record, so it
    /// is not affected by its own settings.
    class DC_CLASS data_set_detail_data_impl : public typed_record_impl<data_set_detail_key_impl, data_set_detail_data_impl>
    {
        typedef data_set_detail_data_impl self;
        friend data_set_detail_data make_data_set_detail_data();

    public:

        /// dot::object_id of the referenced dataset.
        dot::object_id data_set_id;

        /// <summary>
        /// If specified, write operations to the referenced dataset
        /// will result in an error.
        /// </summary>
        dot::nullable<bool> read_only;

        /// Records with TemporalId that is greater than or equal to CutoffTime
        /// will be ignored by load methods and queries, and the latest available
        /// record where TemporalId is less than CutoffTime will be returned instead.
        ///
        /// CutoffTime applies to both the records stored in the dataset itself,
        /// and the reports loaded through the Imports list.
        ///
        /// CutoffTime may be set in data source globally, or for a specific dataset
        /// in its details record. If CutoffTime is set for both, the earlier of the
        /// two values will be used.
        dot::nullable<dot::object_id> cutoff_time;

        /// Imported records (records loaded through the Imports list)
        /// where TemporalId is greater than or equal to CutoffTime
        /// will be ignored by load methods and queries, and the latest
        /// available record where TemporalId is less than CutoffTime will
        /// be returned instead.
        ///
        /// This setting only affects records loaded through the Imports
        /// list. It does not affect records stored in the dataset itself.
        ///
        /// Use this feature to freeze Imports as of a given CreatedTime
        /// (part of TemporalId), isolating the dataset from changes to the
        /// data in imported datasets that occur after that time.
        ///
        /// If ImportsCutoffTime is set for both data source and dataset,
        /// the earlier of the two values will be used.
        dot::nullable<dot::object_id> imports_cutoff_time;

        DOT_TYPE_BEGIN("dc", "data_set_detail_data")
            DOT_TYPE_PROP(data_set_id)
            DOT_TYPE_PROP(read_only)
            DOT_TYPE_PROP(cutoff_time)
            DOT_TYPE_PROP(imports_cutoff_time)
            DOT_TYPE_CTOR(make_data_set_detail_data)
            DOT_TYPE_BASE(typed_record<data_set_detail_key_impl, data_set_detail_data_impl>)
            DOT_TYPE_END()

    protected: // CONSTRUCTORS

        data_set_detail_data_impl() = default;
    };

    inline data_set_detail_data make_data_set_detail_data() { return new data_set_detail_data_impl; }
}
