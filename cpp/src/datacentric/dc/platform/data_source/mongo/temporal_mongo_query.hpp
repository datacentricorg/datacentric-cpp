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
#include <dc/platform/data_source/mongo/temporal_mongo_data_source.hpp>

namespace dc
{
    class TemporalMongoQueryImpl; using TemporalMongoQuery = dot::Ptr<TemporalMongoQueryImpl>;
    class TemporalMongoDataSourceImpl; using TemporalMongoDataSource = dot::Ptr<TemporalMongoDataSourceImpl>;

    /// Implements query for MongoDB.
    /// This implementation combines methods of mongo query with
    /// additional constraints and ordering to retrieve the correct version
    /// of the record across multiple datasets.
    class DC_CLASS TemporalMongoQueryImpl : public dot::ObjectImpl
    {
        typedef TemporalMongoQueryImpl self;

        friend TemporalMongoQuery make_temporal_mongo_query(dot::Collection collection,
            dot::Type type,
            DataSource data_source,
            TemporalId data_set);

    public: // METHODS

        /// Filters a sequence of values based on a predicate.
        TemporalMongoQuery where(dot::FilterTokenBase value);

        /// Sorts the elements of a sequence in ascending order according to the selected key.
        TemporalMongoQuery sort_by(dot::FieldInfo key_selector);

        /// Sorts the elements of a sequence in descending order according to the selected key.
        TemporalMongoQuery sort_by_descending(dot::FieldInfo key_selector);

        /// Converts query to cursor so iteration can be performed.
        dot::ObjectCursorWrapperBase get_cursor();

        /// Makes projection and converts query to cursor so iteration can be performed.
        dot::ObjectCursorWrapperBase select(dot::List<dot::FieldInfo> props, dot::Type element_type);

        /// Sorts the elements of a sequence in ascending order according to the selected key.
        template <class Class, class Prop>
        TemporalMongoQuery sort_by(dot::PropWrapper<Class, Prop> key_selector)
        {
            return sort_by(key_selector.prop_);
        }

        /// Sorts the elements of a sequence in descending order according to the selected key.
        template <class Class, class Prop>
        TemporalMongoQuery sort_by_descending(dot::PropWrapper<Class, Prop> key_selector)
        {
            return sort_by_descending(key_selector.prop_);
        }

        /// Converts query to typed cursor so iteration can be performed.
        template <class TRecord>
        dot::CursorWrapper<TRecord> get_cursor()
        {
            return dot::make_cursor_wrapper<TRecord>(get_cursor());
        }

        /// Makes projection and converts query to typed cursor so iteration can be performed.
        template <class element>
        dot::CursorWrapper<element> select(dot::List<dot::FieldInfo> props)
        {
            return dot::make_cursor_wrapper<element>(select(props, dot::typeof<element>()));
        }

    private:

        /// Private query constructor from collection, type, data source and dataset.
        TemporalMongoQueryImpl(dot::Collection collection,
            dot::Type type,
            DataSource data_source,
            TemporalId data_set)
            : collection_(collection)
            , type_(type)
            , data_source_(data_source)
            , data_set_(data_set)
        {
        }

        dot::Collection collection_;
        dot::Type type_;
        TemporalMongoDataSource data_source_;
        TemporalId data_set_;

        std::vector<dot::FilterTokenBase> where_;
        std::vector<std::pair<dot::FieldInfo, int>> sort_;
    };

    /// Creates query from collection, type, data source and dataset.
    inline TemporalMongoQuery make_temporal_mongo_query(dot::Collection collection,
                                        dot::Type type,
                                        DataSource data_source,
                                        TemporalId data_set)
    {
        return new TemporalMongoQueryImpl(collection, type, data_source, data_set);
    }
}
