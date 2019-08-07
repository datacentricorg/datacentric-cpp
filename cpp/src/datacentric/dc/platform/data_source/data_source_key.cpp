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
#include <dc/platform/data_source/data_source_key.hpp>
#include <dc/platform/context/context_base.hpp>

namespace dc
{
    data_source_key data_source_key_impl::cache = new data_source_key_impl("Cache");

    data_source_key data_source_key_impl::master = new data_source_key_impl("Master");

    data_source_key_impl::data_source_key_impl(dot::string value)
    {
        data_source_id = value;
    }
}
