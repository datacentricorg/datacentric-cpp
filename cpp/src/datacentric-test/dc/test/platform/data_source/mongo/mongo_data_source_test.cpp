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

#include <dc/serialization/BsonWriter.hpp>
#include <dc/serialization/BsonRecordSerializer.hpp>

#include <dc/platform/data_source/data_source_data.hpp>
#include <dc/platform/data_source/mongo/QueryBuilder.hpp>
#include <dc/platform/data_source/mongo/MongoDefaultServerData.hpp>

#include <dc/platform/dataset/data_set_key.hpp>
#include <dc/platform/dataset/data_set_data.hpp>

#include <dc/test/platform/context/Context.hpp>

#include <dc/test/platform/data_source/mongo/MongoTestData.hpp>
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
    dot::object_id SaveMinimalRecord(unit_test_context_base context, dot::string dataSetID, dot::string recordID, int recordIndex, dot::nullable<int> version = dot::nullable<int>())
    {
        MongoTestData rec = make_MongoTestData();
        rec->RecordID = recordID;
        rec->RecordIndex = recordIndex;
        rec->Version = version;

        dot::object_id dataSet = context->get_data_set(dataSetID, context->data_set);
        context->save(rec, dataSet);
        return rec->ID;
    }

    /// Save base record
    dot::object_id SaveBaseRecord(unit_test_context_base context, dot::string dataSetID, dot::string recordID, int recordIndex)
    {
        MongoTestData rec = new MongoTestDataImpl();
        rec->RecordID = recordID;
        rec->RecordIndex = recordIndex;
        rec->DoubleElement = 100.0;
        rec->local_date_element = dot::local_date(2003, 5, 1);
        rec->local_time_element = dot::local_time(10, 15, 30); // 10:15:30
        rec->local_minute_element = dot::local_minute(10, 15); // 10:15
        rec->local_date_time_element = dot::local_date_time(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->EnumValue = MongoTestEnum::EnumValue2;

        dot::object_id dataSet = context->get_data_set(dataSetID, context->get_common());
        context->save(rec, dataSet);
        return rec->ID;
    }

    /// Save derived record
    dot::object_id SaveDerivedRecord(unit_test_context_base context, dot::string dataSetID, dot::string recordID, int recordIndex)
    {
        MongoTestDerivedData rec = make_MongoTestDerivedData();
        rec->RecordID = recordID;
        rec->RecordIndex = recordIndex;
        rec->DoubleElement = 300.0;
        rec->local_date_element = dot::local_date(2003, 5, 1);
        rec->local_time_element = dot::local_time(10, 15, 30); // 10:15:30
        rec->local_minute_element = dot::local_minute(10, 15); // 10:15
        rec->local_date_time_element = dot::local_date_time(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->StringElement2 = dot::string::empty; // Test how empty value is recorded
        rec->DoubleElement2 = 200.0;

        // dot::string collections
        rec->ArrayOfString = dot::make_list<dot::string>({ "A", "B", "C" });
        rec->ListOfString = dot::make_list<dot::string>({ "A", "B", "C" });

        // Double collections
        rec->ArrayOfDouble = dot::make_list<double>({ 1.0, 2.0, 3.0 });
        rec->ListOfDouble = dot::make_list<double>({ 1.0, 2.0, 3.0 });
        rec->ListOfNullableDouble = dot::make_list<dot::nullable<double>>();
        rec->ListOfNullableDouble->add(10.0);
        rec->ListOfNullableDouble->add(dot::nullable<double>());
        rec->ListOfNullableDouble->add(30.0);

        // Data element
        rec->DataElement = make_ElementSampleData();
        rec->DataElement->DoubleElement3 = 1.0;
        rec->DataElement->StringElement3 = "AA";

        // Data element list
        rec->DataElementList = dot::make_list<ElementSampleData>();
        ElementSampleData elementList0 = make_ElementSampleData();
        elementList0->DoubleElement3 = 1.0;
        elementList0->StringElement3 = "A0";
        rec->DataElementList->add(elementList0);
        ElementSampleData elementList1 = make_ElementSampleData();
        elementList1->DoubleElement3 = 2.0;
        elementList1->StringElement3 = "A1";
        rec->DataElementList->add(elementList1);

        // Key element
        rec->KeyElement = make_MongoTestKey();
        rec->KeyElement->RecordID = "BB";
        rec->KeyElement->RecordIndex = 2;

        // Key element list
        rec->KeyElementList = dot::make_list<MongoTestKey>();
        MongoTestKey keyList0 = make_MongoTestKey();
        keyList0->RecordID = "B0";
        keyList0->RecordIndex = 3;
        rec->KeyElementList->add(keyList0);
        MongoTestKey keyList1 = make_MongoTestKey();
        keyList1->RecordID = "B1";
        keyList1->RecordIndex = 4;
        rec->KeyElementList->add(keyList1);

        dot::object_id dataSet = context->get_data_set(dataSetID, context->get_common());
        context->save(rec, dataSet);
        return rec->ID;
    }

    /// Save other derived record.
    dot::object_id SaveOtherDerivedRecord(unit_test_context_base context, dot::string dataSetID, dot::string recordID, int recordIndex)
    {
        MongoTestOtherDerivedData rec = make_MongoTestOtherDerivedData();
        rec->RecordID = recordID;
        rec->RecordIndex = recordIndex;
        rec->DoubleElement = 300.0;
        rec->local_date_element = dot::local_date(2003, 5, 1);
        rec->local_time_element = dot::local_time(10, 15, 30); // 10:15:30
        rec->local_minute_element = dot::local_minute(10, 15); // 10:15
        rec->local_date_time_element = dot::local_date_time(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->OtherStringElement2 = dot::string::empty; // Test how empty value is recorded
        rec->OtherDoubleElement2 = 200.0;

        dot::object_id dataSet = context->get_data_set(dataSetID, context->data_set);
        context->save(rec, dataSet);
        return rec->ID;
    }

    /// Save record that is derived from derived.
    dot::object_id SaveDerivedFromDerivedRecord(unit_test_context_base context, string dataSetID, string recordID, int recordIndex)
    {
        MongoTestDerivedFromDerivedData rec = make_MongoTestDerivedFromDerivedData();
        rec->RecordID = recordID;
        rec->RecordIndex = recordIndex;
        rec->DoubleElement = 300.0;
        rec->local_date_element = dot::local_date(2003, 5, 1);
        rec->local_time_element = dot::local_time(10, 15, 30); // 10:15:30
        rec->local_minute_element = dot::local_minute(10, 15); // 10:15
        rec->local_date_time_element = dot::local_date_time(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->OtherStringElement3 = dot::string::empty; // Test how empty value is recorded
        rec->OtherDoubleElement3 = 200.0;

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
                    key->to_string(), dataSetID, record->type()->name)
                << std::endl;
        }
    }

    /// Query over all records of the specified type in the specified dataset.
    template <class TRecord>
    void VerifyQuery(unit_test_context_base context, dot::string dataSetID)
    {
        // Get dataset and query
        dot::object_id dataSet = context->get_data_set(dataSetID, context->data_set);
        query query = context->DataSource->get_query<TRecord>(dataSet);

        // Iterate over records
        for (TRecord record : query->get_cursor<TRecord>())
        {
            received
                << *dot::string::format(
                    "Record {0} returned by query in dataset {1} and has Type={2}.",
                    record->get_key(), dataSetID, record->type()->name)
                << std::endl;
        }
    }


    TEST_CASE("Smoke")
    {
        mongo_data_source_test test = new mongo_data_source_test_impl;

        unit_test_context_base context = new unit_test_context_impl(test, "Smoke", ".");
        SaveBasicData(context);

        // Get dataset identifiers
        dot::object_id dataSetA = context->get_data_set("A", context->get_common());
        dot::object_id dataSetB = context->get_data_set("B", context->get_common());

        // Create keys
        MongoTestKey keyA0 = make_MongoTestKey();
        keyA0->RecordID = "A";
        keyA0->RecordIndex = dot::nullable<int>(0);

        MongoTestKey keyB0 = make_MongoTestKey();
        keyB0->RecordID = "B";
        keyB0->RecordIndex = dot::nullable<int>(0);

        // Verify the result of loading records from datasets A and B
        VerifyLoad(context, ( key<MongoTestKeyImpl, MongoTestDataImpl>)keyA0, "A");
        VerifyLoad(context, ( key<MongoTestKeyImpl, MongoTestDataImpl>)keyA0, "B");
        VerifyLoad(context, ( key<MongoTestKeyImpl, MongoTestDataImpl>)keyB0, "A");
        VerifyLoad(context, ( key<MongoTestKeyImpl, MongoTestDataImpl>)keyB0, "B");

        context->KeepDb = true;

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("Query")
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

        // Query for RecordID=B
        dc::cursor_wrapper<MongoTestData> query = context->DataSource->get_query<MongoTestData>(dataSetD)
            //->Where(p = > p.RecordID == "B")
            ->where(make_prop(&MongoTestDataImpl::RecordID).in({ "B" }))
            ->where(make_prop(&MongoTestDataImpl::RecordID).in(std::vector<std::string>({ "B" })))
            ->where(make_prop(&MongoTestDataImpl::RecordID).in(in_list))
            ->where(make_prop(&MongoTestDataImpl::RecordID) == "B")
            ->sort_by(make_prop(&MongoTestDataImpl::RecordID))
            ->sort_by(make_prop(&MongoTestDataImpl::RecordIndex))
            ->get_cursor<MongoTestData>();

        for (MongoTestData obj : query)
        {
            dot::string dataSetID = context->DataSource->load_or_null<data_set_data>(obj->data_set)->data_set_id;
            received << *dot::string::format("Key={0} data_set={1} Version={2}", obj->get_key(), dataSetID, obj->Version) << std::endl;
        }

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("Delete")
    {
        mongo_data_source_test test = new mongo_data_source_test_impl;
        unit_test_context_base context = new unit_test_context_impl(test, "Delete", ".");

        SaveBasicData(context);

        // Get dataset identifiers
        dot::object_id dataSetA = context->get_data_set("A", context->data_set);
        dot::object_id dataSetB = context->get_data_set("B", context->data_set);

        // Create keys
        MongoTestKey keyA0 = new MongoTestKeyImpl();
        keyA0->RecordID = "A";
        keyA0->RecordIndex = dot::nullable<int>(0);

        MongoTestKey keyB0 = new MongoTestKeyImpl();
        keyB0->RecordID = "B";
        keyB0->RecordIndex = dot::nullable<int>(0);

        // Verify the result of loading records from datasets A and B
        received << "Initial load" << std::endl;
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "A");
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "B");
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "A");
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "B");

        received << "Query in dataset A" << std::endl;
        VerifyQuery<MongoTestData>(context, "A");
        received << "Query in dataset B" << std::endl;
        VerifyQuery<MongoTestData>(context, "B");

        received << "Delete A0 record in B dataset" << std::endl;
        context->Delete(keyA0, dataSetB);
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "A");
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "B");

        received << "Query in dataset A" << std::endl;
        VerifyQuery<MongoTestData>(context, "A");
        received << "Query in dataset B" << std::endl;
        VerifyQuery<MongoTestData>(context, "B");

        received << "Delete A0 record in A dataset" << std::endl;
        context->Delete(keyA0, dataSetA);
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "A");
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "B");

        received << "Query in dataset A" << std::endl;
        VerifyQuery<MongoTestData>(context, "A");
        received << "Query in dataset B" << std::endl;
        VerifyQuery<MongoTestData>(context, "B");

        received << "Delete B0 record in B dataset" << std::endl;
        context->Delete(keyB0, dataSetB);
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "A");
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "B");

        received << "Query in dataset A" << std::endl;
        VerifyQuery<MongoTestData>(context, "A");
        received << "Query in dataset B" << std::endl;
        VerifyQuery<MongoTestData>(context, "B");

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
    TEST_CASE("TypeChange")
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
        MongoTestKey keyA0 = make_MongoTestKey();
        keyA0->RecordID = "A";
        keyA0->RecordIndex = dot::nullable<int>(0);

        MongoTestKey keyB0 = make_MongoTestKey();
        keyB0->RecordID = "B";
        keyB0->RecordIndex = dot::nullable<int>(0);

        // Verify the result of loading records from datasets A and B
        received << "Initial load" << std::endl;
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "A");
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "B");
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "A");
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "B");

        received << "Query in dataset A for type MongoTestDerivedData" << std::endl;
        VerifyQuery<MongoTestDerivedData>(context, "A");
        received << "Query in dataset B for type MongoTestDerivedData" << std::endl;
        VerifyQuery<MongoTestDerivedData>(context, "B");

        received << "Change A0 record type in B dataset to C" << std::endl;
        SaveOtherDerivedRecord(context, "B", "A", 0);
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "A");
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "B");

        received << "Query in dataset A for type MongoTestDerivedData" << std::endl;
        VerifyQuery<MongoTestDerivedData>(context, "A");
        received << "Query in dataset B for type MongoTestDerivedData" << std::endl;
        VerifyQuery<MongoTestDerivedData>(context, "B");

        received << "Change A0 record type in A dataset to C" << std::endl;
        SaveOtherDerivedRecord(context, "A", "A", 0);
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "A");
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "B");

        received << "Query in dataset A for type MongoTestDerivedData" << std::endl;
        VerifyQuery<MongoTestDerivedData>(context, "A");
        received << "Query in dataset B for type MongoTestDerivedData" << std::endl;
        VerifyQuery<MongoTestDerivedData>(context, "B");

        received << "Change B0 record type in B dataset to C" << std::endl;
        SaveOtherDerivedRecord(context, "B", "B", 0);
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "A");
        VerifyLoad(context, (key<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "B");

        received << "Query in dataset A for type MongoTestDerivedData" << std::endl;
        VerifyQuery<MongoTestDerivedData>(context, "A");
        received << "Query in dataset B for type MongoTestDerivedData" << std::endl;
        VerifyQuery<MongoTestDerivedData>(context, "B");

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("ElementTypesQuery")
    {
        mongo_data_source_test test = new mongo_data_source_test_impl;
        unit_test_context_base context = new unit_test_context_impl(test, "ElementTypesQuery", ".");

        // Saves data in A and B datasets, A is parent of B
        SaveCompleteData(context);

        // Look in B dataset
        dot::object_id dataSetB = context->get_data_set_or_empty("B", context->data_set);

        MongoTestKey key = make_MongoTestKey();
        key->RecordID = "BB";
        key->RecordIndex = dot::nullable<int>(2);

        dc::cursor_wrapper<MongoTestDerivedData> testQuery = context->DataSource->get_query<MongoTestDerivedData>(dataSetB)
            ->where(make_prop(&MongoTestDerivedDataImpl::DataElementList)[0]->*make_prop(&ElementSampleDataImpl::DoubleElement3) == 1.0)
            ->where(make_prop(&MongoTestDerivedDataImpl::DataElementList)[0]->*make_prop(&ElementSampleDataImpl::StringElement3) == "A0")
            ->where(make_prop(&MongoTestDerivedDataImpl::local_date_element) < dot::local_date(2003, 5, 2))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_date_element) > dot::local_date(2003, 4, 30))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_date_element) == dot::local_date(2003, 5, 1))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_time_element) < dot::local_time(10, 15, 31))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_time_element) > dot::local_time(10, 15, 29))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_time_element) == dot::local_time(10, 15, 30))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_date_time_element) < dot::local_date_time(2003, 5, 1, 10, 15, 01))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_date_time_element) > dot::local_date_time(2003, 5, 1, 10, 14, 59))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_date_time_element) == dot::local_date_time(2003, 5, 1, 10, 15))
            ->where(make_prop(&MongoTestDerivedDataImpl::StringElement2) == dot::string::empty)
            ->where(make_prop(&MongoTestDerivedDataImpl::KeyElement) == key->to_string())
            ->get_cursor<MongoTestDerivedData>();

        for (MongoTestData obj : testQuery)
        {
            received << *dot::string::format("Key={0} Type={1}", obj->get_key(), obj->type()->name) << std::endl;
        }

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("PolymorphicQuery")
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
            MongoTestKey key = make_MongoTestKey();
            key->RecordID = "A";
            key->RecordIndex = dot::nullable<int>(0);
            record_base obj = key->load_or_null(context, dataSetD);
            received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->type()->name) << std::endl;
        }
        {
            MongoTestKey key = make_MongoTestKey();
            key->RecordID = "B";
            key->RecordIndex = dot::nullable<int>(0);
            record_base obj = key->load_or_null(context, dataSetD);
            received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->type()->name) << std::endl;
        }
        {
            MongoTestKey key = make_MongoTestKey();
            key->RecordID = "C";
            key->RecordIndex = dot::nullable<int>(0);
            record_base obj = key->load_or_null(context, dataSetD);
            received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->type()->name) << std::endl;
        }
        {
            MongoTestKey key = make_MongoTestKey();
            key->RecordID = "D";
            key->RecordIndex = dot::nullable<int>(0);
            record_base obj = key->load_or_null(context, dataSetD);
            received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->type()->name) << std::endl;
        }
        {
            received << "Query by MongoTestData, unconstrained" << std::endl;
            query query = context->DataSource->get_query<MongoTestData>(dataSetD);
            for (record_base obj : query->get_cursor<record_base>())
            {
                received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->type()->name) << std::endl;
            }
        }
        {
            received << "Query by MongoTestDerivedData : MongoTestData which also picks up MongoTestDerivedFromDerivedData : MongoTestDerivedData, unconstrained" << std::endl;
            query query = context->DataSource->get_query<MongoTestDerivedData>(dataSetD);
            for (record_base obj : query->get_cursor<record_base>())
            {
                received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->type()->name) << std::endl;
            }
        }
        {
            received << "Query by MongoTestOtherDerivedData : MongoTestData, unconstrained" << std::endl;
            MongoTestOtherDerivedDataImpl::typeof();
            query query = context->DataSource->get_query<MongoTestOtherDerivedData>(dataSetD);
            for (record_base obj : query->get_cursor<record_base>())
            {
                received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->type()->name) << std::endl;
            }
        }
        {
            received << "Query by MongoTestDerivedFromDerivedData : MongoTestDerivedData, where MongoTestDerivedData : MongoTestData, unconstrained" << std::endl;
            query query = context->DataSource->get_query<MongoTestDerivedFromDerivedData>(dataSetD);
            for (record_base obj : query->get_cursor<record_base>())
            {
                received << *dot::string::format("    Key={0} Type={1}", obj->get_key(), obj->type()->name) << std::endl;
            }
        }

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("Sort")
    {
        mongo_data_source_test test = new mongo_data_source_test_impl;
        unit_test_context_base context = new unit_test_context_impl(test, "Sort", ".");

        // Saves data in A and B datasets, A is parent of B
        SaveCompleteData(context);

        // Look in B dataset
        dot::object_id dataSetD = context->get_data_set_or_empty("D", context->data_set);

        received << "Query by MongoTestData, sort by RecordIndex descending, then by DoubleElement ascending" << std::endl;
        dc::cursor_wrapper<MongoTestData> baseQuery = context->DataSource->get_query<MongoTestData>(dataSetD)
            ->sort_by_descending(make_prop(&MongoTestDataImpl::RecordIndex))
            ->sort_by(make_prop(&MongoTestDataImpl::DoubleElement))
            ->get_cursor<MongoTestData>();

        for (MongoTestData obj : baseQuery)
        {
            received
                << *dot::string::format(
                    "    RecordIndex={0} DoubleElement={1} Key={2} Type={3}",
                    obj->RecordIndex, obj->DoubleElement, obj->get_key(), obj->type()->name)
                << std::endl;
        }

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("RevisionTime")
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

        dot::object_id cutoff_object_id = context->DataSource->create_ordered_object_id();

        // Create third version of the records
        dot::object_id objA2 = SaveMinimalRecord(context, "A", "A", 0, 2);
        dot::object_id objB2 = SaveMinimalRecord(context, "B", "B", 0, 2);

        // Create new records that did not exist before
        dot::object_id objC0 = SaveMinimalRecord(context, "A", "C", 0, 0);
        dot::object_id objD0 = SaveMinimalRecord(context, "B", "D", 0, 0);

        received << "Load records by dot::object_id without constraint" << std::endl;
        received << *dot::string::format("    Found by dot::object_id=A0 = {0}", context->load_or_null<MongoTestData>(objA0) != nullptr) << std::endl;
        received << *dot::string::format("    Found by dot::object_id=A1 = {0}", context->load_or_null<MongoTestData>(objA1) != nullptr) << std::endl;
        received << *dot::string::format("    Found by dot::object_id=A2 = {0}", context->load_or_null<MongoTestData>(objA2) != nullptr) << std::endl;
        received << *dot::string::format("    Found by dot::object_id=C0 = {0}", context->load_or_null<MongoTestData>(objC0) != nullptr) << std::endl;

        // Load each record by string key
        {
            MongoTestKey loadedA0Key = make_MongoTestKey();
            loadedA0Key->RecordID = "A";
            loadedA0Key->RecordIndex = dot::nullable<int>(0);
            MongoTestData loadedA0 = loadedA0Key->load_or_null(context, dataSetB);

            MongoTestKey loadedC0Key = make_MongoTestKey();
            loadedC0Key->RecordID = "C";
            loadedC0Key->RecordIndex = dot::nullable<int>(0);
            MongoTestData loadedC0 = loadedC0Key->load_or_null(context, dataSetB);

            received << "Load records by string key without constraint" << std::endl;
            if (loadedA0 != nullptr) received << *dot::string::format("    Version found for key=A;0: {0}", loadedA0->Version) << std::endl;
            if (loadedC0 != nullptr) received << *dot::string::format("    Version found for key=C;0: {0}", loadedC0->Version) << std::endl;
        }

        // Query for all records
        {
            cursor_wrapper<MongoTestData> query = context->DataSource->get_query<MongoTestData>(dataSetB)
                ->sort_by(make_prop(&MongoTestDataImpl::RecordID))
                ->sort_by(make_prop(&MongoTestDataImpl::RecordIndex))
                ->get_cursor<MongoTestData>();

            received << "Query records without constraint" << std::endl;
            for (MongoTestData obj : query)
            {
                dot::string dataSetID = context->load_or_null<data_set_data>(obj->data_set)->data_set_id;
                received << *dot::string::format("    Key={0} data_set={1} Version={2}", obj->get_key(), dataSetID, obj->Version) << std::endl;
            }
        }

        // Set new value for the DB server key so that the reference to in-memory
        // record is stored inside the key, otherwise it would not be found
        context->DataSource->db_server = make_MongoDefaultServerData()->to_key();

        // Set revision time constraint
        context->DataSource->revised_before_id = cutoff_object_id;

        // Get each record by dot::object_id
        received << "Load records by dot::object_id with RevisedBeforeId constraint" << std::endl;
        received << *dot::string::format("    Found by dot::object_id=A0 = {0}", context->load_or_null<MongoTestData>(objA0) != nullptr) << std::endl;
        received << *dot::string::format("    Found by dot::object_id=A1 = {0}", context->load_or_null<MongoTestData>(objA1) != nullptr) << std::endl;
        received << *dot::string::format("    Found by dot::object_id=A2 = {0}", context->load_or_null<MongoTestData>(objA2) != nullptr) << std::endl;
        received << *dot::string::format("    Found by dot::object_id=C0 = {0}", context->load_or_null<MongoTestData>(objC0) != nullptr) << std::endl;

        // Load each record by string key
        {
            MongoTestKey loadedA0Key = make_MongoTestKey();
            loadedA0Key->RecordID = "A";
            loadedA0Key->RecordIndex = dot::nullable<int>(0);
            MongoTestData loadedA0 = loadedA0Key->load_or_null(context, dataSetB);

            MongoTestKey loadedC0Key = make_MongoTestKey();
            loadedA0Key->RecordID = "C";
            loadedA0Key->RecordIndex = dot::nullable<int>(0);
            MongoTestData loadedC0 = loadedC0Key->load_or_null(context, dataSetB);

            received << "Load records by string key with RevisedBeforeId constraint" << std::endl;
            if (loadedA0 != nullptr) received << *dot::string::format("    Version found for key=A;0: {0}", loadedA0->Version) << std::endl;
            if (loadedC0 != nullptr) received << *dot::string::format("    Version found for key=C;0: {0}", loadedC0->Version) << std::endl;
        }

        // Query for revised before the cutoff time
        {
            cursor_wrapper<MongoTestData> query = context->DataSource->get_query<MongoTestData>(dataSetB)
                ->sort_by(make_prop(&MongoTestDataImpl::RecordID))
                ->sort_by(make_prop(&MongoTestDataImpl::RecordIndex))
                ->get_cursor<MongoTestData>();

            received << "Query records with RevisedBeforeId constraint" << std::endl;
            for (MongoTestData obj : query)
            {
                dot::string dataSetID = context->load_or_null<data_set_data>(obj->data_set)->data_set_id;
                received << *dot::string::format("    Key={0} data_set={1} Version={2}", obj->get_key(), dataSetID, obj->Version) << std::endl;
            }
        }

        // Clear revision time constraint before exiting to avoid an error
        // about deleting readonly database. The error occurs because
        // revision time constraint makes the data source readonly.
        context->DataSource->revised_before_id = dot::nullable<dot::object_id>();

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }
}
