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

        int int_value;
        nullable<int> nullable_int_value;
        string string_value;
        list<int64_t> list_value;
        test_class object_value;

    public: // REFLECTION

        type get_type() override { return typeof(); }

        static type typeof()
        {
            static type result = []()->type
            {
                return make_type_builder<test_class_impl>("dot", "test_class")
                    ->with_field("int_value", &test_class_impl::int_value)
                    ->with_field("nullable_int_value", &test_class_impl::nullable_int_value)
                    ->with_field("string_value", &test_class_impl::string_value)
                    ->with_field("list_value", &test_class_impl::list_value)
                    ->with_field("object_value", &test_class_impl::object_value)
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
        obj->nullable_int_value = 77;
        obj->string_value = "str66";
        obj->list_value = make_list<int64_t>({ 1, 2, 3 });
        obj->object_value = make_test_class();
        obj->object_value->int_value = 88;
        db_collection->insert_one(obj);

        // Create query and load document
        int doc_count = 0;
        query db_query = make_query(db_collection, typeof<test_class>());
        for (test_class loaded_obj : db_query->get_cursor<test_class>())
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
        const string db_name = "test;mongo_test;delete_item";
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
        db_collection->insert_one(obj);
        db_collection->insert_one(obj);
        obj->int_value = 77;
        db_collection->insert_one(obj);
        db_collection->insert_one(obj);

        // Delete document
        db_collection->delete_one(make_prop(&test_class_impl::int_value) == 66);

        int doc_count;
        query db_query;

        // Create query and count documents
        doc_count = 0;
        db_query = make_query(db_collection, typeof<test_class>());
        for (test_class loaded_obj : db_query->get_cursor<test_class>())
        {
            ++doc_count;
        }

        REQUIRE(doc_count == 4);

        // Delete documents
        db_collection->delete_many(make_prop(&test_class_impl::int_value) == 77);

        // Create query and count documents
        doc_count = 0;
        db_query = make_query(db_collection, typeof<test_class>());
        for (test_class loaded_obj : db_query->get_cursor<test_class>())
        {
            ++doc_count;
        }

        REQUIRE(doc_count == 2);
    }

    TEST_CASE("query")
    {
        const string db_name = "test;mongo_test;query";
        const string db_collection_name = "db_collection";

        // Connect to mongo and drop old db.
        client db_client = make_client(db_url);
        db_client->drop_database(db_name);

        // Create db and collection.
        database db = db_client->get_database(db_name);
        collection db_collection = db->get_collection(db_collection_name);

        // Create documents and write to db
        test_class obj = make_test_class();
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
        query db_query;

        // Create query and count documents where int_value >= 77
        doc_count = 0;
        db_query = make_query(db_collection, typeof<test_class>())
            ->where(make_prop(&test_class_impl::int_value) >= 77);
        for (test_class loaded_obj : db_query->get_cursor<test_class>())
        {
            ++doc_count;
        }

        REQUIRE(doc_count == 4);

        // Create query and count documents where object_value->int_value == 11
        doc_count = 0;
        db_query = make_query(db_collection, typeof<test_class>())
            ->where(make_prop(&test_class_impl::object_value)->*make_prop(&test_class_impl::int_value) == 11);
        for (test_class loaded_obj : db_query->get_cursor<test_class>())
        {
            ++doc_count;
        }

        REQUIRE(doc_count == 5);

        // Create query and count documents where list_value[1] == 2
        doc_count = 0;
        db_query = make_query(db_collection, typeof<test_class>())
            ->where(make_prop(&test_class_impl::list_value)[1] == 2);
        for (test_class loaded_obj : db_query->get_cursor<test_class>())
        {
            ++doc_count;
        }

        REQUIRE(doc_count == 3);

        // Create query and count documents where int_value == 66 && string_value == "str66"
        doc_count = 0;
        db_query = make_query(db_collection, typeof<test_class>())
            ->where(make_prop(&test_class_impl::int_value) == 66 && make_prop(&test_class_impl::string_value) == "str66");
        for (test_class loaded_obj : db_query->get_cursor<test_class>())
        {
            ++doc_count;
        }

        REQUIRE(doc_count == 2);
    }
}
