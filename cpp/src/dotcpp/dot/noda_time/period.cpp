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
#include <dot/noda_time/period.hpp>
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_date.hpp>
#include <dot/noda_time/local_date_time.hpp>

namespace dot
{
    Period::Period(const boost::posix_time::time_duration& d)
        : boost::posix_time::time_duration(d)
    {}

    Period::Period(const boost::gregorian::date_duration& d)
        : boost::posix_time::time_duration(24 * d.days(), 0, 0)
    {}

    Period::Period(const Period& other)
    {
        *this = other;
    }

    Period Period::between(const LocalDate& start, const LocalDate& end)
    {
        return end - start;
    }

    Period Period::between(const LocalDateTime& start, const LocalDateTime& end)
    {
        return end - start;
    }

    Period Period::between(const LocalTime& start, const LocalTime& end)
    {
        return end - start;
    }

    bool Period::equals(const Period& other) const
    {
        return *this == other;
    }

    Period Period::from_days(int days)
    {
        return boost::gregorian::date_duration(days);
    }

    Period Period::from_hours(int64_t hours)
    {
        return boost::posix_time::hours(hours);
    }

    Period Period::from_milliseconds(int64_t milliseconds)
    {
        return boost::posix_time::millisec(milliseconds);
    }

    Period Period::from_minutes(int64_t minutes)
    {
        return boost::posix_time::minutes(minutes);
    }

    Period Period::from_seconds(int64_t seconds)
    {
        return boost::posix_time::seconds(seconds);
    }

    Period Period::from_weeks(int weeks)
    {
        return boost::gregorian::weeks(weeks);
    }

    Period Period::operator+(const Period& other) const
    {
        return static_cast<boost::posix_time::time_duration>(*this) + static_cast<boost::posix_time::time_duration>(other);
    }

    bool Period::operator==(const Period & other) const
    {
        return static_cast<boost::posix_time::time_duration>(*this) == static_cast<boost::posix_time::time_duration>(other);;
    }

    bool Period::operator!=(const Period & other) const
    {
        return static_cast<boost::posix_time::time_duration>(*this) != static_cast<boost::posix_time::time_duration>(other);;
    }

    Period Period::operator-(const Period & other) const
    {
        return static_cast<boost::posix_time::time_duration>(*this) - static_cast<boost::posix_time::time_duration>(other);
    }

    Period::operator boost::gregorian::date_duration() const
    {
        return boost::gregorian::date_duration(base::hours() / 24);
    }
}
