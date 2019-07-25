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

#include <dc/types/record/RecordFor.hpp>

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
        dot::String message;

    public:

        TestDurationCounter(dot::String msg)
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

    class PerformanceTestImpl; using PerformanceTest = dot::Ptr<PerformanceTestImpl>;

    class PerformanceTestImpl : public dot::ObjectImpl
    {
        typedef PerformanceTestImpl self;

    public:

        DOT_TYPE_BEGIN(".Runtime.Test", "PerformanceTest")
        DOT_TYPE_END()
    };

    class PerformanceTestKeyImpl; using PerformanceTestKey = dot::Ptr<PerformanceTestKeyImpl>;
    class PerformanceTestDataImpl; using PerformanceTestData = dot::Ptr<PerformanceTestDataImpl>;

    PerformanceTestKey new_PerformanceTestKey();

    /// <summary>Key class.</summary>
    class PerformanceTestKeyImpl : public KeyForImpl<PerformanceTestKeyImpl, PerformanceTestDataImpl>
    {
        typedef PerformanceTestKeyImpl self;

    public:

        dot::String RecordID;

        DOT_TYPE_BEGIN(".Runtime.Test", "PerformanceTestKey")
            DOT_TYPE_PROP(RecordID)
            DOT_TYPE_BASE(KeyFor<PerformanceTestKeyImpl, PerformanceTestDataImpl>)
            DOT_TYPE_CTOR(new_PerformanceTestKey)
        DOT_TYPE_END()
    };

    inline PerformanceTestKey new_PerformanceTestKey() { return new PerformanceTestKeyImpl; }

    PerformanceTestData new_PerformanceTestData();

    /// <summary>Data class.</summary>
    class PerformanceTestDataImpl : public RecordForImpl<PerformanceTestKeyImpl, PerformanceTestDataImpl>
    {
        typedef PerformanceTestDataImpl self;

    public:

        dot::String RecordID;
        dot::List<double> DoubleList;
        dot::Nullable<int> Version;

        DOT_TYPE_BEGIN(".Runtime.Test", "PerformanceTestData")
            DOT_TYPE_PROP(RecordID)
            DOT_TYPE_PROP(DoubleList)
            DOT_TYPE_PROP(Version)
            DOT_TYPE_BASE(RecordFor<PerformanceTestKeyImpl, PerformanceTestDataImpl>)
            DOT_TYPE_CTOR(new_PerformanceTestData)
        DOT_TYPE_END()
    };

    inline PerformanceTestData new_PerformanceTestData() { return new PerformanceTestDataImpl; }


    // HELPER FUNCTIONS
    dot::String GetRecordKey(int index)
    {
        static const dot::String recordIdPattern = "Key{0}";
        return dot::String::Format(recordIdPattern, index);
    }

    dot::String GetDataSet(int index)
    {
        static const dot::String dataSetPattern = "DS{0}";
        return dot::String::Format(dataSetPattern, index);
    }

    ObjectId SaveRecord(IUnitTestContext context, dot::String dataSetID, dot::String recordId, int recordSize, int recordVersion)
    {
        PerformanceTestData rec = new_PerformanceTestData();
        rec->RecordID = recordId;
        rec->Version = recordVersion;
        rec->DoubleList = dot::new_List<double>();
        rec->DoubleList->Capacity = recordSize;

        for (int i = 0; i < recordSize; ++i)
            rec->DoubleList->Add(std::rand());

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
            dot::String dataSetName = GetDataSet(i);
            context->CreateDataSet(dataSetName, dot::new_List<ObjectId>({ commonDataSet }));
        }

        // Create records
        for (int i = 0; i < recordsCount; ++i)
        {
            dot::String recordId = GetRecordKey(i);
            SaveRecord(context, DataSetKeyImpl::Common->DataSetID, recordId, recordSize, 0);

            for (int dsI = 0; dsI < dataSetsCount; ++dsI)
            {
                dot::String dataSetName = GetDataSet(dsI);

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
                dot::String dataSetName = GetDataSet(dsI);
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
        dot::String recordID = GetRecordKey(2);
        dot::String dataSetName = GetDataSet(2);
        ObjectId dataSet = context->GetDataSetOrEmpty(dataSetName);

        dot::IEnumerable<PerformanceTestData> query = context->DataSource->GetQuery<PerformanceTestData>(dataSet)
      // TODO - fix compilation      ->Where(make_prop(&PerformanceTestDataImpl::Key) == recordID)
            //->Where(make_prop(&PerformanceTestDataImpl::Version) == recordVersions - 1)
            ->AsEnumerable<PerformanceTestData>();

        for (PerformanceTestData data : query)
        {
            std::cout << *data->ToString() << std::endl;
        }
    }
}
