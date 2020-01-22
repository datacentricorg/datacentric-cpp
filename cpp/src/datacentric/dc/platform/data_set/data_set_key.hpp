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

    /// Dataset is a concept similar to a folder, applied to data in any
    /// data source including relational or document databases, OData
    /// endpoints, etc.
    ///
    /// Datasets can be stored in other datasets. The dataset where dataset
    /// record is stored is called parent dataset.
    ///
    /// Dataset has an Imports array which provides the list of TemporalIds of
    /// datasets where records are looked up if they are not found in the
    /// current dataset. The specific lookup rules are specific to the data
    /// source type and described in detail in the data source documentation.
    ///
    /// Some data source types do not support Imports. If such data
    /// source is used with a dataset where Imports array is not empty,
    /// an error will be raised.
    ///
    /// The root dataset uses TemporalId.Empty and does not have versions
    /// or its own DataSet record. It is always last in the dataset
    /// lookup sequence. The root dataset cannot have Imports.
    class DC_CLASS DataSetKeyImpl : public TypedKeyImpl<DataSetKeyImpl, DataSetImpl>
    {
        typedef DataSetKeyImpl self;
        friend DataSetKey make_data_set_key();

    public: // PROPERTIES

        /// Unique dataset name.
        dot::String data_set_name;

        DOT_TYPE_BEGIN("dc", "DataSetKey")
            DOT_TYPE_PROP(data_set_name)
            DOT_TYPE_CTOR(make_data_set_key)
            DOT_TYPE_BASE(TypedKey<DataSetKeyImpl, DataSetImpl>)
        DOT_TYPE_END()

    public: // STATIC

        /// By convention, Common is the default dataset in each data source.
        ///
        /// Common dataset is stored in root dataset.
        ///
        /// It should have no imports, and is usually (but not always)
        /// included in the list of imports for other datasets. It should
        /// be used to store master records and other record types that
        /// should be visible from all other datasets.
        ///
        /// Similar to other datasets, the Common dataset is versioned.
        /// Records that should be stored in a non-versioned dataset, such
        /// as the dataset records themselves, the data source records,
        /// and a few other record types, should set DataSet property to
        /// TemporalId.Empty (root dataset).
        static DataSetKey common;
    };

    inline DataSetKey make_data_set_key() { return new DataSetKeyImpl; }
}

#include <dc/platform/data_set/data_set_data.hpp>
