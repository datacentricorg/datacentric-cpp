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
    dot::Type TimeZoneImpl::type() { return typeof(); }
    dot::Type TimeZoneImpl::typeof()
    {
        static dot::Type type_ =
            dot::make_type_builder<self>("dc", "TimeZone")
            ->with_field("time_zone_id", &self::time_zone_id)
            ->template with_base<TypedRecord<TimeZoneKeyImpl, TimeZoneImpl>>()
            ->with_constructor(&make_time_zone_data, {  })
            ->build();
        return type_;
    }
}
