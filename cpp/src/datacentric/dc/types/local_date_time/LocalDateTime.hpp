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
#include <dot/noda_time/LocalDateTime.hpp>
#include <chrono>

namespace dc
{
    class DC_CLASS LocalDateTimeHelper
    {
    public: // STATIC

        /// <summary>Parse string using standard ISO 8601 date pattern yyyy-mm-ddThh:mm::ss.fff, throw if invalid format.
        /// No variations from the standard format are accepted and no delimiters can be changed or omitted.
        /// Specifically, ISO int-like string using yyyymmddhhmmssfff format without delimiters is not accepted.</summary>
        static dot::local_date_time Parse(dot::string value);

        /// <summary>Convert LocalDateTime to ISO 8601 8 digit long in yyyymmddhhmmssfff format.</summary>
        static int64_t ToIsoLong(dot::local_date_time value);

        /// <summary>Parse ISO 8601 17 digit long in yyyymmddhhmmssfff format, throw if invalid format.</summary>
        static dot::local_date_time ParseIsoLong(int64_t value);

        /// <summary>Convert LocalDateTime to std::chrono::milliseconds.</summary>
        static std::chrono::milliseconds ToStdChrono(dot::local_date_time value);

        /// <summary>Convert std::chrono::milliseconds to LocalDateTime.</summary>
        static dot::local_date_time FromStdChrono(std::chrono::milliseconds value);
    };
}
