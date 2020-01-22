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
#include <dot/system/collections/generic/list.hpp>
#include <dot/system/string.hpp>
#include <dot/system/type.hpp>

namespace dot
{
    static std::stringstream received;

    /// Print double vector to received on one line in JSON format.
    static void print_list(string name, list<double> v)
    {
        received << "\"" << *name << "\" : [ ";
        for (int i = 0; i < v->count(); ++i)
        {
            if (i > 0) received << ", ";
            received << v[i];
        }
        received << " ]" << std::endl;
    }

    TEST_CASE("smoke")
    {
        list<double> a = make_list<double>();
        a->add(0.0);
        a->add(1.0);
        a->add(2.0);

        REQUIRE(a->count() == 3);

    }

    TEST_CASE("interfaces")
    {
        list<double> a = make_list<double>();

        // Add elements to list interface
        list<double> b = a;
        b->add(2.0);
        b->add(1.0);
        b->add(0.0);

        // Check size of the original class and the interface
        REQUIRE(a->count() == 3);
        REQUIRE(b->count() == 3);

        // Access the underlying std::vector<double> class
        print_list("unsorted", a);
        std::sort(a->begin(), a->end());
        print_list("sorted", a);

        // Access by object
        object obj = b;
        // TODO - uncomment next line when output matches C#
        // received << obj->to_string() << std::endl;

        Approvals::verify(received.str());
        received.clear();
    }

    TEST_CASE("iterators")
    {
        list<string> string_list = make_list<string>();
        string_list->add("000");
        string_list->add("111");
        string_list->add("222");
        REQUIRE(string_list->count() == 3);

        int i = 0;
        for (string str : string_list)
        {
            REQUIRE(string_list[i++] == str);
        }
    }

    TEST_CASE("capacity")
    {
        list<string> string_list = make_list<string>();
        string_list->set_capacity(100);
        REQUIRE(string_list->capacity() == 100);
    }

    TEST_CASE("find")
    {
        dot::list<string> string_list = make_list<string>();
        string_list->add("000");
        string_list->add("111");
        string_list->add("222");
        string_list->add("333");
        string_list->add("444");

        // TODO string_list.find_last([](std::string const& s) { return s == "222"; }) = "57";
        // TODO REQUIRE(string_list.find_last_index([](std::string const& s) { return s == "111"; }) == 0);
    }

    TEST_CASE("methods")
    {
        // Add range
        list<int> int_list = make_list<int>();

        int_list->add_range({1, 2});
        REQUIRE(int_list->count() == 2);
        REQUIRE(int_list[0] == 1);
        REQUIRE(int_list[1] == 2);

        int_list->add_range(make_list<int>({ 3, 4 }));
        REQUIRE(int_list->count() == 4);
        REQUIRE(int_list[0] == 1);
        REQUIRE(int_list[1] == 2);
        REQUIRE(int_list[2] == 3);
        REQUIRE(int_list[3] == 4);
    }
}
