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
#include <dot/system/object.hpp>
#include <dot/detail/struct_wrapper.hpp>
#include <dot/noda_time/local_date_time.hpp>
#include <dot/noda_time/period.hpp>
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_date.hpp>
#include <dot/system/string.hpp>

namespace dot
{
    LocalDateTime::LocalDateTime()
        : boost::posix_time::ptime(LocalDate{ 1970, 1, 1 }, { 0, 0, 0 })
    {
    }

    LocalDateTime::LocalDateTime(int year, int month, int day, int hour, int minute)
        : boost::posix_time::ptime(LocalDate {year, month, day}, {hour, minute, 0})
    {}

    LocalDateTime::LocalDateTime(int year, int month, int day, int hour, int minute, int second)
        : boost::posix_time::ptime(LocalDate {year, month, day}, {hour, minute, second})
    {}

    LocalDateTime::LocalDateTime(int year, int month, int day, int hour, int minute, int second, int millisecond)
        : boost::posix_time::ptime(LocalDate {year, month, day}, LocalTime {hour, minute, second, millisecond })
    {}

    LocalDateTime::LocalDateTime(const boost::posix_time::ptime& time)
        : boost::posix_time::ptime(time)
    {}

    LocalDateTime::LocalDateTime(const LocalDateTime& other)
    {
        *this = other;
    }

    LocalDateTime::LocalDateTime(const LocalDate& date, const LocalTime& time) :
        boost::posix_time::ptime(date, time)
    {}

    LocalDateTime::LocalDateTime(Object const& rhs) { *this = rhs.operator LocalDateTime(); }

    LocalDateTime LocalDateTime::add(const LocalDateTime& local_date_time, const Period& period)
    {
        return local_date_time + period;
    }

    int LocalDateTime::compare_to(const LocalDateTime& other) const
    {
        if (*this == other)
            return 0;

        return *this > other ? 1 : -1;
    }

    bool LocalDateTime::equals(const LocalDateTime& other) const
    {
        return *this == other;
    }

    String LocalDateTime::to_string() const
    {
        boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
        facet->format("%Y-%m-%d %H:%M:%S.%f");
        std::stringstream stream;
        stream.imbue(std::locale(std::locale::classic(), facet));
        stream << *this;
        return stream.str().substr(0, 23);
    }

    Period LocalDateTime::minus(const LocalDateTime& local_date_time) const
    {
        return *this - local_date_time;
    }

    LocalDateTime LocalDateTime::minus(const Period& period) const
    {
        return *this - period;
    }

    LocalDateTime LocalDateTime::next(int target_day_of_week) const
    {
        return {LocalDate(date()).next(target_day_of_week), time_of_day()};
    }

    LocalDateTime LocalDateTime::plus(const Period& period) const
    {
        return *this + period;
    }

    LocalDateTime LocalDateTime::plus_days(int days) const
    {
        return *this + boost::gregorian::days(days);
    }

    LocalDateTime LocalDateTime::plus_hours(int64_t hours) const
    {
        return *this + boost::posix_time::hours(hours);
    }

    LocalDateTime LocalDateTime::plus_milliseconds(int64_t milliseconds) const
    {
        return *this + boost::posix_time::milliseconds(milliseconds);
    }

    LocalDateTime LocalDateTime::plus_minutes(int64_t minutes) const
    {
        return *this + boost::posix_time::minutes(minutes);
    }

    LocalDateTime LocalDateTime::plus_months(int months) const
    {
        return *this + boost::gregorian::months(months);
    }

    LocalDateTime LocalDateTime::plus_seconds(int64_t seconds) const
    {
        return *this + boost::posix_time::seconds(seconds);
    }

    LocalDateTime LocalDateTime::plus_weeks(int weeks) const
    {
        return *this + boost::gregorian::weeks(weeks);
    }

    LocalDateTime LocalDateTime::plus_years(int years) const
    {
        return *this + boost::gregorian::years(years);
    }

    LocalDateTime LocalDateTime::previous(int target_day_of_week) const
    {
        return {LocalDate(date()).previous(target_day_of_week), time_of_day()};
    }

    Period LocalDateTime::subtract(const LocalDateTime& lhs, const LocalDateTime& rhs)
    {
        return lhs - rhs;
    }

    LocalDateTime LocalDateTime::subtract(const LocalDateTime& local_date_time, const Period& period)
    {
        return local_date_time - period;
    }

    LocalDateTime LocalDateTime::operator+(const Period& period) const
    {
        return static_cast<boost::posix_time::ptime>(*this) + static_cast<boost::posix_time::time_duration>(period);
    }

    bool LocalDateTime::operator==(const LocalDateTime& other) const
    {
        return static_cast<boost::posix_time::ptime>(*this) == static_cast<boost::posix_time::ptime>(other);
    }

    bool LocalDateTime::operator!=(const LocalDateTime& other) const
    {
        return static_cast<boost::posix_time::ptime>(*this) != static_cast<boost::posix_time::ptime>(other);
    }

    bool LocalDateTime::operator>(const LocalDateTime& other) const
    {
        return static_cast<boost::posix_time::ptime>(*this) > static_cast<boost::posix_time::ptime>(other);
    }

    bool LocalDateTime::operator>=(const LocalDateTime& other) const
    {
        return static_cast<boost::posix_time::ptime>(*this) >= static_cast<boost::posix_time::ptime>(other);
    }

    bool LocalDateTime::operator<(const LocalDateTime& other) const
    {
        return static_cast<boost::posix_time::ptime>(*this) < static_cast<boost::posix_time::ptime>(other);
    }

    bool LocalDateTime::operator<=(const LocalDateTime& other) const
    {
        return static_cast<boost::posix_time::ptime>(*this) <= static_cast<boost::posix_time::ptime>(other);
    }

    Period LocalDateTime::operator-(const LocalDateTime& other) const
    {
        return static_cast<boost::posix_time::ptime>(*this) - static_cast<boost::posix_time::ptime>(other);
    }

    LocalDateTime LocalDateTime::operator-(const Period& period) const
    {
        return static_cast<boost::posix_time::ptime>(*this) - static_cast<boost::posix_time::time_duration>(period);
    }
}
