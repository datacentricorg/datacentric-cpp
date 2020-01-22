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
#include <dot/system/collections/generic/dictionary.hpp>
#include <dot/system/type.hpp>

namespace dot
{
    Dictionary<String, String> create_dictionary()
    {
        Dictionary<String, String> dict = make_dictionary<String, String>();

        dict->add("a", "b");
        dict->add(std::pair<String, String>("c", "d"));
        dict->add("", "");

        return dict;
    }

    TEST_CASE("properties")
    {
        Dictionary<String, String> dict = create_dictionary();

        REQUIRE(dict->count() == 3);

        // Keys
        List<String> keys = dict->keys();
        REQUIRE(keys->count() == 3);
        REQUIRE(keys->contains("a"));
        REQUIRE(keys->contains("c"));
        REQUIRE(keys->contains(""));

        // Values
        List<String> values = dict->values();
        REQUIRE(values->count() == 3);
        REQUIRE(values->contains("b"));
        REQUIRE(values->contains("d"));
        REQUIRE(values->contains(""));
    }

    TEST_CASE("methods")
    {
        Dictionary<String, String> dict = create_dictionary();

        // Get
        REQUIRE(dict["a"] == "b");
        REQUIRE(dict["c"] == "d");
        REQUIRE(dict[""] == "");

        String s = "";
        dict->try_get_value("b", s);
        REQUIRE(s == "");
        dict->try_get_value("a", s);
        REQUIRE(s == "b");

        REQUIRE(dict->contains_key("a"));
        REQUIRE(dict->contains_key("c"));
        REQUIRE(dict->contains_key(""));
        REQUIRE(dict->contains_key("b") == false);

        REQUIRE(dict->contains_value("b"));
        REQUIRE(dict->contains_value("d"));
        REQUIRE(dict->contains_value(""));
        REQUIRE(dict->contains_value("a") == false);

        // Remove
        dict->remove("a");
        REQUIRE(dict->count() == 2);
        REQUIRE(dict->contains_key("a") == false);
        REQUIRE(dict->contains_value("b") == false);

        // Clear
        dict->clear();
        REQUIRE(dict->count() == 0);
    }

    TEST_CASE("interfaces")
    {
        Dictionary<String, String> dict = make_dictionary<String, String>();
        dict->add("a", "b");
        dict->add("c", "d");
        dict->add("e", "f");

        REQUIRE(dict["a"] == "b");
        REQUIRE(dict["c"] == "d");
        REQUIRE(dict["e"] == "f");
    }

    TEST_CASE("object key")
    {
        Object obj0 = make_object();
        Object obj1 = make_object();
        Object obj2 = Object("str2");
        Object obj3 = Object("str2");

        Dictionary<Object, String> dict = make_dictionary<Object, String>();
        dict->add(obj0, "val0");
        CHECK_NOTHROW(dict->add(obj1, "val1"));

        dict->add(obj2, "val2");
        CHECK_THROWS(dict->add(obj3, "val3"));

        REQUIRE(dict[obj0] == "val0");
        REQUIRE(dict[obj1] == "val1");
        REQUIRE(dict[obj2] == "val2");
        REQUIRE(dict[obj3] == "val2");
    }
}
