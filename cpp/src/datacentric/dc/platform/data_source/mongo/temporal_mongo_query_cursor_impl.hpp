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
#include <dc/platform/data_source/mongo/temporal_mongo_query.hpp>
#include <dc/types/record/deleted_record.hpp>

namespace dc
{
    class TemporalMongoQueryIteratorImpl; using TemporalMongoQueryIterator = dot::Ptr<TemporalMongoQueryIteratorImpl>;
    class TemporalMongoQueryCursorImpl; using TemporalMongoQueryCursor = dot::Ptr<TemporalMongoQueryCursorImpl>;

    /// Class implements dot::iterator_wrapper methods.
    /// Constructs from IteratorInnerBase to filter input records and initialize them with context.
    class DC_CLASS TemporalMongoQueryIteratorImpl : public dot::IteratorInnerBaseImpl
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
            return !((*this) == rhs);
        }

        virtual bool operator==(dot::IteratorInnerBase rhs) override
        {
            TemporalMongoQueryIterator temporal_iterator = rhs.as<TemporalMongoQueryIterator>();

            // Two not end iterators are never equal.
            // Return true if both are end.
            return end_ && temporal_iterator->end_;
        }

        /// Constructs from TemporalMongoQuery and end flag.
        TemporalMongoQueryIteratorImpl(TemporalMongoQuery temporal_query, bool end)
            : temporal_query_(temporal_query)
            , end_(end)
        {
            // Return if it is end iterator.
            if (end_) return;

            dot::Type record_type = dot::typeof<Record>();

            // Apply dataset filters to query.
            dot::Query query = dot::make_query(temporal_query_->collection_, temporal_query_->type_);
            query = temporal_query_->data_source_->apply_final_constraints(query, temporal_query_->data_set_);

            // Apply custom filters to query.
            for (dot::FilterTokenBase token : temporal_query_->where_)
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

            // Apply custom sort.
            for (std::pair<dot::FieldInfo, int> sort_token : temporal_query_->sort_)
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

            cursor_ = query->get_cursor();
            iterator_ = cursor_->begin().iterator_;

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
                    if (!obj_type->equals(temporal_query_->type_) && !obj_type->is_subclass_of(temporal_query_->type_)) continue;

                    // Otherwise return the result
                    return to_record(obj);
                }
            }

            end_ = true;
            return nullptr;
        }

        /// Initializes record with context.
        Record to_record(dot::Object obj) const
        {
            Record rec = obj.as<Record>();
            rec->init(temporal_query_->data_source_->context);
            return rec;
        }

    private: // FIELDS

        TemporalMongoQuery temporal_query_;
        bool end_;

        dot::IteratorInnerBase iterator_;
        dot::ObjectCursorWrapperBase cursor_;

        dot::String current_key_;
        Record current_record_;
    };

    /// Class implements dot::ObjectCursorWrapperBase.
    /// Constructs from other ObjectCursorWrapperBase and ContextBase
    /// to use it in iterator.
    class DC_CLASS TemporalMongoQueryCursorImpl : public dot::ObjectCursorWrapperBaseImpl
    {
    public:

        /// Constructs from ObjectCursorWrapperBase and ContextBase.
        TemporalMongoQueryCursorImpl(TemporalMongoQuery temporal_query)
            : temporal_query_(temporal_query)
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
            return dot::IteratorWrappper<dot::Object>(new TemporalMongoQueryIteratorImpl(temporal_query_, false));
        }

        /// A dot::iterator_wrapper<dot::Object> indicating cursor exhaustion, meaning that
        /// no documents are available from the cursor.
        dot::IteratorWrappper<dot::Object> end()
        {
            return dot::IteratorWrappper<dot::Object>(new TemporalMongoQueryIteratorImpl(temporal_query_, true));
        }

    private: // FIELDS

        TemporalMongoQuery temporal_query_;
    };
}
