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

#include <dot/test/implement.hpp>
#include <approvals/ApprovalTests.hpp>
#include <approvals/Catch.hpp>
#include <dot/system/string.hpp>
#include <dot/noda_time/period.hpp>
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_date.hpp>
#include <dot/noda_time/local_date_time.hpp>

namespace dot
{
    TEST_CASE("constructors")
    {
        LocalDateTime default_constructed;
        LocalDateTime jan_one_year_one_constructed(1970, 1, 1, 0, 0, 0);
        REQUIRE(default_constructed == jan_one_year_one_constructed);
    }

    TEST_CASE("properties")
    {
        LocalDateTime d(2005, 1, 10, 12, 10, 20, 30);
        REQUIRE((LocalDate)d.date() == LocalDate(2005, 1, 10));
        REQUIRE(d.year() == 2005);
        REQUIRE(d.month() == 1);
        REQUIRE(d.day() == 10);
        REQUIRE(d.day_of_week() == boost::gregorian::Monday);
        REQUIRE(d.day_of_year() == 10);
        REQUIRE((LocalTime)d.time_of_day() == LocalTime(12, 10, 20, 30));
        REQUIRE(d.hour() == 12);
        REQUIRE(d.minute() == 10);
        REQUIRE(d.second() == 20);
        REQUIRE(d.millisecond() == 30);
    }

    TEST_CASE("methods")
    {
        LocalDateTime dt1(2005, 1, 10, 12, 10, 20, 30);
        REQUIRE(dt1.plus_days(10) == LocalDateTime(2005, 1, 20, 12, 10, 20, 30));
        REQUIRE(dt1.plus_weeks(1) == LocalDateTime(2005, 1, 17, 12, 10, 20, 30));
        REQUIRE(dt1.plus_months(1) == LocalDateTime(2005, 2, 10, 12, 10, 20, 30));
        REQUIRE(dt1.plus_years(1) == LocalDateTime(2006, 1, 10, 12, 10, 20, 30));
        REQUIRE(dt1.plus_hours(1) == LocalDateTime(2005, 1, 10, 13, 10, 20, 30));
        REQUIRE(dt1.plus_minutes(1) == LocalDateTime(2005, 1, 10, 12, 11, 20, 30));
        REQUIRE(dt1.plus_seconds(1) == LocalDateTime(2005, 1, 10, 12, 10, 21, 30));
        REQUIRE(dt1.plus_milliseconds(1) == LocalDateTime(2005, 1, 10, 12, 10, 20, 31));

        LocalDateTime dt2(2005, 1, 10, 12, 10);
        REQUIRE(dt2.previous(boost::gregorian::Monday) == LocalDateTime(2005, 1, 3, 12, 10));
        REQUIRE(dt2.next(boost::gregorian::Monday) == LocalDateTime(2005, 1, 17, 12, 10));

        LocalDateTime dt3(2005, 1, 2, 3, 4, 5, 6);
        String dt3_str = dt3.to_string();
        REQUIRE(dt3_str == "2005-01-02 03:04:05.006");
    }

    TEST_CASE("operators")
    {
        LocalDateTime d(2005, 5, 10, 12, 0, 0, 0);
        LocalDateTime d1(2005, 5, 10, 12, 0, 0, 0);
        LocalDateTime d2(2005, 5, 15, 13, 1, 1, 1);
        LocalDateTime d3(2005, 5, 20, 14, 2, 2, 2);

        Period p1 = Period::between(d1, d2);
        Period p2 = Period::between(d2, d3);

        REQUIRE(d2 + p1 == d3);
        REQUIRE(d2 - d1 == p2);
        REQUIRE(d3 - p2 == d2);

        REQUIRE(d1 == d);
        REQUIRE(d1 != d2);
        REQUIRE(d2 != d3);

        REQUIRE(d1 <= d1);
        REQUIRE(d1 <= d2);
        REQUIRE(d2 <= d3);

        REQUIRE(d1 < d2);
        REQUIRE(d1 < d3);
        REQUIRE(d2 < d3);

        REQUIRE(d1 >= d1);
        REQUIRE(d2 >= d1);
        REQUIRE(d3 >= d2);

        REQUIRE(d2 > d1);
        REQUIRE(d3 > d1);
        REQUIRE(d3 > d2);
    }
}
