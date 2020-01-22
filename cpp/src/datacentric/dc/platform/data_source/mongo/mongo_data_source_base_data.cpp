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
    dot::list<char> mongo_data_source_base_data_impl::prohibited_db_name_symbols_ = dot::make_list<char>({ '/', '\\', '.', ' ', '"', '$', '*', '<', '>', ':', '|', '?' });

    int mongo_data_source_base_data_impl::max_db_name_length_ = 64;

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
        db_name_ = db_name->to_string();
        instance_type_ = db_name->db_instance_type;

        // Perform additional validation for restricted characters and database name length.
        if (db_name_->index_of_any(prohibited_db_name_symbols_) != -1)
            throw dot::exception(
                dot::string::format("MongoDB database name {0} contains a space or another ", db_name_) +
                "prohibited character from the following list: /\\.\"$*<>:|?");
        if (db_name_->length() > max_db_name_length_)
            throw dot::exception(
                dot::string::format("MongoDB database name {0} exceeds the maximum length of 64 characters.", db_name_));

        // Get client interface using the server
        dot::string db_uri = db_server->load(this->context).as<mongo_server_data>()->get_mongo_server_uri();
        client_ = mongocxx::client{ mongocxx::uri(*db_uri) };

        // Get database interface using the client and database name
        db_ = client_[*db_name_];
    }

    dot::object_id mongo_data_source_base_data_impl::create_ordered_object_id()
    {
        check_not_read_only();

        // Generate dot::object_id and check that it is later
        // than the previous generated dot::object_id
        dot::object_id result = dot::object_id::generate_new_id();
        int retry_counter = 0;
        while (result.oid() <= prev_object_id_.oid())
        {
            // Getting inside the while loop will be very rare as this would
            // require the increment to roll from max int to min int within
            // the same second, therefore it is a good idea to log the event
            if (retry_counter++ == 0) std::cerr << "MongoDB generated dot::object_id not in increasing order, retrying." << std::endl;

            // If new dot::object_id is not strictly greater than the previous one,
            // keep generating new dot::object_ids until it changes
            result = dot::object_id::generate_new_id();
        }

        // Report the number of retries
        if (retry_counter != 0)
        {
            std::cerr << *dot::string::format("Generated dot::object_id in increasing order after {0} retries.", retry_counter);
        }

        // Update previous dot::object_id and return
        prev_object_id_ = result;
        return result;
    }

    void mongo_data_source_base_data_impl::delete_db()
    {
        check_not_read_only();

        // Do not delete (drop) the database this class did not create
        if (client_ && db_)
        {
            // As an extra safety measure, this method will delete
            // the database only if the first token of its name is
            // test or dev.
            //
            // Use other tokens such as uat or prod to protect the
            // database from accidental deletion
            if (instance_type_ == instance_type::dev
                || instance_type_ == instance_type::user
                || instance_type_ == instance_type::test)
            {
                // The name is the database key in the standard
                // semicolon delimited format. However this method
                // performs additional validation for restricted
                // characters and database name length.
                client_[*db_name_].drop();
            }
            else
            {
                throw dot::exception(
                    dot::string::format("As an extra safety measure, database {0} cannot be ", db_name_) +
                    "dropped because this operation is not permitted for database " +
                    dot::string::format("instance type {0}.", instance_type_.to_string()));
            }
        }
    }

    mongocxx::collection mongo_data_source_base_data_impl::get_collection(dot::type data_type)
    {
        dot::type curr = data_type;
        while (curr->name != "record" && curr->name != "key")
        {
            curr = curr->get_base_type();
            if (curr.is_empty())
                throw dot::exception(dot::string::format("Couldn't detect collection name for type {0}", data_type->name));
        }
        dot::string type_name = curr->get_generic_arguments()[0]->name;
        return get_collection(type_name);
    }

    mongocxx::collection mongo_data_source_base_data_impl::get_collection(dot::string type_name)
    {
        int prefix_size = 0; //! TODO change to class_info.mapped_name
        dot::string collection_name;
        if (type_name->ends_with("data"))
            collection_name = type_name->substring(prefix_size, type_name->length() - std::string("data").length() - prefix_size);
        else if (type_name->ends_with("key"))
            collection_name = type_name->substring(prefix_size, type_name->length() - std::string("key").length() - prefix_size);
        else throw dot::exception("Unknown type");

        return db_[*collection_name];
    }
}
