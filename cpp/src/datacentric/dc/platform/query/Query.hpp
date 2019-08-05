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
#include <dc/types/record/RecordFor.hpp>
#include <dc/platform/query/IQuery.hpp>
#include <bsoncxx/builder/basic/document.hpp>

namespace dc
{
    class QueryImpl;
    using Query = dot::ptr<QueryImpl>;
    class MongoDataSourceDataImpl;

    /// <summary>
    /// Holds expressions for Where, Select, and OrderBy/ThenBy
    /// parts of a query.
    /// </summary>
    class DC_CLASS QueryImpl : public IQueryImpl
    {
        friend Query new_Query(DataSourceData dataSource, ObjectId dataSet, dot::type_t type);
        friend MongoDataSourceDataImpl;

    public:

        virtual IQuery Where(bsoncxx::document::view_or_value value);

        virtual IQuery SortBy(dot::field_info keySelector);

        virtual IQuery SortByDescending(dot::field_info keySelector);

        virtual dot::IObjectEnumerable AsEnumerable();

        virtual dot::IObjectEnumerable Select(dot::List<dot::field_info> props, dot::type_t elementType);

    private:

        DataSourceData dataSource_;
        ObjectId dataSet_;
        bsoncxx::builder::basic::document where_;
        bsoncxx::builder::basic::document sort_;
        dot::type_t type_;
        dot::type_t elementType_;
        dot::List<dot::field_info> select_;


        QueryImpl(DataSourceData dataSource, ObjectId dataSet, dot::type_t type)
            : dataSource_(dataSource)
            , dataSet_(dataSet)
            , type_(type)
            , elementType_(type)
        {
        }
    };


    inline Query new_Query(DataSourceData dataSource, ObjectId dataSet, dot::type_t type)
    {
        return new QueryImpl(dataSource, dataSet, type);
    }
}

