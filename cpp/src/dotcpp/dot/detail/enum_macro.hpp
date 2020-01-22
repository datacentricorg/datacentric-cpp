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

#pragma once

#define DOT_ENUM_BEGIN(nspace, name)                                                \
private:                                                                            \
    static dot::Object make_self() { return self(); }                               \
                                                                                    \
public:                                                                             \
    typedef self element_type;                                                      \
    typedef dot::StructWrapperImpl<self>* pointer_type;                             \
    using dot::EnumBase::EnumBase;                                                  \
                                                                                    \
    operator dot::Object() { return new dot::StructWrapperImpl<self>(*this); }      \
    operator int() const { return value_; }                                         \
    self& operator=(int rhs) { value_ = rhs; return *this; }                        \
    self& operator=(const self& other) { value_ = other.value_; return *this; }     \
    virtual dot::Type get_type() { return typeof(); }                               \
    static dot::Type typeof()                                                       \
    {                                                                               \
        static dot::Type result = []()->dot::Type                                   \
        {                                                                           \
            dot::Type t = dot::make_type_builder<self>(nspace, name)                \
                ->is_enum()                                                         \
                ->with_constructor(&self::make_self, {})                            \
                ->with_base<EnumBase>()                                             \
                ->build();                                                          \
            return t;                                                               \
        }();                                                                        \
        return result;                                                              \
    }                                                                               \
                                                                                    \
protected:                                                                          \
    virtual dot::Dictionary<dot::String, int> get_enum_map() override               \
    {                                                                               \
        static dot::Dictionary<dot::String, int> enum_map_ = []()                   \
        {                                                                           \
            auto map_ = dot::make_dictionary<dot::String, int>();


#define DOT_ENUM_VALUE(value) \
    map_[#value] = value;


#define DOT_ENUM_END()          \
            return map_;        \
        }();                    \
        return enum_map_;       \
    }
