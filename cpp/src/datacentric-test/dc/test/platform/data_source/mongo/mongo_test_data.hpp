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
#include <dc/attributes/class/index_elements_attribute.hpp>
#include <dc/platform/context/context_base.hpp>
#include <dc/types/record/typed_record.hpp>
#include <dc/types/record/data_type_info.hpp>
#include <dot/system/enum.hpp>

namespace dc
{
    /// Enum type.
    //class MongoTestEnum : public dot::EnumBase
    //{
    //    typedef MongoTestEnum self;
    //
    //public:
    //
    //    enum enum_type
    //    {
    //        empty,
    //        enum_value1,
    //        enum_value2
    //    };
    //
    //    DOT_ENUM_BEGIN("dc", "MongoTestEnum")
    //        DOT_ENUM_VALUE(empty)
    //        DOT_ENUM_VALUE(enum_value1)
    //        DOT_ENUM_VALUE(enum_value2)
    //    DOT_ENUM_END()
    //};

    /// Enum sample.
    enum class MongoTestEnum
    {
        /// Empty value.
        empty,

        /// First value.
        enum_value1,

        /// Second value.
        enum_value2
    };
}
namespace dot
{
    /// Helper class to implement to_string(value) via template specialization
    template <>
    struct ToStringImpl<dc::MongoTestEnum>
    {
        static dot::Dictionary<dot::String, int> get_enum_map(int size)
        {
            static dot::Dictionary<dot::String, int> func = [size]()
            {
                auto result = dot::make_dictionary<dot::String, int>();
                for (int i = 0; i < size; i++)
                {
                    dc::MongoTestEnum enum_value = (dc::MongoTestEnum)i;
                    String string_value = to_string(enum_value);
                    result[string_value] = i;
                }
                return result;
            }();
            return func;
        }

        /// Convert value to String; for empty or null values, return String::empty.
        static String to_string(const dc::MongoTestEnum& value)
        {
            switch (value)
            {
            case dc::MongoTestEnum::empty: return "empty";
            case dc::MongoTestEnum::enum_value1: return "enum_value1";
            case dc::MongoTestEnum::enum_value2: return "enum_value2";
            default: throw dot::Exception("Unknown enum value in to_string(...).");
            }
        }

        /// Convert value to String; for empty or null values, return String::empty.
        static bool try_parse(String value, dc::MongoTestEnum& result)
        {
            dot::Dictionary<dot::String, int> dict = get_enum_map(3); // TODO - size hardcoded, improve
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

        /// Convert String to enum
        static dc::MongoTestEnum parse(String value)
        {
            dot::Dictionary<dot::String, int> dict = get_enum_map(3); // TODO - size hardcoded, improve
            int int_result;
            if (dict->try_get_value(value, int_result))
            {
                return (dc::MongoTestEnum)int_result;
            }
            else
            {
                throw dot::Exception("Couldn't parse String to enum ");
            }
        }
    };

    template <>
    struct type_traits<dc::MongoTestEnum>
    {
        static Type typeof()
        {
            //! TODO resolve recursive typeof<enum>
            if (TypeImpl::get_type_map().find("dc.MongoTestEnum") != TypeImpl::get_type_map().end())
                return TypeImpl::get_type_map()["dc.MongoTestEnum"];

            static Type result = make_type_builder<dc::MongoTestEnum>("dc", "MongoTestEnum")
                ->is_enum()
                ->with_method("parse", &ToStringImpl<dc::MongoTestEnum>::parse, { "value" })
                ->build();
            return result;
        }
    };
}

namespace dc
{

    class MongoTestKeyImpl; using MongoTestKey = dot::Ptr<MongoTestKeyImpl>;
    class MongoTestDataImpl; using MongoTestData = dot::Ptr<MongoTestDataImpl>;

    MongoTestKey make_mongo_test_key();

    /// Key class.
    class MongoTestKeyImpl : public TypedKeyImpl<MongoTestKeyImpl, MongoTestDataImpl>
    {
        typedef MongoTestKeyImpl self;

    public:

        dot::String record_id;
        dot::Nullable<int> record_index;

        DOT_TYPE_BEGIN("dc", "MongoTestKey")
            DOT_TYPE_PROP(record_id)
            DOT_TYPE_PROP(record_index)
            DOT_TYPE_BASE(TypedKey<MongoTestKeyImpl, MongoTestDataImpl>)
            DOT_TYPE_CTOR(make_mongo_test_key)
        DOT_TYPE_END()
    };

    inline MongoTestKey make_mongo_test_key() { return new MongoTestKeyImpl; }

    MongoTestData make_mongo_test_data();

    /// Base data class.
    class MongoTestDataImpl : public TypedRecordImpl<MongoTestKeyImpl, MongoTestDataImpl>
    {
        typedef MongoTestDataImpl self;

    public:

        dot::String record_id;
        dot::Nullable<int> record_index;
        dot::Nullable<double> double_element;
        dot::Nullable<dot::LocalDate> local_date_element;
        dot::Nullable<dot::LocalTime> local_time_element;
        dot::Nullable<dot::LocalMinute> local_minute_element;
        dot::Nullable<dot::LocalDateTime> local_date_time_element;
        MongoTestEnum enum_value = MongoTestEnum::empty;
        dot::Nullable<int> version;

    public: // REFLECTION

        dot::Type get_type() override { return typeof(); }

        static dot::Type typeof()
        {
            static dot::Type result = []()->dot::Type
            {
                dot::Type t = dot::make_type_builder<MongoTestDataImpl>("dc", "MongoTestData", {
                        make_index_elements_attribute("double_element, local_date_element, enum_value"),
                        make_index_elements_attribute("local_date_element"),
                        make_index_elements_attribute("record_id, -version", "custom_index_name"),
                        make_index_elements_attribute("-record_index") })
                    ->with_field("record_id", &MongoTestDataImpl::record_id)
                    ->with_field("record_index", &MongoTestDataImpl::record_index)
                    ->with_field("double_element", &MongoTestDataImpl::double_element)
                    ->with_field("local_date_element", &MongoTestDataImpl::local_date_element)
                    ->with_field("local_time_element", &MongoTestDataImpl::local_time_element)
                    ->with_field("local_minute_element", &MongoTestDataImpl::local_minute_element)
                    ->with_field("local_date_time_element", &MongoTestDataImpl::local_date_time_element)
                    ->with_field("enum_value", &MongoTestDataImpl::enum_value)
                    ->with_field("version", &MongoTestDataImpl::version)
                    ->template with_base<TypedRecord<MongoTestKeyImpl, MongoTestDataImpl>>()
                    ->with_constructor(&make_mongo_test_data, {})
                    ->build();
                return t;
            }();
            return result;
        }
    };

    inline MongoTestData make_mongo_test_data() { return new MongoTestDataImpl; }

    class ElementSampleDataImpl; using ElementSampleData = dot::Ptr<ElementSampleDataImpl>;

    ElementSampleData make_element_sample_data();

    /// Element data class.
    class ElementSampleDataImpl : public DataImpl
    {
        typedef ElementSampleDataImpl self;
    public:
        dot::Nullable<double> double_element3;
        dot::String string_element3;

        DOT_TYPE_BEGIN("dc", "ElementSampleData")
            DOT_TYPE_PROP(double_element3)
            DOT_TYPE_PROP(string_element3)
            DOT_TYPE_BASE(Data)
            DOT_TYPE_CTOR(make_element_sample_data)
        DOT_TYPE_END()
    };

    inline ElementSampleData make_element_sample_data() { return new ElementSampleDataImpl; }

    class MongoTestDerivedDataImpl; using MongoTestDerivedData = dot::Ptr<MongoTestDerivedDataImpl>;

    MongoTestDerivedData make_mongo_test_derived_data();

    /// Derived data class.
    class MongoTestDerivedDataImpl : public MongoTestDataImpl
    {
        typedef MongoTestDerivedDataImpl self;
    public:
        dot::Nullable<double> double_element2;
        dot::String string_element2;
        dot::List<dot::String> array_of_string;
        dot::List<dot::String> list_of_string;
        dot::List<double> array_of_double;
        dot::List<dot::Nullable<double>> array_of_nullable_double;
        dot::List<double> list_of_double;
        dot::List<dot::Nullable<double>> list_of_nullable_double;
        ElementSampleData data_element;
        dot::List<ElementSampleData> data_element_list;
        MongoTestKey key_element;
        dot::List<MongoTestKey> key_element_list;

    public: // REFLECTION

        dot::Type get_type() override { return typeof(); }

        static dot::Type typeof()
        {
            static dot::Type result = []()->dot::Type
            {
                dot::Type t = dot::make_type_builder<MongoTestDerivedDataImpl>("dc", "MongoTestDerivedData", {
                        make_index_elements_attribute("double_element2, -double_element")})
                    ->with_field("double_element2", &MongoTestDerivedDataImpl::double_element2)
                    ->with_field("string_element2", &MongoTestDerivedDataImpl::string_element2)
                    ->with_field("array_of_string", &MongoTestDerivedDataImpl::array_of_string)
                    ->with_field("list_of_string", &MongoTestDerivedDataImpl::list_of_string)
                    ->with_field("array_of_double", &MongoTestDerivedDataImpl::array_of_double)
                    ->with_field("array_of_nullable_double", &MongoTestDerivedDataImpl::array_of_nullable_double)
                    ->with_field("list_of_double", &MongoTestDerivedDataImpl::list_of_double)
                    ->with_field("list_of_nullable_double", &MongoTestDerivedDataImpl::list_of_nullable_double)
                    ->with_field("data_element", &MongoTestDerivedDataImpl::data_element)
                    ->with_field("data_element_list", &MongoTestDerivedDataImpl::data_element_list)
                    ->with_field("key_element", &MongoTestDerivedDataImpl::key_element)
                    ->with_field("key_element_list", &MongoTestDerivedDataImpl::key_element_list)
                    ->template with_base<MongoTestData>()
                    ->with_constructor(&make_mongo_test_derived_data, {})
                    ->build();
                return t;
            }();
            return result;
        }
    };

    inline MongoTestDerivedData make_mongo_test_derived_data() { return new MongoTestDerivedDataImpl; }

    class MongoTestOtherDerivedDataImpl; using MongoTestOtherDerivedData = dot::Ptr<MongoTestOtherDerivedDataImpl>;

    MongoTestOtherDerivedData make_mongo_test_other_derived_data();

    /// Other derived data class.
    class MongoTestOtherDerivedDataImpl : public MongoTestDataImpl
    {
        typedef MongoTestOtherDerivedDataImpl self;

    public:

        dot::Nullable<double> other_double_element2;
        dot::String other_string_element2;

    public: // REFLECTION

        dot::Type get_type() override { return typeof(); }

        static dot::Type typeof()
        {
            static dot::Type result = []()->dot::Type
            {
                dot::Type t = dot::make_type_builder<MongoTestOtherDerivedDataImpl>("dc", "MongoTestOtherDerivedData", {
                        make_index_elements_attribute("other_double_element2, other_string_element2, -record_index") })
                    ->with_field("other_double_element2", &MongoTestOtherDerivedDataImpl::other_double_element2)
                    ->with_field("other_string_element2", &MongoTestOtherDerivedDataImpl::other_string_element2)
                    ->template with_base<MongoTestData>()
                    ->with_constructor(&make_mongo_test_other_derived_data, {})
                    ->build();
                return t;
            }();
            return result;
        }
    };

    inline MongoTestOtherDerivedData make_mongo_test_other_derived_data() { return new MongoTestOtherDerivedDataImpl; }

    class MongoTestDerivedFromDerivedDataImpl; using MongoTestDerivedFromDerivedData = dot::Ptr<MongoTestDerivedFromDerivedDataImpl>;

    MongoTestDerivedFromDerivedData make_mongo_test_derived_from_derived_data();

    /// Next level in the inheritance chain.
    class MongoTestDerivedFromDerivedDataImpl : public MongoTestDerivedDataImpl
    {
        typedef MongoTestDerivedFromDerivedDataImpl self;

    public:

        dot::Nullable<double> other_double_element3;
        dot::String other_string_element2;

        DOT_TYPE_BEGIN("dc", "MongoTestDerivedFromDerivedData")
            DOT_TYPE_PROP(other_double_element3)
            DOT_TYPE_PROP(other_string_element2)
            DOT_TYPE_BASE(MongoTestDerivedData)
            DOT_TYPE_CTOR(make_mongo_test_derived_from_derived_data)
        DOT_TYPE_END()
    };

    inline MongoTestDerivedFromDerivedData make_mongo_test_derived_from_derived_data() { return new MongoTestDerivedFromDerivedDataImpl; }
}
