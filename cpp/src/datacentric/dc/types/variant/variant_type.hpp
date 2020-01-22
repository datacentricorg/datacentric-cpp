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
    /// Type of atomic value held by variant.
    enum class DC_CLASS variant_type : int
    {
        /// Empty value.
        empty,

        /// String value.
        string,

        /// Double value.
        Double,

        /// Boolean value.
        Bool,

        /// 32-bit integer value.
        Int,

        /// 64-bit integer value.
        Long,

        /// Date without the time component.
        date,

        /// Datetime in UTC timezone.
        date_time
    };
}
