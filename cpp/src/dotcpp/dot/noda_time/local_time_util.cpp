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

#include <dot/precompiled.hpp>
#include <dot/implement.hpp>
#include <dot/noda_time/local_time_util.hpp>
#include <dot/system/exception.hpp>
#include <dot/system/string.hpp>

namespace dot
{
    dot::LocalTime LocalTimeUtil::parse(dot::string value)
    {
        boost::posix_time::time_input_facet* facet = new boost::posix_time::time_input_facet();
        facet->format("%H:%M:%S%f");
        std::istringstream stream(*value);
        stream.imbue(std::locale(std::locale::classic(), facet));

        boost::posix_time::ptime ptime;
        stream >> ptime;

        // If default constructed time is passed, error message
        if (ptime == boost::posix_time::not_a_date_time) throw dot::exception(dot::string::format(
            "String representation of default constructed time {0} "
            "passed to LocalTimeUtil.parse(value) method.", value));

        return ptime;
    }

    int LocalTimeUtil::to_iso_int(dot::LocalTime value)
    {
        // LocalTime is serialized to millisecond precision in ISO 8601 9 digit int hhmmssfff format
        int result = value.hour() * 100'00'000 + value.minute() * 100'000 + value.second() * 1000 + value.millisecond();
        return result;
    }

    dot::LocalTime LocalTimeUtil::parse_iso_int(int value)
    {
        // Extract year, month, day
        int hour = value / 100'00'000;
        value -= hour * 100'00'000;
        int minute = value / 100'000;
        value -= minute * 100'000;
        int second = value / 1000;
        value -= second * 1000;
        int millisecond = value;

        // Create new LocalTime object, validates values on input
        return dot::LocalTime(hour, minute, second, millisecond);
    }
}
