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
#include <dc/test/platform/context/context.hpp>

#include <dc/platform/data_source/mongo/mongo_data_source_data.hpp>
#include <dc/platform/data_source/mongo/mongo_default_server_data.hpp>

namespace dc
{
    unit_test_context_impl::unit_test_context_impl()
    {}

    unit_test_context_impl::~unit_test_context_impl()
    {
        if (!keep_db)
        {
            // Permanently delete the unit test database
            // unless keep_db is true
            data_source->delete_db();
        }
    }

    unit_test_context make_unit_test_context(
        dot::Object class_instance,
        dot::String method_name,
        dot::String source_file_path)
    {
        unit_test_context obj = new unit_test_context_impl;

        //if (method_name == nullptr) throw dot::Exception("Method name passed to unit_test_context is null.");
        //if (source_file_path == nullptr) throw dot::Exception("Source file path passed to unit_test_context is null.");

        // Test class path is the path to source file followed by
        // subfolder whose name is source file name without extension
        //if (!source_file_path->ends_with(".cs")) throw dot::Exception(dot::String::format("Source file path '{0}' does not end with '.cs'", source_file_path));
        //dot::String test_class_path = source_file_path->substring(0, source_file_path->size() - 3);

        // Create and initialize data source with test instance Type.
        //
        // This does not create the database until the data source
        // is actually used to access data.
        dot::String mapped_class_name = class_instance->get_type()->name();

        data_source_data data_source = new mongo_data_source_data_impl();
        obj->data_source = data_source;

        data_source->db_server = (make_mongo_default_server_data())->to_key();
        data_source->db_name = new db_name_key_impl();

        data_source->db_name->db_instance_type = instance_type::test;
        data_source->db_name->instance_name = mapped_class_name;
        data_source->db_name->env_name = method_name;

        data_source->init(obj);

        // Delete (drop) the database to clear the existing data
        data_source->delete_db();

        // Create common dataset and assign it to data_set property of this context
        obj->data_set = data_source->create_common();
        return obj;
    }
}
