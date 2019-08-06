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

#include <dc/platform/data_source/DataSourceData.hpp>
#include <dc/platform/data_source/mongo/QueryBuilder.hpp>
#include <dc/platform/data_source/mongo/MongoDefaultServerData.hpp>

#include <dc/platform/dataset/DataSetKey.hpp>
#include <dc/platform/dataset/DataSetData.hpp>

#include <dc/platform/context/Context.hpp>
#include <dc/test/platform/context/Context.hpp>

#include <dc/types/record/record.hpp>

#include <chrono>

namespace dc
{
    // CONSTANTS
    const int dataSetsCount = 10;
    const int recordVersions = 10;

    const int recordsCount = 1024 * 3 / (dataSetsCount * recordVersions);
    const int recordSize = 128 * 200;


    // TEST CLASSES

    /// <summary>This class counts test running time.</summary>
    class TestDurationCounter
    {
        std::chrono::steady_clock::time_point startTime;
        dot::string message;

    public:

        TestDurationCounter(dot::string msg)
            : startTime(std::chrono::steady_clock::now())
            , message(msg)
        {}

        ~TestDurationCounter()
        {
            auto endTime = std::chrono::steady_clock::now();
            std::cout << *message << " "
                << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()
                << "ms" << std::endl;
        }
    };

    class PerformanceTestImpl; using PerformanceTest = dot::ptr<PerformanceTestImpl>;

    class PerformanceTestImpl : public dot::object_impl
    {
        typedef PerformanceTestImpl self;

    public:

        DOT_TYPE_BEGIN("DataCentric", "PerformanceTest")
        DOT_TYPE_END()
    };

    class PerformanceTestKeyImpl; using PerformanceTestKey = dot::ptr<PerformanceTestKeyImpl>;
    class PerformanceTestDataImpl; using PerformanceTestData = dot::ptr<PerformanceTestDataImpl>;

    PerformanceTestKey new_PerformanceTestKey();

    /// <summary>Key class.</summary>
    class PerformanceTestKeyImpl : public key_impl<PerformanceTestKeyImpl, PerformanceTestDataImpl>
    {
        typedef PerformanceTestKeyImpl self;

    public:

        dot::string RecordID;

        DOT_TYPE_BEGIN("DataCentric", "PerformanceTestKey")
            DOT_TYPE_PROP(RecordID)
            DOT_TYPE_BASE(key<PerformanceTestKeyImpl, PerformanceTestDataImpl>)
            DOT_TYPE_CTOR(new_PerformanceTestKey)
        DOT_TYPE_END()
    };

    inline PerformanceTestKey new_PerformanceTestKey() { return new PerformanceTestKeyImpl; }

    PerformanceTestData new_PerformanceTestData();

    /// <summary>Data class.</summary>
    class PerformanceTestDataImpl : public record_impl<PerformanceTestKeyImpl, PerformanceTestDataImpl>
    {
        typedef PerformanceTestDataImpl self;

    public:

        dot::string RecordID;
        dot::list<double> DoubleList;
        dot::nullable<int> Version;

        DOT_TYPE_BEGIN("DataCentric", "PerformanceTestData")
            DOT_TYPE_PROP(RecordID)
            DOT_TYPE_PROP(DoubleList)
            DOT_TYPE_PROP(Version)
            DOT_TYPE_BASE(record<PerformanceTestKeyImpl, PerformanceTestDataImpl>)
            DOT_TYPE_CTOR(new_PerformanceTestData)
        DOT_TYPE_END()
    };

    inline PerformanceTestData new_PerformanceTestData() { return new PerformanceTestDataImpl; }


    // HELPER FUNCTIONS
    dot::string GetRecordKey(int index)
    {
        static const dot::string recordIdPattern = "Key{0}";
        return dot::string::format(recordIdPattern, index);
    }

    dot::string GetDataSet(int index)
    {
        static const dot::string dataSetPattern = "DS{0}";
        return dot::string::format(dataSetPattern, index);
    }

    ObjectId SaveRecord(IUnitTestContext context, dot::string dataSetID, dot::string recordId, int recordSize, int recordVersion)
    {
        PerformanceTestData rec = new_PerformanceTestData();
        rec->RecordID = recordId;
        rec->Version = recordVersion;
        rec->DoubleList = dot::make_list<double>();
        rec->DoubleList->set_capacity(recordSize);

        for (int i = 0; i < recordSize; ++i)
            rec->DoubleList->add(std::rand());

        ObjectId dataSet = context->GetDataSet(dataSetID);
        context->Save(rec, dataSet);
        return rec->ID;
    }

    void SaveRecords(IUnitTestContext context, int recordsCount, int recordSize)
    {
        // Create datasets
        ObjectId commonDataSet = context->GetCommon();
        for (int i = 0; i < dataSetsCount; ++i)
        {
            dot::string dataSetName = GetDataSet(i);
            context->CreateDataSet(dataSetName, dot::make_list<ObjectId>({ commonDataSet }));
        }

        // Create records
        for (int i = 0; i < recordsCount; ++i)
        {
            dot::string recordId = GetRecordKey(i);
            SaveRecord(context, DataSetKeyImpl::Common->DataSetID, recordId, recordSize, 0);

            for (int dsI = 0; dsI < dataSetsCount; ++dsI)
            {
                dot::string dataSetName = GetDataSet(dsI);

                for (int version = 1; version < recordVersions; ++version)
                {
                    SaveRecord(context, dataSetName, recordId, recordSize, version);
                }
            }
        }
    }

    void FillDatabase(IUnitTestContext context)
    {
        SaveRecords(context, recordsCount, recordSize);
    }


    TEST_CASE("Performance")
    {
        PerformanceTest test = new PerformanceTestImpl;
        IUnitTestContext context = new UnitTestContextImpl(test, "Performance", ".");

        //FillDatabase(context);
    }

    TEST_CASE("LoadByKey")
    {
        PerformanceTest test = new PerformanceTestImpl;
        IUnitTestContext context = new UnitTestContextImpl(test, "Performance", ".");
        TestDurationCounter td("Keys loading");

        for (int i = 0; i < recordsCount; ++i)
        {
            PerformanceTestKey key = new_PerformanceTestKey();
            key->RecordID = GetRecordKey(i);
            context->ReloadOrNull(key, context->GetCommon());

            for (int dsI = 0; dsI < dataSetsCount; ++dsI)
            {
                dot::string dataSetName = GetDataSet(dsI);
                ObjectId dataSet = context->GetDataSetOrEmpty(dataSetName);
                context->ReloadOrNull(key, dataSet);
            }
        }
    }

    TEST_CASE("Query")
    {
        PerformanceTest test = new PerformanceTestImpl;
        IUnitTestContext context = new UnitTestContextImpl(test, "Performance", ".");
        TestDurationCounter td("Query loading");

        auto startTime = std::chrono::steady_clock::now();
        dot::string recordID = GetRecordKey(2);
        dot::string dataSetName = GetDataSet(2);
        ObjectId dataSet = context->GetDataSetOrEmpty(dataSetName);

        dc::cursor_wrapper<PerformanceTestData> query = context->DataSource->get_query<PerformanceTestData>(dataSet)
      // TODO - fix compilation      ->Where(make_prop(&PerformanceTestDataImpl::Key) == recordID)
            //->Where(make_prop(&PerformanceTestDataImpl::Version) == recordVersions - 1)
            ->get_cursor<PerformanceTestData>();

        for (PerformanceTestData data : query)
        {
            std::cout << *data->to_string() << std::endl;
        }
    }
}
