﻿/*
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
#include <dot/system/text/string_builder.hpp>
#include <dot/system/exception.hpp>

namespace dot
{
    TEST_CASE("smoke")
    {
        int x = 1;
        double y = 2.5;
        const String s = "{0}";

        StringBuilder result = make_string_builder();
        result->append("abc");
        result->append(123);
        result->append_line();
        result->append_line("xyz");
        result->append_line();

        result->append_format(s, x);
        result->append_line();
        result->append_line(String::format("{0:.3f}", y));
        result->append_line(String::format("{0}, {1}", "123", "456"));

        Approvals::verify(*result);
    }
}
