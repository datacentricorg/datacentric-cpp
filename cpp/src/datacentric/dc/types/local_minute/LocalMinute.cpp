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

#include <dc/implement.hpp>
#include <dc/types/local_minute/LocalMinute.hpp>
#include <dot/system/Exception.hpp>
#include <dot/system/String.hpp>

namespace dc
{
    dot::local_minute LocalMinuteHelper::Parse(dot::string value)
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
            "passed to LocalMinute.Parse(time) method.", value));

        auto time = ptime.time_of_day();
        return dot::local_minute(time.hours(), time.minutes());
    }

    int LocalMinuteHelper::ToIsoInt(dot::local_minute value)
    {
        // Serialized to one minute precision in ISO 8601 4 digit int hhmm format
        int result = value.Hour * 100 + value.Minute;
        return result;
    }

    dot::local_minute LocalMinuteHelper::ParseIsoInt(int value)
    {
        // Extract
        int hour = value / 100;
        value -= hour * 100;
        int minute = value;

        // Create new LocalMinute object, validates values on input
        return dot::local_minute(hour, minute);
    }
}