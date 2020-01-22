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

namespace dc
{

    /// <summary>Binary flags for the dataset create options.</summary>
    enum class data_set_flags
    {
        /// <summary>Specifies that no flags are defined.</summary>
        default_option = 0,

        /// <summary>
        /// By default, a dataset will hold temporal data if the data source
        /// has temporal capability. Specify this flag to create a dataset that
        /// holds non-temporal data in a temporal data source.
        ///
        /// The reason to specify this flag is to avoid accumulation of historical
        /// data for records that are frequently changed and do not require an
        /// audit log, such as user interface preferences, customizations, etc.
        ///
        /// Note that the non-temporal flag applies to the data held in the dataset,
        /// but not to the dataset record itself. In a temporal data source, all
        /// dataset records are themselves temporal, even those dataset that hold
        /// non-temporal data.
        ///
        /// In a non-temporal data source, this flag is ignored as all
        /// datasets in such data source are non-temporal.
        /// </summary>
        non_temporal = 1
    };

}
