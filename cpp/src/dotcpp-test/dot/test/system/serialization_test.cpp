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
#include <boost/core/typeinfo.hpp>
#include <boost/core/demangle.hpp>
#include <typeinfo>
#include <iostream>
#include <dot/system/string.hpp>
#include <dot/system/exception.hpp>
#include <dot/system/reflection/method_info.hpp>
#include <dot/system/reflection/constructor_info.hpp>
#include <dot/system/reflection/activator.hpp>
#include <dot/system/type.hpp>
#include <dot/system/collections/generic/list.hpp>


namespace dot
{
    static std::stringstream received;

    class SampleDataImpl; using SampleData = Ptr<SampleDataImpl>;
    class SampleData2Impl; using SampleData2 = Ptr<SampleData2Impl>;

    class SampleData2Impl : public virtual ObjectImpl
    {
        typedef SampleData2Impl self;

    public: // PROPERTIES

        SampleData data_field;

    public: // STATIC

        static Type typeof()
        {
            static Type result = []()-> Type
            {
                Type t = make_type_builder<self>("dot", "SampleData2")
                    ->with_field("data_field", &self::data_field)
                    ->build();

                return t;
            }();

            return result;
        }

        Type get_type() override
        {
            return typeof();
        }
    };

    class SampleDataImpl; using SampleData = Ptr<SampleDataImpl>;
    SampleData make_sample_data();

    class SampleDataImpl : public virtual ObjectImpl
    {
        typedef SampleDataImpl self;  // This typedef will be used inside property macro and inside registration macro.

    public:

        String string_field;
        int int_field;
        double double_field;
        SampleData2 data_field;
        List<double> double_list_field;

        double foo(int dbl_arg, int int_arg)
        {
            received << "foo(" << dbl_arg << "," << int_arg << ")" << std::endl;
            return dbl_arg + int_arg;
        }

        void bar(int int_arg)
        {
            received << "bar(" << int_arg << ")" << std::endl;
        }

        static void static_foo(int int_arg)
        {
            received << "static_foo(" << int_arg << ")" << std::endl;
        }

    public: // REFLECTION

        DOT_TYPE_BEGIN("dot", "SampleData")
            DOT_TYPE_CTOR(make_sample_data)
            DOT_TYPE_PROP(string_field)
            DOT_TYPE_PROP(int_field)
            DOT_TYPE_PROP(double_field)
            DOT_TYPE_PROP(data_field)
            DOT_TYPE_PROP(double_list_field)
            DOT_TYPE_METHOD(foo, "dbl_arg", "int_arg")
            DOT_TYPE_METHOD(bar, "int_arg")
            DOT_TYPE_METHOD(static_foo, "int_arg")
        DOT_TYPE_END()
    };

    SampleData make_sample_data() { return new SampleDataImpl; }

    String obj_to_string(Object obj)
    {
        if (obj.is_empty()) return "";

        Type t = obj->get_type();

        std::stringstream ss;

        if (t->name() == "List`1")
        {
            List<double> vec = (List<double>)obj;
            for (Object item : vec)
            {
                ss << *(obj_to_string(item));
            }
        }
        else if (t == typeof<String>())
        {
            ss << *(String)obj << "|";
        }
        else if (t->is_class())
        {
        }
        else
        {
            ss << *(obj->to_string()) << "|";
        }

        return ss.str();
    }

    TEST_CASE("simple_serialization")
    {
        SampleData obj = make_sample_data();
        obj->string_field = "str";
        obj->data_field = new SampleData2Impl();
        obj->data_field->data_field = make_sample_data();
        obj->data_field->data_field->string_field = "internal str";
        obj->double_list_field = make_list<double>();
        obj->double_list_field->add(1.);
        obj->double_list_field->add(3.);
        obj->double_list_field->add(2.);

        String s = obj_to_string(obj);

        Type t = obj->get_type();

        SampleData dt = (SampleData)Activator::create_instance(obj->get_type());

        List<dot::Object> params_foo = make_list<Object>(2);
        params_foo[0] = 15;
        params_foo[1] = 42;
        double ret = obj->get_type()->get_methods()[0]->invoke(obj, params_foo);

        List<dot::Object> params_bar = make_list<Object>(1);
        params_bar[0] = 15;
        obj->get_type()->get_methods()[1]->invoke(obj, params_bar);

        Object o2 = obj->get_type()->get_constructors()[0]->invoke({});
    }
}
