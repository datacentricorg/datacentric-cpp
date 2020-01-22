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
#include <dc/platform/time_zone/time_zone_key.hpp>
#include <dc/platform/context/context_base.hpp>

namespace dc
{
    dot::Type ZoneKeyImpl::get_type() { return typeof(); }

    dot::Type ZoneKeyImpl::typeof()
    {
        static dot::Type type_ =
            dot::make_type_builder<self>("dc", "ZoneKey")
            ->with_field("zone_name", &self::zone_name)
            ->template with_base<TypedKey<ZoneKeyImpl, ZoneImpl>>()
            ->with_constructor(&make_zone_key, {  })
            ->build();
        return type_;
    }
}
