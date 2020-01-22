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

#pragma once

#include <dc/declare.hpp>
#include <dot/system/ptr.hpp>

#include <dot/mongo/mongo_db/mongo/collection.hpp>
#include <dot/mongo/mongo_db/query/query.hpp>
#include <dc/platform/data_source/data_source_data.hpp>

namespace dc
{
    class mongo_query_impl; using mongo_query = dot::ptr<mongo_query_impl>;
    class data_source_data_impl; using data_source_data = dot::ptr<data_source_data_impl>;

    class DC_CLASS mongo_query_impl : public dot::object_impl
    {
        typedef mongo_query_impl self;

        friend mongo_query make_mongo_query(dot::collection collection,
            dot::type type,
            data_source_data data_source,
            dot::object_id data_set);

    public: // METHODS

        mongo_query where(dot::token_base value);

        mongo_query sort_by(dot::field_info key_selector);

        mongo_query sort_by_descending(dot::field_info key_selector);

        dot::object_cursor_wrapper_base get_cursor();

        dot::object_cursor_wrapper_base select(dot::list<dot::field_info> props, dot::type element_type);

        template <class class_, class prop>
        mongo_query sort_by(dot::prop_wrapper<class_, prop> key_selector)
        {
            return sort_by(key_selector.prop_);
        }

        template <class class_, class prop>
        mongo_query sort_by_descending(dot::prop_wrapper<class_, prop> key_selector)
        {
            return sort_by_descending(key_selector.prop_);
        }

        template <class t_record>
        dot::cursor_wrapper<t_record> get_cursor()
        {
            return dot::make_cursor_wrapper<t_record>(get_cursor());
        }

        template <class element>
        dot::cursor_wrapper<element> select(dot::list<dot::field_info> props)
        {
            return make_cursor_wrapper<element>(select(props, dot::typeof<element>()));
        }

    private:

        mongo_query_impl(dot::collection collection,
            dot::type type,
            data_source_data data_source,
            dot::object_id data_set)
            : collection_(collection)
            , type_(type)
            , data_source_(data_source)
            , data_set_(data_set)
        {
        }

        dot::collection collection_;
        dot::type type_;
        data_source_data data_source_;
        dot::object_id data_set_;

        std::vector<dot::token_base> where_;
        std::vector<std::pair<dot::field_info, int>> sort_;
    };

    inline mongo_query make_mongo_query(dot::collection collection,
                                        dot::type type,
                                        data_source_data data_source,
                                        dot::object_id data_set)
    {
        return new mongo_query_impl(collection, type, data_source, data_set);
    }


}