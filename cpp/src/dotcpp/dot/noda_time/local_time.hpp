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
#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace dot
{
    class string;
    class period;
    class local_date;
    class local_date_time;
    class object;

    /// local_time is an immutable struct representing a time of day,
    /// with no reference to a particular calendar, time zone or date.
    class DOT_CLASS local_time : public boost::posix_time::ptime
    {
        typedef local_time self;

    public: // CONSTRUCTORS

        /// Because in C\# local_date_time is a struct, it has default constructor
        /// that initializes all backing variables to 0. This means that default
        /// constructed value corresponds to 0001-01-01 00:00:00. We will
        /// replicate this behavior here.
        local_time();

        /// Creates a local time at the given hour and minute, with second, millisecond-of-second and tick-of-millisecond values of zero.
        local_time(int hour, int minute);

        /// Creates a local time at the given hour, minute and second, with millisecond-of-second and tick-of-millisecond values of zero.
        local_time(int hour, int minute, int second);

        /// Creates a local time at the given hour, minute, second and millisecond, with a tick-of-millisecond value of zero.
        local_time(int hour, int minute, int second, int millisecond);

        /// Create from Boost time_duration.
        local_time(const boost::posix_time::time_duration& time);

        /// Create from Boost posix_time.
        local_time(const boost::posix_time::ptime& time);

        /// Create from object.
        local_time(object const& rhs);

        /// Copy constructor.
        local_time(const local_time& other);

    public:
        /// Gets the hour of day of this local time, in the range 0 to 23 inclusive.
        int hour() const { return static_cast<int>(time_of_day().hours()); }

        /// Gets the millisecond of this local time within the second, in the range 0 to 999 inclusive.
        int millisecond() const { return static_cast<int>(time_of_day().fractional_seconds() / 1000); }

        /// Gets the minute of this local time, in the range 0 to 59 inclusive.
        int minute() const { return static_cast<int>(time_of_day().minutes()); }

        /// Gets the second of this local time within the minute, in the range 0 to 59 inclusive.
        int second() const { return static_cast<int>(time_of_day().seconds()); }

    public:
        /// Adds the specified period to the time. Friendly alternative to operator+().
        static local_time add(const local_time& time, const period& period);

        /// Indicates whether this time is earlier, later or the same as another one.
        int compare_to(const local_time& other) const;

        /// Compares this local time with the specified one for equality,
        /// by checking whether the two values represent the exact same local time, down to the tick.
        bool equals(const local_time& other) const;

        /// String that represents the current object.
        string to_string() const;

        /// Subtracts the specified time from this time, returning the result as a period. Fluent alternative to operator-().
        period minus(const local_time& time) const;

        /// Subtracts the specified period from this time. Fluent alternative to operator-().
        local_time minus(const period& period) const;

        /// Combines this local_time with the given local_date into a single local_date_time. Fluent alternative to operator+().
        local_date_time on(const local_date& date);

        /// Adds the specified period to this time. Fluent alternative to operator+().
        local_time plus(const period& period) const;

        /// Returns a new local_time representing the current value with the given number of hours added.
        local_time plus_hours(int64_t hours) const;

        /// Returns a new local_time representing the current value with the given number of milliseconds added.
        local_time plus_milliseconds(int64_t milliseconds) const;

        /// Returns a new local_time representing the current value with the given number of minutes added.
        local_time plus_minutes(int64_t minutes) const;

        /// Returns a new local_time representing the current value with the given number of seconds added.
        local_time plus_seconds(int64_t seconds) const;

        /// Subtracts one time from another, returning the result as a period with units of years, months and days.
        static period subtract(const local_time& lhs, const local_time& rhs);

        /// Subtracts the specified period from the time. Friendly alternative to operator-().
        static local_time subtract(const local_time& time, const period& period);

    public:
        /// Creates a new local time by adding a period to an existing time.
        /// The period must not contain any date-related units (days etc) with non-zero values.
        local_time operator+(const period& period) const;

        /// Compares two local times for equality, by checking whether they represent the exact same local time, down to the tick.
        bool operator==(const local_time& other) const;

        /// Compares two local times for inequality.
        bool operator!=(const local_time& other) const;

        /// Compares two local_time values to see if the left one is strictly later than the right one.
        bool operator>(const local_time& other) const;

        /// Compares two local_time values to see if the left one is later than or equal to the right one.
        bool operator>=(const local_time& other) const;

        /// Compares two local_time values to see if the left one is strictly earlier than the right one.
        bool operator<(const local_time& other) const;

        /// Compares two local_time values to see if the left one is earlier than or equal to the right one.
        bool operator<=(const local_time& other) const;

        /// Subtracts one time from another, returning the result as a period.
        period operator-(const local_time& other) const;

        /// Creates a new local time by subtracting a period from an existing time.
        /// The period must not contain any date-related units (days etc) with non-zero values.
        /// This is a convenience operator over the minus(period) method.
        local_time operator-(const period& period) const;

    public:
        operator boost::posix_time::time_duration() const;
    };
}
