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

#include <dc/platform/data_source/IDataSource.hpp>
#include <dc/platform/data_source/mongo/QueryBuilder.hpp>
#include <dc/platform/data_source/mongo/MongoDefaultServerData.hpp>

#include <dc/platform/dataset/DataSetKey.hpp>
#include <dc/platform/dataset/DataSetData.hpp>

#include <dc/platform/context/Context.hpp>
#include <dc/test/platform/context/Context.hpp>

#include <dc/test/platform/data_source/mongo/MongoTestData.hpp>
#include <bsoncxx/json.hpp>

namespace dc
{
    static std::stringstream received;

    class MongoDataSourceTestImpl; using MongoDataSourceTest = dot::ptr<MongoDataSourceTestImpl>;

    class MongoDataSourceTestImpl : public virtual dot::object_impl
    {
        typedef MongoDataSourceTestImpl self;
    public:

        DOT_TYPE_BEGIN(".Runtime.Test", "MongoDataSourceTest")
        DOT_TYPE_END()
    };

    /// <summary>Save record with minimal data for testing how the records are found. </summary>
    ObjectId SaveMinimalRecord(IUnitTestContext context, dot::string dataSetID, dot::string recordID, int recordIndex, dot::nullable<int> version = dot::nullable<int>())
    {
        MongoTestData rec = new_MongoTestData();
        rec->RecordID = recordID;
        rec->RecordIndex = recordIndex;
        rec->Version = version;

        ObjectId dataSet = context->GetDataSet(dataSetID, context->DataSet);
        context->Save(rec, dataSet);
        return rec->ID;
    }

    /// <summary>Save base record</summary>
    ObjectId SaveBaseRecord(IUnitTestContext context, dot::string dataSetID, dot::string recordID, int recordIndex)
    {
        MongoTestData rec = new MongoTestDataImpl();
        rec->RecordID = recordID;
        rec->RecordIndex = recordIndex;
        rec->DoubleElement = 100.0;
        rec->local_dateElement = dot::local_date(2003, 5, 1);
        rec->local_timeElement = dot::local_time(10, 15, 30); // 10:15:30
        rec->local_minuteElement = dot::local_minute(10, 15); // 10:15
        rec->local_date_timeElement = dot::local_date_time(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->EnumValue = MongoTestEnum::EnumValue2;

        ObjectId dataSet = context->GetDataSet(dataSetID, context->GetCommon());
        context->Save(rec, dataSet);
        return rec->ID;
    }

    /// <summary>Save derived record</summary>
    ObjectId SaveDerivedRecord(IUnitTestContext context, dot::string dataSetID, dot::string recordID, int recordIndex)
    {
        MongoTestDerivedData rec = new_MongoTestDerivedData();
        rec->RecordID = recordID;
        rec->RecordIndex = recordIndex;
        rec->DoubleElement = 300.0;
        rec->local_dateElement = dot::local_date(2003, 5, 1);
        rec->local_timeElement = dot::local_time(10, 15, 30); // 10:15:30
        rec->local_minuteElement = dot::local_minute(10, 15); // 10:15
        rec->local_date_timeElement = dot::local_date_time(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->StringElement2 = dot::string::Empty; // Test how empty value is recorded
        rec->DoubleElement2 = 200.0;

        // dot::string collections
        rec->ArrayOfString = dot::new_Array1D<dot::string>({ "A", "B", "C" });
        rec->ListOfString = dot::new_List<dot::string>({ "A", "B", "C" });

        // Double collections
        rec->ArrayOfDouble = dot::new_Array1D<double>({ 1.0, 2.0, 3.0 });
        rec->ListOfDouble = dot::new_List<double>({ 1.0, 2.0, 3.0 });
        rec->ListOfNullableDouble = dot::new_List<dot::nullable<double>>();
        rec->ListOfNullableDouble->Add(10.0);
        rec->ListOfNullableDouble->Add(dot::nullable<double>());
        rec->ListOfNullableDouble->Add(30.0);

        // Data element
        rec->DataElement = new_ElementSampleData();
        rec->DataElement->DoubleElement3 = 1.0;
        rec->DataElement->StringElement3 = "AA";

        // Data element list
        rec->DataElementList = dot::new_List<ElementSampleData>();
        ElementSampleData elementList0 = new_ElementSampleData();
        elementList0->DoubleElement3 = 1.0;
        elementList0->StringElement3 = "A0";
        rec->DataElementList->Add(elementList0);
        ElementSampleData elementList1 = new_ElementSampleData();
        elementList1->DoubleElement3 = 2.0;
        elementList1->StringElement3 = "A1";
        rec->DataElementList->Add(elementList1);

        // Key element
        rec->KeyElement = new_MongoTestKey();
        rec->KeyElement->RecordID = "BB";
        rec->KeyElement->RecordIndex = 2;

        // Key element list
        rec->KeyElementList = dot::new_List<MongoTestKey>();
        MongoTestKey keyList0 = new_MongoTestKey();
        keyList0->RecordID = "B0";
        keyList0->RecordIndex = 3;
        rec->KeyElementList->Add(keyList0);
        MongoTestKey keyList1 = new_MongoTestKey();
        keyList1->RecordID = "B1";
        keyList1->RecordIndex = 4;
        rec->KeyElementList->Add(keyList1);

        ObjectId dataSet = context->GetDataSet(dataSetID, context->GetCommon());
        context->Save(rec, dataSet);
        return rec->ID;
    }

    /// <summary>Save other derived record.</summary>
    ObjectId SaveOtherDerivedRecord(IUnitTestContext context, dot::string dataSetID, dot::string recordID, int recordIndex)
    {
        MongoTestOtherDerivedData rec = new_MongoTestOtherDerivedData();
        rec->RecordID = recordID;
        rec->RecordIndex = recordIndex;
        rec->DoubleElement = 300.0;
        rec->local_dateElement = dot::local_date(2003, 5, 1);
        rec->local_timeElement = dot::local_time(10, 15, 30); // 10:15:30
        rec->local_minuteElement = dot::local_minute(10, 15); // 10:15
        rec->local_date_timeElement = dot::local_date_time(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->OtherStringElement2 = dot::string::Empty; // Test how empty value is recorded
        rec->OtherDoubleElement2 = 200.0;

        ObjectId dataSet = context->GetDataSet(dataSetID, context->DataSet);
        context->Save(rec, dataSet);
        return rec->ID;
    }

    /// <summary>Save record that is derived from derived.</summary>
    ObjectId SaveDerivedFromDerivedRecord(IUnitTestContext context, string dataSetID, string recordID, int recordIndex)
    {
        MongoTestDerivedFromDerivedData rec = new_MongoTestDerivedFromDerivedData();
        rec->RecordID = recordID;
        rec->RecordIndex = recordIndex;
        rec->DoubleElement = 300.0;
        rec->local_dateElement = dot::local_date(2003, 5, 1);
        rec->local_timeElement = dot::local_time(10, 15, 30); // 10:15:30
        rec->local_minuteElement = dot::local_minute(10, 15); // 10:15
        rec->local_date_timeElement = dot::local_date_time(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->OtherStringElement3 = dot::string::Empty; // Test how empty value is recorded
        rec->OtherDoubleElement3 = 200.0;

        ObjectId dataSet = context->GetDataSet(dataSetID, context->DataSet);
        context->Save(rec, dataSet);
        return rec->ID;
    }

    /// <summary>Two datasets and two objects, one base and one derived.</summary>
    void SaveBasicData(IUnitTestContext context)
    {
        // Create datasets
        ObjectId dataSetA = context->CreateDataSet("A", context->DataSet);
        dot::list<ObjectId> parents = dot::new_List<ObjectId>();
        parents->Add(dataSetA);

        ObjectId dataSetB = context->CreateDataSet("B", parents, context->DataSet);

        // Create records with minimal data
        SaveBaseRecord(context, "A", "A", 0);
        SaveDerivedRecord(context, "B", "B", 0);
    }

    /// <summary>Two datasets and eight objects, split between base and derived.</summary>
    void SaveCompleteData(IUnitTestContext context)
    {
        // Create datasets
        ObjectId dataSetA = context->CreateDataSet("A", context->DataSet);
        ObjectId dataSetB = context->CreateDataSet("B", dot::new_List<ObjectId>({ dataSetA }), context->DataSet);
        ObjectId dataSetC = context->CreateDataSet("C", dot::new_List<ObjectId>({ dataSetA }), context->DataSet);
        ObjectId dataSetD = context->CreateDataSet("D", dot::new_List<ObjectId>({ dataSetA, dataSetB, dataSetC }), context->DataSet);

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

    /// <summary>Minimal data in multiple datasets with overlapping parents.</summary>
    void SaveMultiDataSetData(IUnitTestContext context)
    {
        // Create datasets
        ObjectId dataSetA = context->CreateDataSet("A", context->DataSet);
        ObjectId dataSetB = context->CreateDataSet("B", dot::new_List<ObjectId>({ dataSetA }), context->DataSet);
        ObjectId dataSetC = context->CreateDataSet("C", dot::new_List<ObjectId>({ dataSetA }), context->DataSet);
        ObjectId dataSetD = context->CreateDataSet("D", dot::new_List<ObjectId>({ dataSetA, dataSetB, dataSetC }), context->DataSet);

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

    /// <summary>Load the object and verify the outcome.</summary>
    template <class TKey, class TRecord>
    void VerifyLoad(IUnitTestContext context, KeyFor<TKey, TRecord> key, dot::string dataSetID)
    {
        // Get dataset and try loading the record
        ObjectId dataSet = context->GetDataSet(dataSetID, context->GetCommon());
        dot::ptr<TRecord> record = key->LoadOrNull(context, dataSet);

        if (record == nullptr)
        {
            // Not found
            received << * dot::string::format("Record {0} in dataset {1} not found.", key->ToString(), dataSetID) << std::endl;
        }
        else
        {
            // Found, also checks that the key matches
            REQUIRE(record->getKey() == key->ToString());
            received
                << *dot::string::format("Record {0} in dataset {1} found and has Type={2}.",
                    key->ToString(), dataSetID, record->type()->name)
                << std::endl;
        }
    }

    /// <summary>Query over all records of the specified type in the specified dataset.</summary>
    template <class TRecord>
    void VerifyQuery(IUnitTestContext context, dot::string dataSetID)
    {
        // Get dataset and query
        ObjectId dataSet = context->GetDataSet(dataSetID, context->DataSet);
        IQuery query = context->DataSource->GetQuery<TRecord>(dataSet);

        // Iterate over records
        for (TRecord record : query->AsEnumerable<TRecord>())
        {
            received
                << *dot::string::format(
                    "Record {0} returned by query in dataset {1} and has Type={2}.",
                    record->getKey(), dataSetID, record->type()->name)
                << std::endl;
        }
    }


    TEST_CASE("Smoke")
    {
        MongoDataSourceTest test = new MongoDataSourceTestImpl;

        IUnitTestContext context = new UnitTestContextImpl(test, "Smoke", ".");
        SaveBasicData(context);

        // Get dataset identifiers
        ObjectId dataSetA = context->GetDataSet("A", context->GetCommon());
        ObjectId dataSetB = context->GetDataSet("B", context->GetCommon());

        // Create keys
        MongoTestKey keyA0 = new_MongoTestKey();
        keyA0->RecordID = "A";
        keyA0->RecordIndex = dot::nullable<int>(0);

        MongoTestKey keyB0 = new_MongoTestKey();
        keyB0->RecordID = "B";
        keyB0->RecordIndex = dot::nullable<int>(0);

        // Verify the result of loading records from datasets A and B
        VerifyLoad(context, ( KeyFor<MongoTestKeyImpl, MongoTestDataImpl>)keyA0, "A");
        VerifyLoad(context, ( KeyFor<MongoTestKeyImpl, MongoTestDataImpl>)keyA0, "B");
        VerifyLoad(context, ( KeyFor<MongoTestKeyImpl, MongoTestDataImpl>)keyB0, "A");
        VerifyLoad(context, ( KeyFor<MongoTestKeyImpl, MongoTestDataImpl>)keyB0, "B");

        context->KeepDb = true;

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("Query")
    {
        MongoDataSourceTest test = new MongoDataSourceTestImpl;

        IUnitTestContext context = new UnitTestContextImpl(test, "Query", ".");

        // Create datasets
        ObjectId dataSetA = context->CreateDataSet("A", context->DataSet);
        ObjectId dataSetB = context->CreateDataSet("B", dot::new_List<ObjectId>({ dataSetA }), context->DataSet);
        ObjectId dataSetC = context->CreateDataSet("C", dot::new_List<ObjectId>({ dataSetA }), context->DataSet);
        ObjectId dataSetD = context->CreateDataSet("D", dot::new_List<ObjectId>({ dataSetA, dataSetB, dataSetC }), context->DataSet);

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

        dot::list<dot::string> in_list = dot::new_List<dot::string>();
        in_list->Add("B");

        // Query for RecordID=B
        dot::IEnumerable<MongoTestData> query = context->DataSource->GetQuery<MongoTestData>(dataSetD)
            //->Where(p = > p.RecordID == "B")
            ->Where(make_prop(&MongoTestDataImpl::RecordID).in({ "B" }))
            ->Where(make_prop(&MongoTestDataImpl::RecordID).in(std::vector<std::string>({ "B" })))
            ->Where(make_prop(&MongoTestDataImpl::RecordID).in(in_list))
            ->Where(make_prop(&MongoTestDataImpl::RecordID) == "B")
            ->SortBy(make_prop(&MongoTestDataImpl::RecordID))
            ->SortBy(make_prop(&MongoTestDataImpl::RecordIndex))
            ->AsEnumerable<MongoTestData>();

        for (MongoTestData obj : query)
        {
            dot::string dataSetID = context->DataSource->LoadOrNull<DataSetData>(obj->DataSet)->DataSetID;
            received << *dot::string::format("Key={0} DataSet={1} Version={2}", obj->getKey(), dataSetID, obj->Version) << std::endl;
        }

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("Delete")
    {
        MongoDataSourceTest test = new MongoDataSourceTestImpl;
        IUnitTestContext context = new UnitTestContextImpl(test, "Delete", ".");

        SaveBasicData(context);

        // Get dataset identifiers
        ObjectId dataSetA = context->GetDataSet("A", context->DataSet);
        ObjectId dataSetB = context->GetDataSet("B", context->DataSet);

        // Create keys
        MongoTestKey keyA0 = new MongoTestKeyImpl();
        keyA0->RecordID = "A";
        keyA0->RecordIndex = dot::nullable<int>(0);

        MongoTestKey keyB0 = new MongoTestKeyImpl();
        keyB0->RecordID = "B";
        keyB0->RecordIndex = dot::nullable<int>(0);

        // Verify the result of loading records from datasets A and B
        received << "Initial load" << std::endl;
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "A");
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "B");
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "A");
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "B");

        received << "Query in dataset A" << std::endl;
        VerifyQuery<MongoTestData>(context, "A");
        received << "Query in dataset B" << std::endl;
        VerifyQuery<MongoTestData>(context, "B");

        received << "Delete A0 record in B dataset" << std::endl;
        context->Delete(keyA0, dataSetB);
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "A");
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "B");

        received << "Query in dataset A" << std::endl;
        VerifyQuery<MongoTestData>(context, "A");
        received << "Query in dataset B" << std::endl;
        VerifyQuery<MongoTestData>(context, "B");

        received << "Delete A0 record in A dataset" << std::endl;
        context->Delete(keyA0, dataSetA);
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "A");
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "B");

        received << "Query in dataset A" << std::endl;
        VerifyQuery<MongoTestData>(context, "A");
        received << "Query in dataset B" << std::endl;
        VerifyQuery<MongoTestData>(context, "B");

        received << "Delete B0 record in B dataset" << std::endl;
        context->Delete(keyB0, dataSetB);
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "A");
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "B");

        received << "Query in dataset A" << std::endl;
        VerifyQuery<MongoTestData>(context, "A");
        received << "Query in dataset B" << std::endl;
        VerifyQuery<MongoTestData>(context, "B");

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    /// <summary>
    /// Test saving object of a different type for the same key.
    ///
    /// The objective of this test is to confirm that LoadOrNull
    /// for the wrong type will give an error, while query will
    /// skip the object of the wrong type even if there is an
    /// earlier version of the object with the same key that
    /// has a compatible type.
    /// </summary>
    TEST_CASE("TypeChange")
    {
        MongoDataSourceTest test = new MongoDataSourceTestImpl;
        IUnitTestContext context = new UnitTestContextImpl(test, "TypeChange", ".");

        // Create datasets
        ObjectId dataSetA = context->CreateDataSet("A", context->DataSet);
        ObjectId dataSetB = context->CreateDataSet("B", dot::new_List<ObjectId>({ dataSetA }), context->DataSet);

        // Create records with minimal data
        SaveDerivedRecord(context, "A", "A", 0);
        SaveDerivedFromDerivedRecord(context, "B", "B", 0);

        // Create keys
        MongoTestKey keyA0 = new_MongoTestKey();
        keyA0->RecordID = "A";
        keyA0->RecordIndex = dot::nullable<int>(0);

        MongoTestKey keyB0 = new_MongoTestKey();
        keyB0->RecordID = "B";
        keyB0->RecordIndex = dot::nullable<int>(0);

        // Verify the result of loading records from datasets A and B
        received << "Initial load" << std::endl;
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "A");
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "B");
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "A");
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "B");

        received << "Query in dataset A for type MongoTestDerivedData" << std::endl;
        VerifyQuery<MongoTestDerivedData>(context, "A");
        received << "Query in dataset B for type MongoTestDerivedData" << std::endl;
        VerifyQuery<MongoTestDerivedData>(context, "B");

        received << "Change A0 record type in B dataset to C" << std::endl;
        SaveOtherDerivedRecord(context, "B", "A", 0);
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "A");
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "B");

        received << "Query in dataset A for type MongoTestDerivedData" << std::endl;
        VerifyQuery<MongoTestDerivedData>(context, "A");
        received << "Query in dataset B for type MongoTestDerivedData" << std::endl;
        VerifyQuery<MongoTestDerivedData>(context, "B");

        received << "Change A0 record type in A dataset to C" << std::endl;
        SaveOtherDerivedRecord(context, "A", "A", 0);
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "A");
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyA0, "B");

        received << "Query in dataset A for type MongoTestDerivedData" << std::endl;
        VerifyQuery<MongoTestDerivedData>(context, "A");
        received << "Query in dataset B for type MongoTestDerivedData" << std::endl;
        VerifyQuery<MongoTestDerivedData>(context, "B");

        received << "Change B0 record type in B dataset to C" << std::endl;
        SaveOtherDerivedRecord(context, "B", "B", 0);
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "A");
        VerifyLoad(context, (KeyFor<MongoTestKeyImpl, MongoTestDataImpl>) keyB0, "B");

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
        MongoDataSourceTest test = new MongoDataSourceTestImpl;
        IUnitTestContext context = new UnitTestContextImpl(test, "ElementTypesQuery", ".");

        // Saves data in A and B datasets, A is parent of B
        SaveCompleteData(context);

        // Look in B dataset
        ObjectId dataSetB = context->GetDataSetOrEmpty("B", context->DataSet);

        MongoTestKey key = new_MongoTestKey();
        key->RecordID = "BB";
        key->RecordIndex = dot::nullable<int>(2);

        dot::IEnumerable<MongoTestDerivedData> testQuery = context->DataSource->GetQuery<MongoTestDerivedData>(dataSetB)
            ->Where(make_prop(&MongoTestDerivedDataImpl::DataElementList)[0]->*make_prop(&ElementSampleDataImpl::DoubleElement3) == 1.0)
            ->Where(make_prop(&MongoTestDerivedDataImpl::DataElementList)[0]->*make_prop(&ElementSampleDataImpl::StringElement3) == "A0")
            ->Where(make_prop(&MongoTestDerivedDataImpl::local_dateElement) < dot::local_date(2003, 5, 2))
            ->Where(make_prop(&MongoTestDerivedDataImpl::local_dateElement) > dot::local_date(2003, 4, 30))
            ->Where(make_prop(&MongoTestDerivedDataImpl::local_dateElement) == dot::local_date(2003, 5, 1))
            ->Where(make_prop(&MongoTestDerivedDataImpl::local_timeElement) < dot::local_time(10, 15, 31))
            ->Where(make_prop(&MongoTestDerivedDataImpl::local_timeElement) > dot::local_time(10, 15, 29))
            ->Where(make_prop(&MongoTestDerivedDataImpl::local_timeElement) == dot::local_time(10, 15, 30))
            ->Where(make_prop(&MongoTestDerivedDataImpl::local_date_timeElement) < dot::local_date_time(2003, 5, 1, 10, 15, 01))
            ->Where(make_prop(&MongoTestDerivedDataImpl::local_date_timeElement) > dot::local_date_time(2003, 5, 1, 10, 14, 59))
            ->Where(make_prop(&MongoTestDerivedDataImpl::local_date_timeElement) == dot::local_date_time(2003, 5, 1, 10, 15))
            ->Where(make_prop(&MongoTestDerivedDataImpl::StringElement2) == dot::string::Empty)
            ->Where(make_prop(&MongoTestDerivedDataImpl::KeyElement) == key->ToString())
            ->AsEnumerable<MongoTestDerivedData>();

        for (MongoTestData obj : testQuery)
        {
            received << *dot::string::format("Key={0} Type={1}", obj->getKey(), obj->type()->name) << std::endl;
        }

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("PolymorphicQuery")
    {
        MongoDataSourceTest test = new MongoDataSourceTestImpl;
        IUnitTestContext context = new UnitTestContextImpl(test, "PolymorphicQuery", ".");

        // Saves data in A and B datasets, A is parent of B
        SaveCompleteData(context);

        // Look in B dataset
        ObjectId dataSetD = context->GetDataSetOrEmpty("D", context->DataSet);

        // Load record of derived types by base
        received << "Load all types by key to type A" << std::endl;
        {
            MongoTestKey key = new_MongoTestKey();
            key->RecordID = "A";
            key->RecordIndex = dot::nullable<int>(0);
            record_type obj = key->LoadOrNull(context, dataSetD);
            received << *dot::string::format("    Key={0} Type={1}", obj->getKey(), obj->type()->name) << std::endl;
        }
        {
            MongoTestKey key = new_MongoTestKey();
            key->RecordID = "B";
            key->RecordIndex = dot::nullable<int>(0);
            record_type obj = key->LoadOrNull(context, dataSetD);
            received << *dot::string::format("    Key={0} Type={1}", obj->getKey(), obj->type()->name) << std::endl;
        }
        {
            MongoTestKey key = new_MongoTestKey();
            key->RecordID = "C";
            key->RecordIndex = dot::nullable<int>(0);
            record_type obj = key->LoadOrNull(context, dataSetD);
            received << *dot::string::format("    Key={0} Type={1}", obj->getKey(), obj->type()->name) << std::endl;
        }
        {
            MongoTestKey key = new_MongoTestKey();
            key->RecordID = "D";
            key->RecordIndex = dot::nullable<int>(0);
            record_type obj = key->LoadOrNull(context, dataSetD);
            received << *dot::string::format("    Key={0} Type={1}", obj->getKey(), obj->type()->name) << std::endl;
        }
        {
            received << "Query by MongoTestData, unconstrained" << std::endl;
            IQuery query = context->DataSource->GetQuery<MongoTestData>(dataSetD);
            for (record_type obj : query->AsEnumerable<record_type>())
            {
                received << *dot::string::format("    Key={0} Type={1}", obj->getKey(), obj->type()->name) << std::endl;
            }
        }
        {
            received << "Query by MongoTestDerivedData : MongoTestData which also picks up MongoTestDerivedFromDerivedData : MongoTestDerivedData, unconstrained" << std::endl;
            IQuery query = context->DataSource->GetQuery<MongoTestDerivedData>(dataSetD);
            for (record_type obj : query->AsEnumerable<record_type>())
            {
                received << *dot::string::format("    Key={0} Type={1}", obj->getKey(), obj->type()->name) << std::endl;
            }
        }
        {
            received << "Query by MongoTestOtherDerivedData : MongoTestData, unconstrained" << std::endl;
            MongoTestOtherDerivedDataImpl::typeof();
            IQuery query = context->DataSource->GetQuery<MongoTestOtherDerivedData>(dataSetD);
            for (record_type obj : query->AsEnumerable<record_type>())
            {
                received << *dot::string::format("    Key={0} Type={1}", obj->getKey(), obj->type()->name) << std::endl;
            }
        }
        {
            received << "Query by MongoTestDerivedFromDerivedData : MongoTestDerivedData, where MongoTestDerivedData : MongoTestData, unconstrained" << std::endl;
            IQuery query = context->DataSource->GetQuery<MongoTestDerivedFromDerivedData>(dataSetD);
            for (record_type obj : query->AsEnumerable<record_type>())
            {
                received << *dot::string::format("    Key={0} Type={1}", obj->getKey(), obj->type()->name) << std::endl;
            }
        }

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("Sort")
    {
        MongoDataSourceTest test = new MongoDataSourceTestImpl;
        IUnitTestContext context = new UnitTestContextImpl(test, "Sort", ".");

        // Saves data in A and B datasets, A is parent of B
        SaveCompleteData(context);

        // Look in B dataset
        ObjectId dataSetD = context->GetDataSetOrEmpty("D", context->DataSet);

        received << "Query by MongoTestData, sort by RecordIndex descending, then by DoubleElement ascending" << std::endl;
        dot::IEnumerable<MongoTestData> baseQuery = context->DataSource->GetQuery<MongoTestData>(dataSetD)
            ->SortByDescending(make_prop(&MongoTestDataImpl::RecordIndex))
            ->SortBy(make_prop(&MongoTestDataImpl::DoubleElement))
            ->AsEnumerable<MongoTestData>();

        for (MongoTestData obj : baseQuery)
        {
            received
                << *dot::string::format(
                    "    RecordIndex={0} DoubleElement={1} Key={2} Type={3}",
                    obj->RecordIndex, obj->DoubleElement, obj->getKey(), obj->type()->name)
                << std::endl;
        }

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }

    TEST_CASE("RevisionTime")
    {
        MongoDataSourceTest test = new MongoDataSourceTestImpl;
        IUnitTestContext context = new UnitTestContextImpl(test, "RevisionTime", ".");

        // Create datasets
        ObjectId dataSetA = context->CreateDataSet("A", context->DataSet);
        ObjectId dataSetB = context->CreateDataSet("B", dot::new_List<ObjectId>({ dataSetA }), context->DataSet);

        // Create initial version of the records
        ObjectId objA0 = SaveMinimalRecord(context, "A", "A", 0, 0);
        ObjectId objB0 = SaveMinimalRecord(context, "B", "B", 0, 0);

        // Create second version of the records
        ObjectId objA1 = SaveMinimalRecord(context, "A", "A", 0, 1);
        ObjectId objB1 = SaveMinimalRecord(context, "B", "B", 0, 1);

        ObjectId cutoffObjectId = context->DataSource->CreateOrderedObjectId();

        // Create third version of the records
        ObjectId objA2 = SaveMinimalRecord(context, "A", "A", 0, 2);
        ObjectId objB2 = SaveMinimalRecord(context, "B", "B", 0, 2);

        // Create new records that did not exist before
        ObjectId objC0 = SaveMinimalRecord(context, "A", "C", 0, 0);
        ObjectId objD0 = SaveMinimalRecord(context, "B", "D", 0, 0);

        received << "Load records by ObjectId without constraint" << std::endl;
        received << *dot::string::format("    Found by ObjectId=A0 = {0}", context->LoadOrNull<MongoTestData>(objA0) != nullptr) << std::endl;
        received << *dot::string::format("    Found by ObjectId=A1 = {0}", context->LoadOrNull<MongoTestData>(objA1) != nullptr) << std::endl;
        received << *dot::string::format("    Found by ObjectId=A2 = {0}", context->LoadOrNull<MongoTestData>(objA2) != nullptr) << std::endl;
        received << *dot::string::format("    Found by ObjectId=C0 = {0}", context->LoadOrNull<MongoTestData>(objC0) != nullptr) << std::endl;

        // Load each record by string key
        {
            MongoTestKey loadedA0Key = new_MongoTestKey();
            loadedA0Key->RecordID = "A";
            loadedA0Key->RecordIndex = dot::nullable<int>(0);
            MongoTestData loadedA0 = loadedA0Key->LoadOrNull(context, dataSetB);

            MongoTestKey loadedC0Key = new_MongoTestKey();
            loadedC0Key->RecordID = "C";
            loadedC0Key->RecordIndex = dot::nullable<int>(0);
            MongoTestData loadedC0 = loadedC0Key->LoadOrNull(context, dataSetB);

            received << "Load records by string key without constraint" << std::endl;
            if (loadedA0 != nullptr) received << *dot::string::format("    Version found for key=A;0: {0}", loadedA0->Version) << std::endl;
            if (loadedC0 != nullptr) received << *dot::string::format("    Version found for key=C;0: {0}", loadedC0->Version) << std::endl;
        }

        // Query for all records
        {
            dot::IEnumerable<MongoTestData> query = context->DataSource->GetQuery<MongoTestData>(dataSetB)
                ->SortBy(make_prop(&MongoTestDataImpl::RecordID))
                ->SortBy(make_prop(&MongoTestDataImpl::RecordIndex))
                ->AsEnumerable<MongoTestData>();

            received << "Query records without constraint" << std::endl;
            for (MongoTestData obj : query)
            {
                dot::string dataSetID = context->LoadOrNull<DataSetData>(obj->DataSet)->DataSetID;
                received << *dot::string::format("    Key={0} DataSet={1} Version={2}", obj->getKey(), dataSetID, obj->Version) << std::endl;
            }
        }

        // Set new value for the DB server key so that the reference to in-memory
        // record is stored inside the key, otherwise it would not be found
        context->DataSource->DbServer = new_MongoDefaultServerData()->ToKey();

        // Set revision time constraint
        context->DataSource->RevisedBeforeId = cutoffObjectId;

        // Get each record by ObjectId
        received << "Load records by ObjectId with RevisedBeforeId constraint" << std::endl;
        received << *dot::string::format("    Found by ObjectId=A0 = {0}", context->LoadOrNull<MongoTestData>(objA0) != nullptr) << std::endl;
        received << *dot::string::format("    Found by ObjectId=A1 = {0}", context->LoadOrNull<MongoTestData>(objA1) != nullptr) << std::endl;
        received << *dot::string::format("    Found by ObjectId=A2 = {0}", context->LoadOrNull<MongoTestData>(objA2) != nullptr) << std::endl;
        received << *dot::string::format("    Found by ObjectId=C0 = {0}", context->LoadOrNull<MongoTestData>(objC0) != nullptr) << std::endl;

        // Load each record by string key
        {
            MongoTestKey loadedA0Key = new_MongoTestKey();
            loadedA0Key->RecordID = "A";
            loadedA0Key->RecordIndex = dot::nullable<int>(0);
            MongoTestData loadedA0 = loadedA0Key->LoadOrNull(context, dataSetB);

            MongoTestKey loadedC0Key = new_MongoTestKey();
            loadedA0Key->RecordID = "C";
            loadedA0Key->RecordIndex = dot::nullable<int>(0);
            MongoTestData loadedC0 = loadedC0Key->LoadOrNull(context, dataSetB);

            received << "Load records by string key with RevisedBeforeId constraint" << std::endl;
            if (loadedA0 != nullptr) received << *dot::string::format("    Version found for key=A;0: {0}", loadedA0->Version) << std::endl;
            if (loadedC0 != nullptr) received << *dot::string::format("    Version found for key=C;0: {0}", loadedC0->Version) << std::endl;
        }

        // Query for revised before the cutoff time
        {
            dot::IEnumerable<MongoTestData> query = context->DataSource->GetQuery<MongoTestData>(dataSetB)
                ->SortBy(make_prop(&MongoTestDataImpl::RecordID))
                ->SortBy(make_prop(&MongoTestDataImpl::RecordIndex))
                ->AsEnumerable<MongoTestData>();

            received << "Query records with RevisedBeforeId constraint" << std::endl;
            for (MongoTestData obj : query)
            {
                dot::string dataSetID = context->LoadOrNull<DataSetData>(obj->DataSet)->DataSetID;
                received << *dot::string::format("    Key={0} DataSet={1} Version={2}", obj->getKey(), dataSetID, obj->Version) << std::endl;
            }
        }

        // Clear revision time constraint before exiting to avoid an error
        // about deleting readonly database. The error occurs because
        // revision time constraint makes the data source readonly.
        context->DataSource->RevisedBeforeId = dot::nullable<ObjectId>();

        std::string toVerify = received.str();
        received.str("");
        Approvals::verify(toVerify);
    }
}
