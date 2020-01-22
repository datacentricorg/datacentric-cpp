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
    class String;
    class LocalTime;

    class DOT_CLASS LocalMinute
    {
        typedef LocalMinute self;

    public: // CONSTRUCTORS

        /// Default constructor.
        LocalMinute() = default;

        /// Creates local time to one minute precision from the specified hour and minute.
        LocalMinute(int hour, int minute);

        /// Copy constructor.
        LocalMinute(const LocalMinute& other);

    public: // PROPERTIES

        /// The hour of day, in the range 0 to 23 inclusive.
        int hour() const { return hour_; }

        /// The minute of the hour, in the range 0 to 59 inclusive.
        int minute() const { return minute_; }

        /// The minute of the day, in the range 0 to 59 inclusive.
        int minute_of_day() const
        {
            return 60 * hour() + minute();
        }

    public: // METHODS

        /// Converts this LocalMinute to LocalTime.
        LocalTime to_local_time() const;

        /// Indicates whether this time is earlier, later or the same as another one.
        int compare_to(const LocalMinute& other) const;

        /// Returns a hash code for this local time.
        size_t hash_code() const;

        /// Compares this local time with the specified one for equality,
        /// by checking whether the two values represent the exact same
        /// local minute.
        bool equals(const LocalMinute& other) const;

        /// Convert LocalMinute to ISO 8601 String in hh:mm format.
        String to_string() const;

    public: // OPERATORS

        /// Compares two local times for equality, by checking whether
        /// they represent the exact same local time, down to the tick.
        bool operator==(const LocalMinute& other) const;

        /// Compares two local times for inequality.
        bool operator!=(const LocalMinute& other) const;

        /// Compares two LocalMinute values to see if the left one
        /// is strictly earlier than the right one.
        bool operator<(const LocalMinute& other) const;

        /// Compares two LocalMinute values to see if the left one
        /// is earlier than or equal to the right one.
        bool operator<=(const LocalMinute& other) const;

        /// Compares two LocalMinute values to see if the left one
        /// is strictly later than the right one.
        bool operator>(const LocalMinute& other) const;

        /// Compares two LocalMinute values to see if the left one
        /// is later than or equal to the right one.
        bool operator>=(const LocalMinute& other) const;

    private:
        int hour_;
        int minute_;
    };
}
