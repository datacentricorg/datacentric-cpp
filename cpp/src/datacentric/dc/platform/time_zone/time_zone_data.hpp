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
#include <dc/platform/context/context_base.hpp>
#include <dc/types/record/typed_record.hpp>
#include <dc/platform/time_zone/time_zone_key.hpp>

namespace dc
{
    class ZoneImpl; using Zone = dot::Ptr<ZoneImpl>;
    class ZoneKeyImpl; using ZoneKey = dot::Ptr<ZoneKeyImpl>;

    inline Zone make_zone();

    /// This class provides timezone conversion between UTC
    /// and local datetime for the specified timezone.
    ///
    /// Only the following timezone names are permitted:
    ///
    /// * UTC; and
    /// * IANA city timezones such as America/New_York
    ///
    /// Other 3-letter regional timezones such as EST or EDT are
    /// not permitted because they do not handle the transition
    /// between winter and summer time automatically for those
    /// regions where winter time is defined.
    ///
    /// Because zone_name is used to look up timezone conventions,
    /// it must match either the string UTC or the code in IANA
    /// timezone database precisely. The IANA city timezone code
    /// has two slash-delimited tokens, the first referencing the
    /// country and the other the city, for example America/New_York.
    class DC_CLASS ZoneImpl : public TypedRecordImpl<ZoneKeyImpl, ZoneImpl>
    {
        typedef ZoneImpl self;
        typedef TypedRecordImpl<ZoneKeyImpl, ZoneImpl> base;
        friend Zone make_zone();

    public: // FIELDS

        /// Unique timezone name.
        ///
        /// Only the following timezone names are permitted:
        ///
        /// * UTC; and
        /// * IANA city timezones such as America/New_York
        ///
        /// Other 3-letter regional timezones such as EST or EDT are
        /// not permitted because they do not handle the transition
        /// between winter and summer time automatically for those
        /// regions where winter time is defined.
        ///
        /// Because zone_name is used to look up timezone conventions,
        /// it must match either the string UTC or the code in IANA
        /// timezone database precisely. The IANA city timezone code
        /// has two slash-delimited tokens, the first referencing the
        /// country and the other the city, for example America/New_York.
        dot::String zone_name;

    public: // METHODS

        /// Set Context property and perform validation of the record's data,
        /// then initialize any fields or properties that depend on that data.
        ///
        /// This method must work when called multiple times for the same instance,
        /// possibly with a different context parameter for each subsequent call.
        ///
        /// All overrides of this method must call base.Init(context) first, then
        /// execute the rest of the code in the override.
        virtual void init(ContextBase context) override;

    public: // KEYS

        /// UTC timezone.
        static ZoneKey get_utc();

        /// New York City (United States) timezone.
        static ZoneKey get_nyc();

        /// London (Great Britain) timezone.
        static ZoneKey get_london();

    public: // STATIC

        /// This method will be invoked by context.Configure() for every
        /// class that is accessible by the executing assembly and marked
        /// with [Configurable] attribute.
        ///
        /// The method Configure(context) may be used to configure:
        ///
        /// * Reference data, and
        /// * In case of test mocks, test data
        ///
        /// The order in which Configure(context) method is invoked when
        /// multiple classes marked by [Configurable] attribute are present
        /// is undefined. The implementation of Configure(context) should
        /// not rely on any existing data, and should not invoke other
        /// Configure(context) method of other classes.
        ///
        /// The attribute [Configurable] is not inherited. To invoke
        /// Configure(context) method for multiple classes within the same
        /// inheritance chain, specify [Configurable] attribute for each
        /// class that provides Configure(context) method.
        static void configure(ContextBase context);

    public: // REFLECTION

        virtual dot::Type get_type();
        static dot::Type typeof();
    };

    /// Create an empty instance.
    inline Zone make_zone() { return new ZoneImpl; }
}
