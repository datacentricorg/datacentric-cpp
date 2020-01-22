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
    class DC_CLASS DataSetImpl : public TypedRecordImpl<DataSetKeyImpl, DataSetImpl>
    {
        typedef DataSetImpl self;
        friend DataSet make_data_set_data();

    public: // PROPERTIES

        /// Unique dataset name.
        dot::String data_set_name;

        /// Flag indicating that the dataset is non-temporal even if the
        /// data source supports temporal data.
        ///
        /// For the data stored in datasets where NonTemporal == false, a
        /// temporal data source keeps permanent history of changes to each
        /// record within the dataset, and provides the ability to access
        /// the record as of the specified TemporalId, where TemporalId serves
        /// as a timeline (records created later have greater TemporalId than
        /// records created earlier).
        ///
        /// For the data stored in datasets where NonTemporal == true, the
        /// data source keeps only the latest version of the record. All
        /// child datasets of a non-temporal dataset must also be non-temporal.
        ///
        /// In a non-temporal data source, this flag is ignored as all
        /// datasets in such data source are non-temporal.
        bool non_temporal;

        /// List of datasets where records are looked up if they are
        /// not found in the current dataset.
        ///
        /// The specific lookup rules are specific to the data source
        /// type and described in detail in the data source documentation.
        ///
        /// The parent dataset is not included in the list of Imports by
        /// default and must be included in the list of Imports explicitly.
        dot::List<TemporalId> imports;

    public: // METHODS

        /// Set Context property and perform validation of the record's data,
        /// then initialize any fields or properties that depend on that data.
        ///
        /// This method must work when called multiple times for the same instance,
        /// possibly with a different context parameter for each subsequent call.
        ///
        /// All overrides of this method must call base.Init(context) first, then
        /// execute the rest of the code in the override.
        virtual void init(ContextBase context);

        DOT_TYPE_BEGIN("dc", "DataSet")
            DOT_TYPE_PROP(data_set_name)
            DOT_TYPE_PROP(non_temporal)
            DOT_TYPE_PROP(imports)
            DOT_TYPE_CTOR(make_data_set_data)
            DOT_TYPE_BASE(TypedRecord<DataSetKeyImpl, DataSetImpl>)
        DOT_TYPE_END()

    protected: // CONSTRUCTORS

        DataSetImpl() = default;
    };

    inline DataSet make_data_set_data() { return new DataSetImpl; }
}
