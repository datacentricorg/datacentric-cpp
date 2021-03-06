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
    const String db_url = "mongodb://localhost/";

    class TestClassImpl; using TestClass = Ptr<TestClassImpl>;
    inline TestClass make_test_class();

    /// Test class for saving to db.
    class TestClassImpl : public ObjectImpl
    {
        typedef TestClassImpl self;

    public:

        int int_value;
        Nullable<int> nullable_int_value;
        String string_value;
        List<int64_t> list_value;
        TestClass object_value;

    public: // REFLECTION

        Type get_type() override { return typeof(); }

        static Type typeof()
        {
            static Type result = []()->Type
            {
                return make_type_builder<TestClassImpl>("dot", "TestClass")
                    ->with_field("int_value", &TestClassImpl::int_value)
                    ->with_field("nullable_int_value", &TestClassImpl::nullable_int_value)
                    ->with_field("string_value", &TestClassImpl::string_value)
                    ->with_field("list_value", &TestClassImpl::list_value)
                    ->with_field("object_value", &TestClassImpl::object_value)
                    ->with_constructor(&make_test_class, {})
                    ->build();
            }();

            return result;
        }
    };

    inline TestClass make_test_class() { return new TestClassImpl; }

    TEST_CASE("create_collection")
    {
        const String db_name = "test;mongo_test;create_collection";
        const String db_collection_name = "db_collection";

        // Connect to mongo and drop old db.
        Client db_client = make_client(db_url);
        db_client->drop_database(db_name);

        // Create db and collection.
        Database db = db_client->get_database(db_name);
        Collection db_collection = db->get_collection(db_collection_name);

        // Create document and write to db
        TestClass obj = make_test_class();
        obj->int_value = 66;
        obj->nullable_int_value = 77;
        obj->string_value = "str66";
        obj->list_value = make_list<int64_t>({ 1, 2, 3 });
        obj->object_value = make_test_class();
        obj->object_value->int_value = 88;
        db_collection->insert_one(obj);

        // Create query and load document
        int doc_count = 0;
        Query db_query = make_query(db_collection, typeof<TestClass>());
        for (TestClass loaded_obj : db_query->get_cursor<TestClass>())
        {
            // Check loaded class values
            REQUIRE(loaded_obj->int_value == obj->int_value);
            REQUIRE(loaded_obj->nullable_int_value.value() == obj->nullable_int_value.value());
            REQUIRE(loaded_obj->string_value == obj->string_value);
            REQUIRE(loaded_obj->list_value != nullptr);
            REQUIRE(loaded_obj->list_value[0] == obj->list_value[0]);
            REQUIRE(loaded_obj->list_value[1] == obj->list_value[1]);
            REQUIRE(loaded_obj->list_value[2] == obj->list_value[2]);
            REQUIRE(loaded_obj->object_value != nullptr);
            REQUIRE(loaded_obj->object_value->int_value == obj->object_value->int_value);
            ++doc_count;
        }

        REQUIRE(doc_count == 1);
    }

    TEST_CASE("delete_item")
    {
        const String db_name = "test;mongo_test;delete_item";
        const String db_collection_name = "db_collection";

        // Connect to mongo and drop old db.
        Client db_client = make_client(db_url);
        db_client->drop_database(db_name);

        // Create db and collection.
        Database db = db_client->get_database(db_name);
        Collection db_collection = db->get_collection(db_collection_name);

        // Create document and write to db
        TestClass obj = make_test_class();
        obj->int_value = 66;
        obj->string_value = "str66";
        db_collection->insert_one(obj);
        db_collection->insert_one(obj);
        db_collection->insert_one(obj);
        obj->int_value = 77;
        db_collection->insert_one(obj);
        db_collection->insert_one(obj);

        // Delete document
        db_collection->delete_one(make_prop(&TestClassImpl::int_value) == 66);

        int doc_count;
        Query db_query;

        // Create query and count documents
        doc_count = 0;
        db_query = make_query(db_collection, typeof<TestClass>());
        for (TestClass loaded_obj : db_query->get_cursor<TestClass>())
        {
            ++doc_count;
        }

        REQUIRE(doc_count == 4);

        // Delete documents
        db_collection->delete_many(make_prop(&TestClassImpl::int_value) == 77);

        // Create query and count documents
        doc_count = 0;
        db_query = make_query(db_collection, typeof<TestClass>());
        for (TestClass loaded_obj : db_query->get_cursor<TestClass>())
        {
            ++doc_count;
        }

        REQUIRE(doc_count == 2);
    }

    TEST_CASE("query")
    {
        const String db_name = "test;mongo_test;query";
        const String db_collection_name = "db_collection";

        // Connect to mongo and drop old db.
        Client db_client = make_client(db_url);
        db_client->drop_database(db_name);

        // Create db and collection.
        Database db = db_client->get_database(db_name);
        Collection db_collection = db->get_collection(db_collection_name);

        // Create index
        List<std::tuple<String, int>> indexes = make_list<std::tuple<String, int>>();
        indexes->add({ "int_value", 1 });
        indexes->add({ "string_value", -1 });
        IndexOptions idx_options = make_index_options();
        idx_options->name = "test_index";
        db_collection->create_index(indexes, idx_options);

        // Create documents and write to db
        TestClass obj = make_test_class();
        obj->int_value = 66;
        obj->string_value = "str66";
        obj->list_value = make_list<int64_t>({ 1, 2, 3 });
        obj->object_value = make_test_class();
        obj->object_value->int_value = 11;
        db_collection->insert_one(obj);
        db_collection->insert_one(obj);

        obj = make_test_class();
        obj->int_value = 88;
        obj->string_value = "str88";
        obj->list_value = make_list<int64_t>({ 2, 3, 4 });
        obj->object_value = make_test_class();
        obj->object_value->int_value = 11;
        db_collection->insert_one(obj);
        db_collection->insert_one(obj);
        db_collection->insert_one(obj);

        obj = make_test_class();
        obj->int_value = 77;
        obj->string_value = "str77";
        obj->list_value = make_list<int64_t>({ 1, 2, 3 });
        obj->object_value = make_test_class();
        obj->object_value->int_value = 22;
        db_collection->insert_one(obj);

        int doc_count;
        Query db_query;

        // Create query and count documents where int_value >= 77
        doc_count = 0;
        db_query = make_query(db_collection, typeof<TestClass>())
            ->where(make_prop(&TestClassImpl::int_value) >= 77);
        for (TestClass loaded_obj : db_query->get_cursor<TestClass>())
        {
            ++doc_count;
        }

        REQUIRE(doc_count == 4);

        // Create query and count documents where object_value->int_value == 11
        doc_count = 0;
        db_query = make_query(db_collection, typeof<TestClass>())
            ->where(make_prop(&TestClassImpl::object_value)->*make_prop(&TestClassImpl::int_value) == 11);
        for (TestClass loaded_obj : db_query->get_cursor<TestClass>())
        {
            ++doc_count;
        }

        REQUIRE(doc_count == 5);

        // Create query and count documents where list_value[1] == 2
        doc_count = 0;
        db_query = make_query(db_collection, typeof<TestClass>())
            ->where(make_prop(&TestClassImpl::list_value)[1] == 2);
        for (TestClass loaded_obj : db_query->get_cursor<TestClass>())
        {
            ++doc_count;
        }

        REQUIRE(doc_count == 3);

        // Create query and count documents where int_value == 66 && string_value == "str66"
        doc_count = 0;
        db_query = make_query(db_collection, typeof<TestClass>())
            ->where(make_prop(&TestClassImpl::int_value) == 66 && make_prop(&TestClassImpl::string_value) == "str66");
        for (TestClass loaded_obj : db_query->get_cursor<TestClass>())
        {
            ++doc_count;
        }

        REQUIRE(doc_count == 2);
    }
}
