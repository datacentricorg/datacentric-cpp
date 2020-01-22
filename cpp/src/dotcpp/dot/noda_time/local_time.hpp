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
    class String;
    class Period;
    class LocalDate;
    class LocalDateTime;
    class Object;

    /// LocalTime is an immutable struct representing a time of day,
    /// with no reference to a particular calendar, time zone or date.
    class DOT_CLASS LocalTime : public boost::posix_time::ptime
    {
        typedef LocalTime self;

    public: // CONSTRUCTORS

        /// Because in C\# LocalDateTime is a struct, it has default constructor
        /// that initializes all backing variables to 0. This means that default
        /// constructed value corresponds to 0001-01-01 00:00:00. We will
        /// replicate this behavior here.
        LocalTime();

        /// Creates a local time at the given hour and minute, with second, millisecond-of-second and tick-of-millisecond values of zero.
        LocalTime(int hour, int minute);

        /// Creates a local time at the given hour, minute and second, with millisecond-of-second and tick-of-millisecond values of zero.
        LocalTime(int hour, int minute, int second);

        /// Creates a local time at the given hour, minute, second and millisecond, with a tick-of-millisecond value of zero.
        LocalTime(int hour, int minute, int second, int millisecond);

        /// Create from Boost time_duration.
        LocalTime(const boost::posix_time::time_duration& time);

        /// Create from Boost posix_time.
        LocalTime(const boost::posix_time::ptime& time);

        /// Create from Object.
        LocalTime(Object const& rhs);

        /// Copy constructor.
        LocalTime(const LocalTime& other);

    public:
        /// Gets the hour of day of this local time, in the range 0 to 23 inclusive.
        int hour() const { return static_cast<int>(time_of_day().hours()); }

        /// Gets the millisecond of this local time within the second, in the range 0 to 999 inclusive.
        int millisecond() const { return static_cast<int>(time_of_day().fractional_seconds() / (boost::posix_time::time_duration::ticks_per_second() / 1000)); }

        /// Gets the minute of this local time, in the range 0 to 59 inclusive.
        int minute() const { return static_cast<int>(time_of_day().minutes()); }

        /// Gets the second of this local time within the minute, in the range 0 to 59 inclusive.
        int second() const { return static_cast<int>(time_of_day().seconds()); }

    public:
        /// Adds the specified Period to the time. Friendly alternative to operator+().
        static LocalTime add(const LocalTime& time, const Period& period);

        /// Indicates whether this time is earlier, later or the same as another one.
        int compare_to(const LocalTime& other) const;

        /// Compares this local time with the specified one for equality,
        /// by checking whether the two values represent the exact same local time, down to the tick.
        bool equals(const LocalTime& other) const;

        /// String that represents the current Object.
        String to_string() const;

        /// Subtracts the specified time from this time, returning the result as a Period. Fluent alternative to operator-().
        Period minus(const LocalTime& time) const;

        /// Subtracts the specified Period from this time. Fluent alternative to operator-().
        LocalTime minus(const Period& period) const;

        /// Combines this LocalTime with the given LocalDate into a single LocalDateTime. Fluent alternative to operator+().
        LocalDateTime on(const LocalDate& date);

        /// Adds the specified Period to this time. Fluent alternative to operator+().
        LocalTime plus(const Period& period) const;

        /// Returns a new LocalTime representing the current value with the given number of hours added.
        LocalTime plus_hours(int64_t hours) const;

        /// Returns a new LocalTime representing the current value with the given number of milliseconds added.
        LocalTime plus_milliseconds(int64_t milliseconds) const;

        /// Returns a new LocalTime representing the current value with the given number of minutes added.
        LocalTime plus_minutes(int64_t minutes) const;

        /// Returns a new LocalTime representing the current value with the given number of seconds added.
        LocalTime plus_seconds(int64_t seconds) const;

        /// Subtracts one time from another, returning the result as a Period with units of years, months and days.
        static Period subtract(const LocalTime& lhs, const LocalTime& rhs);

        /// Subtracts the specified Period from the time. Friendly alternative to operator-().
        static LocalTime subtract(const LocalTime& time, const Period& period);

    public:
        /// Creates a new local time by adding a Period to an existing time.
        /// The Period must not contain any date-related units (days etc) with non-zero values.
        LocalTime operator+(const Period& period) const;

        /// Compares two local times for equality, by checking whether they represent the exact same local time, down to the tick.
        bool operator==(const LocalTime& other) const;

        /// Compares two local times for inequality.
        bool operator!=(const LocalTime& other) const;

        /// Compares two LocalTime values to see if the left one is strictly later than the right one.
        bool operator>(const LocalTime& other) const;

        /// Compares two LocalTime values to see if the left one is later than or equal to the right one.
        bool operator>=(const LocalTime& other) const;

        /// Compares two LocalTime values to see if the left one is strictly earlier than the right one.
        bool operator<(const LocalTime& other) const;

        /// Compares two LocalTime values to see if the left one is earlier than or equal to the right one.
        bool operator<=(const LocalTime& other) const;

        /// Subtracts one time from another, returning the result as a Period.
        Period operator-(const LocalTime& other) const;

        /// Creates a new local time by subtracting a Period from an existing time.
        /// The Period must not contain any date-related units (days etc) with non-zero values.
        /// This is a convenience operator over the minus(Period) method.
        LocalTime operator-(const Period& period) const;

    public:
        operator boost::posix_time::time_duration() const;
    };
}
