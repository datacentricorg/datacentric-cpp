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
#include <dc/platform/data_set/data_set_key.hpp>

namespace dc
{
    class DataSetKeyImpl; using DataSetKey = dot::Ptr<DataSetKeyImpl>;
    class DataSetImpl; using DataSet = dot::Ptr<DataSetImpl>;

    inline DataSet make_data_set_data();

    /// data_set key is a required field for all stored records.
    /// It is used to separate records into logical groups within the
    /// same DB collection or table.
    class DC_CLASS DataSetImpl : public TypedRecordImpl<DataSetKeyImpl, DataSetImpl>
    {
        typedef DataSetImpl self;
        friend DataSet make_data_set_data();

    public:

        /// TemporalId of the dataset where the record is stored.
        ///
        /// This override for the DataSet record sets data_set to
        /// TemporalId.empty for the common dataset.
        TemporalId data_set;

        /// Unique dataset identifier.
        dot::String data_set_id;

        /// Set context and perform initialization or validation of object data.
        ///
        /// All derived classes overriding this method must call base.init(context)
        /// before executing the the rest of the code in the method override.
        virtual void init(ContextBase context);

        /// data_set parents.
        dot::List<TemporalId> parents;

        DOT_TYPE_BEGIN("dc", "DataSet")
            DOT_TYPE_PROP(data_set_id)
            DOT_TYPE_PROP(parents)
            DOT_TYPE_CTOR(make_data_set_data)
            DOT_TYPE_BASE(TypedRecord<DataSetKeyImpl, DataSetImpl>)
        DOT_TYPE_END()

    protected: // CONSTRUCTORS

        DataSetImpl() = default;
    };

    inline DataSet make_data_set_data() { return new DataSetImpl; }
}
