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
#include <dc/types/record/RecordFor.hpp>
#include <dc/platform/dataset/DataSetKey.hpp>

namespace dc
{
    class DataSetKeyImpl; using DataSetKey = dot::ptr<DataSetKeyImpl>;
    class DataSetDataImpl; using DataSetData = dot::ptr<DataSetDataImpl>;
    class ObjectId;

    inline DataSetData new_DataSetData();

    /// <summary>DataSet key is a required field for all stored records.
    /// It is used to separate records into logical groups within the
    /// same DB collection or table.</summary>
    class DC_CLASS DataSetDataImpl : public record_impl<DataSetKeyImpl, DataSetDataImpl>
    {
        typedef DataSetDataImpl self;
        friend DataSetData new_DataSetData();

    public:

        /// <summary>
        /// ObjectId of the dataset where the record is stored.
        ///
        /// This override for the DataSetData record sets DataSet to
        /// ObjectId.Empty for the Common dataset.
        /// </summary>
        ObjectId DataSet;

        /// <summary>Unique dataset identifier.</summary>
        dot::string DataSetID;

        /// <summary>
        /// Set context and perform initialization or validation of object data.
        ///
        /// All derived classes overriding this method must call base.Init(context)
        /// before executing the the rest of the code in the method override.
        /// </summary>
        virtual void Init(IContext context);

        /// <summary>DataSet parents.</summary>
        dot::list<ObjectId> Parents;

        DOT_TYPE_BEGIN(".Analyst", "DataSetData")
            DOT_TYPE_PROP(DataSetID)
            DOT_TYPE_PROP(Parents)
            DOT_TYPE_CTOR(new_DataSetData)
            DOT_TYPE_BASE(record<DataSetKeyImpl, DataSetDataImpl>)
        DOT_TYPE_END()

    protected: // CONSTRUCTORS

        DataSetDataImpl() = default;
    };

    inline DataSetData new_DataSetData() { return new DataSetDataImpl; }
}
