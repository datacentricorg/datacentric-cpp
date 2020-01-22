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

#include <dc/declare.hpp>
#include <dot/system/double.hpp>

namespace dc
{
    /// Static helper class for double.
    class double_util
    {
    public: // STATIC

        /// Function equal(double) using tolerance-based comparison.
        /// Treats values that differ by less than double.tolerance as equal.
        static bool equal(double lhs, double rhs) { return lhs >= rhs - dot::DoubleImpl::tolerance && lhs <= rhs + dot::DoubleImpl::tolerance; }

        /// Return $lhs > rhs$ using tolerance-based comparison.
        /// Treats values that differ by less than double.tolerance as equal.
        static bool more(double lhs, double rhs) { return lhs > rhs + dot::DoubleImpl::tolerance; }

        /// Return $lhs >= rhs$ using tolerance-based comparison.
        /// Treats values that differ by less than double.tolerance as equal.
        static bool more_or_equal(double lhs, double rhs) { return lhs >= rhs - dot::DoubleImpl::tolerance; }

        /// Return $lhs \lt rhs$ using tolerance-based comparison.
        /// Treats values that differ by less than double.tolerance as equal.
        static bool less(double lhs, double rhs) { return lhs < rhs - dot::DoubleImpl::tolerance; }

        /// Return $lhs \le rhs$ using tolerance-based comparison.
        /// Treats values that differ by less than double.tolerance as equal.
        static bool less_or_equal(double lhs, double rhs) { return lhs <= rhs + dot::DoubleImpl::tolerance; }

        /// Returns $1$ for $x \gt y$, $-1$ for $x \lt y$, and $0$ for $x==y$.
        /// Treats values that differ by less than double.tolerance as equal.
        static int compare(double x, double y)
        {
            if (x > y + dot::DoubleImpl::tolerance) return 1;
            else if (x < y - dot::DoubleImpl::tolerance) return -1;
            else return 0;
        }
    };
}
