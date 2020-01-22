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
        LocalTime default_constructed;
        LocalTime jan_one_year_one_constructed(0, 0, 0);
        REQUIRE(default_constructed == jan_one_year_one_constructed);
    }

    TEST_CASE("properties")
    {
        LocalTime t(12, 10, 20, 30);
        REQUIRE(t.hour() == 12);
        REQUIRE(t.minute() == 10);
        REQUIRE(t.second() == 20);
        REQUIRE(t.millisecond() == 30);
    }

    TEST_CASE("methods")
    {
        LocalTime t1(12, 10, 20, 30);
        REQUIRE(t1.plus_hours(1) == LocalTime(13, 10, 20, 30));
        REQUIRE(t1.plus_minutes(1) == LocalTime(12, 11, 20, 30));
        REQUIRE(t1.plus_seconds(1) == LocalTime(12, 10, 21, 30));
        REQUIRE(t1.plus_milliseconds(1) == LocalTime(12, 10, 20, 31));

        LocalTime t2(23, 59);
        REQUIRE(t2.plus_hours(2) == LocalTime(1, 59));
        REQUIRE(t2.plus_minutes(2) == LocalTime(0, 1));

        LocalDateTime dt3(2005, 5, 10, 12, 10);
        LocalDate d3(2005, 5, 10);
        LocalTime t3(12, 10);
        REQUIRE(t3.on(d3) == dt3);

        LocalTime t4(1, 2, 3, 4);
        String t4_str = t4.to_string();
        REQUIRE(t4_str == "01:02:03.004");
    }

    TEST_CASE("milliseconds")
    {
        LocalTime t1(0, 0, 1, 500);
        Period p1 = Period::from_milliseconds(600);
        LocalTime r1 = t1 + p1;
        LocalTime t3(0, 0, 2, 100);
        REQUIRE(r1 == t3);
        REQUIRE(r1.millisecond() == 100);
    }

    TEST_CASE("operators")
    {
        LocalTime t(12, 0, 0, 0);
        LocalTime t1(12, 0, 0, 0);
        LocalTime t2(13, 1, 1, 1);
        LocalTime t3(14, 2, 2, 2);
        Period p1 = Period::between(t1, t2);
        Period p2 = Period::between(t2, t3);

        REQUIRE(t2 + p1 == t3);
        REQUIRE(t2 - t1 == p2);
        REQUIRE(t3 - p2 == t2);

        REQUIRE(t1 == t);
        REQUIRE(t1 != t2);
        REQUIRE(t2 != t3);

        REQUIRE(t1 <= t1);
        REQUIRE(t1 <= t2);
        REQUIRE(t2 <= t3);

        REQUIRE(t1 < t2);
        REQUIRE(t1 < t3);
        REQUIRE(t2 < t3);

        REQUIRE(t1 >= t1);
        REQUIRE(t2 >= t1);
        REQUIRE(t3 >= t2);

        REQUIRE(t2 > t1);
        REQUIRE(t3 > t1);
        REQUIRE(t3 > t2);
    }
}
