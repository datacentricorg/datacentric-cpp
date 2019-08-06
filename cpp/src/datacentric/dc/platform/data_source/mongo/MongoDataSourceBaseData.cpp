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

#include <dc/implement.hpp>
#include <dc/platform/data_source/mongo/MongoDataSourceBaseData.hpp>
#include <dc/platform/data_source/mongo/MongoServerData.hpp>
#include <dc/platform/context/IContext.hpp>

#include <mongocxx/instance.hpp>

namespace dc
{
    dot::list<char> MongoDataSourceBaseDataImpl::prohibitedDbNameSymbols_ = dot::make_list<char>({ '/', '\\', '.', ' ', '"', '$', '*', '<', '>', ':', '|', '?' });

    int MongoDataSourceBaseDataImpl::maxDbNameLength_ = 64;

    void MongoDataSourceBaseDataImpl::Init(IContext context)
    {
        static mongocxx::instance instance{};

        // Initialize the base class
        data_source_data_impl::Init(context);

        // Configures serialization conventions for standard types
        if (db_name == nullptr) throw dot::exception("DB key is null or empty.");
        if (db_name->instance_type == instance_type::Empty) throw dot::exception("DB instance type is not specified.");
        if (dot::string::is_null_or_empty(db_name->instance_name)) throw dot::exception("DB instance name is not specified.");
        if (dot::string::is_null_or_empty(db_name->env_name)) throw dot::exception("DB environment name is not specified.");

        // The name is the database key in the standard semicolon delimited format.
        dbName_ = db_name->to_string();
        instance_type_ = db_name->instance_type;

        // Perform additional validation for restricted characters and database name length.
        if (dbName_->index_of_any(prohibitedDbNameSymbols_) != -1)
            throw dot::exception(
                dot::string::format("MongoDB database name {0} contains a space or another ", dbName_) +
                "prohibited character from the following list: /\\.\"$*<>:|?");
        if (dbName_->length() > maxDbNameLength_)
            throw dot::exception(
                dot::string::format("MongoDB database name {0} exceeds the maximum length of 64 characters.", dbName_));

        // Get client interface using the server
        dot::string dbUri = db_server->Load(Context).as<MongoServerData>()->GetMongoServerUri();
        client_ = mongocxx::client{ mongocxx::uri(*dbUri) };

        // Get database interface using the client and database name
        db_ = client_[*dbName_];
    }

    ObjectId MongoDataSourceBaseDataImpl::create_ordered_object_id()
    {
        check_not_read_only();

        // Generate ObjectId and check that it is later
        // than the previous generated ObjectId
        ObjectId result = ObjectId::GenerateNewId();
        int retryCounter = 0;
        while (result._id <= prevObjectId_._id)
        {
            // Getting inside the while loop will be very rare as this would
            // require the increment to roll from max int to min int within
            // the same second, therefore it is a good idea to log the event
            if (retryCounter++ == 0) std::cerr << "MongoDB generated ObjectId not in increasing order, retrying." << std::endl;

            // If new ObjectId is not strictly greater than the previous one,
            // keep generating new ObjectIds until it changes
            result = ObjectId::GenerateNewId();
        }

        // Report the number of retries
        if (retryCounter != 0)
        {
            std::cerr << *dot::string::format("Generated ObjectId in increasing order after {0} retries.", retryCounter);
        }

        // Update previous ObjectId and return
        prevObjectId_ = result;
        return result;
    }

    void MongoDataSourceBaseDataImpl::delete_db()
    {
        check_not_read_only();

        // Do not delete (drop) the database this class did not create
        if (client_ && db_)
        {
            // As an extra safety measure, this method will delete
            // the database only if the first token of its name is
            // TEST or DEV.
            //
            // Use other tokens such as UAT or PROD to protect the
            // database from accidental deletion
            if (instance_type_ == instance_type::DEV
                || instance_type_ == instance_type::USER
                || instance_type_ == instance_type::TEST)
            {
                // The name is the database key in the standard
                // semicolon delimited format. However this method
                // performs additional validation for restricted
                // characters and database name length.
                client_[*dbName_].drop();
            }
            else
            {
                throw dot::exception(
                    dot::string::format("As an extra safety measure, database {0} cannot be ", dbName_) +
                    "dropped because this operation is not permitted for database " +
                    dot::string::format("instance type {0}.", instance_type_.to_string()));
            }
        }
    }

    mongocxx::collection MongoDataSourceBaseDataImpl::GetCollection(dot::type_t dataType)
    {
        dot::type_t curr = dataType;
        while (curr->name != "RecordFor" && curr->name != "KeyFor")
        {
            curr = curr->base_type();
            if (curr.is_empty())
                throw dot::exception(dot::string::format("Couldn't detect collection name for type {0}", dataType->name));
        }
        dot::string typeName = curr->get_generic_arguments()[0]->name;
        return GetCollection(typeName);
    }

    mongocxx::collection MongoDataSourceBaseDataImpl::GetCollection(dot::string typeName)
    {
        int prefixSize = 0; //! TODO change to ClassInfo.MappedName
        dot::string collectionName;
        if (typeName->ends_with("Data"))
            collectionName = typeName->substring(prefixSize, typeName->length() - std::string("Data").length() - prefixSize);
        else if (typeName->ends_with("Key"))
            collectionName = typeName->substring(prefixSize, typeName->length() - std::string("Key").length() - prefixSize);
        else throw dot::exception("Unknown type");

        return db_[*collectionName];
    }
}
