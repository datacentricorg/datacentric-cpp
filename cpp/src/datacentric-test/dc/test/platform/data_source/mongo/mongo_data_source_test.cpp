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
#include <approvals/ApprovalTests.hpp>
#include <approvals/Catch.hpp>

#include <dc/serialization/bson_writer.hpp>
#include <dc/serialization/bson_record_serializer.hpp>

#include <dc/platform/data_source/data_source_data.hpp>
#include <dc/platform/data_source/mongo/query_builder.hpp>
#include <dc/platform/data_source/mongo/mongo_default_server_data.hpp>

#include <dc/platform/data_set/data_set_key.hpp>
#include <dc/platform/data_set/data_set_data.hpp>

#include <dc/test/platform/context/context.hpp>

#include <dc/test/platform/data_source/mongo/mongo_test_data.hpp>
#include <bsoncxx/json.hpp>

namespace dc
{
    static std::stringstream received;

    class mongo_data_source_test_impl; using mongo_data_source_test = dot::ptr<mongo_data_source_test_impl>;

    class mongo_data_source_test_impl : public virtual dot::object_impl
    {
        typedef mongo_data_source_test_impl self;
    public:

        DOT_TYPE_BEGIN("dc", "mongo_data_source_test")
        DOT_TYPE_END()
    };

    /// Save record with minimal data for testing how the records are found.
    dot::object_id SaveMinimalRecord(unit_test_context_base context, dot::string data_set_id, dot::string record_id, int record_index, dot::nullable<int> version = dot::nullable<int>())
    {
        mongo_test_data rec = make_mongo_test_data();
        rec->record_id = record_id;
        rec->record_index = record_index;
        rec->version = version;

        dot::object_id data_set = context->get_data_set(data_set_id, context->data_set);
        context->save(rec, data_set);
        return rec->ID;
    }

    /// Save base record
    dot::object_id SaveBaseRecord(unit_test_context_base context, dot::string dataSetID, dot::string recordID, int recordIndex)
    {
        mongo_test_data rec = new mongo_test_data_impl();
        rec->record_id = recordID;
        rec->record_index = recordIndex;
        rec->double_element = 100.0;
        rec->local_date_element = dot::local_date(2003, 5, 1);
        rec->local_time_element = dot::local_time(10, 15, 30); // 10:15:30
        rec->local_minute_element = dot::local_minute(10, 15); // 10:15
        rec->local_date_time_element = dot::local_date_time(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->enum_value = mongo_test_enum::enum_value2;

        dot::object_id dataSet = context->get_data_set(dataSetID, context->get_common());
        context->save(rec, dataSet);

        mongo_test_data rec2 = context->load_or_null<mongo_test_data>(rec->ID);
        REQUIRE(rec->enum_value == rec2->enum_value);

        return rec->ID;
    }

    /// Save derived record
    dot::object_id SaveDerivedRecord(unit_test_context_base context, dot::string dataSetID, dot::string recordID, int recordIndex)
    {
        mongo_test_derived_data rec = make_mongo_test_derived_data();
        rec->record_id = recordID;
        rec->record_index = recordIndex;
        rec->double_element = 300.0;
        rec->local_date_element = dot::local_date(2003, 5, 1);
        rec->local_time_element = dot::local_time(10, 15, 30); // 10:15:30
        rec->local_minute_element = dot::local_minute(10, 15); // 10:15
        rec->local_date_time_element = dot::local_date_time(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->string_element2 = dot::string::empty; // Test how empty value is recorded
        rec->double_element2 = 200.0;

        // dot::string collections
        rec->array_of_string = dot::make_list<dot::string>({ "A", "B", "C" });
        rec->list_of_string = dot::make_list<dot::string>({ "A", "B", "C" });

        // Double collections
        rec->array_of_double = dot::make_list<double>({ 1.0, 2.0, 3.0 });
        rec->list_of_double = dot::make_list<double>({ 1.0, 2.0, 3.0 });
        rec->list_of_nullable_double = dot::make_list<dot::nullable<double>>();
        rec->list_of_nullable_double->add(10.0);
        rec->list_of_nullable_double->add(dot::nullable<double>());
        rec->list_of_nullable_double->add(30.0);

        // Data element
        rec->data_element = make_element_sample_data();
        rec->data_element->double_element3 = 1.0;
        rec->data_element->string_element3 = "AA";

        // Data element list
        rec->data_element_list = dot::make_list<element_sample_data>();
        element_sample_data elementList0 = make_element_sample_data();
        elementList0->double_element3 = 1.0;
        elementList0->string_element3 = "A0";
        rec->data_element_list->add(elementList0);
        element_sample_data elementList1 = make_element_sample_data();
        elementList1->double_element3 = 2.0;
        elementList1->string_element3 = "A1";
        rec->data_element_list->add(elementList1);

        // Key element
        rec->key_element = make_mongo_test_key();
        rec->key_element->record_id = "BB";
        rec->key_element->record_index = 2;

        // Key element list
        rec->key_element_list = dot::make_list<mongo_test_key>();
        mongo_test_key keyList0 = make_mongo_test_key();
        keyList0->record_id = "B0";
        keyList0->record_index = 3;
        rec->key_element_list->add(keyList0);
        mongo_test_key keyList1 = make_mongo_test_key();
        keyList1->record_id = "B1";
        keyList1->record_index = 4;
        rec->key_element_list->add(keyList1);

        dot::object_id dataSet = context->get_data_set(dataSetID, context->get_common());
        context->save(rec, dataSet);
        return rec->ID;
    }

    /// Save other derived record.
    dot::object_id SaveOtherDerivedRecord(unit_test_context_base context, dot::string dataSetID, dot::string recordID, int recordIndex)
    {
        mongo_test_other_derived_data rec = make_mongo_test_other_derived_data();
        rec->record_id = recordID;
        rec->record_index = recordIndex;
        rec->double_element = 300.0;
        rec->local_date_element = dot::local_date(2003, 5, 1);
        rec->local_time_element = dot::local_time(10, 15, 30); // 10:15:30
        rec->local_minute_element = dot::local_minute(10, 15); // 10:15
        rec->local_date_time_element = dot::local_date_time(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->other_string_element2 = dot::string::empty; // Test how empty value is recorded
        rec->other_double_element2 = 200.0;

        dot::object_id dataSet = context->get_data_set(dataSetID, context->data_set);
        context->save(rec, dataSet);
        return rec->ID;
    }

    /// Save record that is derived from derived.
    dot::object_id SaveDerivedFromDerivedRecord(unit_test_context_base context, string dataSetID, string recordID, int recordIndex)
    {
        mongo_test_derived_from_derived_data rec = make_mongo_test_derived_from_derived_data();
        rec->record_id = recordID;
        rec->record_index = recordIndex;
        rec->double_element = 300.0;
        rec->local_date_element = dot::local_date(2003, 5, 1);
        rec->local_time_element = dot::local_time(10, 15, 30); // 10:15:30
        rec->local_minute_element = dot::local_minute(10, 15); // 10:15
        rec->local_date_time_element = dot::local_date_time(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->other_string_element2 = dot::string::empty; // Test how empty value is recorded
        rec->other_double_element3 = 200.0;

        dot::object_id dataSet = context->get_data_set(dataSetID, context->data_set);
        context->save(rec, dataSet);
        return rec->ID;
    }

    /// Two datasets and two objects, one base and one derived.
    void SaveBasicData(unit_test_context_base context)
    {
        // Create datasets
        dot::object_id dataSetA = context->create_data_set("A", context->data_set);
        dot::list<dot::object_id> parents = dot::make_list<dot::object_id>();
        parents->add(dataSetA);

        dot::object_id dataSetB = context->create_data_set("B", parents, context->data_set);

        // Create records with minimal data
        SaveBaseRecord(context, "A", "A", 0);
        SaveDerivedRecord(context, "B", "B", 0);
    }

    /// Two datasets and eight objects, split between base and derived.
    void SaveCompleteData(unit_test_context_base context)
    {
        // Create datasets
        dot::object_id dataSetA = context->create_data_set("A", context->data_set);
        dot::object_id dataSetB = context->create_data_set("B", dot::make_list<dot::object_id>({ dataSetA }), context->data_set);
        dot::object_id dataSetC = context->create_data_set("C", dot::make_list<dot::object_id>({ dataSetA }), context->data_set);
        dot::object_id dataSetD = context->create_data_set("D", dot::make_list<dot::object_id>({ dataSetA, dataSetB, dataSetC }), context->data_set);

        // Create records with minimal data
        SaveBaseRecord(context, "A", "A", 0);
        SaveDerivedRecord(context, "B", "B", 0);
        SaveOtherDerivedRecord(context, "C", "C", 0);
        SaveDerivedFromDerivedRecord(context, "D", "D", 0);
        SaveBaseRecord(context, "A", "A", 1);
        SaveDerivedRecord(context, "B", "B", 1);
        SaveOtherDerivedRecord(context, "C", "C", 1);
        SaveDerivedFromDerivedRecord(context, "D", "D", 1);
        SaveBaseRecord(context, "A", "A", 2);
        SaveDerivedRecord(context, "B", "B", 2);
        SaveOtherDerivedRecord(context, "C", "C", 2);
        SaveDerivedFromDerivedRecord(context, "D", "D", 2);
        SaveBaseRecord(context, "A", "A", 3);
        SaveDerivedRecord(context, "B", "B", 3);
        SaveOtherDerivedRecord(context, "C", "C", 3);
        SaveDerivedFromDerivedRecord(context, "D", "D", 3);
    }

    /// Minimal data in multiple datasets with overlapping parents.
    void SaveMultidata_set_data(unit_test_context_base context)
    {
        // Create datasets
        dot::object_id dataSetA = context->create_data_set("A", context->data_set);
        dot::object_id dataSetB = context->create_data_set("B", dot::make_list<dot::object_id>({ dataSetA }), context->data_set);
        dot::object_id dataSetC = context->create_data_set("C", dot::make_list<dot::object_id>({ dataSetA }), context->data_set);
        dot::object_id dataSetD = context->create_data_set("D", dot::make_list<dot::object_id>({ dataSetA, dataSetB, dataSetC }), context->data_set);

        // Create records
        SaveMinimalRecord(context, "A", "A", 0);
        SaveMinimalRecord(context, "A", "A", 1);
        SaveMinimalRecord(context, "B", "B", 0);
        SaveMinimalRecord(context, "B", "B", 1);
        SaveMinimalRecord(context, "C", "C", 0);
        SaveMinimalRecord(context, "C", "C", 1);
        SaveMinimalRecord(context, "D", "D", 0);
        SaveMinimalRecord(context, "D", "D", 1);
    }

    /// Load the object and verify the outcome.
    template <class TKey, class TRecord>
    void VerifyLoad(unit_test_context_base context, key<TKey, TRecord> key, dot::string dataSetID)
    {
        // Get dataset and try loading the record
        dot::object_id dataSet = context->get_data_set(dataSetID, context->get_common());
        dot::ptr<TRecord> record = key->load_or_null(context, dataSet);

        if (record == nullptr)
        {
            // Not found
            received << * dot::string::format("Record {0} in dataset {1} not found.", key->to_string(), dataSetID) << std::endl;
        }
        else
        {
            // Found, also checks that the key matches
            REQUIRE(record->get_key() == key->to_string());
            received
                << *dot::string::format("Record {0} in dataset {1} found and has Type={2}.",
                    key->to_string(), dataSetID, record->get_type()->name)
                << std::endl;
        }
    }

    /// Query over all records of the specified type in the specified dataset.
    template <class TRecord>
    void VerifyQuery(unit_test_context_base context, dot::string dataSetID)
    {
        // Get dataset and query
        dot::object_id dataSet = context->get_data_set(dataSetID, context->data_set);
        query query = context->data_source->get_query<TRecord>(dataSet);

        // Iterate over records
        for (TRecord record : query->get_cursor<TRecord>())
        {
            received
                << *dot::string::format(
                    "Record {0} returned by query in dataset {1} and has Type={2}.",
                    record->get_key(), dataSetID, record->get_type()->name)
                << std::endl;
        }
    }


    TEST_CASE("smoke")
    {
        mongo_data_source_test test = new mongo_data_source_test_impl;

        unit_test_context_base context = new unit_test_context_impl(test, "Smoke", ".");
        SaveBasicData(context);

        // Get dataset identifiers
        dot::object_id dataSetA = context->get_data_set("A", context->get_common());
        dot::object_id dataSetB = context->get_data_set("B", context->get_common());

        // Create keys
        mongo_test_key keyA0 = make_mongo_test_key();
        keyA0->record_id = "A";
        keyA0->record_index = dot::nullable<int>(0);

        mongo_test_key keyB0 = make_mongo_test_key();
        keyB0->record_id = "B";
        keyB0->record_index = dot::nullable<int>(0);

        // Verify the result of loading records from datasets A and B
        VerifyLoad(context, ( key<mongo_test_key_impl, mongo_test_data_impl>)keyA0, "A");
        VerifyLoad(context, ( key<mongo_test_key_impl, mongo_test_data_impl>)keyA0, "B");
        VerifyLoad(context, ( key<mongo_test_key_impl, mongo_test_data_impl>)keyB0, "A");
        VerifyLoad(context, ( key<mongo_test_key_impl, mongo_test_data_impl>)keyB0, "B");

        context->keep_db = true;

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("query")
    {
        mongo_data_source_test test = new mongo_data_source_test_impl;

        unit_test_context_base context = new unit_test_context_impl(test, "Query", ".");

        // Create datasets
        dot::object_id dataSetA = context->create_data_set("A", context->data_set);
        dot::object_id dataSetB = context->create_data_set("B", dot::make_list<dot::object_id>({ dataSetA }), context->data_set);
        dot::object_id dataSetC = context->create_data_set("C", dot::make_list<dot::object_id>({ dataSetA }), context->data_set);
        dot::object_id dataSetD = context->create_data_set("D", dot::make_list<dot::object_id>({ dataSetA, dataSetB, dataSetC }), context->data_set);

        // Create initial version of the records
        SaveMinimalRecord(context, "A", "A", 0, 0);
        SaveMinimalRecord(context, "A", "B", 1, 0);
        SaveMinimalRecord(context, "A", "A", 2, 0);
        SaveMinimalRecord(context, "A", "B", 3, 0);
        SaveMinimalRecord(context, "B", "A", 4, 0);
        SaveMinimalRecord(context, "B", "B", 5, 0);
        SaveMinimalRecord(context, "B", "A", 6, 0);
        SaveMinimalRecord(context, "B", "B", 7, 0);
        SaveMinimalRecord(context, "C", "A", 8, 0);
        SaveMinimalRecord(context, "C", "B", 9, 0);
        SaveMinimalRecord(context, "D", "A", 10, 0);
        SaveMinimalRecord(context, "D", "B", 11, 0);

        // Create second version of some records
        SaveMinimalRecord(context, "A", "A", 0, 1);
        SaveMinimalRecord(context, "A", "B", 1, 1);
        SaveMinimalRecord(context, "A", "A", 2, 1);
        SaveMinimalRecord(context, "A", "B", 3, 1);
        SaveMinimalRecord(context, "B", "A", 4, 1);
        SaveMinimalRecord(context, "B", "B", 5, 1);
        SaveMinimalRecord(context, "B", "A", 6, 1);
        SaveMinimalRecord(context, "B", "B", 7, 1);

        // Create third version of even fewer records
        SaveMinimalRecord(context, "A", "A", 0, 2);
        SaveMinimalRecord(context, "A", "B", 1, 2);
        SaveMinimalRecord(context, "A", "A", 2, 2);
        SaveMinimalRecord(context, "A", "B", 3, 2);

        dot::list<dot::string> in_list = dot::make_list<dot::string>();
        in_list->add("B");

        // Query for record_id=B
        dc::cursor_wrapper<mongo_test_data> query = context->data_source->get_query<mongo_test_data>(dataSetD)
            //->Where(p = > p.record_id == "B")
            ->where(make_prop(&mongo_test_data_impl::record_id).in({ "B" }))
            ->where(make_prop(&mongo_test_data_impl::record_id).in(std::vector<std::string>({ "B" })))
            ->where(make_prop(&mongo_test_data_impl::record_id).in(in_list))
            ->where(make_prop(&mongo_test_data_impl::record_id) == "B")
            ->sort_by(make_prop(&mongo_test_data_impl::record_id))
            ->sort_by(make_prop(&mongo_test_data_impl::record_index))
            ->get_cursor<mongo_test_data>();

        for (mongo_test_data obj : query)
        {
            dot::string dataSetID = context->data_source->load_or_null<data_set_data>(obj->data_set)->data_set_id;
            received << *dot::string::format("Key={0} data_set={1} Version={2}", obj->get_key(), dataSetID, obj->version) << std::endl;
        }

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("delete")
    {
        mongo_data_source_test test = new mongo_data_source_test_impl;
        unit_test_context_base context = new unit_test_context_impl(test, "Delete", ".");

        SaveBasicData(context);

        // Get dataset identifiers
        dot::object_id dataSetA = context->get_data_set("A", context->data_set);
        dot::object_id dataSetB = context->get_data_set("B", context->data_set);

        // Create keys
        mongo_test_key keyA0 = new mongo_test_key_impl();
        keyA0->record_id = "A";
        keyA0->record_index = dot::nullable<int>(0);

        mongo_test_key keyB0 = new mongo_test_key_impl();
        keyB0->record_id = "B";
        keyB0->record_index = dot::nullable<int>(0);

        // Verify the result of loading records from datasets A and B
        received << "Initial load" << std::endl;
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyA0, "A");
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyA0, "B");
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyB0, "A");
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyB0, "B");

        received << "Query in dataset A" << std::endl;
        VerifyQuery<mongo_test_data>(context, "A");
        received << "Query in dataset B" << std::endl;
        VerifyQuery<mongo_test_data>(context, "B");

        received << "Delete A0 record in B dataset" << std::endl;
        context->delete_record(keyA0, dataSetB);
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyA0, "A");
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyA0, "B");

        received << "Query in dataset A" << std::endl;
        VerifyQuery<mongo_test_data>(context, "A");
        received << "Query in dataset B" << std::endl;
        VerifyQuery<mongo_test_data>(context, "B");

        received << "Delete A0 record in A dataset" << std::endl;
        context->delete_record(keyA0, dataSetA);
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyA0, "A");
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyA0, "B");

        received << "Query in dataset A" << std::endl;
        VerifyQuery<mongo_test_data>(context, "A");
        received << "Query in dataset B" << std::endl;
        VerifyQuery<mongo_test_data>(context, "B");

        received << "Delete B0 record in B dataset" << std::endl;
        context->delete_record(keyB0, dataSetB);
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyB0, "A");
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyB0, "B");

        received << "Query in dataset A" << std::endl;
        VerifyQuery<mongo_test_data>(context, "A");
        received << "Query in dataset B" << std::endl;
        VerifyQuery<mongo_test_data>(context, "B");

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    /// Test saving object of a different type for the same key.
    ///
    /// The objective of this test is to confirm that load_or_null
    /// for the wrong type will give an error, while query will
    /// skip the object of the wrong type even if there is an
    /// earlier version of the object with the same key that
    /// has a compatible type.
    TEST_CASE("type_change")
    {
        mongo_data_source_test test = new mongo_data_source_test_impl;
        unit_test_context_base context = new unit_test_context_impl(test, "TypeChange", ".");

        // Create datasets
        dot::object_id dataSetA = context->create_data_set("A", context->data_set);
        dot::object_id dataSetB = context->create_data_set("B", dot::make_list<dot::object_id>({ dataSetA }), context->data_set);

        // Create records with minimal data
        SaveDerivedRecord(context, "A", "A", 0);
        SaveDerivedFromDerivedRecord(context, "B", "B", 0);

        // Create keys
        mongo_test_key keyA0 = make_mongo_test_key();
        keyA0->record_id = "A";
        keyA0->record_index = dot::nullable<int>(0);

        mongo_test_key keyB0 = make_mongo_test_key();
        keyB0->record_id = "B";
        keyB0->record_index = dot::nullable<int>(0);

        // Verify the result of loading records from datasets A and B
        received << "Initial load" << std::endl;
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyA0, "A");
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyA0, "B");
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyB0, "A");
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyB0, "B");

        received << "Query in dataset A for type MongoTestDerivedData" << std::endl;
        VerifyQuery<mongo_test_derived_data>(context, "A");
        received << "Query in dataset B for type MongoTestDerivedData" << std::endl;
        VerifyQuery<mongo_test_derived_data>(context, "B");

        received << "Change A0 record type in B dataset to C" << std::endl;
        SaveOtherDerivedRecord(context, "B", "A", 0);
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyA0, "A");
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyA0, "B");

        received << "Query in dataset A for type MongoTestDerivedData" << std::endl;
        VerifyQuery<mongo_test_derived_data>(context, "A");
        received << "Query in dataset B for type MongoTestDerivedData" << std::endl;
        VerifyQuery<mongo_test_derived_data>(context, "B");

        received << "Change A0 record type in A dataset to C" << std::endl;
        SaveOtherDerivedRecord(context, "A", "A", 0);
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyA0, "A");
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyA0, "B");

        received << "Query in dataset A for type MongoTestDerivedData" << std::endl;
        VerifyQuery<mongo_test_derived_data>(context, "A");
        received << "Query in dataset B for type MongoTestDerivedData" << std::endl;
        VerifyQuery<mongo_test_derived_data>(context, "B");

        received << "Change B0 record type in B dataset to C" << std::endl;
        SaveOtherDerivedRecord(context, "B", "B", 0);
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyB0, "A");
        VerifyLoad(context, (key<mongo_test_key_impl, mongo_test_data_impl>) keyB0, "B");

        received << "Query in dataset A for type MongoTestDerivedData" << std::endl;
        VerifyQuery<mongo_test_derived_data>(context, "A");
        received << "Query in dataset B for type MongoTestDerivedData" << std::endl;
        VerifyQuery<mongo_test_derived_data>(context, "B");

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("element_types_query")
    {
        mongo_data_source_test test = new mongo_data_source_test_impl;
        unit_test_context_base context = new unit_test_context_impl(test, "ElementTypesQuery", ".");

        // Saves data in A and B datasets, A is parent of B
        SaveCompleteData(context);

        // Look in B dataset
        dot::object_id dataSetB = context->get_data_set_or_empty("B", context->data_set);

        mongo_test_key key = make_mongo_test_key();
        key->record_id = "BB";
        key->record_index = dot::nullable<int>(2);

        dc::cursor_wrapper<mongo_test_derived_data> testQuery = context->data_source->get_query<mongo_test_derived_data>(dataSetB)
            ->where(make_prop(&mongo_test_derived_data_impl::data_element_list)[0]->*make_prop(&element_sample_data_impl::double_element3) == 1.0)
            ->where(make_prop(&mongo_test_derived_data_impl::data_element_list)[0]->*make_prop(&element_sample_data_impl::string_element3) == "A0")
            ->where(make_prop(&mongo_test_derived_data_impl::local_date_element) < dot::local_date(2003, 5, 2))
            ->where(make_prop(&mongo_test_derived_data_impl::local_date_element) > dot::local_date(2003, 4, 30))
            ->where(make_prop(&mongo_test_derived_data_impl::local_date_element) == dot::local_date(2003, 5, 1))
            ->where(make_prop(&mongo_test_derived_data_impl::local_time_element) < dot::local_time(10, 15, 31))
            ->where(make_prop(&mongo_test_derived_data_impl::local_time_element) > dot::local_time(10, 15, 29))
            ->where(make_prop(&mongo_test_derived_data_impl::local_time_element) == dot::local_time(10, 15, 30))
            ->where(make_prop(&mongo_test_derived_data_impl::local_date_time_element) < dot::local_date_time(2003, 5, 1, 10, 15, 01))
            ->where(make_prop(&mongo_test_derived_data_impl::local_date_time_element) > dot::local_date_time(2003, 5, 1, 10, 14, 59))
            ->where(make_prop(&mongo_test_derived_data_impl::local_date_time_element) == dot::local_date_time(2003, 5, 1, 10, 15))
            ->where(make_prop(&mongo_test_derived_data_impl::string_element2) == dot::string::empty)
            ->where(make_prop(&mongo_test_derived_data_impl::key_element) == key->to_string())
            ->get_cursor<mongo_test_derived_data>();

        for (mongo_test_data obj : testQuery)
        {
            received << *dot::string::format("Key={0} Type={1}", obj->get_key(), obj->get_type()->name) << std::endl;
        }

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("polymorphic_query")
    {
        mongo_data_source_test test = new mongo_data_source_test_impl;
        unit_test_context_base context = new unit_test_context_impl(test, "PolymorphicQuery", ".");

        // Saves data in A and B datasets, A is parent of B
        SaveCompleteData(context);

        // Look in B dataset
        dot::object_id dataSetD = context->get_data_set_or_empty("D", context->data_set);

        // Load record of derived types by base
        received << "Load all types by key to type A" << std::endl;
        {
            mongo_test_key key = make_mongo_test_key();
            key->record_id = "A";
            key->record_index = dot::nullable<int>(0);
            record_base obj = key->load_or_null(context, dataSetD);
            received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->get_type()->name) << std::endl;
        }
        {
            mongo_test_key key = make_mongo_test_key();
            key->record_id = "B";
            key->record_index = dot::nullable<int>(0);
            record_base obj = key->load_or_null(context, dataSetD);
            received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->get_type()->name) << std::endl;
        }
        {
            mongo_test_key key = make_mongo_test_key();
            key->record_id = "C";
            key->record_index = dot::nullable<int>(0);
            record_base obj = key->load_or_null(context, dataSetD);
            received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->get_type()->name) << std::endl;
        }
        {
            mongo_test_key key = make_mongo_test_key();
            key->record_id = "D";
            key->record_index = dot::nullable<int>(0);
            record_base obj = key->load_or_null(context, dataSetD);
            received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->get_type()->name) << std::endl;
        }
        {
            received << "Query by MongoTestData, unconstrained" << std::endl;
            query query = context->data_source->get_query<mongo_test_data>(dataSetD);
            for (record_base obj : query->get_cursor<record_base>())
            {
                received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->get_type()->name) << std::endl;
            }
        }
        {
            received << "Query by MongoTestDerivedData : MongoTestData which also picks up MongoTestDerivedFromDerivedData : MongoTestDerivedData, unconstrained" << std::endl;
            query query = context->data_source->get_query<mongo_test_derived_data>(dataSetD);
            for (record_base obj : query->get_cursor<record_base>())
            {
                received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->get_type()->name) << std::endl;
            }
        }
        {
            received << "Query by MongoTestOtherDerivedData : MongoTestData, unconstrained" << std::endl;
            mongo_test_other_derived_data_impl::typeof();
            query query = context->data_source->get_query<mongo_test_other_derived_data>(dataSetD);
            for (record_base obj : query->get_cursor<record_base>())
            {
                received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->get_type()->name) << std::endl;
            }
        }
        {
            received << "Query by MongoTestDerivedFromDerivedData : MongoTestDerivedData, where MongoTestDerivedData : MongoTestData, unconstrained" << std::endl;
            query query = context->data_source->get_query<mongo_test_derived_from_derived_data>(dataSetD);
            for (record_base obj : query->get_cursor<record_base>())
            {
                received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->get_type()->name) << std::endl;
            }
        }

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("sort")
    {
        mongo_data_source_test test = new mongo_data_source_test_impl;
        unit_test_context_base context = new unit_test_context_impl(test, "Sort", ".");

        // Saves data in A and B datasets, A is parent of B
        SaveCompleteData(context);

        // Look in B dataset
        dot::object_id dataSetD = context->get_data_set_or_empty("D", context->data_set);

        received << "Query by MongoTestData, sort by RecordIndex descending, then by DoubleElement ascending" << std::endl;
        dc::cursor_wrapper<mongo_test_data> baseQuery = context->data_source->get_query<mongo_test_data>(dataSetD)
            ->sort_by_descending(make_prop(&mongo_test_data_impl::record_index))
            ->sort_by(make_prop(&mongo_test_data_impl::double_element))
            ->get_cursor<mongo_test_data>();

        for (mongo_test_data obj : baseQuery)
        {
            received
                << *dot::string::format(
                    "    RecordIndex={0} DoubleElement={1} Key={2} Type={3}",
                    obj->record_index, obj->double_element, obj->get_key(), obj->get_type()->name)
                << std::endl;
        }

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("revision_time")
    {
        mongo_data_source_test test = new mongo_data_source_test_impl;
        unit_test_context_base context = new unit_test_context_impl(test, "RevisionTime", ".");

        // Create datasets
        dot::object_id dataSetA = context->create_data_set("A", context->data_set);
        dot::object_id dataSetB = context->create_data_set("B", dot::make_list<dot::object_id>({ dataSetA }), context->data_set);

        // Create initial version of the records
        dot::object_id objA0 = SaveMinimalRecord(context, "A", "A", 0, 0);
        dot::object_id objB0 = SaveMinimalRecord(context, "B", "B", 0, 0);

        // Create second version of the records
        dot::object_id objA1 = SaveMinimalRecord(context, "A", "A", 0, 1);
        dot::object_id objB1 = SaveMinimalRecord(context, "B", "B", 0, 1);

        dot::object_id cutoff_object_id = context->data_source->create_ordered_object_id();

        // Create third version of the records
        dot::object_id objA2 = SaveMinimalRecord(context, "A", "A", 0, 2);
        dot::object_id objB2 = SaveMinimalRecord(context, "B", "B", 0, 2);

        // Create new records that did not exist before
        dot::object_id objC0 = SaveMinimalRecord(context, "A", "C", 0, 0);
        dot::object_id objD0 = SaveMinimalRecord(context, "B", "D", 0, 0);

        received << "Load records by dot::object_id without constraint" << std::endl;
        received << *dot::string::format("    Found by dot::object_id=A0 = {0}", context->load_or_null<mongo_test_data>(objA0) != nullptr) << std::endl;
        received << *dot::string::format("    Found by dot::object_id=A1 = {0}", context->load_or_null<mongo_test_data>(objA1) != nullptr) << std::endl;
        received << *dot::string::format("    Found by dot::object_id=A2 = {0}", context->load_or_null<mongo_test_data>(objA2) != nullptr) << std::endl;
        received << *dot::string::format("    Found by dot::object_id=C0 = {0}", context->load_or_null<mongo_test_data>(objC0) != nullptr) << std::endl;

        // Load each record by string key
        {
            mongo_test_key loadedA0Key = make_mongo_test_key();
            loadedA0Key->record_id = "A";
            loadedA0Key->record_index = dot::nullable<int>(0);
            mongo_test_data loadedA0 = loadedA0Key->load_or_null(context, dataSetB);

            mongo_test_key loadedC0Key = make_mongo_test_key();
            loadedC0Key->record_id = "C";
            loadedC0Key->record_index = dot::nullable<int>(0);
            mongo_test_data loadedC0 = loadedC0Key->load_or_null(context, dataSetB);

            received << "Load records by string key without constraint" << std::endl;
            if (loadedA0 != nullptr) received << *dot::string::format("    Version found for key=A;0: {0}", loadedA0->version) << std::endl;
            if (loadedC0 != nullptr) received << *dot::string::format("    Version found for key=C;0: {0}", loadedC0->version) << std::endl;
        }

        // Query for all records
        {
            cursor_wrapper<mongo_test_data> query = context->data_source->get_query<mongo_test_data>(dataSetB)
                ->sort_by(make_prop(&mongo_test_data_impl::record_id))
                ->sort_by(make_prop(&mongo_test_data_impl::record_index))
                ->get_cursor<mongo_test_data>();

            received << "Query records without constraint" << std::endl;
            for (mongo_test_data obj : query)
            {
                dot::string dataSetID = context->load_or_null<data_set_data>(obj->data_set)->data_set_id;
                received << *dot::string::format("    Key={0} data_set={1} Version={2}", obj->get_key(), dataSetID, obj->version) << std::endl;
            }
        }

        // Set new value for the DB server key so that the reference to in-memory
        // record is stored inside the key, otherwise it would not be found
        context->data_source->db_server = make_mongo_default_server_data()->to_key();

        // Set revision time constraint
        context->data_source->revised_before_id = cutoff_object_id;

        // Get each record by dot::object_id
        received << "Load records by dot::object_id with RevisedBeforeId constraint" << std::endl;
        received << *dot::string::format("    Found by dot::object_id=A0 = {0}", context->load_or_null<mongo_test_data>(objA0) != nullptr) << std::endl;
        received << *dot::string::format("    Found by dot::object_id=A1 = {0}", context->load_or_null<mongo_test_data>(objA1) != nullptr) << std::endl;
        received << *dot::string::format("    Found by dot::object_id=A2 = {0}", context->load_or_null<mongo_test_data>(objA2) != nullptr) << std::endl;
        received << *dot::string::format("    Found by dot::object_id=C0 = {0}", context->load_or_null<mongo_test_data>(objC0) != nullptr) << std::endl;

        // Load each record by string key
        {
            mongo_test_key loadedA0Key = make_mongo_test_key();
            loadedA0Key->record_id = "A";
            loadedA0Key->record_index = dot::nullable<int>(0);
            mongo_test_data loadedA0 = loadedA0Key->load_or_null(context, dataSetB);

            mongo_test_key loadedC0Key = make_mongo_test_key();
            loadedA0Key->record_id = "C";
            loadedA0Key->record_index = dot::nullable<int>(0);
            mongo_test_data loadedC0 = loadedC0Key->load_or_null(context, dataSetB);

            received << "Load records by string key with RevisedBeforeId constraint" << std::endl;
            if (loadedA0 != nullptr) received << *dot::string::format("    Version found for key=A;0: {0}", loadedA0->version) << std::endl;
            if (loadedC0 != nullptr) received << *dot::string::format("    Version found for key=C;0: {0}", loadedC0->version) << std::endl;
        }

        // Query for revised before the cutoff time
        {
            cursor_wrapper<mongo_test_data> query = context->data_source->get_query<mongo_test_data>(dataSetB)
                ->sort_by(make_prop(&mongo_test_data_impl::record_id))
                ->sort_by(make_prop(&mongo_test_data_impl::record_index))
                ->get_cursor<mongo_test_data>();

            received << "Query records with RevisedBeforeId constraint" << std::endl;
            for (mongo_test_data obj : query)
            {
                dot::string dataSetID = context->load_or_null<data_set_data>(obj->data_set)->data_set_id;
                received << *dot::string::format("    Key={0} data_set={1} Version={2}", obj->get_key(), dataSetID, obj->version) << std::endl;
            }
        }

        // Clear revision time constraint before exiting to avoid an error
        // about deleting readonly database. The error occurs because
        // revision time constraint makes the data source readonly.
        context->data_source->revised_before_id = dot::nullable<dot::object_id>();

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }
}
