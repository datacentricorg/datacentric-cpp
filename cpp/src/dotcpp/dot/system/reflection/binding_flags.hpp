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

namespace dot
{
    /// Specifies flags that control binding and the way in which the
    /// search for members and types is conducted by reflection.
    enum class BindingFlags : int
    {
        /// Specifies that no binding flags are defined.
        default_flag = 0,

        /// Specifies that only members declared at the level of the supplied type's
        /// hierarchy should be considered. Inherited members are not considered.
        declared_only_flag = 2,

        /// Specifies that instance members are to be included in the search.
        instance_flag = 4,

        /// Specifies that static members are to be included in the search.
        static_flag = 8,

        /// Specifies that public members are to be included in the search.
        public_flag = 16
    };
}
