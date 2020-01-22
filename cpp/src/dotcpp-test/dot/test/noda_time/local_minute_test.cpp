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
#include <dot/noda_time/local_minute.hpp>
#include <dot/noda_time/local_time.hpp>

namespace dot
{
    TEST_CASE("properties")
    {
        LocalMinute t(12, 10);
        REQUIRE(t.hour() == 12);
        REQUIRE(t.minute() == 10);
        REQUIRE(t.minute_of_day() == 730);
    }

    TEST_CASE("methods")
    {
        LocalMinute local_minute1(12, 10);
        LocalMinute local_minute2(12, 10);
        LocalMinute local_minute_after(13, 20);
        LocalMinute local_minute_before(11, 0);

        LocalTime local_time(12, 10);
        REQUIRE(local_minute1.to_local_time() == local_time);
        REQUIRE(local_minute1.to_string() == "12:10");

        REQUIRE(local_minute1.compare_to(local_minute2) == 0);
        REQUIRE(local_minute1.compare_to(local_minute_after) == -1);
        REQUIRE(local_minute1.compare_to(local_minute_before) == 1);

        REQUIRE(local_minute1.hash_code() == local_minute2.hash_code());
        REQUIRE(local_minute1.hash_code() != local_minute_before.hash_code());
        REQUIRE(local_minute1.hash_code() != local_minute_after.hash_code());

        REQUIRE(local_minute1.equals(local_minute2) == true);
        REQUIRE(local_minute1.equals(local_minute_after) == false);
        REQUIRE(local_minute1.equals(local_minute_before) == false);
    }

    TEST_CASE("operators")
    {
        LocalMinute t(12, 0);
        LocalMinute t1(12, 0);
        LocalMinute t2(13, 1);
        LocalMinute t3(14, 2);

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
