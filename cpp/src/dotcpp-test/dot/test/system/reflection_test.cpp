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
#include <dot/system/exception.hpp>
#include <dot/system/reflection/method_info.hpp>
#include <dot/system/reflection/constructor_info.hpp>
#include <dot/system/type.hpp>
#include <dot/system/collections/generic/list.hpp>

namespace dot
{
    static std::stringstream received;

    class reflection_base_sample_impl : public virtual object_impl
    {
        typedef reflection_base_sample_impl self;

    public: // FIELDS

        int int_field;
        int count;
        double double_field;
        list<double> double_list_field_field;

    private:

        int private_int_field = 42;

    public: // METHODS

        int sample_method(int param)
        {
            received << "invoked reflection_base_sample.sample_method";
            return 42 + param;
        }

        type get_type() override
        {
            // Create type object with thread safety guarantee as per C++ Standard
            static type result = []()->type
            {
                received << "creating type object (this should run only once)." << std::endl;

                return make_type_builder<reflection_base_sample_impl>("dot", "reflection_base_sample")
                    ->with_field("int_field", &reflection_base_sample_impl::int_field)
                    ->with_field("private_int_field", &reflection_base_sample_impl::private_int_field)
                    ->with_field("count", &reflection_base_sample_impl::count)
                    ->with_method("sample_method", &reflection_base_sample_impl::sample_method, { "param" })
                    ->build();
            }();

            return result;
        }
    };

    using reflection_base_sample = ptr<reflection_base_sample_impl>;
    reflection_base_sample make_reflection_base_sample() { return new reflection_base_sample_impl; }

    class reflection_derived_sample_impl : public reflection_base_sample_impl
    {
    public: // FIELDS

        int int_field_in_derived_class;
    };

    using reflection_derived_sample = ptr<reflection_derived_sample_impl>;
    reflection_derived_sample make_reflection_derived_sample() { return new reflection_derived_sample_impl; }

    TEST_CASE("property_info")
    {
        reflection_base_sample obj = make_reflection_base_sample();
        obj->int_field = 15;
        obj->count = 15;

        object x = obj->count;

        type result = obj->get_type();
        list<field_info> props = result->get_fields();
        field_info int_prop = props[0];
        REQUIRE(int_prop->name() == "int_field");
        REQUIRE(int(int_prop->get_value(obj)) == 15);

        int_prop->set_value(obj, 19);
        REQUIRE(obj->int_field == 19);
        REQUIRE(int(int_prop->get_value(obj)) == 19);

        field_info private_int_prop = props[1];
        REQUIRE(private_int_prop->name() == "private_int_field");
        REQUIRE(int(private_int_prop->get_value(obj)) == 42);

        props[2]->set_value(obj, 2384);
        REQUIRE(obj->count == 2384);
        REQUIRE(int(props[2]->get_value(obj)) == 2384);

        reflection_derived_sample obj2 = make_reflection_derived_sample();

        props[2]->set_value(obj2, -15);
        REQUIRE(obj2->count == -15);
        REQUIRE(int(props[2]->get_value(obj2)) == -15);

        list<object> params = make_list<object>(1);
        params[0] = 15;
        REQUIRE(int(result->get_methods()[0]->invoke(obj2, params)) == 42 + 15);

        Approvals::verify(received.str());
        received.clear();
    }
}
