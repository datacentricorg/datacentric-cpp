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
    //class mongo_test_enum : public dot::EnumBase
    //{
    //    typedef mongo_test_enum self;
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
    //    DOT_ENUM_BEGIN("dc", "mongo_test_enum")
    //        DOT_ENUM_VALUE(empty)
    //        DOT_ENUM_VALUE(enum_value1)
    //        DOT_ENUM_VALUE(enum_value2)
    //    DOT_ENUM_END()
    //};

    /// Enum sample.
    enum class mongo_test_enum
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
    struct ToStringImpl<dc::mongo_test_enum>
    {
        static dot::Dictionary<dot::String, int> get_enum_map(int size)
        {
            static dot::Dictionary<dot::String, int> func = [size]()
            {
                auto result = dot::make_dictionary<dot::String, int>();
                for (int i = 0; i < size; i++)
                {
                    dc::mongo_test_enum enum_value = (dc::mongo_test_enum)i;
                    String string_value = to_string(enum_value);
                    result[string_value] = i;
                }
                return result;
            }();
            return func;
        }

        /// Convert value to String; for empty or null values, return String::empty.
        static String to_string(const dc::mongo_test_enum& value)
        {
            switch (value)
            {
            case dc::mongo_test_enum::empty: return "empty";
            case dc::mongo_test_enum::enum_value1: return "enum_value1";
            case dc::mongo_test_enum::enum_value2: return "enum_value2";
            default: throw dot::Exception("Unknown enum value in to_string(...).");
            }
        }

        /// Convert value to String; for empty or null values, return String::empty.
        static bool try_parse(String value, dc::mongo_test_enum& result)
        {
            dot::Dictionary<dot::String, int> dict = get_enum_map(3); // TODO - size hardcoded, improve
            int int_result;
            if (dict->try_get_value(value, int_result))
            {
                result = (dc::mongo_test_enum)int_result;
                return true;
            }
            else
            {
                result = (dc::mongo_test_enum)0;
                return false;
            }
        }

        /// Convert String to enum
        static dc::mongo_test_enum parse(String value)
        {
            dot::Dictionary<dot::String, int> dict = get_enum_map(3); // TODO - size hardcoded, improve
            int int_result;
            if (dict->try_get_value(value, int_result))
            {
                return (dc::mongo_test_enum)int_result;
            }
            else
            {
                throw dot::Exception("Couldn't parse String to enum ");
            }
        }
    };

    template <>
    struct type_traits<dc::mongo_test_enum>
    {
        static Type typeof()
        {
            //! TODO resolve recursive typeof<enum>
            if (TypeImpl::get_type_map().find("dc.mongo_test_enum") != TypeImpl::get_type_map().end())
                return TypeImpl::get_type_map()["dc.mongo_test_enum"];

            static Type result = make_type_builder<dc::mongo_test_enum>("dc", "mongo_test_enum")
                ->is_enum()
                ->with_method("parse", &ToStringImpl<dc::mongo_test_enum>::parse, { "value" })
                ->build();
            return result;
        }
    };
}

namespace dc
{

    class mongo_test_key_impl; using mongo_test_key = dot::Ptr<mongo_test_key_impl>;
    class mongo_test_data_impl; using mongo_test_data = dot::Ptr<mongo_test_data_impl>;

    mongo_test_key make_mongo_test_key();

    /// Key class.
    class mongo_test_key_impl : public typed_key_impl<mongo_test_key_impl, mongo_test_data_impl>
    {
        typedef mongo_test_key_impl self;

    public:

        dot::String record_id;
        dot::Nullable<int> record_index;

        DOT_TYPE_BEGIN("dc", "mongo_test_key")
            DOT_TYPE_PROP(record_id)
            DOT_TYPE_PROP(record_index)
            DOT_TYPE_BASE(typed_key<mongo_test_key_impl, mongo_test_data_impl>)
            DOT_TYPE_CTOR(make_mongo_test_key)
        DOT_TYPE_END()
    };

    inline mongo_test_key make_mongo_test_key() { return new mongo_test_key_impl; }

    mongo_test_data make_mongo_test_data();

    /// Base data class.
    class mongo_test_data_impl : public typed_record_impl<mongo_test_key_impl, mongo_test_data_impl>
    {
        typedef mongo_test_data_impl self;

    public:

        dot::String record_id;
        dot::Nullable<int> record_index;
        dot::Nullable<double> double_element;
        dot::Nullable<dot::LocalDate> local_date_element;
        dot::Nullable<dot::LocalTime> local_time_element;
        dot::Nullable<dot::LocalMinute> local_minute_element;
        dot::Nullable<dot::LocalDateTime> local_date_time_element;
        mongo_test_enum enum_value = mongo_test_enum::empty;
        dot::Nullable<int> version;

    public: // REFLECTION

        dot::Type get_type() override { return typeof(); }

        static dot::Type typeof()
        {
            static dot::Type result = []()->dot::Type
            {
                dot::Type t = dot::make_type_builder<mongo_test_data_impl>("dc", "mongo_test_data", {
                        make_index_elements_attribute("double_element, local_date_element, enum_value"),
                        make_index_elements_attribute("local_date_element"),
                        make_index_elements_attribute("record_id, -version", "custom_index_name"),
                        make_index_elements_attribute("-record_index") })
                    ->with_field("record_id", &mongo_test_data_impl::record_id)
                    ->with_field("record_index", &mongo_test_data_impl::record_index)
                    ->with_field("double_element", &mongo_test_data_impl::double_element)
                    ->with_field("local_date_element", &mongo_test_data_impl::local_date_element)
                    ->with_field("local_time_element", &mongo_test_data_impl::local_time_element)
                    ->with_field("local_minute_element", &mongo_test_data_impl::local_minute_element)
                    ->with_field("local_date_time_element", &mongo_test_data_impl::local_date_time_element)
                    ->with_field("enum_value", &mongo_test_data_impl::enum_value)
                    ->with_field("version", &mongo_test_data_impl::version)
                    ->template with_base<typed_record<mongo_test_key_impl, mongo_test_data_impl>>()
                    ->with_constructor(&make_mongo_test_data, {})
                    ->build();
                return t;
            }();
            return result;
        }
    };

    inline mongo_test_data make_mongo_test_data() { return new mongo_test_data_impl; }

    class element_sample_data_impl; using element_sample_data = dot::Ptr<element_sample_data_impl>;

    element_sample_data make_element_sample_data();

    /// Element data class.
    class element_sample_data_impl : public data_impl
    {
        typedef element_sample_data_impl self;
    public:
        dot::Nullable<double> double_element3;
        dot::String string_element3;

        DOT_TYPE_BEGIN("dc", "element_sample_data")
            DOT_TYPE_PROP(double_element3)
            DOT_TYPE_PROP(string_element3)
            DOT_TYPE_BASE(data)
            DOT_TYPE_CTOR(make_element_sample_data)
        DOT_TYPE_END()
    };

    inline element_sample_data make_element_sample_data() { return new element_sample_data_impl; }

    class mongo_test_derived_data_impl; using mongo_test_derived_data = dot::Ptr<mongo_test_derived_data_impl>;

    mongo_test_derived_data make_mongo_test_derived_data();

    /// Derived data class.
    class mongo_test_derived_data_impl : public mongo_test_data_impl
    {
        typedef mongo_test_derived_data_impl self;
    public:
        dot::Nullable<double> double_element2;
        dot::String string_element2;
        dot::List<dot::String> array_of_string;
        dot::List<dot::String> list_of_string;
        dot::List<double> array_of_double;
        dot::List<dot::Nullable<double>> array_of_nullable_double;
        dot::List<double> list_of_double;
        dot::List<dot::Nullable<double>> list_of_nullable_double;
        element_sample_data data_element;
        dot::List<element_sample_data> data_element_list;
        mongo_test_key key_element;
        dot::List<mongo_test_key> key_element_list;

    public: // REFLECTION

        dot::Type get_type() override { return typeof(); }

        static dot::Type typeof()
        {
            static dot::Type result = []()->dot::Type
            {
                dot::Type t = dot::make_type_builder<mongo_test_derived_data_impl>("dc", "mongo_test_derived_data", {
                        make_index_elements_attribute("double_element2, -double_element")})
                    ->with_field("double_element2", &mongo_test_derived_data_impl::double_element2)
                    ->with_field("string_element2", &mongo_test_derived_data_impl::string_element2)
                    ->with_field("array_of_string", &mongo_test_derived_data_impl::array_of_string)
                    ->with_field("list_of_string", &mongo_test_derived_data_impl::list_of_string)
                    ->with_field("array_of_double", &mongo_test_derived_data_impl::array_of_double)
                    ->with_field("array_of_nullable_double", &mongo_test_derived_data_impl::array_of_nullable_double)
                    ->with_field("list_of_double", &mongo_test_derived_data_impl::list_of_double)
                    ->with_field("list_of_nullable_double", &mongo_test_derived_data_impl::list_of_nullable_double)
                    ->with_field("data_element", &mongo_test_derived_data_impl::data_element)
                    ->with_field("data_element_list", &mongo_test_derived_data_impl::data_element_list)
                    ->with_field("key_element", &mongo_test_derived_data_impl::key_element)
                    ->with_field("key_element_list", &mongo_test_derived_data_impl::key_element_list)
                    ->template with_base<mongo_test_data>()
                    ->with_constructor(&make_mongo_test_derived_data, {})
                    ->build();
                return t;
            }();
            return result;
        }
    };

    inline mongo_test_derived_data make_mongo_test_derived_data() { return new mongo_test_derived_data_impl; }

    class mongo_test_other_derived_data_impl; using mongo_test_other_derived_data = dot::Ptr<mongo_test_other_derived_data_impl>;

    mongo_test_other_derived_data make_mongo_test_other_derived_data();

    /// Other derived data class.
    class mongo_test_other_derived_data_impl : public mongo_test_data_impl
    {
        typedef mongo_test_other_derived_data_impl self;

    public:

        dot::Nullable<double> other_double_element2;
        dot::String other_string_element2;

    public: // REFLECTION

        dot::Type get_type() override { return typeof(); }

        static dot::Type typeof()
        {
            static dot::Type result = []()->dot::Type
            {
                dot::Type t = dot::make_type_builder<mongo_test_other_derived_data_impl>("dc", "mongo_test_other_derived_data", {
                        make_index_elements_attribute("other_double_element2, other_string_element2, -record_index") })
                    ->with_field("other_double_element2", &mongo_test_other_derived_data_impl::other_double_element2)
                    ->with_field("other_string_element2", &mongo_test_other_derived_data_impl::other_string_element2)
                    ->template with_base<mongo_test_data>()
                    ->with_constructor(&make_mongo_test_other_derived_data, {})
                    ->build();
                return t;
            }();
            return result;
        }
    };

    inline mongo_test_other_derived_data make_mongo_test_other_derived_data() { return new mongo_test_other_derived_data_impl; }

    class mongo_test_derived_from_derived_data_impl; using mongo_test_derived_from_derived_data = dot::Ptr<mongo_test_derived_from_derived_data_impl>;

    mongo_test_derived_from_derived_data make_mongo_test_derived_from_derived_data();

    /// Next level in the inheritance chain.
    class mongo_test_derived_from_derived_data_impl : public mongo_test_derived_data_impl
    {
        typedef mongo_test_derived_from_derived_data_impl self;

    public:

        dot::Nullable<double> other_double_element3;
        dot::String other_string_element2;

        DOT_TYPE_BEGIN("dc", "mongo_test_derived_from_derived_data")
            DOT_TYPE_PROP(other_double_element3)
            DOT_TYPE_PROP(other_string_element2)
            DOT_TYPE_BASE(mongo_test_derived_data)
            DOT_TYPE_CTOR(make_mongo_test_derived_from_derived_data)
        DOT_TYPE_END()
    };

    inline mongo_test_derived_from_derived_data make_mongo_test_derived_from_derived_data() { return new mongo_test_derived_from_derived_data_impl; }
}
