/*
Copyright (C) 2013-present The DataCentric Authors.

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

#include <dc/declare.hpp>
#include <dot/system/ptr.hpp>
#include <dc/types/record/typed_record.hpp>
#include <dc/platform/time_zone/time_zone_key.hpp>

namespace dc
{
    class time_zone_data_impl; using time_zone_data = dot::Ptr<time_zone_data_impl>;
    class time_zone_key_impl; using time_zone_key = dot::Ptr<time_zone_key_impl>;

    inline time_zone_data make_time_zone_data();

    /// This interface provides timezone for the conversion between
    /// datetime (by convention, always in UTC) and date, time,
    /// and minute (by convention, always in a specific timezone).
    ///
    /// Only the following timezones can be defined:
    ///
    /// * UTC timezone; and
    /// * IANA city timezones such as America/New_York
    ///
    /// Other 3-letter regional timezones such as EST or EDT are
    /// not permitted because they do not handle the transition
    /// between winter and summer time automatically for those
    /// regions where winter time is defined.
    ///
    /// Because time_zone_id is used to look up timezone conventions,
    /// it must match either the String UTC or the code in IANA
    /// timezone database precisely. The IANA city timezone code
    /// has two slash-delimited tokens, the first referencing the
    /// country and the other the city, for example America/New_York.
    class DC_CLASS time_zone_data_impl : public typed_record_impl<time_zone_key_impl, time_zone_data_impl>
    {
        typedef time_zone_data_impl self;
        friend time_zone_data make_time_zone_data();

    public: // FIELDS

        /// Unique timezone identifier.
        ///
        /// Only the following timezones can be defined:
        ///
        /// * UTC timezone; and
        /// * IANA city timezones such as America/New_York
        ///
        /// Other 3-letter regional timezones such as EST or EDT are
        /// not permitted because they do not handle the transition
        /// between winter and summer time automatically for those
        /// regions where winter time is defined.
        ///
        /// Because time_zone_id is used to look up timezone conventions,
        /// it must match either the String UTC or the code in IANA
        /// timezone database precisely. The IANA city timezone code
        /// has two slash-delimited tokens, the first referencing the
        /// country and the other the city, for example America/New_York.
        dot::String time_zone_id;

    public:
        virtual dot::Type type();
        static dot::Type typeof();
    };

    /// Create an empty instance.
    inline time_zone_data make_time_zone_data() { return new time_zone_data_impl; }
}
