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

#include <dot/declare.hpp>
#include <dot/system/ptr.hpp>
#include <dot/system/object_impl.hpp>
#include <dot/system/exception.hpp>
#include <dot/system/string.hpp>

namespace dot
{
    template <class T> class Nullable;
    class LocalMinute;
    class LocalTime;
    class LocalDate;
    class LocalDateTime;
    template <class T> class EnumImpl;
    template <class T> class struct_wrapper_impl;
    template <class T> using struct_wrapper = Ptr<struct_wrapper_impl<T>>;
    template <class T> Type typeof();

    namespace detail
    {
        template<class W, class T> class InheritToString;
        template<class W, class T> class InheritGetHashCode;
        template<class W, class T> class InheritEquals;
    }

    /// Adds support for boxing value types to Ptr(ObjectImpl).
    class DOT_CLASS Object : public Ptr<ObjectImpl>
    {
        typedef Ptr<ObjectImpl> base;

    public: // CONSTRUCTORS

        /// Default constructor to create untyped Object.
        ///
        /// This constructor is used, among other things,
        /// as argument to lock(...) to provide thread safety.
        Object() = default;

        /// Construct Object from nullptr.
        Object(nullptr_t);

        /// Construct Object from Ptr(ObjectImpl).
        Object(const Ptr<ObjectImpl>& p);

        /// Construct Object from Ptr(T).
        template <class T>
        Object(const Ptr<T>& p) : base(p) {}

        /// Construct Object from ObjectImpl pointer.
        Object(ObjectImpl* value);

        /// Construct Object from String.
        Object(const String& value);

        /// Construct Object from const String.
        Object(const char* value);

        /// Construct Object from bool by boxing.
        Object(bool value);

        /// Construct Object from double by boxing.
        Object(double value);

        /// Construct Object from int by boxing.
        Object(int value);

        /// Construct Object from int by boxing.
        Object(int64_t value);

        /// Construct Object from char by boxing.
        Object(char value);

        /// Construct Object from Nullable by boxing.
        template <class T>
        Object(const Nullable<T>& value) { if (value.has_value()) *this = value.value(); }

        /// Construct Object from LocalMinute by boxing.
        Object(const LocalMinute & value);

        /// Construct Object from LocalTime by boxing.
        Object(const LocalTime& value);

        /// Construct Object from LocalDate by boxing.
        Object(const LocalDate& value);

        /// Construct Object from LocalDateTime by boxing.
        Object(const LocalDateTime& value);

        /// Construct Object from enum by wrapping it in EnumImpl(T).
        template <class T>
        Object(T value, typename std::enable_if<std::is_enum<T>::value>::type* enableif = 0) : base(new EnumImpl<T>(value)) {}

        /// Construct Object from struct wrapper, boxing the value if necessary.
        template <typename T>
        Object(struct_wrapper<T> value) : base(value) {}

        /// Construct Object from tuple, boxing the value if necessary.
        template <typename ... T>
        Object(const std::tuple<T...> & value) : Object(new struct_wrapper_impl<std::tuple<T...>>(value)) {}

    public: // METHODS

        /// Convert Object to enum. Error if Object does is not a boxed T.
        ///
        /// This method does not have a counterpart in C#. It provides a more
        /// convenient alternative to unboxing than using cast to EnumImpl(T).
        template <class T>
        T to_enum(typename std::enable_if<std::is_enum<T>::value>::type* enableif = 0) const { return Ptr<EnumImpl<T>>(*this)->value(); }

    public: // OPERATORS

        /// Forward to operator in Type Ptr(T).
        bool operator==(nullptr_t) const;

        /// Forward to operator in Type Ptr(T).
        bool operator!=(nullptr_t) const;

        /// Assign nullptr to Object.
        Object& operator=(nullptr_t);

        /// Assign Ptr(T) to Object.
        Object& operator=(const Ptr<ObjectImpl>& p);

        /// Assign String to Object by boxing.
        Object& operator=(const String& value);

        /// Assign const String to Object by boxing.
        Object& operator=(const char* value);

        /// Assign bool to Object by boxing.
        Object& operator=(bool value);

        /// Assign double to Object by boxing.
        Object& operator=(double value);

        /// Assign int to Object by boxing.
        Object& operator=(int value);

        /// Assign long to Object by boxing.
        Object& operator=(int64_t value);

        /// Assign char to Object by boxing.
        Object& operator=(char value);

        /// Assign struct_wrapper to Object by boxing.
        template <class T>
        Object& operator=(const struct_wrapper<T>& value) { base::operator=(value); return *this; }

        /// Assign tuple to Object by boxing.
        template <typename ... T>
        Object& operator=(const std::tuple<T...> & value) { base::operator=(new struct_wrapper_impl<std::tuple<T...>>(value)); return *this; }

        /// Assign Nullable to Object by boxing.
        template <class T>
        Object& operator=(const Nullable<T>& value) { if (value.has_value()) *this = value.value(); else *this = nullptr; return *this; }

        /// Assign LocalMinute to Object by boxing.
        Object& operator=(const LocalMinute& value);

        /// Assign LocalTime to Object by boxing.
        Object& operator=(const LocalTime& value);

        /// Assign LocalDate to Object by boxing.
        Object& operator=(const LocalDate& value);

        /// Assign LocalDateTime to Object by boxing.
        Object& operator=(const LocalDateTime& value);

        /// Convert Object to bool by unboxing. Error if Object does is not a boxed double.
        operator bool() const;

        /// Convert Object to double by unboxing. Error if Object does is not a boxed double.
        operator double() const;

        /// Convert Object to int by unboxing. Error if Object does is not a boxed int.
        operator int() const;

        /// Convert Object to long by unboxing. Error if Object does is not a boxed long.
        operator int64_t() const;

        /// Convert Object to char by unboxing. Error if Object does is not a boxed long.
        operator char() const;

        /// Convert Object to LocalMinute by unboxing. Error if Object does is not a boxed LocalMinute.
        operator LocalMinute() const;

        /// Convert Object to LocalTime by unboxing. Error if Object does is not a boxed LocalTime.
        operator LocalTime() const;

        /// Convert Object to LocalDate by unboxing. Error if Object does is not a boxed LocalDate.
        operator LocalDate() const;

        /// Convert Object to LocalDateTime by unboxing. Error if Object does is not a boxed LocalDateTime.
        operator LocalDateTime() const;

        /// Convert Object to struct_wrapper by unboxing. Error if Object does is not a boxed T.
        template <class T>
        operator struct_wrapper<T>() const { return this->as<struct_wrapper<T>>(); } // TODO - replace as by cast_to?

        /// Convert Object to tuple by unboxing. Error if Object does is not a boxed T.
        template <class ... T>
        operator std::tuple<T...>() const { return *this->as<struct_wrapper<std::tuple<T...>>>(); } // TODO - replace as by cast_to?

        template <class T, class enabled = typename std::enable_if<std::is_enum<T>::value>::type* >
        operator T() const { return Ptr<EnumImpl<T>>(*this)->value(); }

        /// Convert Object to String by unboxing. Error if Object does is not a boxed String.
        operator String() const;

        bool operator ==(Object rhs) const { throw Exception("Not implemented"); return false; }

        template <class T>
        operator Ptr<T>() const { return this->as<Ptr<T>>(); }

    public: // STATIC

        /// Determines whether the specified System.Object instances are the same instance.
        static bool reference_equals(Object obj_a, Object obj_b);
    };

    /// Initializes a new instance of Object.
    inline Object make_object() { return Object(new ObjectImpl); }
}

#include <dot/system/enum_impl.hpp>
#include <dot/detail/struct_wrapper.hpp>

namespace dot
{
    /// Wraps struct into Object.
    template <class T>
    class struct_wrapper_impl
        : public virtual ObjectImpl
        , public T
        , public detail::InheritToString<struct_wrapper_impl<T>, T>
        , public detail::InheritGetHashCode<struct_wrapper_impl<T>, T>
        , public detail::InheritEquals<struct_wrapper_impl<T>, T>
    {
    public:
        struct_wrapper_impl(const T& value) : T(value) {}

    public:
        static Type typeof()
        {
            return ::dot::typeof<T>();
        }

        Type get_type() override
        {
            return typeof();
        }

        virtual String to_string() override { return detail::InheritToString<struct_wrapper_impl<T>, T>::to_string(); }

        virtual size_t hash_code() override { return detail::InheritGetHashCode<struct_wrapper_impl<T>, T>::hash_code(); }

        bool equals(Object obj) override { return detail::InheritEquals<struct_wrapper_impl<T>, T>::equals(obj); }
    };
}

namespace std
{
    /// Implements hash struct used by STL unordered_map for Object.
    template <>
    struct hash<dot::Object> : public hash<dot::Ptr<dot::ObjectImpl>>
    {};

    /// Implements equal_to struct used by STL unordered_map for Object.
    template <>
    struct equal_to<dot::Object> : public equal_to<dot::Ptr<dot::ObjectImpl>>
    {};
}
