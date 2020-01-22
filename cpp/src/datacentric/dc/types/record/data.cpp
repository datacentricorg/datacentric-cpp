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
#include <dc/types/record/data.hpp>
#include <dot/system/collections/generic/list.hpp>
#include <dot/system/reflection/activator.hpp>
#include <dc/types/record/key.hpp>
#include <dc/platform/reflection/class_info.hpp>
#include <dc/platform/context/context_base.hpp>
#include <dc/types/record/record.hpp>
#include <dot/mongo/mongo_db/mongo/settings.hpp>

namespace dc
{
    dot::type data_impl::get_type()
    {
        return typeof();
    }

    dot::type data_impl::typeof()
    {
        static dot::type result = []()-> dot::type
        {
            dot::type t = dot::make_type_builder<data_impl>("dc", "data")
                ->build();
            return t;
        }();
        return result;
    }
}
