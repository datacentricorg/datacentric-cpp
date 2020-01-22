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
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace dot
{
    class LocalTime;
    class LocalDate;
    class LocalDateTime;
    class Object;

    /// Represents a period of time expressed in human chronological terms:
    /// hours, days, weeks, months and so on.
    class DOT_CLASS Period : public boost::posix_time::time_duration
    {
        typedef Period self;
        typedef boost::posix_time::time_duration base;

    public:
        Period(const boost::posix_time::time_duration& d);
        Period(const boost::gregorian::date_duration& d);

        /// Copy constructor.
        Period(const Period& other);

    public:
        /// Gets the number of days within this Period.
        int days() const
        {
            return static_cast<int>(base::hours() / 24);
        }

        /// Gets the number of hours within this Period.
        int64_t hours() const
        {
            return base::hours() % 24; // base::hours() returns total number of hours
        }

        /// Gets the number of milliseconds within this Period.
        int64_t milliseconds() const
        {
            return fractional_seconds() / 1000;
        }

    public:
        /// Returns the exact difference between two dates.
        static Period between(const LocalDate& start, const LocalDate& end);

        /// Returns the exact difference between two date/times.
        static Period between(const LocalDateTime& start, const LocalDateTime& end);

        /// Returns the exact difference between two times.
        static Period between(const LocalTime& start, const LocalTime& end);

        /// Compares the given Period for equality with this one.
        bool equals(const Period& other) const;

        /// Creates a Period representing the specified number of days.
        static Period from_days(int days);

        /// Creates a Period representing the specified number of hours.
        static Period from_hours(int64_t hours);

        /// Creates a Period representing the specified number of milliseconds.
        static Period from_milliseconds(int64_t milliseconds);

        /// Creates a Period representing the specified number of minutes.
        static Period from_minutes(int64_t minutes);

        /// Creates a Period representing the specified number of seconds.
        static Period from_seconds(int64_t seconds);

        /// Creates a Period representing the specified number of weeks.
        static Period from_weeks(int weeks);

    public:
        /// Adds two periods together, by simply adding the values for each property.
        Period operator+(const Period& other) const;

        /// Subtracts one period from another, by simply subtracting each property value.
        Period operator-(const Period& other) const;

        /// Compares two periods for equality.
        bool operator==(const Period& other) const;

        /// Compares two periods for inequality.
        bool operator!=(const Period& other) const;

    public:
        operator boost::gregorian::date_duration() const;
    };
}
