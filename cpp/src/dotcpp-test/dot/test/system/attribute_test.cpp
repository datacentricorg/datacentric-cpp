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

#include <dot/system/attribute.hpp>
#include <dot/system/type.hpp>

namespace dot
{
    class test_attribute_impl; using test_attribute = ptr<test_attribute_impl>;

    /// Test attribute class.
    class test_attribute_impl : public attribute_impl
    {
        friend test_attribute make_test_attribute(string);

    private: // FIELDS

        string message_;

    protected:

        /// Create with message.
        test_attribute_impl(string message)
            : message_(message)
        {}

    public:

        /// Returns attribute message.
        string get_message() { return message_; }

    public: // REFLECTION

        type get_type() override { return typeof(); }

        static type typeof()
        {
            static type result = []()->type
            {
                return make_type_builder<test_attribute_impl>("dot", "test_attribute")
                    ->with_method("get_message", &test_attribute_impl::get_message, {})
                    ->build();
            }();

            return result;
        }
    };

    test_attribute make_test_attribute(string message) { return new test_attribute_impl(message); }

    class test_class_impl; using test_class = ptr<test_class_impl>;

    /// Test class with attributes.
    class test_class_impl : public object_impl
    {
    public: // FIELDS

        int int_field;

    public: // METHODS

        int get_int_field() { return int_field; }

        int calculate_sum(int a, int b) { return a + b; }

    public: // REFLECTION

        type get_type() override { return typeof(); }

        static type typeof()
        {
            static type result = []()->type
            {
                return make_type_builder<test_class_impl>("dot", "test_class", { make_test_attribute("class attribute") })
                    //->with_field("message", &test_attribute_impl::get_message)
                    //->with_method("get_message", &test_attribute_impl::get_message, {})
                    ->build();
            }();

            return result;
        }
    };

    test_class make_test_class() { return new test_class_impl; }


    TEST_CASE("attributes reflection")
    {
        attribute attr;

        // Create object
        object obj = make_test_class();
        type obj_type = obj->get_type();

        // Get class attributes
        list<attribute> class_attributes = obj_type->get_custom_attributes();
        REQUIRE(class_attributes->count() == 1);

        attr = class_attributes[0];
        REQUIRE(attr.is<test_attribute>());
        test_attribute class_attribute = (test_attribute) attr;
        REQUIRE(class_attribute->get_message() == "class attribute");
        REQUIRE(attr->get_type()->get_method("get_message")->invoke(attr, make_list<object>())->equals("class attribute"));
    }
}
