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
#include <dc/types/record/record.hpp>
#include <dc/platform/cursor/CursorWrapper.hpp>
#include <bsoncxx/builder/basic/document.hpp>

namespace dc
{
    class query_impl;
    using query = dot::ptr<query_impl>;
    class mongo_data_source_data_impl;
    template <class class_, class prop>
    struct prop_wrapper;
    class data_source_data_impl; using data_source_data = dot::ptr<data_source_data_impl>;

    /// Holds expressions for Where, Select, and OrderBy/ThenBy
    /// parts of a query.
    class DC_CLASS query_impl : public dot::object_impl
    {
        friend query make_query(data_source_data data_source, dot::object_id data_set, dot::type_t type);
        friend mongo_data_source_data_impl;

    public:

        virtual query where(bsoncxx::document::view_or_value value);

        virtual query sort_by(dot::field_info keySelector);

        virtual query sort_by_descending(dot::field_info keySelector);

        virtual object_cursor_wrapper get_cursor();

        virtual object_cursor_wrapper select(dot::list<dot::field_info> props, dot::type_t element_type);

        template <class class_, class prop>
        query sort_by(prop_wrapper<class_, prop> key_selector)
        {
            return sort_by(key_selector.prop_);
        }

        template <class class_, class prop>
        query sort_by_descending(prop_wrapper<class_, prop> key_selector)
        {
            return sort_by_descending(key_selector.prop_);
        }

        template <class t_record>
        cursor_wrapper<t_record> get_cursor()
        {
            return get_cursor()->template to_cursor_wrapper<t_record>();
        }

        template <class element>
        cursor_wrapper<element> select(dot::list<dot::field_info> props)
        {
            return select(props, dot::typeof<element>())->template to_cursor_wrapper<element>();
        }

    private:

        data_source_data data_source_;
        dot::object_id data_set_;
        bsoncxx::builder::basic::document where_;
        bsoncxx::builder::basic::document sort_;
        dot::type_t type_;
        dot::type_t element_type_;
        dot::list<dot::field_info> select_;


        query_impl(data_source_data data_source, dot::object_id data_set, dot::type_t type)
            : data_source_(data_source)
            , data_set_(data_set)
            , type_(type)
            , element_type_(type)
        {
        }
    };


    inline query make_query(data_source_data data_source, dot::object_id data_set, dot::type_t type)
    {
        return new query_impl(data_source, data_set, type);
    }
}

