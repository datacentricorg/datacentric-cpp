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

#include <dc/types/record/record.hpp>

#include <chrono>

namespace dc
{
    // CONSTANTS
    const int data_set_count = 10;
    const int recordVersions = 10;

    const int record_count = 1024 * 3 / (data_set_count * recordVersions);
    const int recordSize = 128 * 200;


    // TEST CLASSES

    /// This class counts test running time.
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

        DOT_TYPE_BEGIN("dc", "PerformanceTest")
        DOT_TYPE_END()
    };

    class PerformanceTestKeyImpl; using PerformanceTestKey = dot::ptr<PerformanceTestKeyImpl>;
    class PerformanceTestDataImpl; using PerformanceTestData = dot::ptr<PerformanceTestDataImpl>;

    PerformanceTestKey make_PerformanceTestKey();

    /// Key class.
    class PerformanceTestKeyImpl : public key_impl<PerformanceTestKeyImpl, PerformanceTestDataImpl>
    {
        typedef PerformanceTestKeyImpl self;

    public:

        dot::string RecordID;

        DOT_TYPE_BEGIN("dc", "PerformanceTestKey")
            DOT_TYPE_PROP(RecordID)
            DOT_TYPE_BASE(key<PerformanceTestKeyImpl, PerformanceTestDataImpl>)
            DOT_TYPE_CTOR(make_PerformanceTestKey)
        DOT_TYPE_END()
    };

    inline PerformanceTestKey make_PerformanceTestKey() { return new PerformanceTestKeyImpl; }

    PerformanceTestData make_PerformanceTestData();

    /// Data class.
    class PerformanceTestDataImpl : public record_impl<PerformanceTestKeyImpl, PerformanceTestDataImpl>
    {
        typedef PerformanceTestDataImpl self;

    public:

        dot::string RecordID;
        dot::list<double> DoubleList;
        dot::nullable<int> Version;

        DOT_TYPE_BEGIN("dc", "PerformanceTestData")
            DOT_TYPE_PROP(RecordID)
            DOT_TYPE_PROP(DoubleList)
            DOT_TYPE_PROP(Version)
            DOT_TYPE_BASE(record<PerformanceTestKeyImpl, PerformanceTestDataImpl>)
            DOT_TYPE_CTOR(make_PerformanceTestData)
        DOT_TYPE_END()
    };

    inline PerformanceTestData make_PerformanceTestData() { return new PerformanceTestDataImpl; }


    // HELPER FUNCTIONS
    dot::string GetRecordKey(int index)
    {
        static const dot::string recordIdPattern = "Key{0}";
        return dot::string::format(recordIdPattern, index);
    }

    dot::string get_data_set(int index)
    {
        static const dot::string dataSetPattern = "DS{0}";
        return dot::string::format(dataSetPattern, index);
    }

    dot::object_id SaveRecord(unit_test_context_base context, dot::string dataSetID, dot::string recordId, int recordSize, int recordVersion)
    {
        PerformanceTestData rec = make_PerformanceTestData();
        rec->RecordID = recordId;
        rec->Version = recordVersion;
        rec->DoubleList = dot::make_list<double>();
        rec->DoubleList->set_capacity(recordSize);

        for (int i = 0; i < recordSize; ++i)
            rec->DoubleList->add(std::rand());

        dot::object_id dataSet = context->get_data_set(dataSetID);
        context->save(rec, dataSet);
        return rec->ID;
    }

    void SaveRecords(unit_test_context_base context, int record_count, int recordSize)
    {
        // Create datasets
        dot::object_id common_data_set = context->get_common();
        for (int i = 0; i < data_set_count; ++i)
        {
            dot::string data_set_name = get_data_set(i);
            context->create_data_set(data_set_name, dot::make_list<dot::object_id>({ common_data_set }));
        }

        // Create records
        for (int i = 0; i < record_count; ++i)
        {
            dot::string recordId = GetRecordKey(i);
            SaveRecord(context, data_set_key_impl::common->data_set_id, recordId, recordSize, 0);

            for (int data_set_index = 0; data_set_index < data_set_count; ++data_set_index)
            {
                dot::string data_set_name = get_data_set(data_set_index);

                for (int version = 1; version < recordVersions; ++version)
                {
                    SaveRecord(context, data_set_name, recordId, recordSize, version);
                }
            }
        }
    }

    void FillDatabase(unit_test_context_base context)
    {
        SaveRecords(context, record_count, recordSize);
    }


    TEST_CASE("Performance")
    {
        PerformanceTest test = new PerformanceTestImpl;
        unit_test_context_base context = new unit_test_context_impl(test, "Performance", ".");

        //FillDatabase(context);
    }

    TEST_CASE("LoadByKey")
    {
        PerformanceTest test = new PerformanceTestImpl;
        unit_test_context_base context = new unit_test_context_impl(test, "Performance", ".");
        TestDurationCounter td("Keys loading");

        for (int i = 0; i < record_count; ++i)
        {
            PerformanceTestKey key = make_PerformanceTestKey();
            key->RecordID = GetRecordKey(i);
            context->reload_or_null(key, context->get_common());

            for (int data_set_index = 0; data_set_index < data_set_count; ++data_set_index)
            {
                dot::string data_set_name = get_data_set(data_set_index);
                dot::object_id dataSet = context->get_data_set_or_empty(data_set_name);
                context->reload_or_null(key, dataSet);
            }
        }
    }

    TEST_CASE("Query")
    {
        PerformanceTest test = new PerformanceTestImpl;
        unit_test_context_base context = new unit_test_context_impl(test, "Performance", ".");
        TestDurationCounter td("Query loading");

        auto startTime = std::chrono::steady_clock::now();
        dot::string recordID = GetRecordKey(2);
        dot::string data_set_name = get_data_set(2);
        dot::object_id dataSet = context->get_data_set_or_empty(data_set_name);

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
