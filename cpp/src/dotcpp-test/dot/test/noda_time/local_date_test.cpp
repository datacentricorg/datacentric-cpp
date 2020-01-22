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
        LocalDate default_constructed;
        LocalDate jan_one_year_one_constructed(1970, 1, 1);
        REQUIRE(default_constructed == jan_one_year_one_constructed);
    }

    TEST_CASE("properties")
    {
        LocalDate d(2005, 1, 10);
        REQUIRE(d.year() == 2005);
        REQUIRE(d.month() == 1);
        REQUIRE(d.day() == 10);
        REQUIRE(d.day_of_week() == boost::gregorian::Monday);
        REQUIRE(d.day_of_year() == 10);
    }

    TEST_CASE("methods")
    {
        LocalDate d1(2005, 1, 10);
        REQUIRE(d1.plus_days(10) == LocalDate(2005, 1, 20));
        REQUIRE(d1.plus_weeks(1) == LocalDate(2005, 1, 17));
        REQUIRE(d1.plus_months(1) == LocalDate(2005, 2, 10));
        REQUIRE(d1.plus_years(1) == LocalDate(2006, 1, 10));

        LocalDate d2(2005, 1, 10);
        REQUIRE(d2.previous(boost::gregorian::Monday) == LocalDate(2005, 1, 3));
        REQUIRE(d2.next(boost::gregorian::Monday) == LocalDate(2005, 1, 17));

        LocalDateTime dt3(2005, 5, 10, 12, 10);
        LocalDate d3(2005, 5, 10);
        LocalTime t3(12, 10);
        REQUIRE(d3.at(t3) == dt3);
        REQUIRE(d3.at_midnight() == LocalDateTime(2005, 5, 10, 0, 0));

        LocalDate d4(2005, 1, 2);
        string d4_str = d4.to_string();
        REQUIRE(d4_str == "2005-01-02");
    }

    TEST_CASE("operators")
    {
        LocalDate d(2005, 5, 10);
        LocalDate d1(2005, 5, 10);
        LocalDate d2(2005, 5, 15);
        LocalDate d3(2005, 5, 20);

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
