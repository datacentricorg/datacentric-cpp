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
    class MongoQueryIteratorImpl; using MongoQueryIterator = dot::Ptr<MongoQueryIteratorImpl>;
    class MongoQueryCursorImpl; using MongoQueryCursor = dot::Ptr<MongoQueryCursorImpl>;

    /// Class implements dot::iterator_wrapper methods.
    /// Constructs from IteratorInnerBase to filter input records and initialize them with context.
    class DC_CLASS MongoQueryIteratorImpl : public dot::IteratorInnerBaseImpl
    {
    public:

        virtual dot::Object operator*() override
        {
            return current_record_;
        }

        virtual dot::Object operator*() const override
        {
            return current_record_;
        }

        virtual void operator++() override
        {
            iterator_->operator++();
            current_record_ = skip_records();
        }

        virtual bool operator!=(dot::IteratorInnerBase rhs) override
        {
            return *iterator_ != rhs.as<MongoQueryIterator>()->iterator_;
        }

        virtual bool operator==(dot::IteratorInnerBase rhs) override
        {
            return *iterator_ == rhs.as<MongoQueryIterator>()->iterator_;
        }

        /// Constructs from IteratorInnerBase and ContextBase.
        MongoQueryIteratorImpl(dot::IteratorInnerBase iterator, dot::ObjectCursorWrapperBase cursor, dot::Type query_type, ContextBase context)
            : iterator_(iterator)
            , cursor_(cursor)
            , query_type_(query_type)
            , context_(context)
        {
            current_record_ = skip_records();
        }

    private:

        /// Skips old and deleted documents and returns next relevant record.
        Record skip_records()
        {
            // Iterate over documents returned by the cursor
            for(; *iterator_ != cursor_->end().iterator_; iterator_->operator++())
            {
                Record obj = iterator_->operator*().as<Record>();
                dot::String obj_key = obj->get_key();

                if (!current_key_.is_empty() && current_key_ == obj_key)
                {
                    // The key was encountered before. Because the data is sorted by
                    // key and then by dataset and ID, this indicates that the Object
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
                    if(obj.is<DeletedRecord>()) continue;

                    // Check if Object could cast to query_type_.
                    // Skip, do not throw, if the cast fails.
                    //
                    // This behavior is different from loading by TemporalId or String key
                    // using load_or_null method. In case of load_or_null, the API requires
                    // an error when wrong type is requested. Here, we want to proceed
                    // as though the record does not exist because the query is expected
                    // to skip over records of type not derived from query_type_.
                    dot::Type obj_type = obj->get_type();
                    if (!obj_type->equals(query_type_) && !obj_type->is_subclass_of(query_type_)) continue;

                    // Otherwise return the result
                    return to_record(obj);
                }
            }

            return nullptr;
        }

        /// Initializes record with context.
        Record to_record(dot::Object obj) const
        {
            Record rec = obj.as<Record>();
            rec->init(context_);
            return rec;
        }

    private: // FIELDS

        dot::IteratorInnerBase iterator_;
        dot::ObjectCursorWrapperBase cursor_;
        dot::Type query_type_;
        ContextBase context_;

        dot::String current_key_;
        Record current_record_;
    };

    /// Class implements dot::ObjectCursorWrapperBase.
    /// Constructs from other ObjectCursorWrapperBase and ContextBase
    /// to use it in iterator.
    class MongoQueryCursorImpl : public dot::ObjectCursorWrapperBaseImpl
    {
    public:

        /// Constructs from ObjectCursorWrapperBase and ContextBase.
        MongoQueryCursorImpl(dot::ObjectCursorWrapperBase cursor, dot::Type query_type, ContextBase context)
            : cursor_(cursor)
            , query_type_(query_type)
            , context_(context)
        {}

        /// A dot::iterator_wrapper<dot::Object> that points to the beginning of any available
        /// results. If begin() is called more than once, the dot::iterator_wrapper
        /// returned points to the next remaining result, not the result of
        /// the original call to begin().
        ///
        /// For a tailable cursor, when cursor.begin() == cursor.end(), no
        /// documents are available.  Each call to cursor.begin() checks again
        /// for newly-available documents.
        dot::IteratorWrappper<dot::Object> begin()
        {
            return dot::IteratorWrappper<dot::Object>(new MongoQueryIteratorImpl(cursor_->begin().iterator_, cursor_, query_type_, context_));
        }

        /// A dot::iterator_wrapper<dot::Object> indicating cursor exhaustion, meaning that
        /// no documents are available from the cursor.
        dot::IteratorWrappper<dot::Object> end()
        {
            return dot::IteratorWrappper<dot::Object>(new MongoQueryIteratorImpl(cursor_->end().iterator_, cursor_, query_type_, context_));
        }

    private: // FIELDS

        dot::ObjectCursorWrapperBase cursor_;
        dot::Type query_type_;
        ContextBase context_;
    };
}
