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

namespace dc
{
    class DataSetKeyImpl; using DataSetKey = dot::Ptr<DataSetKeyImpl>;
    class DataSetImpl; using DataSet = dot::Ptr<DataSetImpl>;

    inline DataSetKey make_data_set_key();

    /// data_set key is a required field for all stored records.
    /// It is used to separate records into logical groups within the
    /// same DB collection or table.
    class DC_CLASS DataSetKeyImpl : public TypedKeyImpl<DataSetKeyImpl, DataSetImpl>
    {
        typedef DataSetKeyImpl self;
        friend DataSetKey make_data_set_key();

    public: // PROPERTIES

        /// Unique dataset identifier.
        dot::String data_set_id;

        DOT_TYPE_BEGIN("dc", "DataSetKey")
            DOT_TYPE_PROP(data_set_id)
            DOT_TYPE_CTOR(make_data_set_key)
            DOT_TYPE_BASE(TypedKey<DataSetKeyImpl, DataSetImpl>)
        DOT_TYPE_END()

    public: // STATIC

        static DataSetKey common;
    };

    inline DataSetKey make_data_set_key() { return new DataSetKeyImpl; }
}

#include <dc/platform/data_set/data_set_data.hpp>
