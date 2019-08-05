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
#include <dot/system/array.hpp>
#include <dc/platform/data_source/InstanceType.hpp>
#include <dc/platform/data_source/DataSourceData.hpp>
#include <dc/platform/data_source/mongo/ObjectId.hpp>
#include <mongocxx/database.hpp>
#include <mongocxx/client.hpp>

namespace dc
{
    class MongoDataSourceBaseDataImpl; using MongoDataSourceData = dot::ptr<MongoDataSourceBaseDataImpl>;
    class IContextImpl; using IContext = dot::ptr<IContextImpl>;
    class KeyTypeImpl; using KeyType = dot::ptr<KeyTypeImpl>;
    class DataImpl; using Data = dot::ptr<DataImpl>;

    /// <summary>
    /// Abstract base class for data source implementations based on MongoDB.
    ///
    /// This class provides functionality shared by all MongoDB data source types.
    /// </summary>
    class DC_CLASS MongoDataSourceBaseDataImpl : public data_source_data_impl
    {
        typedef MongoDataSourceBaseDataImpl self;

    protected: // FIELDS

        /// <summary>Prohibited characters in database name.</summary>
        static dot::array<char> prohibitedDbNameSymbols_;

        /// <summary>Maximum length of the database on Mongo server including delimiters.</summary>
        static int maxDbNameLength_;

    private: // FIELDS

        /// <summary>
        /// Type of instance controls the ability to do certain
        /// actions such as deleting (dropping) the database.
        /// </summary>
        InstanceType instanceType_;

        /// <summary>Full name of the database on Mongo server including delimiters.</summary>
        dot::string dbName_;

        /// <summary>Interface to Mongo client in MongoDB C++ driver.</summary>
        mongocxx::client client_;

        /// <summary>Interface to Mongo database in MongoDB C++ driver.</summary>
        mongocxx::database db_;

        /// <summary>Previous ObjectId returned by CreateOrderedObjectId() method.</summary>
        ObjectId prevObjectId_ = ObjectId::Empty;

    public: // METHODS

        /// <summary>
        /// Set context and perform initialization or validation of object data.
        ///
        /// All derived classes overriding this method must call base.Init(context)
        /// before executing the the rest of the code in the method override.
        /// </summary>
        void Init(IContext context) override;

        /// <summary>
        /// The returned ObjectIds have the following order guarantees:
        ///
        /// * For this data source instance, to arbitrary resolution; and
        /// * Across all processes and machines, to one second resolution
        ///
        /// One second resolution means that two ObjectIds created within
        /// the same second by different instances of the data source
        /// class may not be ordered chronologically unless they are at
        /// least one second apart.
        /// </summary>
        virtual ObjectId CreateOrderedObjectId() override;

        /// <summary>
        /// Permanently deletes (drops) the database with all records
        /// in it without the possibility to recover them later.
        ///
        /// This method should only be used to free storage. For
        /// all other purposes, methods that preserve history should
        /// be used.
        ///
        /// ATTENTION - THIS METHOD WILL DELETE ALL DATA WITHOUT
        /// THE POSSIBILITY OF RECOVERY. USE WITH CAUTION.
        /// </summary>
        virtual void DeleteDb() override;

    protected: // PROTECTED

        /// <summary>
        /// Get collection with name based on the type.
        /// </summary>
        mongocxx::collection GetCollection(dot::type_t dataType);

        /// <summary>
        /// Get collection with name based on the type.
        /// </summary>
        mongocxx::collection GetCollection(dot::string typeName);
    };
}
