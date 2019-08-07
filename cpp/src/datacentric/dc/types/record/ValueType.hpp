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
    /// Enumeration for the atomic value type.
    enum class ValueType : int
    {
        /// Empty
        empty,

        /// String value.
        String,

        /// Double value.
        Double,

        /// Boolean value.
        Bool,

        /// 32-bit integer value.
        Int,

        /// 64-bit long value.
        Long,

        /// Date without the time component (does not specify timezone).
        local_date,

        /// Time without the date component (does not specify timezone).
        local_time,

        /// Time without the date component to one minute resolution (does not specify timezone).
        local_minute,

        /// Datetime (does not specify timezone).
        local_date_time,

        /// Enumeration.
        Enum
    };
}
