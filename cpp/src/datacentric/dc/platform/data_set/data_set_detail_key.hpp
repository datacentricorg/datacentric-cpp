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
#include <dc/platform/data_set/data_set_detail_data.hpp>

namespace dc
{
    class DataSetDetailKeyImpl; using DataSetDetailKey = dot::Ptr<DataSetDetailKeyImpl>;
    class DataSetDetailImpl; using DataSetDetail = dot::Ptr<DataSetDetailImpl>;

    inline DataSetDetailKey make_data_set_detail_key();

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
    class DC_CLASS DataSetDetailKeyImpl : public TypedKeyImpl<DataSetDetailKeyImpl, DataSetDetailImpl>
    {
        typedef DataSetDetailKeyImpl self;
        friend DataSetDetailKey make_data_set_detail_key();

    public:

        /// TemporalId of the referenced dataset.
        TemporalId data_set_id;

        DOT_TYPE_BEGIN("dc", "DataSetDetailKey")
            DOT_TYPE_PROP(data_set_id)
            DOT_TYPE_CTOR(make_data_set_detail_key)
            DOT_TYPE_BASE(TypedKey<DataSetDetailKeyImpl, DataSetDetailImpl>)
            DOT_TYPE_END()

    protected: // CONSTRUCTORS

        DataSetDetailKeyImpl() = default;
    };

    inline DataSetDetailKey make_data_set_detail_key() { return new DataSetDetailKeyImpl; }
}
