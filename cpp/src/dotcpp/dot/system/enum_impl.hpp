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
#include <dot/system/object_impl.hpp>

namespace dot
{
    /// Wrapper around double to make it convertible to object (boxing).
    template <class T>
    class enum_impl : public virtual object_impl
    {
        friend object;
        T value_;

    public: // CONSTRUCTORS

        /// Create from value (box).
        enum_impl(T value) : value_(value) {}

        /// Create from object (first step in two-step conversion during unboxing).
        enum_impl(object rhs) : value_(ptr<enum_impl<T>>(rhs)->value_) { }

    public: // METHODS

        /// Returns enum value
        T value() { return value_; };

        /// Returns a value indicating whether this instance is equal to a specified object.
        bool equals(object obj) override
        {
            if (this == &(*obj)) return true;

            if (obj.is<ptr<enum_impl<T>>>())
            {
                return value_ == obj.as<ptr<enum_impl<T>>>()->value_;
            }

            return false;
        }

        /// Returns the hash code for this instance.
        virtual size_t hash_code() override { return std::hash<int>()((int)value_); }

        /// Converts the numeric value of this instance to its equivalent string representation.
        virtual string to_string() override { return dot::to_string(value_); }

    public: // OPERATORS

        /// Convert to enum value
        operator T() const { return value_; }

    public: // REFLECTION

        static type_t typeof() { return dot::typeof<T>(); }
        virtual type_t type() override { return typeof(); }
    };
}
