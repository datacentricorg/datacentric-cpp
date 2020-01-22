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
#include <dc/platform/data_source/mongo/temporal_mongo_query.hpp>
#include <dc/types/record/record.hpp>
#include <dc/types/record/deleted_record.hpp>
#include <dot/mongo/mongo_db/cursor/cursor_wrapper.hpp>
#include <dot/system/collections/generic/hash_set.hpp>

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
            current_record_ = get_next_record();
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
            , begin_(!end)
            , end_(end)
        {
            // Return if it is end iterator.
            if (end_) return;

            dot::Type record_type = dot::typeof<Record>();

            // Apply final constraints to query.
            dot::Query query = dot::make_query(temporal_query_->collection_, temporal_query_->type_);
            query = temporal_query_->data_source_->apply_final_constraints(query, temporal_query_->load_from_);

            // Apply custom filters to query.
            for (dot::FilterTokenBase token : temporal_query_->where_)
            {
                query->where(token);
            }

            // Apply custom sort.
            for (std::pair<dot::FieldInfo, int> sort_token : temporal_query_->sort_)
            {
                if (sort_token.second == 1)
                    query->then_by(sort_token.first);
                if (sort_token.second == -1)
                    query->then_by_descending(sort_token.first);
            }

            // Temporal query consists of three parts.
            //
            // * First, a typed query with filter and sort options specified by the caller
            //   is executed in batches, and its result is projected to a list of keys.
            // * Second, untyped query for all keys retrieved during the batch is executed
            //   and projected to (Id, DataSet, Key) elements only. Using Imports lookup
            //   sequence and FreezeImports flag, a list of TemporalIds for the latest
            //   record in the latest dataset is obtained. Records for which type does
            //   not match are skipped.
            // * Finally, typed query is executed for each of these Ids and the results
            //   are yield returned to the caller.
            //
            // Project to key instead of returning the entire record
            projected_batch_queryable_ = query
                ->select<std::tuple<TemporalId, dot::String>>(dot::make_list<dot::FieldInfo>({ record_type->get_field("_id"), record_type->get_field("_key") }));
            step_one_enumerator_ = projected_batch_queryable_->begin();

            current_record_ = get_next_record();
        }

    private:

        // Returns next relevant record.
        Record get_next_record()
        {
            while (!end_)
            {
                // Firstly, load batch of records
                if (batch_ids_list_item_ < 0)
                {
                    end_ = !batch_load();
                    if (end_) break;
                }

                // Then iterate over batch and return records
                if (++batch_ids_list_item_ >= batch_ids_list_->count())
                {
                    batch_ids_list_item_ = -1;
                    continue;
                }

                // Using the list ensures that records are returned
                // in the same order as the original query
                TemporalId batch_id = batch_ids_list_[batch_ids_list_item_];

                // If a record TemporalId is present in batchIds but not
                // in recordDict, this indicates that the record found
                // by the query is not the latest and it should be skipped
                Record result;
                if (record_dict_->try_get_value(batch_id, result))
                {
                    // Skip if the result is a deleted record
                    if (result.is<DeletedRecord>()) continue;

                    // Check if Object could cast to query_type_.
                    // Skip, do not throw, if the cast fails.
                    //
                    // This behavior is different from loading by TemporalId or String key
                    // using load_or_null method. In case of load_or_null, the API requires
                    // an error when wrong type is requested. Here, we want to proceed
                    // as though the record does not exist because the query is expected
                    // to skip over records of type not derived from query_type_.
                    dot::Type obj_type = result->get_type();
                    if (!obj_type->equals(temporal_query_->type_) && !obj_type->is_subclass_of(temporal_query_->type_)) continue;

                    // Yield return the result
                    return init_record(result);
                }
            }

            return nullptr;
        }

        // Loads batch of records.
        bool batch_load()
        {
            const int batch_size = 1000;
            dot::Type record_type = dot::typeof<Record>();

            while (continue_query_)
            {
                // First step is to get all keys in this batch returned
                // by the user specified query and sort order
                int batch_index = 0;
                dot::HashSet<dot::String> batch_keys_hash_set = dot::make_hash_set<dot::String>();
                dot::HashSet<TemporalId> batch_ids_hash_set = dot::make_hash_set<TemporalId>();
                batch_ids_list_ = dot::make_list<TemporalId>();
                while (true)
                {
                    // Advance cursor and check if there are more results left in the query
                    if (begin_) begin_ = false;
                    else ++step_one_enumerator_;
                    continue_query_ = step_one_enumerator_ != projected_batch_queryable_->end();

                    if (continue_query_)
                    {
                        // If yes, get key from the enumerator
                        std::tuple<TemporalId, dot::String> record_info = *step_one_enumerator_;
                        TemporalId batch_id = std::get<0>(record_info);
                        dot::String batch_key = std::get<1>(record_info);

                        // Add Key and Id to hashsets, increment
                        // batch index only if this is a new key
                        if (batch_keys_hash_set->add(batch_key)) batch_index++;
                        batch_ids_hash_set->add(batch_id);
                        batch_ids_list_->add(batch_id);

                        // Break if batch size has been reached
                        if (batch_index == batch_size) break;
                    }
                    else
                    {
                        // Otherwise exit from the while loop
                        break;
                    }
                }

                // We reach this point in the code if batch size is reached,
                // or there are no more records in the query. Break from while
                // loop if query is complete but there is nothing in the batch
                if (!continue_query_ && batch_index == 0) break;

                // The second step is to get (Id,DataSet,Key) for records with
                // the specified keys using a query without type restriction.
                //
                // First, query base collection for records with keys in the hashset
                dot::List<dot::String> batch_keys_list = dot::make_list<dot::String>(std::vector<dot::String>(batch_keys_hash_set->begin(), batch_keys_hash_set->end()));
                dot::Query id_queryable = dot::make_query(temporal_query_->collection_, temporal_query_->type_);
                id_queryable->where(new dot::OperatorWrapperImpl("_key", "$in", batch_keys_list));

                // Apply the same final constraints (list of datasets, savedBy, etc.)
                id_queryable = temporal_query_->data_source_->apply_final_constraints(id_queryable, temporal_query_->load_from_);

                // Apply ordering to get last object in last dataset for the keys
                id_queryable = id_queryable
                    ->sort_by(record_type->get_field("_key")) // _key
                    ->then_by_descending(record_type->get_field("_dataset")) // _dataset
                    ->then_by_descending(record_type->get_field("_id")); // _id

                // Finally, project to (Id,DataSet,Key)
                dot::CursorWrapper<std::tuple<TemporalId, TemporalId, dot::String>> projected_id_queryable = id_queryable
                    ->select<std::tuple<TemporalId, TemporalId, dot::String>>(dot::make_list<dot::FieldInfo>({ record_type->get_field("_id"), record_type->get_field("_dataset"), record_type->get_field("_key") }));

                // Gets ImportsCutoffTime from the dataset detail record.
                // Returns null if dataset detail record is not found.
                dot::Nullable<TemporalId> imports_cutoff_time = temporal_query_->data_source_->get_imports_cutoff_time(temporal_query_->load_from_);

                // Create a list of TemporalIds for the records obtained using
                // dataset lookup rules for the keys in the batch
                dot::List<TemporalId> record_ids = dot::make_list<TemporalId>();
                dot::String current_key;
                for (auto obj : projected_id_queryable)
                {
                    dot::String obj_key = std::get<2>(obj);
                    if (!current_key.is_empty() && current_key == obj_key)
                    {
                        // The key was encountered before. Because the data is sorted by
                        // key and then by dataset and ID, this indicates that the object
                        // is not the latest and can be skipped
                        continue;
                    }
                    else
                    {
                        TemporalId record_id = std::get<0>(obj);
                        TemporalId record_data_set = std::get<1>(obj);

                        // Include the record if one of the following is true:
                        //
                        // * ImportsCutoffTime is not set
                        // * ImportsCutoffTime does not apply because the record
                        //   is in the dataset itself, not its Imports list
                        // * The record is in the list of Imports, and its TemporalId
                        //   is earlier than ImportsCutoffTime
                        if (imports_cutoff_time == nullptr
                            || record_data_set == temporal_query_->load_from_
                            || record_id < imports_cutoff_time)
                        {
                            // Iterating over the dataset lookup list in descending order,
                            // we reached dataset of the record before finding a dataset
                            // that is earlier than the record. This is the latest record
                            // in the latest dataset for this key subject to the freeze rule.

                            // Take the first object for a new key, relying on sorting
                            // by dataset and then by record's TemporalId in descending
                            // order.
                            current_key = obj_key;

                            // Add to dictionary only if found in the list of batch Ids
                            // Otherwise this is not the latest record in the latest
                            // dataset (subject to freeze rule) and it should be skipped.
                            if (batch_ids_hash_set->contains(record_id))
                            {
                                record_ids->add(record_id);
                            }
                        }
                    }
                }

                // If the list of record Ids is empty, continue
                if (record_ids->count() == 0) break;

                // Finally, retrieve the records only for the Ids in the list
                //
                // Create a typed queryable
                dot::Query record_queryable = dot::make_query(temporal_query_->collection_, temporal_query_->type_);
                record_queryable
                    ->where(new dot::OperatorWrapperImpl("_id", "$in", record_ids));

                // Populate a dictionary of records by Id
                record_dict_ = dot::make_dictionary<TemporalId, Record>();
                for (Record record : record_queryable->get_cursor<Record>())
                {
                    record_dict_->add(record->id, record);
                }

                return true;
            }

            return false;
        }

        /// Initializes record with context.
        Record init_record(Record rec) const
        {
            rec->init(temporal_query_->data_source_->context);
            return rec;
        }

    private: // FIELDS

        TemporalMongoQuery temporal_query_;
        bool begin_;
        bool end_;
        Record current_record_;

        dot::CursorWrapper<std::tuple<TemporalId, dot::String>> projected_batch_queryable_;
        dot::IteratorWrappper<std::tuple<TemporalId, dot::String>> step_one_enumerator_;

        bool continue_query_ = true;
        int batch_ids_list_item_ = -1;
        dot::List<TemporalId> batch_ids_list_;
        dot::Dictionary<TemporalId, Record> record_dict_;
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
