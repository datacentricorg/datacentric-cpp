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
#include <dc/platform/data_source/mongo/mongo_query.hpp>
#include <dc/platform/context/context_base.hpp>

namespace dc
{

    mongo_query mongo_query_impl::where(dot::token_base value)
    {
        where_.push_back(value);
        return this;
    }

    mongo_query mongo_query_impl::sort_by(dot::field_info key_selector)
    {

        sort_.push_back(std::make_pair(key_selector, 1));
        return this;
    }

    mongo_query mongo_query_impl::sort_by_descending(dot::field_info key_selector)
    {
        sort_.push_back(std::make_pair(key_selector, -1));
        return this;
    }

    dot::object_cursor_wrapper_base mongo_query_impl::get_cursor()
    {
        dot::hash_set<dot::object_id> lookup_set = data_source_->get_data_set_lookup_list(data_set_);
        dot::list<dot::object_id> lookup_list = dot::make_list<dot::object_id>(std::vector<dot::object_id>(lookup_set->begin(), lookup_set->end()));
        dot::type record_type = dot::typeof<record_base>();

        dot::query query = dot::make_query(collection_, type_)
            ->where(new dot::operator_wrapper_impl("_dataset", "$in", lookup_list));

        for (dot::token_base token : where_)
        {
            query->where(token);
        }

        query
            ->sort_by(record_type->get_field("_key"))
            ->then_by_descending(record_type->get_field("_dataset"))
            ->then_by_descending(record_type->get_field("_id"));

        query->group_by(record_type->get_field("_key"));

        dot::list<dot::type> derivedTypes = dot::type_impl::get_derived_types(query->type_);

        if (derivedTypes != nullptr)
        {
            dot::list<dot::string> derivedTypeNames = dot::make_list<dot::string>();
            for (dot::type derType : derivedTypes)
                derivedTypeNames->add(derType->name);

            query->where(dot::token_base(new dot::operator_wrapper_impl("_t", "$eq", query->type_->name))
                || dot::token_base(new dot::operator_wrapper_impl("_t", "$in", derivedTypeNames)));
        }
        else
            query->where(dot::token_base(new dot::operator_wrapper_impl("_t", "$eq", query->type_->name)));

        for (std::pair<dot::field_info, int> sort_token : sort_)
        {
            if (sort_token.second == 1)
                query->then_by(sort_token.first);
            if (sort_token.second == -1)
                query->then_by_descending(sort_token.first);
        }

        query
            ->then_by(record_type->get_field("_key"))
            ->then_by_descending(record_type->get_field("_dataset"))
            ->then_by_descending(record_type->get_field("_id"));

        return query->get_cursor();
    }

    dot::object_cursor_wrapper_base mongo_query_impl::select(dot::list<dot::field_info> props, dot::type element_type)
    {
        dot::hash_set<dot::object_id> lookup_set = data_source_->get_data_set_lookup_list(data_set_);
        dot::list<dot::object_id> lookup_list = dot::make_list<dot::object_id>(std::vector<dot::object_id>(lookup_set->begin(), lookup_set->end()));
        dot::type record_type = dot::typeof<record_base>();

        dot::query query = dot::make_query(collection_, type_)
            ->where(new dot::operator_wrapper_impl("_dataset", "$in", lookup_list));

        for (dot::token_base token : where_)
        {
            query->where(token);
        }

        query
            ->sort_by(record_type->get_field("_key"))
            ->then_by_descending(record_type->get_field("_dataset"))
            ->then_by_descending(record_type->get_field("_id"));

        query->group_by(record_type->get_field("_key"));

        dot::list<dot::type> derivedTypes = dot::type_impl::get_derived_types(query->type_);

        if (derivedTypes != nullptr)
        {
            dot::list<dot::string> derivedTypeNames = dot::make_list<dot::string>();
            for (dot::type derType : derivedTypes)
                derivedTypeNames->add(derType->name);

            query->where(dot::token_base(new dot::operator_wrapper_impl("_t", "$eq", query->type_->name))
                || dot::token_base(new dot::operator_wrapper_impl("_t", "$in", derivedTypeNames)));
        }
        else
            query->where(dot::token_base(new dot::operator_wrapper_impl("_t", "$eq", query->type_->name)));

        for (std::pair<dot::field_info, int> sort_token : sort_)
        {
            if (sort_token.second == 1)
                query->then_by(sort_token.first);
            if (sort_token.second == -1)
                query->then_by_descending(sort_token.first);
        }

        query
            ->then_by(record_type->get_field("_key"))
            ->then_by_descending(record_type->get_field("_dataset"))
            ->then_by_descending(record_type->get_field("_id"));

        return query->select(props, element_type);
    }

}