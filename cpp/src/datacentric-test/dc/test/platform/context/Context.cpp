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


#include <dc/test/implement.hpp>
#include <dc/test/platform/context/Context.hpp>

#include <dc/platform/data_source/mongo/mongo_data_source_data.hpp>
#include <dc/platform/data_source/mongo/MongoDefaultServerData.hpp>
//#include <dc/platform/data_source/mongo/Mongo.hpp>

namespace dc
{
    unit_test_context_impl::unit_test_context_impl(dot::object classInstance,
        dot::string methodName,
        dot::string sourceFilePath)
    {
        //if (methodName == nullptr) throw dot::exception("Method name passed to unit_test_context is null.");
        //if (sourceFilePath == nullptr) throw dot::exception("Source file path passed to unit_test_context is null.");

        // Test class path is the path to source file followed by
        // subfolder whose name is source file name without extension
        //if (!sourceFilePath->ends_with(".cs")) throw dot::exception(dot::string::format("Source file path '{0}' does not end with '.cs'", sourceFilePath));
        dot::string testClassPath = sourceFilePath->substring(0, sourceFilePath->size() - 3);

        // Create and initialize data source with TEST instance type.
        //
        // This does not create the database until the data source
        // is actually used to access data.
        dot::string mappedClassName = classInstance->get_type()->name;

        data_source = new mongo_data_source_data_impl();
        //data_source->DbServer = new DbServerKeyImpl;
        //data_source->DbServer->DbServerID = (new MongoDefaultServerDataImpl())->DbServerID;
        data_source->db_server = (make_MongoDefaultServerData())->to_key();
        data_source->db_name = new db_name_key_impl();

        data_source->db_name->instance_type = instance_type::TEST;
        data_source->db_name->instance_name = mappedClassName;
        data_source->db_name->env_name = methodName;

        data_source->init(this);

        // Delete (drop) the database to clear the existing data
        data_source->delete_db();

        // Create common dataset and assign it to data_set property of this context
        data_set = data_source->create_common();
    }

    unit_test_context_impl::~unit_test_context_impl()
    {
        if (!KeepDb)
        {
            // Permanently delete the unit test database
            // unless KeepDb is true
            data_source->delete_db();
        }
    }
}