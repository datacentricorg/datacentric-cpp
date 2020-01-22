/*
Copyright (C) 2015-present The DotCpp Authors.

This file is part of .C++, a native C++ implementation of
popular .NET class library APIs developed to facilitate
code reuse between C# and C++.

    http://github.com/dotcpp/dotcpp (source)
    http://dotcpp.org (documentation)

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

#include <dot/mongo/declare.hpp>
#include <dot/system/ptr.hpp>
#include <dot/mongo/mongo_db/cursor/cursor_wrapper.hpp>
#include <dot/mongo/mongo_db/query/query_builder.hpp>
#include <dot/mongo/mongo_db/mongo/collection.hpp>

namespace dot
{
    class query_impl;
    using query = dot::ptr<query_impl>;
    class mongo_data_source_data_impl;
    template <class class_, class prop>
    struct prop_wrapper;
    class data_source_data_impl; using data_source_data = dot::ptr<data_source_data_impl>;

    /// Holds expressions for where, select, and order_by/then_by
    /// parts of a query.
    class DOT_MONGO_CLASS query_impl : public dot::object_impl
    {
        friend query make_query(dot::collection collection, dot::type type);
        friend mongo_data_source_data_impl;
        friend class query_inner_impl;

    public:

        /// Adds filter to query.
        /// Example:
        /// @code
        ///   query->where(make_prop(&class_name::field_name) == "A" || make_prop(&class_name::field_name).in({ "B", "C" }) )
        /// @endcode
        virtual query where(token_base value);

        /// Makes group by some field.
        /// Example:
        /// @code
        ///   query->group_by(class_type->get_field("field_name"))
        /// @endcode
        virtual query group_by(dot::field_info key_selector);

        /// Sorts records by specified field.
        /// For subsequent sorting use then_by/then_by_descending after sort_by.
        /// Example:
        /// @code
        ///   query->sort_by(class_type->get_field("field_name"))
        /// @endcode
        virtual query sort_by(dot::field_info key_selector);

        /// Sorts by descending records by specified field.
        /// For subsequent sorting use then_by/then_by_descending after sort_by_descending.
        /// Example:
        /// @code
        ///   query->sort_by_descending(class_type->get_field("field_name"))
        /// @endcode
        virtual query sort_by_descending(dot::field_info key_selector);

        /// Sorts records by specified field.
        /// Use in subsequent sorting after sort_by/sort_by_descending.
        /// Example:
        /// @code
        ///   query->then_by(class_type->get_field("field_name"))
        /// @endcode
        virtual query then_by(dot::field_info key_selector);

        /// Sorts by descending records by specified field.
        /// Use in subsequent sorting after sort_by/sort_by_descending.
        /// Example:
        /// @code
        ///   query->then_by_descending(class_type->get_field("field_name"))
        /// @endcode
        virtual query then_by_descending(dot::field_info key_selector);

        /// Returns non-typed cursor to the result set of a query on a MongoDB server.
        virtual object_cursor_wrapper_base get_cursor();

        /// Makes projection according to specified fields.
        /// Result of projection is represented by tuple.
        /// Returns non-typed cursor to the result set of a query on a MongoDB server.
        virtual object_cursor_wrapper_base select(dot::list<dot::field_info> props, dot::type element_type);

        /// Makes group by some field.
        /// Example:
        /// @code
        ///   query->group_by(make_prop(&class_name::field_name))
        /// @endcode
        template <class class_, class prop>
        query group_by(prop_wrapper<class_, prop> key_selector)
        {
            return group_by(key_selector.prop_);
        }

        /// Sorts records by specified field.
        /// For subsequent sorting use then_by/then_by_descending after sort_by.
        /// Example:
        /// @code
        ///   query->sort_by(make_prop(&class_name::field_name))
        /// @endcode
        template <class class_, class prop>
        query sort_by(prop_wrapper<class_, prop> key_selector)
        {
            return sort_by(key_selector.prop_);
        }

        /// Sorts by descending records by specified field.
        /// For subsequent sorting use then_by/then_by_descending after sort_by_descending.
        /// Example:
        /// @code
        ///   query->sort_by_descending(make_prop(&class_name::field_name))
        /// @endcode
        template <class class_, class prop>
        query then_by(prop_wrapper<class_, prop> key_selector)
        {
            return then_by(key_selector.prop_);
        }

        /// Sorts records by specified field.
        /// Use in subsequent sorting after sort_by/sort_by_descending.
        /// Example:
        /// @code
        ///   query->then_by(make_prop(&class_name::field_name))
        /// @endcode
        template <class class_, class prop>
        query sort_by_descending(prop_wrapper<class_, prop> key_selector)
        {
            return sort_by_descending(key_selector.prop_);
        }

        /// Sorts by descending records by specified field.
        /// Use in subsequent sorting after sort_by/sort_by_descending.
        /// Example:
        /// @code
        ///   query->then_by_descending(make_prop(&class_name::field_name))
        /// @endcode
        template <class class_, class prop>
        query then_by_descending(prop_wrapper<class_, prop> key_selector)
        {
            return then_by_descending(key_selector.prop_);
        }

        /// Returns typed cursor to the result set of a query on a MongoDB server.
        /// Example:
        /// @code
        ///   query->get_cursor<class_name>()
        /// @endcode
        template <class t_record>
        cursor_wrapper<t_record> get_cursor()
        {
            return make_cursor_wrapper<t_record>(get_cursor());
        }

        /// Makes projection according to specified fields.
        /// Result of projection is represented by tuple.
        /// Returns typed cursor to the result set of a query on a MongoDB server.
        /// Example:
        /// @code
        ///   query->select<std::tuple<elem_type1, elemtype2, ...>>(list_with_field_info)
        /// @endcode
        template <class element>
        cursor_wrapper<element> select(dot::list<dot::field_info> props)
        {
            return make_cursor_wrapper<element>(select(props, dot::typeof<element>()));
        }

        /// Limits the number of documents passed to the next stage in the pipeline.
        query limit(int32_t limit_size);

        type type_;

    private:

        /// Base class for query implementation classes.
        /// Derived query impl class is hidden to cpp.
        class DOT_MONGO_CLASS query_inner_base_impl : public object_impl
        {
        public:

            virtual void where(token_base value) = 0;

            virtual void group_by(dot::field_info key_selectors) = 0;

            virtual void sort_by(dot::field_info key_selectors) = 0;

            virtual void sort_by_descending(dot::field_info key_selectors) = 0;

            virtual void then_by(dot::field_info key_selectors) = 0;

            virtual void then_by_descending(dot::field_info key_selectors) = 0;

            virtual object_cursor_wrapper_base get_cursor() = 0;

            virtual object_cursor_wrapper_base select(dot::list<dot::field_info> props, dot::type element_type) = 0;

            virtual void limit(int32_t limit_size) = 0;
        };

        using query_inner_base = ptr<query_inner_base_impl>;

        query_inner_base impl_;

        query_impl(dot::collection collection, dot::type type);
    };

    // Returns dot::query consturcted from given collection and type.
    inline query make_query(dot::collection collection, dot::type type)
    {
        return new query_impl(collection, type);
    }
}

