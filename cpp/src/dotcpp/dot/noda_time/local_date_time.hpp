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
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_date.hpp>

namespace dot
{
    class string;
    class Period;
    class object;

    /// A date and time in a particular calendar system.
    /// A LocalDateTime value does not represent an instant on the global time line,
    /// because it has no associated time zone: "November 12th 2009 7pm, ISO calendar"
    /// occurred at different instants for different people around the world.
    ///
    class DOT_CLASS LocalDateTime : public boost::posix_time::ptime
    {
        typedef LocalDateTime self;
        typedef boost::posix_time::ptime base;
        friend LocalDate;
        friend LocalTime;

    public: // CONSTRUCTORS

        /// In C\# local datetime is a struct, and as all structs has default constructor
        /// that initializes all backing variables to 0. This means that default
        /// constructed value corresponds to 0001-01-01 00:00:00. Because Boost date_time
        /// library does not accept the date 0001-01-01, we will instead use the Unix epoch
        /// 1970-01-01 as default constructed value.
        LocalDateTime();

        /// Initializes a new instance of the LocalDateTime struct using the ISO calendar system.
        LocalDateTime(int year, int month, int day, int hour, int minute);

        /// Initializes a new instance of the LocalDateTime struct using the ISO calendar system.
        LocalDateTime(int year, int month, int day, int hour, int minute, int second);

        /// Initializes a new instance of the LocalDateTime struct using the ISO calendar system.
        LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond);

        /// Create from Boost posix_time.
        LocalDateTime(const boost::posix_time::ptime& time);

        /// Create from object.
        LocalDateTime(object const& rhs);


        /// Copy constructor.
        LocalDateTime(const LocalDateTime& other);

    private: // CONSTRUCTORS

        /// Create from date and time.
        LocalDateTime(const LocalDate& date, const LocalTime& time);

    public: // PROPERTIES

        /// Gets the day of this local date and time within the month.
        int day() const { return date().day(); }

        /// Gets the week day of this local date and time expressed as an iso_day_of_week value.
        int day_of_week() const { return date().day_of_week(); }

        /// Gets the day of this local date and time within the year.
        int day_of_year() const { return date().day_of_year(); }

        /// Gets the hour of day of this local date and time, in the range 0 to 23 inclusive.
        int hour() const { return static_cast<int>(base::time_of_day().hours()); }

        /// Gets the millisecond of this local date and time within the second, in the range 0 to 999 inclusive.
        int millisecond() const { return static_cast<int>(base::time_of_day().fractional_seconds() / 1000); }

        /// Gets the minute of this local date and time, in the range 0 to 59 inclusive.
        int minute() const { return static_cast<int>(base::time_of_day().minutes()); }

        /// Gets the month of this local date and time within the year.
        int month() const { return date().month(); }

        /// Gets the second of this local date and time within the minute, in the range 0 to 59 inclusive.
        int second() const { return static_cast<int>(base::time_of_day().seconds()); }

        /// Gets the time portion of this local date and time as a LocalTime.
        LocalTime time_of_day() const { return base::time_of_day(); }

        /// Gets the year of this local date and time.
        int year() const { return date().year(); }


    public:
        /// Add the specified Period to the date and time. Friendly alternative to operator+().
        static LocalDateTime add(const LocalDateTime& local_date_time, const Period& period);

        /// Indicates whether this date/time is earlier, later or the same as another one.
        int compare_to(const LocalDateTime& other) const;

        /// Indicates whether the current object is equal to another object of the same type.
        bool equals(const LocalDateTime& other) const;

        /// String that represents the current object.
        string to_string() const;

        /// Subtracts the specified date/time from this date/time, returning the result as a Period. Fluent alternative to operator-().
        Period minus(const LocalDateTime& local_date_time) const;

        /// Subtracts a Period from a local date/time. Fields are subtracted in the order provided by the Period.
        LocalDateTime minus(const Period& period) const;

        /// Returns the next LocalDateTime falling on the specified iso_day_of_week, at the same time of day as this value.
        /// This is a strict "next" - if this value on already falls on the target day of the week,
        /// the returned value will be a week later.
        ///
        LocalDateTime next(int target_day_of_week) const;

        /// Adds a Period to this local date/time. Fields are added in the order provided by the Period.
        LocalDateTime plus(const Period& period) const;

        /// Returns a new LocalDateTime representing the current value with the given number of days added.
        LocalDateTime plus_days(int days) const;

        /// Returns a new LocalDateTime representing the current value with the given number of hours added.
        LocalDateTime plus_hours(int64_t hours) const;

        /// Returns a new LocalDateTime representing the current value with the given number of milliseconds added.
        LocalDateTime plus_milliseconds(int64_t milliseconds) const;

        /// Returns a new LocalDateTime representing the current value with the given number of minutes added.
        LocalDateTime plus_minutes(int64_t minutes) const;

        /// Returns a new LocalDateTime representing the current value with the given number of months added.
        LocalDateTime plus_months(int months) const;

        /// Returns a new LocalDateTime representing the current value with the given number of seconds added.
        LocalDateTime plus_seconds(int64_t seconds) const;

        /// Returns a new LocalDateTime representing the current value with the given number of weeks added.
        LocalDateTime plus_weeks(int weeks) const;

        /// Returns a new LocalDateTime representing the current value with the given number of years added.
        LocalDateTime plus_years(int years) const;

        /// Returns the previous LocalDateTime falling on the specified iso_day_of_week, at the same time of day as this value.
        /// This is a strict "previous" - if this value on already falls on the target day of the week,
        /// the returned value will be a week earlier.
        LocalDateTime previous(int target_day_of_week) const;

        /// Subtracts one date/time from another, returning the result as a Period.
        static Period subtract(const LocalDateTime& lhs, const LocalDateTime& rhs);

        /// Subtracts the specified Period from the date and time. Friendly alternative to operator-().
        static LocalDateTime subtract(const LocalDateTime& local_date_time, const Period& period);

    public:
        /// Adds a Period to a local date/time. Fields are added in the order provided by the Period.
        /// This is a convenience operator over the plus(Period) method.
        LocalDateTime operator+(const Period& period) const;

        /// Implements the operator == (equality).
        bool operator==(const LocalDateTime& other) const;

        /// Implements the operator != (inequality).
        bool operator!=(const LocalDateTime& other) const;

        /// Compares two LocalDateTime values to see if the left one is strictly later than the right one.
        bool operator>(const LocalDateTime& other) const;

        /// Compares two LocalDateTime values to see if the left one is later than or equal to the right one.
        bool operator>=(const LocalDateTime& other) const;

        /// Compares two LocalDateTime values to see if the left one is strictly earlier than the right one.
        bool operator<(const LocalDateTime& other) const;

        /// Compares two LocalDateTime values to see if the left one is earlier than or equal to the right one.
        bool operator<=(const LocalDateTime& other) const;

        /// Subtracts one date/time from another, returning the result as a Period.
        Period operator-(const LocalDateTime& other) const;

        /// Subtracts a Period from a local date/time.
        /// Fields are subtracted in the order provided by the Period.
        /// This is a convenience operator over the minus(Period) method.
        LocalDateTime operator-(const Period& period) const;
    };
}
