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

namespace dot
{
    class Object;
    class StringImpl; class String;
    class TypeImpl; using Type = Ptr<TypeImpl>;

    /// All classes with reference semantics should derive from this type.
    /// It works with Ptr to provide an emulation of reference semantics in C++.
    class DOT_CLASS ObjectImpl : public ReferenceCounter
    {
        template<typename T>
        friend class Ptr;
        friend class Object;

    public: // METHODS

        /// Determines whether the specified Object is equal to the current Object.
        ///
        /// Default implementation in Object compares pointers. Derived classes
        /// can override this method to compare by value.
        virtual bool equals(Object obj);

        /// Serves as the default hash function.
        ///
        /// Default implementation in Object uses hash based on the pointer.
        /// Derived classes can override this method to provide value based hash.
        ///
        /// Methods equals() and hash_code() must always be overriden together
        /// to avoid the situation when objects are equal but hash is not.
        virtual size_t hash_code();

        /// Gets the Type of the current instance.
        virtual Type get_type();

        /// Gets the Type of the class.
        static Type typeof();

        /// String that represents the current Object.
        ///
        /// Default implementation in Object returns full name
        /// of the class by calling get_type().full_name. Derived types
        /// can override this method to provide custom conversion
        /// to String.
        virtual String to_string();
    };
}
