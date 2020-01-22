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

#include <dc/types/record/record.hpp>
#include <dot/mongo/mongo_db/cursor/cursor_wrapper.hpp>
#include <dc/types/record/deleted_record.hpp>

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
            return current_record_;
        }

        virtual dot::object operator*() const override
        {
            return current_record_;
        }

        virtual void operator++() override
        {
            iterator_->operator++();
            current_record_ = skip_records();
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
        mongo_query_iterator_impl(dot::iterator_inner_base iterator, dot::object_cursor_wrapper_base cursor, dot::type query_type, context_base context)
            : iterator_(iterator)
            , cursor_(cursor)
            , query_type_(query_type)
            , context_(context)
        {
            current_record_ = skip_records();
        }

    private:

        /// Skips old and deleted documents and returns next relevant record.
        record skip_records()
        {
            // Iterate over documents returned by the cursor
            for(; *iterator_ != cursor_->end().iterator_; iterator_->operator++())
            {
                record obj = iterator_->operator*().as<record>();
                dot::string obj_key = obj->get_key();

                if (!current_key_.is_empty() && current_key_ == obj_key)
                {
                    // The key was encountered before. Because the data is sorted by
                    // key and then by dataset and ID, this indicates that the object
                    // is not the latest and can be skipped
                    // Continue to next record without returning
                    // the next item in the iterator result
                    continue;
                }
                else
                {
                    // The key was not encountered before, assign new value
                    current_key_ = obj_key;

                    // Skip if the result is a delete marker
                    if(obj.is<deleted_record>()) continue;

                    // Check if object could cast to query_type_.
                    // Skip, do not throw, if the cast fails.
                    //
                    // This behavior is different from loading by object_id or string key
                    // using load_or_null method. In case of load_or_null, the API requires
                    // an error when wrong type is requested. Here, we want to proceed
                    // as though the record does not exist because the query is expected
                    // to skip over records of type not derived from query_type_.
                    dot::type obj_type = obj->get_type();
                    if (!obj_type->equals(query_type_) && !obj_type->is_subclass_of(query_type_)) continue;

                    // Otherwise return the result
                    return to_record(obj);
                }
            }

            return nullptr;
        }

        /// Initializes record with context.
        record to_record(dot::object obj) const
        {
            record rec = obj.as<record>();
            rec->init(context_);
            return rec;
        }

    private: // FIELDS

        dot::iterator_inner_base iterator_;
        dot::object_cursor_wrapper_base cursor_;
        dot::type query_type_;
        context_base context_;

        dot::string current_key_;
        record current_record_;
    };

    /// Class implements dot::object_cursor_wrapper_base.
    /// Constructs from other object_cursor_wrapper_base and context_base
    /// to use it in iterator.
    class mongo_query_cursor_impl : public dot::object_cursor_wrapper_base_impl
    {
    public:

        /// Constructs from object_cursor_wrapper_base and context_base.
        mongo_query_cursor_impl(dot::object_cursor_wrapper_base cursor, dot::type query_type, context_base context)
            : cursor_(cursor)
            , query_type_(query_type)
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
            return dot::iterator_wrappper<dot::object>(new mongo_query_iterator_impl(cursor_->begin().iterator_, cursor_, query_type_, context_));
        }

        /// A dot::iterator_wrapper<dot::object> indicating cursor exhaustion, meaning that
        /// no documents are available from the cursor.
        dot::iterator_wrappper<dot::object> end()
        {
            return dot::iterator_wrappper<dot::object>(new mongo_query_iterator_impl(cursor_->end().iterator_, cursor_, query_type_, context_));
        }

    private: // FIELDS

        dot::object_cursor_wrapper_base cursor_;
        dot::type query_type_;
        context_base context_;
    };
}
