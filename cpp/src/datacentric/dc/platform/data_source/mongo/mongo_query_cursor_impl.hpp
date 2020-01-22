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

#include <dc/types/record/record_base.hpp>
#include <dot/mongo/mongo_db/cursor/cursor_wrapper.hpp>

namespace dc
{
    class mongo_query_iterator_impl; using mongo_query_iterator = dot::ptr<mongo_query_iterator_impl>;
    class mongo_query_cursor_impl; using mongo_query_cursor = dot::ptr<mongo_query_cursor_impl>;

    /// Class implements dot::iterator_wrapper methods.
    /// Constructs from iterator_inner_base to filter input records and initialize them with context.
    class DC_CLASS mongo_query_iterator_impl : public dot::iterator_inner_base_impl
    {
    public:

        virtual dot::object operator*() override
        {
            return to_record(iterator_->operator*());
        }

        virtual dot::object operator*() const override
        {
            return to_record(iterator_->operator*());
        }

        virtual void operator++() override
        {
            iterator_->operator++();
        }

        virtual bool operator!=(dot::iterator_inner_base rhs) override
        {
            return *iterator_ != rhs.as<mongo_query_iterator>()->iterator_;
        }

        virtual bool operator==(dot::iterator_inner_base rhs) override
        {
            return *iterator_ == rhs.as<mongo_query_iterator>()->iterator_;
        }

        /// Constructs from iterator_inner_base and context_base.
        mongo_query_iterator_impl(dot::iterator_inner_base iterator, context_base context)
            : iterator_(iterator)
            , context_(context)
        {}

    private:

        /// Initializes record with context.
        record_base to_record(dot::object obj) const
        {
            record_base rec = obj.as<record_base>();
            rec->init(context_);
            return rec;
        }

    private: // FIELDS

        dot::iterator_inner_base iterator_;
        context_base context_;
    };

    /// Class implements dot::object_cursor_wrapper_base.
    /// Constructs from other object_cursor_wrapper_base and context_base
    /// to use it in iterator.
    class mongo_query_cursor_impl : public dot::object_cursor_wrapper_base_impl
    {
    public:

        /// Constructs from object_cursor_wrapper_base and context_base.
        mongo_query_cursor_impl(dot::object_cursor_wrapper_base cursor, context_base context)
            : cursor_(cursor)
            , context_(context)
        {}

        /// A dot::iterator_wrapper<dot::object> that points to the beginning of any available
        /// results. If begin() is called more than once, the dot::iterator_wrapper
        /// returned points to the next remaining result, not the result of
        /// the original call to begin().
        ///
        /// For a tailable cursor, when cursor.begin() == cursor.end(), no
        /// documents are available.  Each call to cursor.begin() checks again
        /// for newly-available documents.
        dot::iterator_wrappper<dot::object> begin()
        {
            return dot::iterator_wrappper<dot::object>(new mongo_query_iterator_impl(cursor_->begin().iterator_, context_));
        }

        /// A dot::iterator_wrapper<dot::object> indicating cursor exhaustion, meaning that
        /// no documents are available from the cursor.
        dot::iterator_wrappper<dot::object> end()
        {
            return dot::iterator_wrappper<dot::object>(new mongo_query_iterator_impl(cursor_->end().iterator_, context_));
        }

    private: // FIELDS

        dot::object_cursor_wrapper_base cursor_;
        context_base context_;
    };
}
