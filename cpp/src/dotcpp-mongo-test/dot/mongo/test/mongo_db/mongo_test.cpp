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

#include <dot/mongo/test/implement.hpp>
#include <approvals/ApprovalTests.hpp>
#include <approvals/Catch.hpp>

#include <dot/system/object.hpp>
#include <dot/mongo/mongo_db/mongo/client.hpp>
#include <dot/mongo/mongo_db/query/query.hpp>

namespace dot
{
    const string db_url = "mongodb://localhost/";

    class test_class_impl; using test_class = ptr<test_class_impl>;
    inline test_class make_test_class();

    /// Test class for saving to db.
    class test_class_impl : public object_impl
    {
        typedef test_class_impl self;

    public:

        int int_value = 777;
        string string_value = "str_value";

    public: // REFLECTION

        type get_type() override { return typeof(); }

        static type typeof()
        {
            static type result = []()->type
            {
                return make_type_builder<test_class_impl>("dot", "test_class")
                    ->with_field("int_value", &test_class_impl::int_value)
                    ->with_field("string_value", &test_class_impl::string_value)
                    ->with_constructor(&make_test_class, {})
                    ->build();
            }();

            return result;
        }
    };

    inline test_class make_test_class() { return new test_class_impl; }

    TEST_CASE("create_collection")
    {
        const string db_name = "test;mongo_test;create_collection";
        const string db_collection_name = "db_collection";

        // Connect to mongo and drop old db.
        client db_client = make_client(db_url);
        db_client->drop_database(db_name);

        // Create db and collection.
        database db = db_client->get_database(db_name);
        collection db_collection = db->get_collection(db_collection_name);

        // Create document and write to db
        test_class obj = make_test_class();
        obj->int_value = 66;
        obj->string_value = "str66";
        db_collection->insert_one(obj);

        // Create query and load document
        query db_query = make_query(db_collection, typeof<test_class>());
        for (test_class loaded_obj : db_query->get_cursor<test_class>())
        {
            REQUIRE(loaded_obj->int_value == obj->int_value);
            REQUIRE(loaded_obj->string_value == obj->string_value);
        }
    }
}
