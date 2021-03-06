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
#include <dc/types/record/deleted_record.hpp>
#include <dc/platform/context/context_base.hpp>

namespace dc
{
    DeletedRecordImpl::DeletedRecordImpl(Key key)
        : key_(key->to_string())
    {}

    DeletedRecordImpl::DeletedRecordImpl()
        : key_()
    {}

    dot::String DeletedRecordImpl::get_key()
    {
        return key_;
    }
}
