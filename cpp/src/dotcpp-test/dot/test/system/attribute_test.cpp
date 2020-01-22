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
    class TestAttributeImpl; using TestAttribute = Ptr<TestAttributeImpl>;

    /// Test attribute class.
    class TestAttributeImpl : public AttributeImpl
    {
        friend TestAttribute make_test_attribute(String);

    private: // FIELDS

        String message_;

    protected:

        /// Create with message.
        TestAttributeImpl(String message)
            : message_(message)
        {}

    public:

        /// Returns attribute message.
        String get_message() { return message_; }

    public: // REFLECTION

        Type get_type() override { return typeof(); }

        static Type typeof()
        {
            static Type result = []()->Type
            {
                return make_type_builder<TestAttributeImpl>("dot", "TestAttribute")
                    ->with_method("get_message", &TestAttributeImpl::get_message, {})
                    ->build();
            }();

            return result;
        }
    };

    inline TestAttribute make_test_attribute(String message) { return new TestAttributeImpl(message); }

    class TestClassImpl; using TestClass = Ptr<TestClassImpl>;
    TestClass make_test_class();

    inline TestClass make_test_class();

    /// Test class with attributes.
    class TestClassImpl : public ObjectImpl
    {
        friend TestClass make_test_class();

    public: // FIELDS

        /// Int field.
        int int_field;

    public: // METHODS

        /// Function without parameters.
        int get_int_field() { return int_field; }

        /// Function with parameters.
        int calculate_sum(int a, int b) { return a + b; }

        /// Static function.
        static int static_func() { return 10; }

    public: // CONSTRUCTORS

        /// Constrictor.
        TestClassImpl() = default;

    public: // REFLECTION

        Type get_type() override { return typeof(); }

        static Type typeof()
        {
            static Type result = []()->Type
            {
                return make_type_builder<TestClassImpl>("dot", "TestClass", { make_test_attribute("class attribute") })
                    ->with_field("int_field", &TestClassImpl::int_field, { make_test_attribute("class field attribute") })
                    ->with_method("get_int_field", &TestClassImpl::get_int_field, {}, { make_test_attribute("class get_int_field method attribute") })
                    ->with_method("calculate_sum", &TestClassImpl::calculate_sum, { "a", { "b", make_test_attribute("method parameter b attribute") } })
                    ->with_method("static_func", &TestClassImpl::static_func, {}, { make_test_attribute("class static_func method attribute") })
                    ->with_constructor(&make_test_class, {}, { make_test_attribute("class constructor attribute") })
                    ->build();
            }();

            return result;
        }
    };

    inline TestClass make_test_class() { return new TestClassImpl; }


    TEST_CASE("attributes reflection")
    {
        Attribute attr;

        // Create Object
        Object obj = make_test_class();
        Type obj_type = obj->get_type();

        // Get class attributes
        List<Attribute> class_attributes = obj_type->get_custom_attributes(false);
        REQUIRE(class_attributes->count() == 1);

        attr = class_attributes[0];
        REQUIRE(attr.is<TestAttribute>());
        TestAttribute class_attribute = (TestAttribute) attr;
        REQUIRE(class_attribute->get_message() == "class attribute");
        REQUIRE(attr->get_type()->get_method("get_message")->invoke(attr, make_list<Object>())->equals("class attribute"));

        // Get class field attributes
        List<Attribute> field_attributes = obj_type->get_field("int_field")->get_custom_attributes(false);
        REQUIRE(field_attributes->count() == 1);

        attr = field_attributes[0];
        REQUIRE(attr.is<TestAttribute>());
        TestAttribute field_attribute = (TestAttribute) attr;
        REQUIRE(field_attribute->get_message() == "class field attribute");

        // Get class method attributes
        List<Attribute> method_attributes = obj_type->get_method("get_int_field")->get_custom_attributes(false);
        REQUIRE(method_attributes->count() == 1);

        attr = method_attributes[0];
        REQUIRE(attr.is<TestAttribute>());
        TestAttribute method_attribute = (TestAttribute) attr;
        REQUIRE(method_attribute->get_message() == "class get_int_field method attribute");

        // Get class method attributes
        List<Attribute> method2_attributes = obj_type->get_method("calculate_sum")->get_custom_attributes(false);
        REQUIRE(method2_attributes->count() == 0);

        // Get class method parameters attributes
        List<ParameterInfo> method_parameters = obj_type->get_method("calculate_sum")->get_parameters();
        REQUIRE(method_parameters->count() == 2);

        List<Attribute> method_parameter_attributes = method_parameters[0]->get_custom_attributes(false);
        REQUIRE(method_parameter_attributes->count() == 0);

        method_parameter_attributes = method_parameters[1]->get_custom_attributes(false);
        REQUIRE(method_parameter_attributes->count() == 1);

        attr = method_parameter_attributes[0];
        REQUIRE(attr.is<TestAttribute>());
        TestAttribute method_parameter_attribute = (TestAttribute) attr;
        REQUIRE(method_parameter_attribute->get_message() == "method parameter b attribute");

        // Get class static method attributes
        List<Attribute> static_method_attributes = obj_type->get_method("static_func")->get_custom_attributes(false);
        REQUIRE(static_method_attributes->count() == 1);

        attr = static_method_attributes[0];
        REQUIRE(attr.is<TestAttribute>());
        TestAttribute static_method_attribute = (TestAttribute) attr;
        REQUIRE(static_method_attribute->get_message() == "class static_func method attribute");

        // Get class constructor attributes
        List<constqructor_info> class_constructors = obj_type->get_constructors();
        REQUIRE(class_constructors->count() == 1);

        List<Attribute> constructor_attributes = class_constructors[0]->get_custom_attributes(false);
        REQUIRE(constructor_attributes->count() == 1);

        attr = constructor_attributes[0];
        REQUIRE(attr.is<TestAttribute>());
        TestAttribute constructor_attribute = (TestAttribute)attr;
        REQUIRE(constructor_attribute->get_message() == "class constructor attribute");
    }
}
