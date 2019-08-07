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


namespace dc
{
    class DataSetKeyImpl; using DataSetKey = dot::ptr<DataSetKeyImpl>;
    class DataSetDataImpl; using DataSetData = dot::ptr<DataSetDataImpl>;

    inline DataSetKey new_DataSetKey();

    /// DataSet key is a required field for all stored records.
    /// It is used to separate records into logical groups within the
    /// same DB collection or table.
    class DC_CLASS DataSetKeyImpl : public key_impl<DataSetKeyImpl, DataSetDataImpl>
    {
        typedef DataSetKeyImpl self;
        friend DataSetKey new_DataSetKey();

    public: // PROPERTIES

        /// Unique dataset identifier.
        dot::string DataSetID;

        DOT_TYPE_BEGIN(".Analyst", "DataSetKey")
            DOT_TYPE_PROP(DataSetID)
            DOT_TYPE_CTOR(new_DataSetKey)
            DOT_TYPE_BASE(key<DataSetKeyImpl, DataSetDataImpl>)
        DOT_TYPE_END()

    public: // STATIC

        static DataSetKey Common;
    };

    inline DataSetKey new_DataSetKey() { return new DataSetKeyImpl; }
}

#include <dc/platform/dataset/DataSetData.hpp>
