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
#include <dot/noda_time/local_date_time_util.hpp>
#include <dot/system/exception.hpp>
#include <dot/system/string.hpp>

namespace dot
{
    dot::local_date_time local_date_time_util::parse(dot::string value)
    {
        boost::posix_time::time_input_facet* facet = new boost::posix_time::time_input_facet();
        facet->format("%Y-%m-%d %H:%M:%S%f");
        std::istringstream stream(*value);
        stream.imbue(std::locale(std::locale::classic(), facet));

        boost::posix_time::ptime ptime;
        stream >> ptime;

        // If default constructed datetime is passed, error message
        if (ptime == boost::posix_time::not_a_date_time) throw dot::exception(dot::string::format(
            "String representation of default constructed datetime {0} "
            "passed to local_date_time_util.parse(value) method.", value));

        return ptime;
    }

    int64_t local_date_time_util::to_iso_long(dot::local_date_time value)
    {
        // local_date_time is serialized as readable ISO int64 in yyyymmddhhmmsssss format
        int iso_date = value.year() * 10'000 + value.month() * 100 + value.day();
        int iso_time = value.hour() * 100'00'000 + value.minute() * 100'000 + value.second() * 1000 + value.millisecond();
        int64_t result = ((int64_t)iso_date) * 100'00'00'000 + (int64_t)iso_time;
        return result;
    }

    dot::local_date_time local_date_time_util::parse_iso_long(int64_t value)
    {
        // Split into date and time using int64 arithmetic
        int64_t iso_date_long = value / 100'00'00'000;
        int64_t iso_time_long = value - 100'00'00'000 * iso_date_long;

        // Check that it will fit into int32 range
        if (iso_date_long < INT32_MIN || iso_date_long > INT32_MAX)
            throw dot::exception(dot::string::format("Date portion of datetime {0} has invalid format.", value));
        if (iso_time_long < INT32_MIN || iso_time_long > INT32_MAX)
            throw dot::exception(dot::string::format("Time portion of datetime {0} has invalid format.", value));

        // Convert to int32
        int iso_date = (int)iso_date_long;
        int iso_time = (int)iso_time_long;

        // Extract year, month, day
        int year = iso_date / 100'00;
        iso_date -= year * 100'00;
        int month = iso_date / 100;
        iso_date -= month * 100;
        int day = iso_date;

        // Extract year, month, day
        int hour = iso_time / 100'00'000;
        iso_time -= hour * 100'00'000;
        int minute = iso_time / 100'000;
        iso_time -= minute * 100'000;
        int second = iso_time / 1000;
        iso_time -= second * 1000;
        int millisecond = iso_time;

        // Create local_date_time object
        return dot::local_date_time(year, month, day, hour, minute, second, millisecond);
    }

    std::chrono::milliseconds local_date_time_util::to_std_chrono(dot::local_date_time value)
    {
        boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
        boost::posix_time::time_duration d = (boost::posix_time::ptime)value - epoch;
        return std::chrono::milliseconds(d.total_milliseconds());
    }

    dot::local_date_time local_date_time_util::from_std_chrono(std::chrono::milliseconds value)
    {
        boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
        boost::posix_time::time_duration d = boost::posix_time::milliseconds(value.count());
        return epoch + d;
    }
}