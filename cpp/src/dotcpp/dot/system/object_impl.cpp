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

#include <dot/precompiled.hpp>
#include <dot/implement.hpp>
#include <dot/system/object_impl.hpp>
#include <dot/system/object.hpp>
#include <dot/system/string.hpp>
#include <dot/system/type.hpp>

namespace dot
{
    /// Determines whether the specified object is equal to the current object.
    ///
    /// Default implementation in object compares pointers. Derived classes
    /// can override this method to compare by value.
    bool object_impl::equals(object obj)
    {
        return this == &(*obj);
    }

    /// Serves as the default hash function.
    ///
    /// Default implementation in object uses hash based on the pointer.
    /// Derived classes can override this method to provide value based hash.
    ///
    /// Methods equals() and hash_code() must always be overriden together
    /// to avoid the situation when objects are equal but hash is not.
    size_t object_impl::hash_code()
    {
        return size_t(this);
    }

    /// string that represents the current object.
    ///
    /// Default implementation in object returns full name
    /// of the class by calling get_type().FullName. Derived types
    /// can override this method to provide custom conversion
    /// to string.
    string object_impl::to_string()
    {
        return get_type()->full_name();
    }
}
