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
#include <dot/system/string.hpp>
#include <dot/system/object.hpp>

namespace dot
{
    TEST_CASE("smoke")
    {
        // Create from std::string
        String s1 = std::string("abc");
        REQUIRE(s1 == "abc");
        REQUIRE(s1 == std::string("abc"));

        // Create from string literal
        String s2 = "abc";
        REQUIRE(s2 == "abc");

        // Cast to Object and back
        Object obj1 = s1;
        // TODO String s11 = obj1.cast<String>();
        // TODO  REQUIRE(s11 == "abc");
    }

    TEST_CASE("format")
    {
        int x = 1;
        double y = 2.5;
        const String s = "{0}";

        REQUIRE(String::format(s, x) == "1");
        REQUIRE(String::format("{0:.3f}", y) == "2.500");
        REQUIRE(String::format(s, s) == "{0}");
        REQUIRE(String::format("{0}, {1}", s, s) == "{0}, {0}");

        REQUIRE(String::format(String("{0}, {1}, {2}"), 1, "str1", String("str2")) == "1, str1, str2");
        REQUIRE(String::format("123") == "123");

        REQUIRE(String::format("{0}, {1}, {2}", 'a', 'b', 'c') == "a, b, c");
        REQUIRE(String::format("{}, {}, {}", 'a', 'b', 'c') == "a, b, c");
        REQUIRE(String::format("{2}, {1}, {0}", 'a', 'b', 'c') == "c, b, a");
        REQUIRE(String::format("{0}{1}{0}", "abra", "cad") == "abracadabra");

        REQUIRE(String::format("{:<15}", "left aligned") == "left aligned   ");
        REQUIRE(String::format("{:>15}", "right aligned") == "  right aligned");
        REQUIRE(String::format("{:^16}", "centered") == "    centered    ");
        REQUIRE(String::format("{:*^16}", "centered") == "****centered****");
        REQUIRE(String::format("{:<{}}", "left aligned", 15) == "left aligned   ");

        REQUIRE(String::format("{:.{}f}", 3.14, 1) == "3.1");
        REQUIRE(String::format("{:+f}; {:+f}", 3.14, -3.14) == "+3.140000; -3.140000");
        REQUIRE(String::format("{: f}; {: f}", 3.14, -3.14) == " 3.140000; -3.140000");
        REQUIRE(String::format("{:-f}; {:-f}", 3.14, -3.14) == "3.140000; -3.140000");

        REQUIRE(String::format("int: {0:d};  hex: {0:x};  oct: {0:o}; bin: {0:b}", 42) == "int: 42;  hex: 2a;  oct: 52; bin: 101010");
        REQUIRE(String::format("int: {0:d};  hex: {0:#x};  oct: {0:#o};  bin: {0:#b}", 42) == "int: 42;  hex: 0x2a;  oct: 052;  bin: 0b101010");
        REQUIRE(String::format("{:#04x}", 0) == "0x00");
    }

    TEST_CASE("compare")
    {
        // Compare to literal strings
        String str = "abcd";
        REQUIRE(str == "abcd");

        // Check comparison case sensitivity
        REQUIRE_FALSE(str == "Abcd");

        // Compare two strings that have the same value but are not the same instances
        String str2 = "abcd";
        REQUIRE(str == str2);

        // Compare two strings that have the same value but are not
        // the same instances after casting one or both to Object
        REQUIRE(str == Object(str2));
        // TODO - fix by implemnenting equals(...) REQUIRE((Object)str == str2);
        // TODO - fix by implemnenting equals(...) REQUIRE((Object)str == (Object)str2);
    }

    TEST_CASE("methods")
    {
        // To lower/upper case
        {
            String str = "ABCabc123_!@#$%^&*()=+-:;";
            String lower_str = str->to_lower();
            REQUIRE(lower_str == "abcabc123_!@#$%^&*()=+-:;");
            String upper_str = str->to_upper();
            REQUIRE(upper_str == "ABCABC123_!@#$%^&*()=+-:;");
            REQUIRE(str == "ABCabc123_!@#$%^&*()=+-:;");
        }

        // Contains
        {
            String str = "string";
            String substr1 = "str";
            String substr2 = "ing";
            String substr3 = "rin";
            String substr4 = "abc";

            REQUIRE(str->contains(substr1));
            REQUIRE(str->contains(substr2));
            REQUIRE(str->contains(substr3));
            REQUIRE(!str->contains(substr4));
        }

        // Split
        {
            String str = ";ab;ef;";
            List<String> seplited_str = make_list<String>({ "", "ab", "ef", "" });

            List<String> res_list = str->split(';');
            REQUIRE(res_list->count() == seplited_str->count());
            REQUIRE(res_list[0] == seplited_str[0]);
            REQUIRE(res_list[1] == seplited_str[1]);
            REQUIRE(res_list[2] == seplited_str[2]);
            REQUIRE(res_list[3] == seplited_str[3]);

            str = "42, 12, 19";
            seplited_str = make_list<String>({ "42", "", "12", "", "19" });
            res_list = str->split(" ,");
            REQUIRE(res_list->count() == seplited_str->count());
            REQUIRE(res_list[0] == seplited_str[0]);
            REQUIRE(res_list[1] == seplited_str[1]);
            REQUIRE(res_list[2] == seplited_str[2]);
            REQUIRE(res_list[3] == seplited_str[3]);

            str = "42..12..19";
            seplited_str = make_list<String>({ "42", "", "12", "", "19" });
            res_list = str->split(".");
            REQUIRE(res_list->count() == seplited_str->count());
            REQUIRE(res_list[0] == seplited_str[0]);
            REQUIRE(res_list[1] == seplited_str[1]);
            REQUIRE(res_list[2] == seplited_str[2]);
            REQUIRE(res_list[3] == seplited_str[3]);
        }

        // Trim
        {
            String str = "";
            REQUIRE(str->trim() == "");
            REQUIRE(str->trim_start() == "");
            REQUIRE(str->trim_end() == "");

            str = " \n\r\t\v\f ";
            REQUIRE(str->trim() == "");
            REQUIRE(str->trim_start() == "");
            REQUIRE(str->trim_end() == "");

            str = " \n ab cd \n ";
            REQUIRE(str->trim() == "ab cd");
            REQUIRE(str->trim_start() == "ab cd \n ");
            REQUIRE(str->trim_end() == " \n ab cd");
        }
    }
}
