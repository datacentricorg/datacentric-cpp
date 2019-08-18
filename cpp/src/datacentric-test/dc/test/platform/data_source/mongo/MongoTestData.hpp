/*
Copyright (C) 2013-present The DataCentric Authors.

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

#pragma once

#include <dc/test/implement.hpp>
#include <dc/platform/context/context_base.hpp>
#include <dc/types/record/record.hpp>
#include <dot/system/Enum.hpp>

namespace dc
{
    /// Enum type.
    //class MongoTestEnum : public dot::enum_base
    //{
    //    typedef MongoTestEnum self;
    //
    //public:
    //
    //    enum enum_type
    //    {
    //        empty,
    //        EnumValue1,
    //        EnumValue2
    //    };
    //
    //    DOT_ENUM_BEGIN("dc", "MongoTestEnum")
    //        DOT_ENUM_VALUE(empty)
    //        DOT_ENUM_VALUE(EnumValue1)
    //        DOT_ENUM_VALUE(EnumValue2)
    //    DOT_ENUM_END()
    //};

    /// Enum sample.
    enum class MongoTestEnum
    {
        /// Empty value.
        empty,

        /// First value.
        EnumValue1,

        /// Second value.
        EnumValue2
    };
}
namespace dot
{
    /// Helper class to implement to_string(value) via template specialization
    template <>
    struct to_string_impl<dc::MongoTestEnum>
    {
        static dot::dictionary<dot::string, int> get_enum_map(int size)
        {
            static dot::dictionary<dot::string, int> func = [size]()
            {
                auto result = dot::make_dictionary<dot::string, int>();
                for (int i = 0; i < size; i++)
                {
                    dc::MongoTestEnum enum_value = (dc::MongoTestEnum)i;
                    string string_value = to_string(enum_value);
                    result[string_value] = i;
                }
                return result;
            }();
            return func;
        }

        /// Convert value to string; for empty or null values, return string::empty.
        static string to_string(const dc::MongoTestEnum& value)
        {
            switch (value)
            {
            case dc::MongoTestEnum::empty: return "empty";
            case dc::MongoTestEnum::EnumValue1: return "EnumValue1";
            case dc::MongoTestEnum::EnumValue2: return "EnumValue2";
            default: throw dot::exception("Unknown enum value in to_string(...).");
            }
        }

        /// Convert value to string; for empty or null values, return string::empty.
        static bool try_parse(string value, dc::MongoTestEnum& result)
        {
            dot::dictionary<dot::string, int> dict = get_enum_map(3); // TODO - size hardcoded, improve
            int int_result;
            if (dict->try_get_value(value, int_result))
            {
                result = (dc::MongoTestEnum)int_result;
                return true;
            }
            else
            {
                result = (dc::MongoTestEnum)0;
                return false;
            }
        }

        /// Convert string to enum
        static dc::MongoTestEnum parse(string value)
        {
            dot::dictionary<dot::string, int> dict = get_enum_map(3); // TODO - size hardcoded, improve
            int int_result;
            if (dict->try_get_value(value, int_result))
            {
                return (dc::MongoTestEnum)int_result;
            }
            else
            {
                throw dot::exception("Couldn't parse string to enum ");
            }
        }
    };

    template <>
    struct type_traits<dc::MongoTestEnum>
    {
        static type typeof()
        {
            //! TODO resolve recursive typeof<enum>
            if (type_impl::get_type_map().find("dc.MongoTestEnum") != type_impl::get_type_map().end())
                return type_impl::get_type_map()["dc.MongoTestEnum"];

            static type result = make_type_builder<dc::MongoTestEnum>("dc", "MongoTestEnum")
                ->is_enum()
                ->with_method("parse", &to_string_impl<dc::MongoTestEnum>::parse, { "value" })
                ->build();
            return result;
        }
    };
}

namespace dc
{

    class MongoTestKeyImpl; using MongoTestKey = dot::ptr<MongoTestKeyImpl>;
    class MongoTestDataImpl; using MongoTestData = dot::ptr<MongoTestDataImpl>;

    MongoTestKey make_MongoTestKey();

    /// Key class.
    class MongoTestKeyImpl : public key_impl<MongoTestKeyImpl, MongoTestDataImpl>
    {
        typedef MongoTestKeyImpl self;

    public:

        dot::string RecordID;
        dot::nullable<int> RecordIndex;

        DOT_TYPE_BEGIN("dc", "MongoTestKey")
            DOT_TYPE_PROP(RecordID)
            DOT_TYPE_PROP(RecordIndex)
            DOT_TYPE_BASE(key<MongoTestKeyImpl, MongoTestDataImpl>)
            DOT_TYPE_CTOR(make_MongoTestKey)
        DOT_TYPE_END()
    };

    inline MongoTestKey make_MongoTestKey() { return new MongoTestKeyImpl; }

    MongoTestData make_MongoTestData();

    /// Base data class.
    class MongoTestDataImpl : public record_impl<MongoTestKeyImpl, MongoTestDataImpl>
    {
        typedef MongoTestDataImpl self;

    public:

        dot::string RecordID;
        dot::nullable<int> RecordIndex;
        dot::nullable<double> DoubleElement;
        dot::nullable<dot::local_date> local_date_element;
        dot::nullable<dot::local_time> local_time_element;
        dot::nullable<dot::local_minute> local_minute_element;
        dot::nullable<dot::local_date_time> local_date_time_element;
        MongoTestEnum EnumValue = MongoTestEnum::empty;
        dot::nullable<int> Version;

        DOT_TYPE_BEGIN("dc", "MongoTestData")
            DOT_TYPE_PROP(RecordID)
            DOT_TYPE_PROP(RecordIndex)
            DOT_TYPE_PROP(DoubleElement)
            DOT_TYPE_PROP(local_date_element)
            DOT_TYPE_PROP(local_time_element)
            DOT_TYPE_PROP(local_minute_element)
            DOT_TYPE_PROP(local_date_time_element)
            DOT_TYPE_PROP(EnumValue)
            DOT_TYPE_PROP(Version)
            DOT_TYPE_BASE(record<MongoTestKeyImpl, MongoTestDataImpl>)
            DOT_TYPE_CTOR(make_MongoTestData)
        DOT_TYPE_END()
    };

    inline MongoTestData make_MongoTestData() { return new MongoTestDataImpl; }

    class ElementSampleDataImpl; using ElementSampleData = dot::ptr<ElementSampleDataImpl>;

    ElementSampleData make_ElementSampleData();

    /// Element data class.
    class ElementSampleDataImpl : public data_impl
    {
        typedef ElementSampleDataImpl self;
    public:
        dot::nullable<double> DoubleElement3;
        dot::string StringElement3;

        DOT_TYPE_BEGIN("dc", "ElementSampleData")
            DOT_TYPE_PROP(DoubleElement3)
            DOT_TYPE_PROP(StringElement3)
            DOT_TYPE_BASE(data)
            DOT_TYPE_CTOR(make_ElementSampleData)
        DOT_TYPE_END()
    };

    inline ElementSampleData make_ElementSampleData() { return new ElementSampleDataImpl; }

    class MongoTestDerivedDataImpl; using MongoTestDerivedData = dot::ptr<MongoTestDerivedDataImpl>;

    MongoTestDerivedData make_MongoTestDerivedData();

    /// Derived data class.
    class MongoTestDerivedDataImpl : public MongoTestDataImpl
    {
        typedef MongoTestDerivedDataImpl self;
    public:
        dot::nullable<double> DoubleElement2;
        dot::string StringElement2;
        dot::list<dot::string> ArrayOfString;
        dot::list<dot::string> ListOfString;
        dot::list<double> ArrayOfDouble;
        dot::list<dot::nullable<double>> ArrayOfNullableDouble;
        dot::list<double> ListOfDouble;
        dot::list<dot::nullable<double>> ListOfNullableDouble;
        ElementSampleData DataElement;
        dot::list<ElementSampleData> DataElementList;
        MongoTestKey KeyElement;
        dot::list<MongoTestKey> KeyElementList;

        DOT_TYPE_BEGIN("dc", "MongoTestDerivedData")
            DOT_TYPE_PROP(DoubleElement2)
            DOT_TYPE_PROP(StringElement2)
            DOT_TYPE_PROP(ArrayOfString)
            DOT_TYPE_PROP(ListOfString)
            DOT_TYPE_PROP(ArrayOfDouble)
            DOT_TYPE_PROP(ArrayOfNullableDouble)
            DOT_TYPE_PROP(ListOfDouble)
            DOT_TYPE_PROP(ListOfNullableDouble)
            DOT_TYPE_PROP(DataElement)
            DOT_TYPE_PROP(DataElementList)
            DOT_TYPE_PROP(KeyElement)
            DOT_TYPE_PROP(KeyElementList)
            DOT_TYPE_BASE(MongoTestData)
            DOT_TYPE_CTOR(make_MongoTestDerivedData)
        DOT_TYPE_END()
    };

    inline MongoTestDerivedData make_MongoTestDerivedData() { return new MongoTestDerivedDataImpl; }

    class MongoTestOtherDerivedDataImpl; using MongoTestOtherDerivedData = dot::ptr<MongoTestOtherDerivedDataImpl>;

    MongoTestOtherDerivedData make_MongoTestOtherDerivedData();

    /// Other derived data class.
    class MongoTestOtherDerivedDataImpl : public MongoTestDataImpl
    {
        typedef MongoTestOtherDerivedDataImpl self;

    public:

        dot::nullable<double> OtherDoubleElement2;
        dot::string OtherStringElement2;

        DOT_TYPE_BEGIN("dc", "MongoTestOtherDerivedData")
            DOT_TYPE_PROP(OtherDoubleElement2)
            DOT_TYPE_PROP(OtherStringElement2)
            DOT_TYPE_BASE(MongoTestData)
            DOT_TYPE_CTOR(make_MongoTestOtherDerivedData)
        DOT_TYPE_END()
    };

    inline MongoTestOtherDerivedData make_MongoTestOtherDerivedData() { return new MongoTestOtherDerivedDataImpl; }

    class MongoTestDerivedFromDerivedDataImpl; using MongoTestDerivedFromDerivedData = dot::ptr<MongoTestDerivedFromDerivedDataImpl>;

    MongoTestDerivedFromDerivedData make_MongoTestDerivedFromDerivedData();

    /// Next level in the inheritance chain.
    class MongoTestDerivedFromDerivedDataImpl : public MongoTestDerivedDataImpl
    {
        typedef MongoTestDerivedFromDerivedDataImpl self;

    public:

        dot::nullable<double> OtherDoubleElement3;
        dot::string OtherStringElement3;

        DOT_TYPE_BEGIN("dc", "MongoTestDerivedFromDerivedData")
            DOT_TYPE_PROP(OtherDoubleElement3)
            DOT_TYPE_PROP(OtherStringElement3)
            DOT_TYPE_BASE(MongoTestDerivedData)
            DOT_TYPE_CTOR(make_MongoTestDerivedFromDerivedData)
        DOT_TYPE_END()
    };

    inline MongoTestDerivedFromDerivedData make_MongoTestDerivedFromDerivedData() { return new MongoTestDerivedFromDerivedDataImpl; }
}
