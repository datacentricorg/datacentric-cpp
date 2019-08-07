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
    /// Static helper class for Double.
    class double_util
    {
    public: // STATIC

        /// Function Equal(double) using tolerance-based comparison.
        /// Treats values that differ by less than Double.tolerance as equal.
        static bool Equal(double lhs, double rhs) { return lhs >= rhs - dot::double_impl::tolerance && lhs <= rhs + dot::double_impl::tolerance; }

        /// Return $lhs > rhs$ using tolerance-based comparison.
        /// Treats values that differ by less than Double.tolerance as equal.
        static bool More(double lhs, double rhs) { return lhs > rhs + dot::double_impl::tolerance; }

        /// Return $lhs >= rhs$ using tolerance-based comparison.
        /// Treats values that differ by less than Double.tolerance as equal.
        static bool MoreOrEqual(double lhs, double rhs) { return lhs >= rhs - dot::double_impl::tolerance; }

        /// Return $lhs \lt rhs$ using tolerance-based comparison.
        /// Treats values that differ by less than Double.tolerance as equal.
        static bool Less(double lhs, double rhs) { return lhs < rhs - dot::double_impl::tolerance; }

        /// Return $lhs \le rhs$ using tolerance-based comparison.
        /// Treats values that differ by less than Double.tolerance as equal.
        static bool LessOrEqual(double lhs, double rhs) { return lhs <= rhs + dot::double_impl::tolerance; }

        /// Returns $1$ for $x \gt y$, $-1$ for $x \lt y$, and $0$ for $x==y$.
        /// Treats values that differ by less than Double.tolerance as equal.
        static int Compare(double x, double y)
        {
            if (x > y + dot::double_impl::tolerance) return 1;
            else if (x < y - dot::double_impl::tolerance) return -1;
            else return 0;
        }
    };
}
