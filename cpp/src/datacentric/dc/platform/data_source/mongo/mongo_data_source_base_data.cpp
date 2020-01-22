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

namespace dc
{
    dot::List<char> MongoDataSourceBaseImpl::prohibited_db_name_symbols_ = dot::make_list<char>({ '/', '\\', '.', ' ', '"', '$', '*', '<', '>', ':', '|', '?' });

    int MongoDataSourceBaseImpl::max_db_name_length_ = 64;

    void MongoDataSourceBaseImpl::init(ContextBase context)
    {
        // Initialize the base class
        DataSourceImpl::init(context);

        // Perform validation
        if (env_type == EnvType::empty) throw dot::Exception("DB environment type is not specified.");
        if (dot::String::is_null_or_empty(env_group)) throw dot::Exception("DB environment group is not specified.");
        if (dot::String::is_null_or_empty(env_name)) throw dot::Exception("DB environment name is not specified.");

        // Database name is a semicolon delimited format.
        env_type_ = env_type;
        db_name_ = dot::String();
        switch (env_type_)
        {
            case EnvType::prod:
            case EnvType::uat:
            case EnvType::dev:
            case EnvType::test:
                db_name_ = env_type_.to_string()->to_upper() + ";" + env_group + ";" + env_name;
                break;
            case EnvType::custom:
                if (!dot::String::is_null_or_empty(env_group))
                    throw dot::Exception(dot::String::format(
                        "EnvGroup={0} is specified, but "
                        "should be empty for Custom environment type.",
                        env_group));

                db_name_ = env_name;
                break;
            case EnvType::empty:
                throw dot::Exception(dot::String::format("env_type is empty for data_source_name={0}.", data_source_name));
            default:
                throw dot::Exception(dot::String::format("Unknown env_type={0}.", env_type));
        }

        // Perform additional validation for restricted characters and database name length.
        if (db_name_->index_of_any(prohibited_db_name_symbols_) != -1)
            throw dot::Exception(
                dot::String::format("MongoDB database name {0} contains a space or another ", db_name_) +
                "prohibited character from the following list: /\\.\"$*<>:|?");
        if (db_name_->length() > max_db_name_length_)
            throw dot::Exception(
                dot::String::format("MongoDB database name {0} exceeds the maximum length of 64 characters.", db_name_));

        // Get client interface using the server
        if (mongo_server != nullptr)
        {
            // Create with the specified server URI
            client_ = dot::make_client(mongo_server->mongo_server_uri);
        }
        else
        {
            throw dot::Exception("mongo_server is not specified for MongoDataSourceBase.");
        }

        // Get database interface using the client and database name
        db_ = client_->get_database(db_name_);
    }

    TemporalId MongoDataSourceBaseImpl::create_ordered_object_id()
    {
        // Generate TemporalId and check that it is later
        // than the previous generated TemporalId
        TemporalId result = TemporalId::generate_new_id();
        int retry_counter = 0;
        while (result <= prev_object_id_)
        {
            // Getting inside the while loop will be very rare as this would
            // require the increment to roll from max int to min int within
            // the same second, therefore it is a good idea to log the event
            if (retry_counter++ == 0) std::cerr << "MongoDB generated TemporalId not in increasing order, retrying." << std::endl;

            // If new TemporalId is not strictly greater than the previous one,
            // keep generating new temporal_ids until it changes
            result = TemporalId::generate_new_id();
        }

        // Report the number of retries
        if (retry_counter != 0)
        {
            std::cerr << *dot::String::format("Generated TemporalId in increasing order after {0} retries.", retry_counter);
        }

        // Update previous TemporalId and return
        prev_object_id_ = result;
        return result;
    }

    void MongoDataSourceBaseImpl::delete_db()
    {
        if (read_only)
        {
            throw dot::Exception(dot::String::format("Attempting to drop (delete) database for the data source {0} where ReadOnly flag is set.", data_source_name));
        }

        // Do not delete (drop) the database this class did not create
        if (!client_.is_empty() && !db_.is_empty())
        {
            // As an extra safety measure, this method will delete
            // the database only if the first token of its name is
            // TEST or DEV.
            //
            // Use other tokens such as UAT or PROD to protect the
            // database from accidental deletion
            if (env_type_ == EnvType::dev
                || env_type_ == EnvType::user
                || env_type_ == EnvType::test)
            {
                // The name is the database key in the standard
                // semicolon delimited format. However this method
                // performs additional validation for restricted
                // characters and database name length.
                client_->drop_database(db_name_);
            }
            else
            {
                throw dot::Exception(
                    dot::String::format("As an extra safety measure, database {0} cannot be ", db_name_) +
                    "dropped because this operation is not permitted for database " +
                    dot::String::format("instance type {0}.", env_type_.to_string()));
            }
        }
    }
}
