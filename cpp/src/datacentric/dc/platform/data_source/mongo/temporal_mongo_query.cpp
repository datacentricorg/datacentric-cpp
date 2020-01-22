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
#include <dc/platform/data_source/mongo/temporal_mongo_query.hpp>
#include <dc/platform/data_source/mongo/temporal_mongo_query_cursor_impl.hpp>
#include <dc/platform/context/context_base.hpp>

namespace dc
{
    TemporalMongoQuery TemporalMongoQueryImpl::where(dot::FilterTokenBase value)
    {
        // Save filter.
        where_.push_back(value);
        return this;
    }

    TemporalMongoQuery TemporalMongoQueryImpl::sort_by(dot::FieldInfo key_selector)
    {
        // Save sort key.
        sort_.push_back(std::make_pair(key_selector, 1));
        return this;
    }

    TemporalMongoQuery TemporalMongoQueryImpl::sort_by_descending(dot::FieldInfo key_selector)
    {
        // Save sort key.
        sort_.push_back(std::make_pair(key_selector, -1));
        return this;
    }

    dot::ObjectCursorWrapperBase TemporalMongoQueryImpl::get_cursor()
    {
        return new TemporalMongoQueryCursorImpl(this);
    }

    dot::ObjectCursorWrapperBase TemporalMongoQueryImpl::select(dot::List<dot::FieldInfo> props, dot::Type element_type)
    {
        if (props.is_empty() || props->size() != element_type->get_generic_arguments()->size())
        {
            throw dot::Exception("Wrong number of FieldInfo passed to select method.");
        }
        dot::Type record_type = dot::typeof<Record>();

        // Apply dataset filters to query.
        dot::Query query = dot::make_query(collection_, type_);
        query = data_source_->apply_final_constraints(query, data_set_);

        for (dot::FilterTokenBase token : where_)
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

        dot::List<dot::Type> derived_types = dot::TypeImpl::get_derived_types(query->type_);

        // Apply filter by types.
        if (derived_types != nullptr)
        {
            dot::List<dot::String> derived_type_names = dot::make_list<dot::String>();
            for (dot::Type der_type : derived_types)
                derived_type_names->add(der_type->name());

            query->where(dot::FilterTokenBase(new dot::OperatorWrapperImpl("_t", "$eq", query->type_->name()))
                || dot::FilterTokenBase(new dot::OperatorWrapperImpl("_t", "$in", derived_type_names)));
        }
        else
            query->where(dot::FilterTokenBase(new dot::OperatorWrapperImpl("_t", "$eq", query->type_->name())));

        // Apply custom sort.
        for (std::pair<dot::FieldInfo, int> sort_token : sort_)
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
