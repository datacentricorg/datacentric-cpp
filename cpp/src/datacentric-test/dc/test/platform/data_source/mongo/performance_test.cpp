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

#include <dc/platform/data_source/data_source_data.hpp>
#include <dc/platform/data_source/mongo/mongo_query.hpp>

#include <dc/platform/data_set/data_set_key.hpp>
#include <dc/platform/data_set/data_set_data.hpp>

#include <dc/test/platform/context/context.hpp>

#include <dc/types/record/typed_record.hpp>

#include <dot/system/console.hpp>
#include <chrono>

namespace dc
{
    // CONSTANTS
    const int data_set_count = 3; // 10;
    const int record_versions = 3; // 10;

    const int record_count = 10; // 1024 * 3 / (data_set_count * record_versions);
    const int record_size = 10; // 128 * 200;


    // TEST CLASSES

    /// This class counts test running time.
    class TestDurationCounter
    {
        std::chrono::steady_clock::time_point start_time;
        dot::String message;

    public:

        TestDurationCounter(dot::String msg)
            : start_time(std::chrono::steady_clock::now())
            , message(msg)
        {}

        ~TestDurationCounter()
        {
            auto end_time = std::chrono::steady_clock::now();
            std::cout << *message << " "
                << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
                << "ms" << std::endl;
        }
    };

    class PerformanceTestImpl; using PerformanceTest = dot::Ptr<PerformanceTestImpl>;

    class PerformanceTestImpl : public dot::ObjectImpl
    {
        typedef PerformanceTestImpl self;

    public:

        DOT_TYPE_BEGIN("dc", "PerformanceTest")
        DOT_TYPE_END()
    };

    class PerformanceTestKeyImpl; using PerformanceTestKey = dot::Ptr<PerformanceTestKeyImpl>;
    class PerformanceTestDataImpl; using PerformanceTestData = dot::Ptr<PerformanceTestDataImpl>;

    PerformanceTestKey make_performance_test_key();

    /// Key class.
    class PerformanceTestKeyImpl : public TypedKeyImpl<PerformanceTestKeyImpl, PerformanceTestDataImpl>
    {
        typedef PerformanceTestKeyImpl self;

    public:

        dot::String record_id;

        DOT_TYPE_BEGIN("dc", "PerformanceTestKey")
            DOT_TYPE_PROP(record_id)
            DOT_TYPE_BASE(TypedKey<PerformanceTestKeyImpl, PerformanceTestDataImpl>)
            DOT_TYPE_CTOR(make_performance_test_key)
        DOT_TYPE_END()
    };

    inline PerformanceTestKey make_performance_test_key() { return new PerformanceTestKeyImpl; }

    PerformanceTestData make_performance_test_data();

    /// Data class.
    class PerformanceTestDataImpl : public TypedRecordImpl<PerformanceTestKeyImpl, PerformanceTestDataImpl>
    {
        typedef PerformanceTestDataImpl self;

    public:

        dot::String record_id;
        dot::List<double> double_list;
        dot::Nullable<int> version;

        DOT_TYPE_BEGIN("dc", "PerformanceTestData")
            DOT_TYPE_PROP(record_id)
            DOT_TYPE_PROP(double_list)
            DOT_TYPE_PROP(version)
            DOT_TYPE_BASE(TypedRecord<PerformanceTestKeyImpl, PerformanceTestDataImpl>)
            DOT_TYPE_CTOR(make_performance_test_data)
        DOT_TYPE_END()
    };

    inline PerformanceTestData make_performance_test_data() { return new PerformanceTestDataImpl; }


    // HELPER FUNCTIONS
    dot::String get_record_key(int index)
    {
        static const dot::String record_id_pattern = "key_{0}";
        return dot::String::format(record_id_pattern, index);
    }

    dot::String get_data_set(int index)
    {
        static const dot::String data_set_pattern = "data_set_{0}";
        return dot::String::format(data_set_pattern, index);
    }

    TemporalId save_record(UnitTestContextBase context, dot::String data_set_id, dot::String record_id, int record_version, int record_size)
    {
        PerformanceTestData rec = make_performance_test_data();
        rec->record_id = record_id;
        rec->version = record_version;
        rec->double_list = dot::make_list<double>();
        rec->double_list->set_capacity(record_size);

        for (int i = 0; i < record_size; ++i)
            rec->double_list->add(std::rand());

        TemporalId data_set = context->get_data_set(data_set_id);
        context->save_one(rec, data_set);
        return rec->id;
    }

    void save_records(UnitTestContextBase context, int record_count, int record_size)
    {
        // Create datasets
        TemporalId common_data_set = context->get_common();
        for (int i = 0; i < data_set_count; ++i)
        {
            dot::String data_set_name = get_data_set(i);
            context->create_data_set(data_set_name, dot::make_list<TemporalId>({ common_data_set }));
        }

        // Create records
        for (int i = 0; i < record_count; ++i)
        {
            dot::String record_id = get_record_key(i);
            save_record(context, DataSetKeyImpl::common->data_set_id, record_id, 0, record_size);

            for (int data_set_index = 0; data_set_index < data_set_count; ++data_set_index)
            {
                dot::String data_set_name = get_data_set(data_set_index);

                for (int version = 1; version < record_versions; ++version)
                {
                    save_record(context, data_set_name, record_id, version, record_size);
                }
            }
        }
    }

    void fill_database(UnitTestContextBase context)
    {
        save_records(context, record_count, record_size);
    }


    TEST_CASE("performance")
    {
        PerformanceTest test = new PerformanceTestImpl;
        UnitTestContextBase context = make_unit_test_context(test, "performance", ".");

        //fill_database(context);
    }

    TEST_CASE("load_by_key")
    {
        PerformanceTest test = new PerformanceTestImpl;
        UnitTestContextBase context = make_unit_test_context(test, "performance", ".");

        fill_database(context);
        TestDurationCounter td("Keys loading");

        for (int i = 0; i < record_count; ++i)
        {
            PerformanceTestKey key = make_performance_test_key();
            key->record_id = get_record_key(i);
            context->load_or_null(key, context->data_set);

            for (int data_set_index = 0; data_set_index < data_set_count; ++data_set_index)
            {
                dot::String data_set_name = get_data_set(data_set_index);
                TemporalId data_set = context->get_data_set(data_set_name);
                context->load_or_null(key, data_set);
            }
        }
    }

    TEST_CASE("query")
    {
        PerformanceTest test = new PerformanceTestImpl;
        UnitTestContextBase context = make_unit_test_context(test, "performance", ".");

        fill_database(context);
        TestDurationCounter td("Query loading");

        dot::String record_id = get_record_key(2);
        dot::String data_set_name = get_data_set(2);
        TemporalId data_set = context->get_data_set(data_set_name);

        dot::CursorWrapper<PerformanceTestData> query = context->data_source->get_query<PerformanceTestData>(data_set)
            ->where(make_prop(&PerformanceTestDataImpl::record_id) == record_id)
            ->where(make_prop(&PerformanceTestDataImpl::version) == record_versions - 1)
            ->get_cursor<PerformanceTestData>();

        for (PerformanceTestData data : query)
        {
            dot::Console::write_line(data->to_string());
        }
    }
}
