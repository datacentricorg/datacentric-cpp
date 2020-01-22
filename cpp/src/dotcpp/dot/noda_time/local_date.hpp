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
    class Period;
    class LocalTime;
    class LocalDateTime;
    class object;

    /// LocalDate is an immutable struct representing a date within the calendar,
    /// with no reference to a particular time zone or time of day.
    class DOT_CLASS LocalDate : public boost::gregorian::date
    {
        typedef LocalDate self;

    public: // CONSTRUCTORS

        /// In C\# local date is a struct, and as all structs it has default constructor
        /// that initializes all backing variables to 0. This means that default
        /// constructed value corresponds to 0001-01-01. As Boost date_time library
        /// does not accept the date 0001-01-01, we will instead use the Unix epoch
        /// 1970-01-01 as default constructed value.
        LocalDate();

        /// Constructs an instance for the given year, month and day in the ISO calendar.
        LocalDate(int year, int month, int day);

        /// Create from Boost gregorian date.
        LocalDate(boost::gregorian::date date);

        /// Copy constructor.
        LocalDate(const LocalDate& other);

        /// Create from object.
        LocalDate(object const& rhs);

    public:
        /// Adds the specified Period to the date. Friendly alternative to operator+().
        static LocalDate add(const LocalDate& date, const Period& period);

        /// Combines this LocalDate with the given LocalTime into a single LocalDateTime. Fluent alternative to operator+().
        LocalDateTime at(const LocalTime& time) const;

        /// Gets a LocalDateTime at midnight on the date represented by this local date.
        LocalDateTime at_midnight() const;

        /// Indicates whether this date is earlier, later or the same as another one.
        int compare_to(const LocalDate& other) const;

        /// Compares two LocalDate values for equality. This requires that the dates be the same, within the same calendar.
        bool equals(const LocalDate& other) const;

        /// String that represents the current object.
        string to_string() const;

        /// Subtracts the specified date from this date, returning the result as a Period with units of years, months and days. Fluent alternative to operator-().
        Period minus(const LocalDate& date) const;

        /// Subtracts the specified Period from this date. Fluent alternative to operator-().
        LocalDate minus(const Period& period) const;

        /// Returns the next LocalDate falling on the specified iso_day_of_week.
        /// This is a strict "next" - if this date on already falls on the target day of the week,
        /// the returned value will be a week later.
        LocalDate next(int target_day_of_week) const;

        /// Adds the specified Period to this date. Fluent alternative to operator+().
        LocalDate plus(const Period& period) const;

        /// Returns a new LocalDate representing the current value with the given number of days added.
        LocalDate plus_days(int days) const;

        /// Returns a new LocalDate representing the current value with the given number of months added.
        LocalDate plus_months(int months) const;

        /// Returns a new LocalDate representing the current value with the given number of weeks added.
        LocalDate plus_weeks(int weeks) const;

        /// Returns a new LocalDate representing the current value with the given number of years added.
        LocalDate plus_years(int years) const;

        /// Returns the previous LocalDate falling on the specified iso_day_of_week.
        /// This is a strict "previous" - if this date on already falls on the
        /// target day of the week, the returned value will be a week earlier.
        LocalDate previous(int target_day_of_week) const;

        /// Subtracts one date from another, returning the result as a Period with units of years, months and days.
        static Period subtract(const LocalDate& lhs, const LocalDate& rhs);

        /// Subtracts the specified Period from the date. Friendly alternative to operator-().
        static LocalDate subtract(const LocalDate& date, const Period& period);

    public:
        /// Combines the given LocalDate and LocalTime components into a single LocalDateTime.
        LocalDateTime operator+(const LocalTime& time) const;

        /// Adds the specified Period to the date.
        LocalDate operator+(const Period& period) const;

        /// Compares two LocalDate values for equality. This requires that the dates be the same, within the same calendar.
        bool operator==(const LocalDate& other) const;

        /// Compares two LocalDate values for inequality.
        bool operator!=(const LocalDate& other) const;

        /// Compares two dates to see if the left one is strictly later than the right one.
        bool operator>(const LocalDate& other) const;

        /// Compares two dates to see if the left one is later than or equal to the right one.
        bool operator>=(const LocalDate& other) const;

        /// Compares two dates to see if the left one is strictly earlier than the right one.
        bool operator<(const LocalDate& other) const;

        /// Compares two dates to see if the left one is earlier than or equal to the right one.
        bool operator<=(const LocalDate& other) const;

        /// Subtracts one date from another, returning the result as a Period with units of years, months and days.
        Period operator-(const LocalDate& other) const;

        /// Subtracts the specified Period from the date. This is a convenience operator over the minus(Period) method.
        LocalDate operator-(const Period& period) const;
    };
}
