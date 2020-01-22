﻿/*
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

#include <dot/test/declare.hpp>
#include <dot/system/object.hpp>
#include <dot/system/enum_impl.hpp>
#include <dot/system/collections/generic/dictionary.hpp>

namespace dot
{
    /// Enum sample.
    enum class colors_sample
    {
        /// Empty value.
        empty,

        /// First value.
        blue,

        /// Second value.
        red
    };

    template <>
    struct type_traits<colors_sample>
    {
        static Type typeof()
        {
            static Type result = make_type_builder<char>("dot", "colors_sample")->is_enum()->build();
            return result;
        }
    };

    /// Helper class to implement to_string(value) via template specialization
    template <>
    struct ToStringImpl<colors_sample>
    {
        static dot::Dictionary<dot::String, int> get_enum_map(int size)
        {
            static dot::Dictionary<dot::String, int> func = [size]()
            {
                auto result = dot::make_dictionary<dot::String, int>();
                for (int i = 0; i < size; i++)
                {
                    colors_sample enum_value = (colors_sample)i;
                    String string_value = to_string(enum_value);
                    result[string_value] = i;
                }
                return result;
            }();
            return func;
        }

        /// Convert value to String; for empty or null values, return String::empty.
        static String to_string(const colors_sample& value)
        {
            switch (value)
            {
            case colors_sample::empty: return "empty";
            case colors_sample::blue: return "blue";
            case colors_sample::red: return "red";
            default: throw Exception("Unknown enum value in to_string(...).");
            }
        }

        /// Convert value to String; for empty or null values, return String::empty.
        static bool try_parse(String value, colors_sample& result)
        {
            dot::Dictionary<dot::String, int> dict = get_enum_map(3); // TODO - size hardcoded, improve
            int int_result;
            if (dict->try_get_value(value, int_result))
            {
                result = (colors_sample)int_result;
                return true;
            }
            else
            {
                result = (colors_sample)0;
                return false;
            }
        }
    };
}
