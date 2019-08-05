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

#include <dc/platform/data_source/mongo/MongoDataSourceData.hpp>
#include <dc/platform/data_source/mongo/MongoDefaultServerData.hpp>
//#include <dc/platform/data_source/mongo/Mongo.hpp>

namespace dc
{
    UnitTestContextImpl::UnitTestContextImpl(dot::object classInstance,
        dot::string methodName,
        dot::string sourceFilePath)
    {
        //if (methodName == nullptr) throw dot::exception("Method name passed to UnitTestContext is null.");
        //if (sourceFilePath == nullptr) throw dot::exception("Source file path passed to UnitTestContext is null.");

        // Test class path is the path to source file followed by
        // subfolder whose name is source file name without extension
        //if (!sourceFilePath->EndsWith(".cs")) throw dot::exception(dot::string::format("Source file path '{0}' does not end with '.cs'", sourceFilePath));
        dot::string testClassPath = sourceFilePath->SubString(0, sourceFilePath->size() - 3);

        // Create and initialize data source with TEST instance type.
        //
        // This does not create the database until the data source
        // is actually used to access data.
        dot::string mappedClassName = classInstance->type()->name;

        DataSource = new MongoDataSourceDataImpl();
        //DataSource->DbServer = new DbServerKeyImpl;
        //DataSource->DbServer->DbServerID = (new MongoDefaultServerDataImpl())->DbServerID;
        DataSource->DbServer = (new_MongoDefaultServerData())->ToKey();
        DataSource->DbName = new DbNameKeyImpl();

        DataSource->DbName->InstanceType = InstanceType::TEST;
        DataSource->DbName->InstanceName = mappedClassName;
        DataSource->DbName->EnvName = methodName;

        DataSource->Init(this);

        // Delete (drop) the database to clear the existing data
        DataSource->DeleteDb();

        // Create common dataset and assign it to DataSet property of this context
        DataSet = DataSource->CreateCommon();
    }

    UnitTestContextImpl::~UnitTestContextImpl()
    {
        if (!KeepDb)
        {
            // Permanently delete the unit test database
            // unless KeepDb is true
            DataSource->DeleteDb();
        }
    }
}