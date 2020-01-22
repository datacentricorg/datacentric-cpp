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
#include <dot/system/byte_array.hpp>

namespace dot
{
    TEST_CASE("construction")
    {
        byte_array bytes;

        // Empty
        bytes = make_byte_array(3);
        REQUIRE(bytes->get_length() == 3);
        REQUIRE(bytes[0] == 0);
        REQUIRE(bytes[1] == 0);
        REQUIRE(bytes[2] == 0);

        // From byte array
        char arr[3] = { 0x0, 0xff, 0x80 };
        bytes = make_byte_array(arr, 3);
        REQUIRE(bytes->get_length() == 3);
        REQUIRE((uint8_t) bytes[0] == 0x0);
        REQUIRE((uint8_t) bytes[1] == 0xff);
        REQUIRE((uint8_t) bytes[2] == 0x80);

        REQUIRE(bytes->get_data()[0] == arr[0]);
        REQUIRE(bytes->get_data()[1] == arr[1]);
        REQUIRE(bytes->get_data()[2] == arr[2]);

        // From vector
        bytes = make_byte_array(std::vector<char>({ (char) 0x0, (char) 0xff, (char) 0x80 }));
        REQUIRE(bytes->get_length() == 3);
        REQUIRE((uint8_t) bytes[0] == 0x0);
        REQUIRE((uint8_t) bytes[1] == 0xff);
        REQUIRE((uint8_t) bytes[2] == 0x80);

        // From initializer_list
        bytes = make_byte_array({ 0x0, 0xff, 0x80 });
        REQUIRE(bytes->get_length() == 3);
        REQUIRE(bytes->get(0) == 0x0);
        REQUIRE(bytes->get(1) == 0xff);
        REQUIRE(bytes->get(2) == 0x80);
    }

    TEST_CASE("methods")
    {
        // Compare
        {
            byte_array bytes1 = make_byte_array({ 10, 100, 50 });
            byte_array bytes2 = make_byte_array({ 10, 70, 70 });
            byte_array bytes3 = make_byte_array({ 10 });
            REQUIRE(bytes1->compare(bytes1) == 0);
            REQUIRE(bytes1->compare(bytes2) > 0);
            REQUIRE(bytes2->compare(bytes1) < 0);
            CHECK_THROWS(bytes1->compare(bytes3));
        }

        // Copy
        {
            byte_array bytes = make_byte_array({ 1, 2, 3, 4, 5 });
            byte_array bytes_src = make_byte_array({ 20, 30, 40 });
            bytes->copy(1, bytes_src);

            REQUIRE(bytes->get(0) == 1);
            REQUIRE(bytes->get(1) == 20);
            REQUIRE(bytes->get(2) == 30);
            REQUIRE(bytes->get(3) == 40);
            REQUIRE(bytes->get(4) == 5);
        }

        {
            byte_array bytes = make_byte_array({ 1, 2, 3, 4, 5 });
            byte_array bytes_src = make_byte_array({ 20, 30, 40 });
            bytes->copy(1, bytes_src, 1, 2);

            REQUIRE(bytes->get(0) == 1);
            REQUIRE(bytes->get(1) == 30);
            REQUIRE(bytes->get(2) == 40);
            REQUIRE(bytes->get(3) == 4);
            REQUIRE(bytes->get(4) == 5);
        }

        {
            byte_array bytes = make_byte_array({ 1, 2, 3, 4, 5 });
            byte_array bytes_src = make_byte_array({ 20, 30, 40 });

            CHECK_THROWS(bytes->copy(1, bytes_src, 1, 3));
            CHECK_THROWS(bytes->copy(3, bytes_src));
        }

        // Copy value
        {
            int32_t value = 0xa1ff8020;
            byte_array bytes = make_byte_array(4);
            bytes->copy_value(value);
            REQUIRE(bytes->get(0) == 0xa1);
            REQUIRE(bytes->get(1) == 0xff);
            REQUIRE(bytes->get(2) == 0x80);
            REQUIRE(bytes->get(3) == 0x20);

            int32_t value2 = bytes->to_primitive<int32_t>();
            REQUIRE(value == value2);

            CHECK_THROWS(bytes->copy_value(1, value));
            CHECK_THROWS(bytes->to_primitive<int32_t>(1));
        }

        {
            uint32_t value = 0xa1ff8020;
            byte_array bytes = make_byte_array(6);
            bytes->copy_value(1, value);
            REQUIRE(bytes->get(1) == 0xa1);
            REQUIRE(bytes->get(2) == 0xff);
            REQUIRE(bytes->get(3) == 0x80);
            REQUIRE(bytes->get(4) == 0x20);

            uint32_t value2 = bytes->to_primitive<uint32_t>(1);
            REQUIRE(value == value2);
        }
    }
}
