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
#include <dot/system/string.hpp>

namespace dot
{
    /// Helper class to provide to_string(value) for atomic and external
    /// classes where we cannot define member function to_string().
    ///
    /// This helper class is necessary because C++ permits partial specialization
    /// of classes, but not functions. Partial specialization is necessary when
    /// defining to_string for a type that is itself a template, for example
    /// Nullable.
    template <class T>
    struct ToStringImpl
    {
        /// Convert value to String; for empty or null values, return String::empty.
        ///
        /// The purpose of making this a deleted method here is to generate the
        /// following error when to_string(...) is invoked for a user defined type,
        /// while ToStringImpl template is not specialized for this type.
        ///
        /// dot::String dot::ToStringImpl<T>::to_string(const T &)'
        /// attempting to reference a deleted function
        ///
        /// with
        /// [
        ///   T = {type name here}
        /// ] (compiling source file system {file name here}
        ///
        /// To eliminate this error, add template specialization of ToStringImpl
        /// for the specified type:
        ///
        /// template <>
        /// class ToStringImpl<{type name here}>
        /// {
        /// public:
        ///     static String to_string(const {type name here}& value)
        ///     {
        ///         ...
        ///     }
        /// };
        static String to_string(const T& value) = delete;
    };

    /// Convert value to String; for empty or null values, return String::empty.
    ///
    /// This function must be defined for atomic and external classes where we
    /// cannot define member function to_string() via template specialization of
    /// the helper type ToStringImpl.
    template <class T>
    String to_string(const T& value)
    {
        // If the compiler generates the following error at this location:
        //
        // dot::String dot::ToStringImpl<T>::to_string(const T &)'
        // attempting to reference a deleted function
        //
        // with
        // [
        //   T = {type name here}
        // ] (compiling source file system {file name here}
        //
        // the reason is most likely that to_string(...) was invoked for a
        // user defined type, while ToStringImpl template was not specialized
        // for this type.
        //
        // To eliminate this error, add template specialization of ToStringImpl
        // for the specified type:
        //
        // template <>
        // class ToStringImpl<{type name here}>
        // {
        // public:
        //     static String to_string(const {type name here}& value)
        //     {
        //         ...
        //     }
        // };
        return ToStringImpl<T>::to_string(value);
    }
}
