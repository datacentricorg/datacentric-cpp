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

#include <dc/precompiled.hpp>
#include <dc/implement.hpp>
#include <dc/platform/data_source/mongo/mongo_data_source_base_data.hpp>
#include <dc/platform/data_source/mongo/mongo_server_data.hpp>
#include <dc/platform/context/context_base.hpp>

#include <mongocxx/instance.hpp>

namespace dc
{
    dot::list<char> mongo_data_source_base_data_impl::prohibitedDbNameSymbols_ = dot::make_list<char>({ '/', '\\', '.', ' ', '"', '$', '*', '<', '>', ':', '|', '?' });

    int mongo_data_source_base_data_impl::maxDbNameLength_ = 64;

    void mongo_data_source_base_data_impl::init(context_base context)
    {
        static mongocxx::instance instance{};

        // Initialize the base class
        data_source_data_impl::init(context);

        // Configures serialization conventions for standard types
        if (db_name == nullptr) throw dot::exception("DB key is null or empty.");
        if (db_name->db_instance_type == instance_type::empty) throw dot::exception("DB instance type is not specified.");
        if (dot::string::is_null_or_empty(db_name->instance_name)) throw dot::exception("DB instance name is not specified.");
        if (dot::string::is_null_or_empty(db_name->env_name)) throw dot::exception("DB environment name is not specified.");

        // The name is the database key in the standard semicolon delimited format.
        dbName_ = db_name->to_string();
        instance_type_ = db_name->db_instance_type;

        // Perform additional validation for restricted characters and database name length.
        if (dbName_->index_of_any(prohibitedDbNameSymbols_) != -1)
            throw dot::exception(
                dot::string::format("MongoDB database name {0} contains a space or another ", dbName_) +
                "prohibited character from the following list: /\\.\"$*<>:|?");
        if (dbName_->length() > maxDbNameLength_)
            throw dot::exception(
                dot::string::format("MongoDB database name {0} exceeds the maximum length of 64 characters.", dbName_));

        // Get client interface using the server
        dot::string dbUri = db_server->load(context).as<mongo_server_data>()->get_mongo_server_uri();
        client_ = dot::make_client(dbUri);

        // Get database interface using the client and database name
        db_ = client_->get_database(dbName_);
    }

    dot::object_id mongo_data_source_base_data_impl::create_ordered_object_id()
    {
        check_not_read_only();

        // Generate dot::object_id and check that it is later
        // than the previous generated dot::object_id
        dot::object_id result = dot::object_id::generate_new_id();
        int retryCounter = 0;
        while (result.oid() <= prev_object_id_.oid())
        {
            // Getting inside the while loop will be very rare as this would
            // require the increment to roll from max int to min int within
            // the same second, therefore it is a good idea to log the event
            if (retryCounter++ == 0) std::cerr << "MongoDB generated dot::object_id not in increasing order, retrying." << std::endl;

            // If new dot::object_id is not strictly greater than the previous one,
            // keep generating new dot::object_ids until it changes
            result = dot::object_id::generate_new_id();
        }

        // Report the number of retries
        if (retryCounter != 0)
        {
            std::cerr << *dot::string::format("Generated dot::object_id in increasing order after {0} retries.", retryCounter);
        }

        // Update previous dot::object_id and return
        prev_object_id_ = result;
        return result;
    }

    void mongo_data_source_base_data_impl::delete_db()
    {
        check_not_read_only();

        // Do not delete (drop) the database this class did not create
        if (!client_.is_empty() && !db_.is_empty())
        {
            // As an extra safety measure, this method will delete
            // the database only if the first token of its name is
            // TEST or DEV.
            //
            // Use other tokens such as UAT or PROD to protect the
            // database from accidental deletion
            if (instance_type_ == instance_type::dev
                || instance_type_ == instance_type::user
                || instance_type_ == instance_type::test)
            {
                // The name is the database key in the standard
                // semicolon delimited format. However this method
                // performs additional validation for restricted
                // characters and database name length.
                client_->drop_database(dbName_);
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

    dot::collection mongo_data_source_base_data_impl::get_collection(dot::type dataType)
    {
        dot::type curr = dataType;
        while (curr->name != "record" && curr->name != "key")
        {
            curr = curr->get_base_type();
            if (curr.is_empty())
                throw dot::exception(dot::string::format("Couldn't detect collection name for type {0}", dataType->name));
        }
        dot::string typeName = curr->get_generic_arguments()[0]->name;
        return get_collection(typeName);
    }

    dot::collection mongo_data_source_base_data_impl::get_collection(dot::string typeName)
    {
        int prefixSize = 0; //! TODO change to ClassInfo.MappedName
        dot::string collectionName;
        if (typeName->ends_with("Data") || typeName->ends_with("data"))
            collectionName = typeName->substring(prefixSize, typeName->length() - std::string("Data").length() - prefixSize);
        else if (typeName->ends_with("Key") || typeName->ends_with("key"))
            collectionName = typeName->substring(prefixSize, typeName->length() - std::string("Key").length() - prefixSize);
        else throw dot::exception("Unknown type");

        return db_->get_collection(collectionName);
    }
}
