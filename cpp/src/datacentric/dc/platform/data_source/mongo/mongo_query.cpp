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
#include <dc/platform/data_source/mongo/mongo_query_cursor_impl.hpp>

namespace dc
{

    mongo_query mongo_query_impl::where(dot::filter_token_base value)
    {
        // Save filter.
        where_.push_back(value);
        return this;
    }

    mongo_query mongo_query_impl::sort_by(dot::field_info key_selector)
    {
        // Save sort key.
        sort_.push_back(std::make_pair(key_selector, 1));
        return this;
    }

    mongo_query mongo_query_impl::sort_by_descending(dot::field_info key_selector)
    {
        // Save sort key.
        sort_.push_back(std::make_pair(key_selector, -1));
        return this;
    }

    dot::object_cursor_wrapper_base mongo_query_impl::get_cursor()
    {
        dot::hash_set<dot::object_id> lookup_set = data_source_->get_data_set_lookup_list(data_set_);
        dot::list<dot::object_id> lookup_list = dot::make_list<dot::object_id>(std::vector<dot::object_id>(lookup_set->begin(), lookup_set->end()));
        dot::type record_type = dot::typeof<record_base>();

        // Apply dataset filters to query.
        dot::query query = dot::make_query(collection_, type_)
            ->where(new dot::operator_wrapper_impl("_dataset", "$in", lookup_list));

        // Apply custom filters to query.
        for (dot::filter_token_base token : where_)
        {
            query->where(token);
        }

        // Perform ordering by key, data_set, and _id.
        // Because we are created the ordered queryable for
        // the first time, begin from order_by, not then_by.
        query
            ->sort_by(record_type->get_field("_key"))
            ->then_by_descending(record_type->get_field("_dataset"))
            ->then_by_descending(record_type->get_field("_id"));

        // Perform group by key to get only one document per each key.
        query->group_by(record_type->get_field("_key"));

        dot::list<dot::type> derived_types = dot::type_impl::get_derived_types(query->type_);

        // Apply filter by types.
        if (derived_types != nullptr)
        {
            dot::list<dot::string> derived_type_names = dot::make_list<dot::string>();
            for (dot::type der_type : derived_types)
                derived_type_names->add(der_type->name());

            query->where(dot::filter_token_base(new dot::operator_wrapper_impl("_t", "$eq", query->type_->name()))
                || dot::filter_token_base(new dot::operator_wrapper_impl("_t", "$in", derived_type_names)));
        }
        else
            query->where(dot::filter_token_base(new dot::operator_wrapper_impl("_t", "$eq", query->type_->name())));

        // Apply custom sort.
        for (std::pair<dot::field_info, int> sort_token : sort_)
        {
            if (sort_token.second == 1)
                query->then_by(sort_token.first);
            if (sort_token.second == -1)
                query->then_by_descending(sort_token.first);
        }

        // Apply sort by key, data_set, and _id.
        query
            ->then_by(record_type->get_field("_key"))
            ->then_by_descending(record_type->get_field("_dataset"))
            ->then_by_descending(record_type->get_field("_id"));

        return new mongo_query_cursor_impl(query->get_cursor(), this->data_source_->context);
    }

    dot::object_cursor_wrapper_base mongo_query_impl::select(dot::list<dot::field_info> props, dot::type element_type)
    {
        if (props.is_empty() || props->size() != element_type->get_generic_arguments()->size())
        {
            throw dot::exception("Wrong number of field_info passed to select method.");
        }

        dot::hash_set<dot::object_id> lookup_set = data_source_->get_data_set_lookup_list(data_set_);
        dot::list<dot::object_id> lookup_list = dot::make_list<dot::object_id>(std::vector<dot::object_id>(lookup_set->begin(), lookup_set->end()));
        dot::type record_type = dot::typeof<record_base>();

        // Apply dataset filters to query.
        dot::query query = dot::make_query(collection_, type_)
            ->where(new dot::operator_wrapper_impl("_dataset", "$in", lookup_list));

        for (dot::filter_token_base token : where_)
        {
            query->where(token);
        }

        // Perform ordering by key, data_set, and _id.
        // Because we are created the ordered queryable for
        // the first time, begin from order_by, not then_by.
        query
            ->sort_by(record_type->get_field("_key"))
            ->then_by_descending(record_type->get_field("_dataset"))
            ->then_by_descending(record_type->get_field("_id"));

        // Perform group by key to get only one document per each key.
        query->group_by(record_type->get_field("_key"));

        dot::list<dot::type> derived_types = dot::type_impl::get_derived_types(query->type_);

        // Apply filter by types.
        if (derived_types != nullptr)
        {
            dot::list<dot::string> derived_type_names = dot::make_list<dot::string>();
            for (dot::type der_type : derived_types)
                derived_type_names->add(der_type->name());

            query->where(dot::filter_token_base(new dot::operator_wrapper_impl("_t", "$eq", query->type_->name()))
                || dot::filter_token_base(new dot::operator_wrapper_impl("_t", "$in", derived_type_names)));
        }
        else
            query->where(dot::filter_token_base(new dot::operator_wrapper_impl("_t", "$eq", query->type_->name())));

        // Apply custom sort.
        for (std::pair<dot::field_info, int> sort_token : sort_)
        {
            if (sort_token.second == 1)
                query->then_by(sort_token.first);
            if (sort_token.second == -1)
                query->then_by_descending(sort_token.first);
        }

        // Apply sort by key, data_set, and _id
        query
            ->then_by(record_type->get_field("_key"))
            ->then_by_descending(record_type->get_field("_dataset"))
            ->then_by_descending(record_type->get_field("_id"));

        return query->select(props, element_type);
    }
}
