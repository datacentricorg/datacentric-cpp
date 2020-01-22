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
#include <dot/noda_time/local_minute_util.hpp>
#include <dot/system/exception.hpp>
#include <dot/system/string.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace dot
{
    dot::LocalMinute LocalMinuteUtil::parse(dot::string value)
    {
        boost::posix_time::time_input_facet* facet = new boost::posix_time::time_input_facet();
        facet->format("%H:%M");
        std::istringstream stream(*value);
        stream.imbue(std::locale(std::locale::classic(), facet));

        boost::posix_time::ptime ptime;
        stream >> ptime;

        // If default constructed time is passed, error message
        if (ptime == boost::posix_time::not_a_date_time) throw dot::exception(dot::string::format(
            "String representation of default constructed time {0} "
            "passed to LocalMinuteUtil.parse(time) method.", value));

        auto time = ptime.time_of_day();
        return dot::LocalMinute(time.hours(), time.minutes());
    }

    int LocalMinuteUtil::to_iso_int(dot::LocalMinute value)
    {
        // Serialized to one minute precision in ISO 8601 4 digit int hhmm format
        int result = value.hour() * 100 + value.minute();
        return result;
    }

    dot::LocalMinute LocalMinuteUtil::parse_iso_int(int value)
    {
        // Extract
        int hour = value / 100;
        value -= hour * 100;
        int minute = value;

        // Create new LocalMinute object, validates values on input
        return dot::LocalMinute(hour, minute);
    }
}
