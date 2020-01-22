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

#include <dc/platform/data_set/data_set_key.hpp>
#include <dc/platform/data_set/data_set_data.hpp>

#include <dc/test/platform/context/context.hpp>

#include <dc/types/record/record.hpp>

#include <chrono>

namespace dc
{
    // CONSTANTS
    const int data_set_count = 10;
    const int record_versions = 10;

    const int record_count = 1024 * 3 / (data_set_count * record_versions);
    const int record_size = 128 * 200;


    // TEST CLASSES

    /// This class counts test running time.
    class test_duration_counter
    {
        std::chrono::steady_clock::time_point start_time;
        dot::string message;

    public:

        test_duration_counter(dot::string msg)
            : start_time(std::chrono::steady_clock::now())
            , message(msg)
        {}

        ~test_duration_counter()
        {
            auto end_time = std::chrono::steady_clock::now();
            std::cout << *message << " "
                << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
                << "ms" << std::endl;
        }
    };

    class performance_test_impl; using performance_test = dot::ptr<performance_test_impl>;

    class performance_test_impl : public dot::object_impl
    {
        typedef performance_test_impl self;

    public:

        DOT_TYPE_BEGIN("dc", "performance_test")
        DOT_TYPE_END()
    };

    class performance_test_key_impl; using performance_test_key = dot::ptr<performance_test_key_impl>;
    class performance_test_data_impl; using performance_test_data = dot::ptr<performance_test_data_impl>;

    performance_test_key make_performance_test_key();

    /// Key class.
    class performance_test_key_impl : public key_impl<performance_test_key_impl, performance_test_data_impl>
    {
        typedef performance_test_key_impl self;

    public:

        dot::string record_id;

        DOT_TYPE_BEGIN("dc", "performance_test_key")
            DOT_TYPE_PROP(record_id)
            DOT_TYPE_BASE(key<performance_test_key_impl, performance_test_data_impl>)
            DOT_TYPE_CTOR(make_performance_test_key)
        DOT_TYPE_END()
    };

    inline performance_test_key make_performance_test_key() { return new performance_test_key_impl; }

    performance_test_data make_performance_test_data();

    /// Data class.
    class performance_test_data_impl : public record_impl<performance_test_key_impl, performance_test_data_impl>
    {
        typedef performance_test_data_impl self;

    public:

        dot::string record_id;
        dot::list<double> double_list;
        dot::nullable<int> version;

        DOT_TYPE_BEGIN("dc", "performance_test_data")
            DOT_TYPE_PROP(record_id)
            DOT_TYPE_PROP(double_list)
            DOT_TYPE_PROP(version)
            DOT_TYPE_BASE(record<performance_test_key_impl, performance_test_data_impl>)
            DOT_TYPE_CTOR(make_performance_test_data)
        DOT_TYPE_END()
    };

    inline performance_test_data make_performance_test_data() { return new performance_test_data_impl; }


    // HELPER FUNCTIONS
    dot::string get_record_key(int index)
    {
        static const dot::string record_id_pattern = "key_{0}";
        return dot::string::format(record_id_pattern, index);
    }

    dot::string get_data_set(int index)
    {
        static const dot::string data_set_pattern = "data_set_{0}";
        return dot::string::format(data_set_pattern, index);
    }

    dot::object_id save_record(unit_test_context_base context, dot::string data_set_id, dot::string record_id, int record_version, int record_size)
    {
        performance_test_data rec = make_performance_test_data();
        rec->record_id = record_id;
        rec->version = record_version;
        rec->double_list = dot::make_list<double>();
        rec->double_list->set_capacity(record_size);

        for (int i = 0; i < record_size; ++i)
            rec->double_list->add(std::rand());

        dot::object_id data_set = context->get_data_set(data_set_id);
        context->save(rec, data_set);
        return rec->id;
    }

    void save_records(unit_test_context_base context, int record_count, int record_size)
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
            dot::string record_id = get_record_key(i);
            save_record(context, data_set_key_impl::common->data_set_id, record_id, 0, record_size);

            for (int data_set_index = 0; data_set_index < data_set_count; ++data_set_index)
            {
                dot::string data_set_name = get_data_set(data_set_index);

                for (int version = 1; version < record_versions; ++version)
                {
                    save_record(context, data_set_name, record_id, version, record_size);
                }
            }
        }
    }

    void fill_database(unit_test_context_base context)
    {
        save_records(context, record_count, record_size);
    }


    TEST_CASE("performance")
    {
        performance_test test = new performance_test_impl;
        unit_test_context_base context = new unit_test_context_impl(test, "performance", ".");

        //fill_database(context);
    }

    TEST_CASE("load_by_key")
    {
        performance_test test = new performance_test_impl;
        unit_test_context_base context = new unit_test_context_impl(test, "performance", ".");
        test_duration_counter td("Keys loading");

        for (int i = 0; i < record_count; ++i)
        {
            performance_test_key key = make_performance_test_key();
            key->record_id = get_record_key(i);
            context->reload_or_null(key, context->get_common());

            for (int data_set_index = 0; data_set_index < data_set_count; ++data_set_index)
            {
                dot::string data_set_name = get_data_set(data_set_index);
                dot::object_id data_set = context->get_data_set_or_empty(data_set_name);
                context->reload_or_null(key, data_set);
            }
        }
    }

    TEST_CASE("query")
    {
        performance_test test = new performance_test_impl;
        unit_test_context_base context = new unit_test_context_impl(test, "performance", ".");
        test_duration_counter td("Query loading");

        dot::string record_id = get_record_key(2);
        dot::string data_set_name = get_data_set(2);
        dot::object_id data_set = context->get_data_set_or_empty(data_set_name);

        dot::cursor_wrapper<performance_test_data> query = context->data_source->get_query<performance_test_data>(data_set)
      // TODO - fix compilation      ->where(make_prop(&performance_test_data_impl::key) == record_id)
            //->where(make_prop(&performance_test_data_impl::version) == record_versions - 1)
            ->get_cursor<performance_test_data>();

        for (performance_test_data data : query)
        {
            std::cout << *data->to_string() << std::endl;
        }
    }
}
