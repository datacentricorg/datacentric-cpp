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

#include <dc/precompiled.hpp>
#include <dc/implement.hpp>
#include <dc/platform/time_zone/time_zone_data.hpp>
#include <dc/platform/context/context_base.hpp>

namespace dc
{
    void ZoneImpl::init(ContextBase context)
    {
        // Initialize base before executing the rest of the code in this method
        base::init(context);

        // Cache NodaTime timezone value in a private field
        //ZoneKey key = make_zone_key();
        //key->zone_name = zone_name;
        //date_time_zone_ = key->get_date_time_zone();
    }

    ZoneKey ZoneImpl::get_utc()
    {
        static ZoneKey key = []() {
            ZoneKey res = make_zone_key();
            res->zone_name = "UTC";
            return res;
        }();

        return key;
    }

    ZoneKey ZoneImpl::get_nyc()
    {
        static ZoneKey key = []() {
            ZoneKey res = make_zone_key();
            res->zone_name = "America/New_York";
            return res;
        }();

        return key;
    }

    ZoneKey ZoneImpl::get_london()
    {
        static ZoneKey key = []() {
            ZoneKey res = make_zone_key();
            res->zone_name = "Europe/London";
            return res;
        }();

        return key;
    }

    void ZoneImpl::configure(ContextBase context)
    {
        Zone zone1 = make_zone();
        zone1->zone_name = ZoneImpl::get_utc()->zone_name;

        Zone zone2 = make_zone();
        zone2->zone_name = ZoneImpl::get_nyc()->zone_name;

        Zone zone3 = make_zone();
        zone3->zone_name = ZoneImpl::get_london()->zone_name;

        dot::List<Record> result = dot::make_list<Record>({ zone1, zone2, zone3 });

        context->save_many(result);
    }

    dot::Type ZoneImpl::get_type() { return typeof(); }

    dot::Type ZoneImpl::typeof()
    {
        static dot::Type type_ =
            dot::make_type_builder<self>("dc", "Zone")
            ->with_field("time_zone_id", &self::zone_name)
            ->template with_base<TypedRecord<ZoneKeyImpl, ZoneImpl>>()
            ->with_constructor(&make_zone, { })
            ->build();
        return type_;
    }
}
