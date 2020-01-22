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
#include <dot/noda_time/local_date.hpp>
#include <dot/noda_time/period.hpp>
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_date_time.hpp>
#include <dot/system/object.hpp>
#include <dot/system/string.hpp>

namespace dot
{
    LocalDate::LocalDate()
        : boost::gregorian::date(1970, 1, 1)
    {
    }

    LocalDate::LocalDate(int year, int month, int day)
        : boost::gregorian::date(year, month, day)
    {}

    LocalDate::LocalDate(boost::gregorian::date date)
        : boost::gregorian::date(date)
    {}

    LocalDate::LocalDate(const LocalDate& other)
    {
        *this = other;
    }

    LocalDate::LocalDate(Object const& rhs) { *this = rhs.operator LocalDate(); }

    LocalDate LocalDate::add(const LocalDate& date, const Period& period)
    {
        return date + period;
    }

    LocalDateTime LocalDate::at(const LocalTime& time) const
    {
        return *this + time;
    }

    LocalDateTime LocalDate::at_midnight() const
    {
        return *this + LocalTime(0, 0);
    }

    int LocalDate::compare_to(const LocalDate& other) const
    {
        if (*this == other)
            return 0;

        return *this > other ? 1 : -1;
    }

    bool LocalDate::equals(const LocalDate& other) const
    {
        return *this == other;
    }

    String LocalDate::to_string() const
    {
        boost::posix_time::time_facet* facet = new boost::posix_time::time_facet();
        facet->format("%Y-%m-%d");
        std::stringstream stream;
        stream.imbue(std::locale(std::locale::classic(), facet));
        stream << this->at_midnight();
        return stream.str();
    }

    Period LocalDate::minus(const LocalDate& date) const
    {
        return *this - date;
    }

    LocalDate LocalDate::minus(const Period& period) const
    {
        return *this - period;
    }

    LocalDate LocalDate::next(int target_day_of_week) const
    {
        if (day_of_week() == target_day_of_week)
            return plus_weeks(1);
        auto wd = boost::gregorian::greg_weekday(target_day_of_week);
        return boost::gregorian::next_weekday(*this, wd);
    }

    LocalDate LocalDate::plus(const Period& period) const
    {
        return *this + period;
    }

    LocalDate LocalDate::plus_days(int days) const
    {
        return *this + boost::gregorian::days(days);
    }

    LocalDate LocalDate::plus_months(int months) const
    {
        return *this + boost::gregorian::months(months);
    }

    LocalDate LocalDate::plus_weeks(int weeks) const
    {
        return *this + boost::gregorian::weeks(weeks);
    }

    LocalDate LocalDate::plus_years(int years) const
    {
        return *this + boost::gregorian::years(years);
    }

    LocalDate LocalDate::previous(int target_day_of_week) const
    {
        if (day_of_week() == target_day_of_week)
            return plus_weeks(-1);
        auto wd = boost::gregorian::greg_weekday(target_day_of_week);
        return boost::gregorian::previous_weekday(*this, wd);
    }

    Period LocalDate::subtract(const LocalDate& lhs, const LocalDate& rhs)
    {
        return lhs - rhs;
    }

    LocalDate LocalDate::subtract(const LocalDate& date, const Period& period)
    {
        return date + period;
    }

    LocalDateTime LocalDate::operator+(const LocalTime& time) const
    {
        return {*this, time};
    }

    LocalDate LocalDate::operator+(const Period& period) const
    {
        return static_cast<boost::gregorian::date>(*this) + static_cast<boost::gregorian::date_duration>(period);
    }

    bool LocalDate::operator==(const LocalDate& other) const
    {
        return static_cast<boost::gregorian::date>(*this) == static_cast<boost::gregorian::date>(other);
    }

    bool LocalDate::operator!=(const LocalDate& other) const
    {
        return static_cast<boost::gregorian::date>(*this) != static_cast<boost::gregorian::date>(other);
    }

    bool LocalDate::operator>(const LocalDate& other) const
    {
        return static_cast<boost::gregorian::date>(*this) > static_cast<boost::gregorian::date>(other);
    }

    bool LocalDate::operator>=(const LocalDate& other) const
    {
        return static_cast<boost::gregorian::date>(*this) >= static_cast<boost::gregorian::date>(other);
    }

    bool LocalDate::operator<(const LocalDate& other) const
    {
        return static_cast<boost::gregorian::date>(*this) < static_cast<boost::gregorian::date>(other);
    }

    bool LocalDate::operator<=(const LocalDate& other) const
    {
        return static_cast<boost::gregorian::date>(*this) <= static_cast<boost::gregorian::date>(other);
    }

    Period LocalDate::operator-(const LocalDate& other) const
    {
        return static_cast<boost::gregorian::date>(*this) - static_cast<boost::gregorian::date>(other);
    }

    LocalDate LocalDate::operator-(const Period& period) const
    {
        return static_cast<boost::gregorian::date>(*this) - static_cast<boost::gregorian::date_duration>(period);
    }
}
