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

    class ReflectionBaseSampleImpl : public virtual ObjectImpl
    {
        typedef ReflectionBaseSampleImpl self;

    public: // FIELDS

        int int_field;
        int count;
        double double_field;
        List<double> double_list_field_field;

    private:

        int private_int_field = 42;

    public: // METHODS

        int sample_method(int param)
        {
            received << "invoked ReflectionBaseSample.sample_method";
            return 42 + param;
        }

        Type get_type() override
        {
            // Create Type Object with thread safety guarantee as per C++ Standard
            static Type result = []()->Type
            {
                received << "creating Type object (this should run only once)." << std::endl;

                return make_type_builder<ReflectionBaseSampleImpl>("dot", "ReflectionBaseSample")
                    ->with_field("int_field", &ReflectionBaseSampleImpl::int_field)
                    ->with_field("private_int_field", &ReflectionBaseSampleImpl::private_int_field)
                    ->with_field("count", &ReflectionBaseSampleImpl::count)
                    ->with_method("sample_method", &ReflectionBaseSampleImpl::sample_method, { "param" })
                    ->build();
            }();

            return result;
        }
    };

    using ReflectionBaseSample = Ptr<ReflectionBaseSampleImpl>;
    ReflectionBaseSample make_reflection_base_sample() { return new ReflectionBaseSampleImpl; }

    class ReflectionDerivedSampleImpl : public ReflectionBaseSampleImpl
    {
    public: // FIELDS

        int int_field_in_derived_class;
    };

    using ReflectionDerivedSample = Ptr<ReflectionDerivedSampleImpl>;
    ReflectionDerivedSample make_reflection_derived_sample() { return new ReflectionDerivedSampleImpl; }

    TEST_CASE("property_info")
    {
        ReflectionBaseSample obj = make_reflection_base_sample();
        obj->int_field = 15;
        obj->count = 15;

        Object x = obj->count;

        Type result = obj->get_type();
        List<FieldInfo> props = result->get_fields();
        FieldInfo int_prop = props[0];
        REQUIRE(int_prop->name() == "int_field");
        REQUIRE(int(int_prop->get_value(obj)) == 15);

        int_prop->set_value(obj, 19);
        REQUIRE(obj->int_field == 19);
        REQUIRE(int(int_prop->get_value(obj)) == 19);

        FieldInfo private_int_prop = props[1];
        REQUIRE(private_int_prop->name() == "private_int_field");
        REQUIRE(int(private_int_prop->get_value(obj)) == 42);

        props[2]->set_value(obj, 2384);
        REQUIRE(obj->count == 2384);
        REQUIRE(int(props[2]->get_value(obj)) == 2384);

        ReflectionDerivedSample obj2 = make_reflection_derived_sample();

        props[2]->set_value(obj2, -15);
        REQUIRE(obj2->count == -15);
        REQUIRE(int(props[2]->get_value(obj2)) == -15);

        List<Object> params = make_list<Object>(1);
        params[0] = 15;
        REQUIRE(int(result->get_methods()[0]->invoke(obj2, params)) == 42 + 15);

        Approvals::verify(received.str());
        received.clear();
    }
}
