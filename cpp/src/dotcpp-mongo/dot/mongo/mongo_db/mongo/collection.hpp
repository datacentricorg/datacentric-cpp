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
#include <dot/system/object_impl.hpp>
#include <dot/system/ptr.hpp>
#include <dot/mongo/mongo_db/mongo/index_options.hpp>

namespace dot
{
    class CollectionImpl; using Collection = Ptr<CollectionImpl>;
    class FilterTokenBaseImpl; using FilterTokenBase = Ptr<FilterTokenBaseImpl>;

    /// Class representing server side document groupings within a MongoDB database.
    ///
    /// Collections do not require or enforce a schema and documents inside of a collection can have
    /// different fields. While not a requirement, typically documents in a collection have a similar
    /// shape or related purpose.
    class DOT_MONGO_CLASS CollectionImpl : public ObjectImpl
    {
    private:

        friend class CollectionInner;
        friend class DatabaseInner;
        friend class QueryInnerImpl;

        /// Base class for collection implementation classes.
        /// Derived collection impl class is hidden to cpp.
        class DOT_MONGO_CLASS CollectionInnerBase
        {
            friend class CollectionImpl;

        public:
            virtual ~CollectionInnerBase() = default;

        protected:

            /// Inserts a single Object into the collection. If the Object->_id is missing or empty
            /// one will be generated for it.
            virtual void insert_one(Object obj) = 0;

            /// Inserts a many objects into the collection. If the Object->_id is missing or empty
            /// one will be generated for it.
            virtual void insert_many(dot::ListBase objs) = 0;

            /// Deletes a single matching document from the collection.
            virtual void delete_one(FilterTokenBase filter) = 0;

            /// Deletes all matching documents from the collection.
            virtual void delete_many(FilterTokenBase filter) = 0;

            /// Creates an index over the collection for the provided keys with the provided options.
            virtual void create_index(List<std::tuple<String, int>> indexes, IndexOptions options) = 0;
        };

    public:

        /// Inserts a single Object into the collection. If the Object->_id is missing or empty
        /// one will be generated for it.
        void insert_one(Object obj);

        /// Inserts a many objects into the collection. If the Object->_id is missing or empty
        /// one will be generated for it.
        void insert_many(dot::ListBase objs);

        /// Deletes a single matching document from the collection.
        void delete_one(FilterTokenBase filter);

        /// Deletes all matching documents from the collection.
        void delete_many(FilterTokenBase filter);

        /// Creates an index over the collection for the provided keys with the provided options.
        void create_index(List<std::tuple<String, int>> indexes, IndexOptions options = nullptr);

    private:

        CollectionImpl(std::unique_ptr<CollectionInnerBase> && impl);

        std::unique_ptr<CollectionInnerBase> impl_;
    };
}
