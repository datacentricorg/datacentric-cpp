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
    local_date_time::local_date_time()
        : boost::posix_time::ptime(local_date{ 1970, 1, 1 }, { 0, 0, 0 })
    {
    }

    local_date_time::local_date_time(int year, int month, int day, int hour, int minute)
        : boost::posix_time::ptime(local_date {year, month, day}, {hour, minute, 0})
    {}

    local_date_time::local_date_time(int year, int month, int day, int hour, int minute, int second)
        : boost::posix_time::ptime(local_date {year, month, day}, {hour, minute, second})
    {}

    local_date_time::local_date_time(int year, int month, int day, int hour, int minute, int second, int millisecond)
        : boost::posix_time::ptime(local_date {year, month, day}, {hour, minute, second, millisecond * 1000})
    {}

    local_date_time::local_date_time(const boost::posix_time::ptime& time)
        : boost::posix_time::ptime(time)
    {}

    local_date_time::local_date_time(const local_date_time& other)
    {
        *this = other;
    }

    local_date_time::local_date_time(const local_date& date, const local_time& time) :
        boost::posix_time::ptime(date, time)
    {}

    local_date_time::local_date_time(object const& rhs) { *this = rhs.operator local_date_time(); }

    local_date_time local_date_time::add(const local_date_time& local_date_time, const period& period)
    {
        return local_date_time + period;
    }

    int local_date_time::compare_to(const local_date_time& other) const
    {
        if (*this == other)
            return 0;

        return *this > other ? 1 : -1;
    }

    bool local_date_time::equals(const local_date_time& other) const
    {
        return *this == other;
    }

    string local_date_time::to_string() const
    {
        boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
        facet->format("%Y-%m-%d %H:%M:%S.%f");
        std::stringstream stream;
        stream.imbue(std::locale(std::locale::classic(), facet));
        stream << *this;
        return stream.str().substr(0, 23);
    }

    period local_date_time::minus(const local_date_time& local_date_time) const
    {
        return *this - local_date_time;
    }

    local_date_time local_date_time::minus(const period& period) const
    {
        return *this - period;
    }

    local_date_time local_date_time::next(int target_day_of_week) const
    {
        return {local_date(date()).next(target_day_of_week), time_of_day()};
    }

    local_date_time local_date_time::plus(const period& period) const
    {
        return *this + period;
    }

    local_date_time local_date_time::plus_days(int days) const
    {
        return *this + boost::gregorian::days(days);
    }

    local_date_time local_date_time::plus_hours(int64_t hours) const
    {
        return *this + boost::posix_time::hours(hours);
    }

    local_date_time local_date_time::plus_milliseconds(int64_t milliseconds) const
    {
        return *this + boost::posix_time::milliseconds(milliseconds);
    }

    local_date_time local_date_time::plus_minutes(int64_t minutes) const
    {
        return *this + boost::posix_time::minutes(minutes);
    }

    local_date_time local_date_time::plus_months(int months) const
    {
        return *this + boost::gregorian::months(months);
    }

    local_date_time local_date_time::plus_seconds(int64_t seconds) const
    {
        return *this + boost::posix_time::seconds(seconds);
    }

    local_date_time local_date_time::plus_weeks(int weeks) const
    {
        return *this + boost::gregorian::weeks(weeks);
    }

    local_date_time local_date_time::plus_years(int years) const
    {
        return *this + boost::gregorian::years(years);
    }

    local_date_time local_date_time::previous(int target_day_of_week) const
    {
        return {local_date(date()).previous(target_day_of_week), time_of_day()};
    }

    period local_date_time::subtract(const local_date_time& lhs, const local_date_time& rhs)
    {
        return lhs - rhs;
    }

    local_date_time local_date_time::subtract(const local_date_time& local_date_time, const period& period)
    {
        return local_date_time - period;
    }

    local_date_time local_date_time::operator+(const period& period) const
    {
        return static_cast<boost::posix_time::ptime>(*this) + static_cast<boost::posix_time::time_duration>(period);
    }

    bool local_date_time::operator==(const local_date_time& other) const
    {
        return static_cast<boost::posix_time::ptime>(*this) == static_cast<boost::posix_time::ptime>(other);
    }

    bool local_date_time::operator!=(const local_date_time& other) const
    {
        return static_cast<boost::posix_time::ptime>(*this) != static_cast<boost::posix_time::ptime>(other);
    }

    bool local_date_time::operator>(const local_date_time& other) const
    {
        return static_cast<boost::posix_time::ptime>(*this) > static_cast<boost::posix_time::ptime>(other);
    }

    bool local_date_time::operator>=(const local_date_time& other) const
    {
        return static_cast<boost::posix_time::ptime>(*this) >= static_cast<boost::posix_time::ptime>(other);
    }

    bool local_date_time::operator<(const local_date_time& other) const
    {
        return static_cast<boost::posix_time::ptime>(*this) < static_cast<boost::posix_time::ptime>(other);
    }

    bool local_date_time::operator<=(const local_date_time& other) const
    {
        return static_cast<boost::posix_time::ptime>(*this) <= static_cast<boost::posix_time::ptime>(other);
    }

    period local_date_time::operator-(const local_date_time& other) const
    {
        return static_cast<boost::posix_time::ptime>(*this) - static_cast<boost::posix_time::ptime>(other);
    }

    local_date_time local_date_time::operator-(const period& period) const
    {
        return static_cast<boost::posix_time::ptime>(*this) - static_cast<boost::posix_time::time_duration>(period);
    }
}
