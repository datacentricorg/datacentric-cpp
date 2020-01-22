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
    TEST_CASE("create")
    {
        {
            LocalDateTime date1(2005, 1, 1, 0, 0);
            LocalDateTime date2(2006, 1, 1, 0, 0);

            Period p = Period::between(date1, date2);
            REQUIRE(p.days() == 365);
        }

        {
            LocalDateTime date1(2005, 1, 1, 1, 11, 11, 111);
            LocalDateTime date2(2005, 1, 10, 2, 22, 22, 222);
            Period p = Period::between(date1, date2);
            REQUIRE(p.days() == 9);
            REQUIRE(p.hours() == 1);
            REQUIRE(p.minutes() == 11);
            REQUIRE(p.seconds() == 11);
            REQUIRE(p.milliseconds() == 111);
        }

        {
            LocalDate date1(2005, 1, 1);
            LocalDate date2(2005, 1, 10);
            Period p = Period::between(date1, date2);
            REQUIRE(p.days() == 9);
            REQUIRE(p.hours() == 0);
            REQUIRE(p.minutes() == 0);
            REQUIRE(p.seconds() == 0);
            REQUIRE(p.milliseconds() == 0);

        }

        {
            LocalTime time1(1, 11, 11, 111);
            LocalTime time2(2, 22, 22, 222);
            Period p = Period::between(time2, time1);
            REQUIRE(p.days() == 0);
            REQUIRE(p.hours() == -1);
            REQUIRE(p.minutes() == -11);
            REQUIRE(p.seconds() == -11);
            REQUIRE(p.milliseconds() == -111);

        }

        {
            Period p = Period::from_weeks(1);
            REQUIRE(p.days() == 7);

            p = Period::from_days(1);
            REQUIRE(p.days() == 1);

            p = Period::from_hours(1);
            REQUIRE(p.hours() == 1);

            p = Period::from_minutes(1);
            REQUIRE(p.minutes() == 1);

            p = Period::from_seconds(1);
            REQUIRE(p.seconds() == 1);

            p = Period::from_milliseconds(1);
            REQUIRE(p.milliseconds() == 1);
        }
    }

    TEST_CASE("methods")
    {
        LocalDateTime date1(2005, 1, 1, 0, 0, 0, 0);
        LocalDateTime date2(2005, 1, 2, 1, 1, 1, 1);
        LocalDateTime date3(2005, 1, 3, 2, 2, 2, 2);
        Period p1 = Period::between(date1, date2);
        Period p2 = Period::between(date2, date3);
        Period p3 = Period::between(date1, date3);

        REQUIRE(p1 == p2);
        REQUIRE(p1 + p2 == p3);
        REQUIRE(p3 - p2 == p1);
        REQUIRE(p3 - p1 == p2);
    }
}
