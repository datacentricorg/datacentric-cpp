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
#include <dc/types/local_date/LocalDate.hpp>
#include <dot/system/Exception.hpp>
#include <dot/system/String.hpp>

namespace dc
{
    dot::local_date LocalDateHelper::Parse(dot::string value)
    {
        boost::posix_time::time_input_facet* facet = new boost::posix_time::time_input_facet();
        facet->format("%Y-%m-%d");
        std::istringstream stream(*value);
        stream.imbue(std::locale(std::locale::classic(), facet));

        boost::posix_time::ptime ptime;
        stream >> ptime;

        // If default constructed date is passed, error message
        if (ptime == boost::posix_time::not_a_date_time) throw dot::new_Exception(dot::string::Format(
            "String representation of default constructed date {0} "
            "passed to LocalDate.Parse(date) method.", value));

        return ptime.date();
    }

    int LocalDateHelper::ToIsoInt(dot::local_date value)
    {
        int result = value.getYear() * 10'000 + value.getMonth() * 100 + value.getDay();
        return result;
    }

    dot::local_date LocalDateHelper::ParseIsoInt(int value)
    {
        // Extract year, month, day
        int year = value / 100'00;
        value -= year * 100'00;
        int month = value / 100;
        value -= month * 100;
        int day = value;

        // Create LocalDate object
        return dot::local_date(year, month, day);
    }
}
