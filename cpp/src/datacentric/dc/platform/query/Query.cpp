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
#include <dc/platform/query/Query.hpp>
#include <dc/platform/context/context_base.hpp>

namespace dc
{
    query query_impl::where(bsoncxx::document::view_or_value value)
    {
        where_.append(bsoncxx::builder::basic::concatenate(value));
        return this;
    }

    object_cursor_wrapper query_impl::get_cursor()
    {
        return data_source_->load_by_query(this);
    }

    query query_impl::sort_by(dot::field_info key_selector)
    {
        sort_.append(bsoncxx::builder::basic::kvp((std::string) * (dot::string)key_selector->name, 1));
        return this;
    }

    query query_impl::sort_by_descending(dot::field_info key_selector)
    {
        sort_.append(bsoncxx::builder::basic::kvp((std::string) * (dot::string)key_selector->name, -1));
        return this;
    }

    object_cursor_wrapper query_impl::select(dot::list<dot::field_info> props, dot::type_t element_type)
    {
        this->element_type_ = element_type;
        this->select_ = props;
        return data_source_->load_by_query(this);
    }

}