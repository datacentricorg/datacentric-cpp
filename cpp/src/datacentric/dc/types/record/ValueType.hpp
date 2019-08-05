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
    /// <summary>Enumeration for the atomic value type.</summary>
    enum class ValueType : int
    {
        /// <summary>Empty</summary>
        Empty,

        /// <summary>String value.</summary>
        String,

        /// <summary>Double value.</summary>
        Double,

        /// <summary>Boolean value.</summary>
        Bool,

        /// <summary>32-bit integer value.</summary>
        Int,

        /// <summary>64-bit long value.</summary>
        Long,

        /// <summary>Date without the time component (does not specify timezone).</summary>
        local_date,

        /// <summary>Time without the date component (does not specify timezone).</summary>
        local_time,

        /// <summary>Time without the date component to one minute resolution (does not specify timezone).</summary>
        local_minute,

        /// <summary>Datetime (does not specify timezone).</summary>
        local_date_time,

        /// <summary>Enumeration.</summary>
        Enum
    };
}
