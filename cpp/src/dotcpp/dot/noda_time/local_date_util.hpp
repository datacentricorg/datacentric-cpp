/*
Copyright (C) 2015-present The DotCpp Authors.

This file is part of .C++, a native C++ implementation of
popular .NET class library APIs developed to facilitate
code reuse between C# and C++.

    http://github.com/dotcpp/dotcpp (source)
    http://dotcpp.org (documentation)

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

#include <dot/declare.hpp>
#include <dot/noda_time/local_date.hpp>

namespace dot
{
    class DOT_CLASS LocalDateUtil
    {
    public: // STATIC

        /// Parse string using standard ISO 8601 date pattern yyyy-mm-dd, throw if invalid format.
        /// No variations from the standard format are accepted and no delimiters can be changed or omitted.
        /// Specifically, ISO int-like string using yyyymmdd format without delimiters is not accepted.
        static dot::LocalDate parse(dot::string value);

        /// Convert LocalDate to to ISO 8601 8 digit int in yyyymmdd format.
        static int to_iso_int(dot::LocalDate value);

        /// Parse ISO 8601 8 digit int in yyyymmdd format, throw if invalid format.
        static dot::LocalDate parse_iso_int(int value);
    };
}
