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
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/period.hpp>
#include <dot/noda_time/local_date_time.hpp>
#include <dot/system/string.hpp>
#include <dot/system/object.hpp>

namespace dot
{
    LocalTime::LocalTime()
        : boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1), boost::posix_time::time_duration{ 0, 0, 0 })
    {}

    LocalTime::LocalTime(int hour, int minute)
        : boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1), boost::posix_time::time_duration {hour, minute, 0})
    {}

    LocalTime::LocalTime(int hour, int minute, int second)
        : boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1), boost::posix_time::time_duration {hour, minute, second})
    {}

    LocalTime::LocalTime(int hour, int minute, int second, int millisecond)
        : boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1), boost::posix_time::time_duration {hour, minute, second, millisecond * (boost::posix_time::time_duration::ticks_per_second() / 1000) })
    {}

    LocalTime::LocalTime(const boost::posix_time::time_duration& time)
        : boost::posix_time::ptime(boost::gregorian::date(1970, 1, 1), time)
    {}

    LocalTime::LocalTime(const boost::posix_time::ptime& time)
        : boost::posix_time::ptime(time)
    {}

    LocalTime::LocalTime(Object const& rhs) { *this = rhs.operator LocalTime(); }

    LocalTime::LocalTime(const LocalTime& other)
    {
        *this = other;
    }

    LocalTime LocalTime::add(const LocalTime& time, const Period& period)
    {
        return time + period;
    }

    int LocalTime::compare_to(const LocalTime& other) const
    {
        if (*this == other)
            return 0;

        return *this > other ? 1 : -1;
    }

    bool LocalTime::equals(const LocalTime& other) const
    {
        return *this == other;
    }

    String LocalTime::to_string() const
    {
        boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
        facet->format("%H:%M:%S.%f");
        std::stringstream stream;
        stream.imbue(std::locale(std::locale::classic(), facet));
        stream << *this;
        return stream.str().substr(0, 12);
    }

    Period LocalTime::minus(const LocalTime& time) const
    {
        return *this - time;
    }

    LocalTime LocalTime::minus(const Period& period) const
    {
        return *this - period;
    }

    LocalDateTime LocalTime::on(const LocalDate& date)
    {
        return {date, *this};
    }

    LocalTime LocalTime::plus(const Period& period) const
    {
        return *this + period;
    }

    LocalTime LocalTime::plus_hours(int64_t hours) const
    {
        return *this + boost::posix_time::hours(hours);
    }

    LocalTime LocalTime::plus_milliseconds(int64_t milliseconds) const
    {
        return *this + boost::posix_time::milliseconds(milliseconds);
    }

    LocalTime LocalTime::plus_minutes(int64_t minutes) const
    {
        return *this + boost::posix_time::minutes(minutes);
    }

    LocalTime LocalTime::plus_seconds(int64_t seconds) const
    {
        return *this + boost::posix_time::seconds(seconds);
    }

    Period LocalTime::subtract(const LocalTime& lhs, const LocalTime& rhs)
    {
        return lhs - rhs;
    }

    LocalTime LocalTime::subtract(const LocalTime& time, const Period& period)
    {
        return time - period;
    }

    LocalTime LocalTime::operator+(const Period& period) const
    {
        return static_cast<boost::posix_time::ptime>(*this) + static_cast<boost::posix_time::time_duration>(period);
    }

    bool LocalTime::operator==(const LocalTime& other) const
    {
        return time_of_day() == other.time_of_day();
    }

    bool LocalTime::operator!=(const LocalTime& other) const
    {
        return time_of_day() != other.time_of_day();
    }

    bool LocalTime::operator>(const LocalTime& other) const
    {
        return time_of_day() > other.time_of_day();
    }

    bool LocalTime::operator>=(const LocalTime& other) const
    {
        return time_of_day() >= other.time_of_day();
    }

    bool LocalTime::operator<(const LocalTime& other) const
    {
        return time_of_day() < other.time_of_day();
    }

    bool LocalTime::operator<=(const LocalTime& other) const
    {
        return time_of_day() <= other.time_of_day();
    }

    Period LocalTime::operator-(const LocalTime& other) const
    {
        return static_cast<boost::posix_time::ptime>(*this) - static_cast<boost::posix_time::ptime>(other);
    }

    LocalTime LocalTime::operator-(const Period& period) const
    {
        return static_cast<boost::posix_time::ptime>(*this) - static_cast<boost::posix_time::time_duration>(period);
    }

    LocalTime::operator boost::posix_time::time_duration() const
    {
        return time_of_day();
    }
}
