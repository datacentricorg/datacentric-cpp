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

namespace dot
{
    class object_impl;
    class string;
    class object;
    class object_impl;

    namespace detail
    {
        /// Empty structure.
        class DummyNoToString : public virtual object_impl
        {};

        /// objects inherit this structure in case their inner struct class has method to_string
        /// so object also have these method.
        template <class W, class T>
        class ObjToString : public virtual object_impl
        {
        public:
            virtual string to_string() override { return static_cast<T*>(static_cast<W*>(this))->to_string(); }
        };

        /// Detects existance of to_string method.
        template<class T>
        struct HasToString
        {
        private:
            static DummyNoToString detect(...);
            template<class U> static decltype(std::declval<U>().to_string()) detect(const U&);
        public:
            static constexpr bool value = !std::is_same<DummyNoToString, decltype(detect(std::declval<T>()))>::value;
            typedef std::integral_constant<bool, value> type;
        };

        /// For inheritance of to_string method.
        template<class W, class T>
        class InheritToString : public std::conditional<
                HasToString<T>::value,
                ObjToString<W, T>,
                DummyNoToString
            >::type
        {};

        /// Empty structure.
        class DummyNoGetHashCode : public virtual object_impl
        {};

        /// objects inherit this structure in case their inner struct class has method hash_code
        /// so object also have these method.
        template <class W, class T>
        class ObjGetHashCode : public virtual object_impl
        {
        public:
            virtual size_t hash_code() override { return static_cast<T*>(static_cast<W*>(this))->hash_code(); }
        };

        /// Detects existance of hash_code method.
        template<class T>
        struct HasGetHashCode
        {
        private:
            static DummyNoGetHashCode detect(...);
            template<class U> static decltype(std::declval<U>().hash_code()) detect(const U&);
        public:
            static constexpr bool value = !std::is_same<DummyNoGetHashCode, decltype(detect(std::declval<T>()))>::value;
            typedef std::integral_constant<bool, value> type;
        };

        /// For inheritance of hash_code method.
        template<class W, class T>
        class InheritGetHashCode : public std::conditional<
            HasGetHashCode<T>::value,
            ObjGetHashCode<W, T>,
            DummyNoGetHashCode
        >::type
        {};

        /// Empty structure.
        class DummyNoEquals : public virtual object_impl
        {};

        /// objects inherit this structure in case their inner struct class has method equals
        /// so object also have these method.
        template <class W, class T>
        class ObjEquals : public virtual object_impl
        {
        public:
            bool equals(object obj) override { return static_cast<T*>(static_cast<W*>(this))->equals(obj); }
        };

        /// Detects existance of equals method.
        template<class T>
        struct HasEquals
        {
        private:
            static DummyNoEquals detect(...);
            template<class U> static decltype(std::declval<U>().equals(std::declval<object>())) detect(const U&);
        public:
            static constexpr bool value = !std::is_same<DummyNoEquals, decltype(detect(std::declval<T>()))>::value;
            typedef std::integral_constant<bool, value> type;
        };

        /// For inheritance of equals method.
        template<class W, class T>
        class InheritEquals : public std::conditional<
            HasEquals<T>::value,
            ObjEquals<W, T>,
            DummyNoEquals
        >::type
        {};
    }
}
