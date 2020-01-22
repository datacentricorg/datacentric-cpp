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
#include <dc/test/platform/data_source/mongo/mongo_test_data.hpp>

#include <dot/mongo/mongo_db/mongo/settings.hpp>

namespace dc
{
    static std::stringstream received;

    class MongoDataSourceTestImpl; using MongoDataSourceTest = dot::Ptr<MongoDataSourceTestImpl>;

    class MongoDataSourceTestImpl : public virtual dot::ObjectImpl
    {
        typedef MongoDataSourceTestImpl self;
    public:

        DOT_TYPE_BEGIN("dc", "MongoDataSourceTest")
        DOT_TYPE_END()
    };

    /// Save record with minimal data for testing how the records are found.
    TemporalId save_minimal_record(UnitTestContextBase context, dot::String data_set_id, dot::String record_id, int record_index, dot::Nullable<int> version = dot::Nullable<int>())
    {
        MongoTestData rec = make_mongo_test_data();
        rec->record_id = record_id;
        rec->record_index = record_index;
        rec->version = version;

        TemporalId data_set = context->get_data_set(data_set_id, context->data_set);
        context->save_one(rec, data_set);
        return rec->id;
    }

    /// Save base record
    TemporalId save_base_record(UnitTestContextBase context, dot::String data_set_id, dot::String record_id, int record_index)
    {
        MongoTestData rec = new MongoTestDataImpl();
        rec->record_id = record_id;
        rec->record_index = record_index;
        rec->double_element = 100.0;
        rec->local_date_element = dot::LocalDate(2003, 5, 1);
        rec->local_time_element = dot::LocalTime(10, 15, 30); // 10:15:30
        rec->local_minute_element = dot::LocalMinute(10, 15); // 10:15
        rec->local_date_time_element = dot::LocalDateTime(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->enum_value = MongoTestEnum::enum_value2;

        TemporalId data_set = context->get_data_set(data_set_id, context->data_set);
        context->save_one(rec, data_set);

        MongoTestData rec2 = context->load_or_null<MongoTestData>(rec->id);
        REQUIRE(rec->enum_value == rec2->enum_value);

        return rec->id;
    }

    /// Save derived record
    TemporalId save_derived_record(UnitTestContextBase context, dot::String data_set_id, dot::String record_id, int record_index)
    {
        MongoTestDerivedData rec = make_mongo_test_derived_data();
        rec->record_id = record_id;
        rec->record_index = record_index;
        rec->double_element = 300.0;
        rec->local_date_element = dot::LocalDate(2003, 5, 1);
        rec->local_time_element = dot::LocalTime(10, 15, 30); // 10:15:30
        rec->local_minute_element = dot::LocalMinute(10, 15); // 10:15
        rec->local_date_time_element = dot::LocalDateTime(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->string_element2 = dot::String::empty; // Test how empty value is recorded
        rec->double_element2 = 200.0;

        // dot::String collections
        rec->array_of_string = dot::make_list<dot::String>({ "A", "B", "C" });
        rec->list_of_string = dot::make_list<dot::String>({ "A", "B", "C" });

        // Double collections
        rec->array_of_double = dot::make_list<double>({ 1.0, 2.0, 3.0 });
        rec->list_of_double = dot::make_list<double>({ 1.0, 2.0, 3.0 });
        rec->list_of_nullable_double = dot::make_list<dot::Nullable<double>>();
        rec->list_of_nullable_double->add(10.0);
        rec->list_of_nullable_double->add(dot::Nullable<double>());
        rec->list_of_nullable_double->add(30.0);

        // Data element
        rec->data_element = make_element_sample_data();
        rec->data_element->double_element3 = 1.0;
        rec->data_element->string_element3 = "AA";

        // Data element list
        rec->data_element_list = dot::make_list<ElementSampleData>();
        ElementSampleData element_list0 = make_element_sample_data();
        element_list0->double_element3 = 1.0;
        element_list0->string_element3 = "A0";
        rec->data_element_list->add(element_list0);
        ElementSampleData element_list1 = make_element_sample_data();
        element_list1->double_element3 = 2.0;
        element_list1->string_element3 = "A1";
        rec->data_element_list->add(element_list1);

        // Key element
        rec->key_element = make_mongo_test_key();
        rec->key_element->record_id = "BB";
        rec->key_element->record_index = 2;

        // Key element list
        rec->key_element_list = dot::make_list<MongoTestKey>();
        MongoTestKey key_list0 = make_mongo_test_key();
        key_list0->record_id = "B0";
        key_list0->record_index = 3;
        rec->key_element_list->add(key_list0);
        MongoTestKey key_list1 = make_mongo_test_key();
        key_list1->record_id = "B1";
        key_list1->record_index = 4;
        rec->key_element_list->add(key_list1);

        TemporalId data_set = context->get_data_set(data_set_id, context->data_set);
        context->save_one(rec, data_set);
        return rec->id;
    }

    /// Save other derived record.
    TemporalId save_other_derived_record(UnitTestContextBase context, dot::String data_set_id, dot::String record_id, int record_index)
    {
        MongoTestOtherDerivedData rec = make_mongo_test_other_derived_data();
        rec->record_id = record_id;
        rec->record_index = record_index;
        rec->double_element = 300.0;
        rec->local_date_element = dot::LocalDate(2003, 5, 1);
        rec->local_time_element = dot::LocalTime(10, 15, 30); // 10:15:30
        rec->local_minute_element = dot::LocalMinute(10, 15); // 10:15
        rec->local_date_time_element = dot::LocalDateTime(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->other_string_element2 = dot::String::empty; // Test how empty value is recorded
        rec->other_double_element2 = 200.0;

        TemporalId data_set = context->get_data_set(data_set_id, context->data_set);
        context->save_one(rec, data_set);
        return rec->id;
    }

    /// Save record that is derived from derived.
    TemporalId save_derived_from_derived_record(UnitTestContextBase context, dot::String data_set_id, dot::String record_id, int record_index)
    {
        MongoTestDerivedFromDerivedData rec = make_mongo_test_derived_from_derived_data();
        rec->record_id = record_id;
        rec->record_index = record_index;
        rec->double_element = 300.0;
        rec->local_date_element = dot::LocalDate(2003, 5, 1);
        rec->local_time_element = dot::LocalTime(10, 15, 30); // 10:15:30
        rec->local_minute_element = dot::LocalMinute(10, 15); // 10:15
        rec->local_date_time_element = dot::LocalDateTime(2003, 5, 1, 10, 15); // 2003-05-01T10:15:00
        rec->other_string_element2 = dot::String::empty; // Test how empty value is recorded
        rec->other_double_element3 = 200.0;

        TemporalId data_set = context->get_data_set(data_set_id, context->data_set);
        context->save_one(rec, data_set);
        return rec->id;
    }

    /// Two datasets and two objects, one base and one derived.
    void save_basic_data(UnitTestContextBase context)
    {
        // Create datasets
        TemporalId data_set_a = context->create_data_set("A", context->data_set);
        dot::List<TemporalId> parents = dot::make_list<TemporalId>();
        parents->add(data_set_a);

        TemporalId data_set_b = context->create_data_set("B", parents, context->data_set);

        // Create records with minimal data
        save_base_record(context, "A", "A", 0);
        save_derived_record(context, "B", "B", 0);
    }

    /// Two datasets and eight objects, split between base and derived.
    void save_complete_data(UnitTestContextBase context)
    {
        // Create datasets
        TemporalId data_set_a = context->create_data_set("A", context->data_set);
        TemporalId data_set_b = context->create_data_set("B", dot::make_list<TemporalId>({ data_set_a }), context->data_set);
        TemporalId data_set_c = context->create_data_set("C", dot::make_list<TemporalId>({ data_set_a }), context->data_set);
        TemporalId data_set_d = context->create_data_set("D", dot::make_list<TemporalId>({ data_set_a, data_set_b, data_set_c }), context->data_set);

        // Create records with minimal data
        save_base_record(context, "A", "A", 0);
        save_derived_record(context, "B", "B", 0);
        save_other_derived_record(context, "C", "C", 0);
        save_derived_from_derived_record(context, "D", "D", 0);
        save_base_record(context, "A", "A", 1);
        save_derived_record(context, "B", "B", 1);
        save_other_derived_record(context, "C", "C", 1);
        save_derived_from_derived_record(context, "D", "D", 1);
        save_base_record(context, "A", "A", 2);
        save_derived_record(context, "B", "B", 2);
        save_other_derived_record(context, "C", "C", 2);
        save_derived_from_derived_record(context, "D", "D", 2);
        save_base_record(context, "A", "A", 3);
        save_derived_record(context, "B", "B", 3);
        save_other_derived_record(context, "C", "C", 3);
        save_derived_from_derived_record(context, "D", "D", 3);
    }

    /// Minimal data in multiple datasets with overlapping parents.
    void save_multi_data_set_data(UnitTestContextBase context)
    {
        // Create datasets
        TemporalId data_set_a = context->create_data_set("A", context->data_set);
        TemporalId data_set_b = context->create_data_set("B", dot::make_list<TemporalId>({ data_set_a }), context->data_set);
        TemporalId data_set_c = context->create_data_set("C", dot::make_list<TemporalId>({ data_set_a }), context->data_set);
        TemporalId data_set_d = context->create_data_set("D", dot::make_list<TemporalId>({ data_set_a, data_set_b, data_set_c }), context->data_set);

        // Create records
        save_minimal_record(context, "A", "A", 0);
        save_minimal_record(context, "A", "A", 1);
        save_minimal_record(context, "B", "B", 0);
        save_minimal_record(context, "B", "B", 1);
        save_minimal_record(context, "C", "C", 0);
        save_minimal_record(context, "C", "C", 1);
        save_minimal_record(context, "D", "D", 0);
        save_minimal_record(context, "D", "D", 1);
    }

    /// Load the Object and verify the outcome.
    template <class TKey, class TRecord>
    void verify_load(UnitTestContextBase context, TypedKey<TKey, TRecord> key, dot::String data_set_id)
    {
        // Get dataset and try loading the record
        TemporalId data_set = context->get_data_set(data_set_id, context->data_set);
        dot::Ptr<TRecord> record = context->load_or_null(key, data_set);

        if (record == nullptr)
        {
            // Not found
            received << * dot::String::format("record {0} in dataset {1} not found.", key->to_string(), data_set_id) << std::endl;
        }
        else
        {
            // Found, also checks that the key matches
            REQUIRE(record->get_key() == key->to_string());
            received
                << *dot::String::format("record {0} in dataset {1} found and has type={2}.",
                    key->to_string(), data_set_id, record->get_type()->name())
                << std::endl;
        }
    }

    /// Query over all records of the specified type in the specified dataset.
    template <class TRecord>
    void verify_query(UnitTestContextBase context, dot::String data_set_id)
    {
        // Get dataset and query
        TemporalId data_set = context->get_data_set(data_set_id, context->data_set);
        MongoQuery query = context->data_source->get_query<TRecord>(data_set);

        // Iterate over records
        for (TRecord record : query->get_cursor<TRecord>())
        {
            received
                << *dot::String::format(
                    "record {0} returned by query in dataset {1} and has type={2}.",
                    record->get_key(), data_set_id, record->get_type()->name())
                << std::endl;
        }
    }


    TEST_CASE("smoke")
    {
        //dot::MongoClientSettings::set_discriminator_convention(dot::DiscriminatorConvention::hierarchical);
        auto g = dot::MongoClientSettings::get_discriminator_convention();

        MongoDataSourceTest test = new MongoDataSourceTestImpl;

        UnitTestContextBase context = make_unit_test_context(test, "smoke", ".");
        save_basic_data(context);

        // Get dataset identifiers
        TemporalId data_set_a = context->get_data_set("A", context->data_set);
        TemporalId data_set_b = context->get_data_set("B", context->data_set);

        // Create keys
        MongoTestKey key_a0 = make_mongo_test_key();
        key_a0->record_id = "A";
        key_a0->record_index = dot::Nullable<int>(0);

        MongoTestKey key_b0 = make_mongo_test_key();
        key_b0->record_id = "B";
        key_b0->record_index = dot::Nullable<int>(0);

        // Verify the result of loading records from datasets A and B
        verify_load(context, ( TypedKey<MongoTestKeyImpl, MongoTestDataImpl>)key_a0, "A");
        verify_load(context, ( TypedKey<MongoTestKeyImpl, MongoTestDataImpl>)key_a0, "B");
        verify_load(context, ( TypedKey<MongoTestKeyImpl, MongoTestDataImpl>)key_b0, "A");
        verify_load(context, ( TypedKey<MongoTestKeyImpl, MongoTestDataImpl>)key_b0, "B");

        context->keep_db = true;

        std::string to_verify = received.str();
        received.str("");
        Approvals::verify(to_verify);
    }

    TEST_CASE("query")
    {
        MongoDataSourceTest test = new MongoDataSourceTestImpl;

        UnitTestContextBase context = make_unit_test_context(test, "query", ".");

        // Create datasets
        TemporalId data_set_a = context->create_data_set("A", context->data_set);
        TemporalId data_set_b = context->create_data_set("B", dot::make_list<TemporalId>({ data_set_a }), context->data_set);
        TemporalId data_set_c = context->create_data_set("C", dot::make_list<TemporalId>({ data_set_a }), context->data_set);
        TemporalId data_set_d = context->create_data_set("D", dot::make_list<TemporalId>({ data_set_a, data_set_b, data_set_c }), context->data_set);

        // Create initial version of the records
        save_minimal_record(context, "A", "A", 0, 0);
        save_minimal_record(context, "A", "B", 1, 0);
        save_minimal_record(context, "A", "A", 2, 0);
        save_minimal_record(context, "A", "B", 3, 0);
        save_minimal_record(context, "B", "A", 4, 0);
        save_minimal_record(context, "B", "B", 5, 0);
        save_minimal_record(context, "B", "A", 6, 0);
        save_minimal_record(context, "B", "B", 7, 0);
        save_minimal_record(context, "C", "A", 8, 0);
        save_minimal_record(context, "C", "B", 9, 0);
        save_minimal_record(context, "D", "A", 10, 0);
        save_minimal_record(context, "D", "B", 11, 0);

        // Create second version of some records
        save_minimal_record(context, "A", "A", 0, 1);
        save_minimal_record(context, "A", "B", 1, 1);
        save_minimal_record(context, "A", "A", 2, 1);
        save_minimal_record(context, "A", "B", 3, 1);
        save_minimal_record(context, "B", "A", 4, 1);
        save_minimal_record(context, "B", "B", 5, 1);
        save_minimal_record(context, "B", "A", 6, 1);
        save_minimal_record(context, "B", "B", 7, 1);

        // Create third version of even fewer records
        save_minimal_record(context, "A", "A", 0, 2);
        save_minimal_record(context, "A", "B", 1, 2);
        save_minimal_record(context, "A", "A", 2, 2);
        save_minimal_record(context, "A", "B", 3, 2);

        dot::List<dot::String> in_list = dot::make_list<dot::String>();
        in_list->add("B");

        // Query for record_id=B
        dot::CursorWrapper<MongoTestData> query = context->data_source->get_query<MongoTestData>(data_set_d)
            //->where(p = > p.record_id == "B")
            ->where(make_prop(&MongoTestDataImpl::record_id).in({ "B" }))
            //->where(make_prop(&MongoTestDataImpl::record_id).in(std::vector<std::string>({ "B" })))
            ->where(make_prop(&MongoTestDataImpl::record_id).in(in_list))
            ->where(make_prop(&MongoTestDataImpl::record_id) == "B")
            ->sort_by(make_prop(&MongoTestDataImpl::record_id))
            ->sort_by(make_prop(&MongoTestDataImpl::record_index))
            ->get_cursor<MongoTestData>()
            ;


        for (MongoTestData obj : query)
        {
            dot::String data_set_id = context->data_source->load_or_null<DataSet>(obj->data_set)->data_set_id;
            received << *dot::String::format("key={0} data_set={1} version={2}", obj->get_key(), data_set_id, obj->version) << std::endl;
        }

        std::string to_verify = received.str();
        received.str("");
        Approvals::verify(to_verify);
    }

    TEST_CASE("delete")
    {
        MongoDataSourceTest test = new MongoDataSourceTestImpl;
        UnitTestContextBase context = make_unit_test_context(test, "delete", ".");

        save_basic_data(context);

        // Get dataset identifiers
        TemporalId data_set_a = context->get_data_set("A", context->data_set);
        TemporalId data_set_b = context->get_data_set("B", context->data_set);

        // Create keys
        MongoTestKey key_a0 = new MongoTestKeyImpl();
        key_a0->record_id = "A";
        key_a0->record_index = dot::Nullable<int>(0);

        MongoTestKey key_b0 = new MongoTestKeyImpl();
        key_b0->record_id = "B";
        key_b0->record_index = dot::Nullable<int>(0);

        // Verify the result of loading records from datasets A and B
        received << "initial load" << std::endl;
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_a0, "A");
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_a0, "B");
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_b0, "A");
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_b0, "B");

        received << "query in dataset A" << std::endl;
        verify_query<MongoTestData>(context, "A");
        received << "query in dataset B" << std::endl;
        verify_query<MongoTestData>(context, "B");

        received << "delete A0 record in B dataset" << std::endl;
        context->delete_record(key_a0, data_set_b);
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_a0, "A");
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_a0, "B");

        received << "query in dataset A" << std::endl;
        verify_query<MongoTestData>(context, "A");
        received << "query in dataset B" << std::endl;
        verify_query<MongoTestData>(context, "B");

        received << "delete A0 record in A dataset" << std::endl;
        context->delete_record(key_a0, data_set_a);
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_a0, "A");
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_a0, "B");

        received << "query in dataset A" << std::endl;
        verify_query<MongoTestData>(context, "A");
        received << "query in dataset B" << std::endl;
        verify_query<MongoTestData>(context, "B");

        received << "delete B0 record in B dataset" << std::endl;
        context->delete_record(key_b0, data_set_b);
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_b0, "A");
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_b0, "B");

        received << "query in dataset A" << std::endl;
        verify_query<MongoTestData>(context, "A");
        received << "query in dataset B" << std::endl;
        verify_query<MongoTestData>(context, "B");

        std::string to_verify = received.str();
        received.str("");
        Approvals::verify(to_verify);
    }

    /// Test saving Object of a different type for the same key.
    ///
    /// The objective of this test is to confirm that load_or_null
    /// for the wrong type will give an error, while query will
    /// skip the Object of the wrong type even if there is an
    /// earlier version of the Object with the same key that
    /// has a compatible type.
    TEST_CASE("type_change")
    {
        MongoDataSourceTest test = new MongoDataSourceTestImpl;
        UnitTestContextBase context = make_unit_test_context(test, "type_change", ".");

        // Create datasets
        TemporalId data_set_a = context->create_data_set("A", context->data_set);
        TemporalId data_set_b = context->create_data_set("B", dot::make_list<TemporalId>({ data_set_a }), context->data_set);

        // Create records with minimal data
        save_derived_record(context, "A", "A", 0);
        save_derived_from_derived_record(context, "B", "B", 0);

        // Create keys
        MongoTestKey key_a0 = make_mongo_test_key();
        key_a0->record_id = "A";
        key_a0->record_index = dot::Nullable<int>(0);

        MongoTestKey key_b0 = make_mongo_test_key();
        key_b0->record_id = "B";
        key_b0->record_index = dot::Nullable<int>(0);

        // Verify the result of loading records from datasets A and B
        received << "initial load" << std::endl;
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_a0, "A");
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_a0, "B");
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_b0, "A");
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_b0, "B");

        received << "query in dataset A for type MongoTestDerivedData" << std::endl;
        verify_query<MongoTestDerivedData>(context, "A");
        received << "query in dataset B for type MongoTestDerivedData" << std::endl;
        verify_query<MongoTestDerivedData>(context, "B");

        received << "change A0 record type in B dataset to C" << std::endl;
        save_other_derived_record(context, "B", "A", 0);
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_a0, "A");
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_a0, "B");

        received << "query in dataset A for type MongoTestDerivedData" << std::endl;
        verify_query<MongoTestDerivedData>(context, "A");
        received << "query in dataset B for type MongoTestDerivedData" << std::endl;
        verify_query<MongoTestDerivedData>(context, "B");

        received << "change A0 record type in A dataset to C" << std::endl;
        save_other_derived_record(context, "A", "A", 0);
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_a0, "A");
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_a0, "B");

        received << "query in dataset A for type MongoTestDerivedData" << std::endl;
        verify_query<MongoTestDerivedData>(context, "A");
        received << "query in dataset B for type MongoTestDerivedData" << std::endl;
        verify_query<MongoTestDerivedData>(context, "B");

        received << "change B0 record type in B dataset to C" << std::endl;
        save_other_derived_record(context, "B", "B", 0);
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_b0, "A");
        verify_load(context, (TypedKey<MongoTestKeyImpl, MongoTestDataImpl>) key_b0, "B");

        received << "query in dataset A for type MongoTestDerivedData" << std::endl;
        verify_query<MongoTestDerivedData>(context, "A");
        received << "query in dataset B for type MongoTestDerivedData" << std::endl;
        verify_query<MongoTestDerivedData>(context, "B");

        std::string to_verify = received.str();
        received.str("");
        Approvals::verify(to_verify);
    }

    TEST_CASE("element_types_query")
    {
        MongoDataSourceTest test = new MongoDataSourceTestImpl;
        UnitTestContextBase context = make_unit_test_context(test, "element_types_query", ".");

        // Saves data in A and B datasets, A is parent of B
        save_complete_data(context);

        // Look in B dataset
        TemporalId data_set_b = context->get_data_set("B", context->data_set);

        MongoTestKey key = make_mongo_test_key();
        key->record_id = "BB";
        key->record_index = dot::Nullable<int>(2);

        dot::CursorWrapper<MongoTestDerivedData> test_query = context->data_source->get_query<MongoTestDerivedData>(data_set_b)
            ->where(make_prop(&MongoTestDerivedDataImpl::data_element_list)[0]->*make_prop(&ElementSampleDataImpl::double_element3) == 1.0)
            ->where(make_prop(&MongoTestDerivedDataImpl::data_element_list)[0]->*make_prop(&ElementSampleDataImpl::string_element3) == "A0")
            ->where(make_prop(&MongoTestDerivedDataImpl::local_date_element) < dot::LocalDate(2003, 5, 2))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_date_element) > dot::LocalDate(2003, 4, 30))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_date_element) == dot::LocalDate(2003, 5, 1))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_time_element) < dot::LocalTime(10, 15, 31))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_time_element) > dot::LocalTime(10, 15, 29))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_time_element) == dot::LocalTime(10, 15, 30))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_date_time_element) < dot::LocalDateTime(2003, 5, 1, 10, 15, 01))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_date_time_element) > dot::LocalDateTime(2003, 5, 1, 10, 14, 59))
            ->where(make_prop(&MongoTestDerivedDataImpl::local_date_time_element) == dot::LocalDateTime(2003, 5, 1, 10, 15))
            ->where(make_prop(&MongoTestDerivedDataImpl::string_element2) == dot::String::empty)
            ->where(make_prop(&MongoTestDerivedDataImpl::key_element) == key->to_string())
            ->get_cursor<MongoTestDerivedData>();

        for (MongoTestData obj : test_query)
        {
            received << *dot::String::format("key={0} type={1}", obj->get_key(), obj->get_type()->name()) << std::endl;
        }

        std::string to_verify = received.str();
        received.str("");
        Approvals::verify(to_verify);
    }

    TEST_CASE("polymorphic_query")
    {
        MongoDataSourceTest test = new MongoDataSourceTestImpl;
        UnitTestContextBase context = make_unit_test_context(test, "polymorphic_query", ".");

        // Saves data in A and B datasets, A is parent of B
        save_complete_data(context);

        // Look in B dataset
        TemporalId data_set_d = context->get_data_set("D", context->data_set);

        // Load record of derived types by base
        received << "load all types by key to type A" << std::endl;
        {
            MongoTestKey key = make_mongo_test_key();
            key->record_id = "A";
            key->record_index = dot::Nullable<int>(0);
            Record obj = context->load_or_null(key, data_set_d);
            received << *dot::String::format("    key={0} type={1}", obj->get_key(), obj->get_type()->name()) << std::endl;
        }
        {
            MongoTestKey key = make_mongo_test_key();
            key->record_id = "B";
            key->record_index = dot::Nullable<int>(0);
            Record obj = context->load_or_null(key, data_set_d);
            received << *dot::String::format("    key={0} type={1}", obj->get_key(), obj->get_type()->name()) << std::endl;
        }
        {
            MongoTestKey key = make_mongo_test_key();
            key->record_id = "C";
            key->record_index = dot::Nullable<int>(0);
            Record obj = context->load_or_null(key, data_set_d);
            received << *dot::String::format("    key={0} type={1}", obj->get_key(), obj->get_type()->name()) << std::endl;
        }
        {
            MongoTestKey key = make_mongo_test_key();
            key->record_id = "D";
            key->record_index = dot::Nullable<int>(0);
            Record obj = context->load_or_null(key, data_set_d);
            received << *dot::String::format("    key={0} type={1}", obj->get_key(), obj->get_type()->name()) << std::endl;
        }
        {
            received << "query by MongoTestData, unconstrained" << std::endl;
            MongoQuery query = context->data_source->get_query<MongoTestData>(data_set_d);
            for (Record obj : query->get_cursor<Record>())
            {
                received << *dot::String::format("    key={0} type={1}", obj->get_key(), obj->get_type()->name()) << std::endl;
            }
        }
        {
            received << "query by MongoTestDerivedData : MongoTestData which also picks up MongoTestDerivedFromDerivedData : MongoTestDerivedData, unconstrained" << std::endl;
            MongoQuery query = context->data_source->get_query<MongoTestDerivedData>(data_set_d);
            for (Record obj : query->get_cursor<Record>())
            {
                received << *dot::String::format("    key={0} type={1}", obj->get_key(), obj->get_type()->name()) << std::endl;
            }
        }
        {
            received << "query by MongoTestOtherDerivedData : MongoTestData, unconstrained" << std::endl;
            MongoTestOtherDerivedDataImpl::typeof();
            MongoQuery query = context->data_source->get_query<MongoTestOtherDerivedData>(data_set_d);
            for (Record obj : query->get_cursor<Record>())
            {
                received << *dot::String::format("    key={0} type={1}", obj->get_key(), obj->get_type()->name()) << std::endl;
            }
        }
        {
            received << "query by MongoTestDerivedFromDerivedData : MongoTestDerivedData, where MongoTestDerivedData : MongoTestData, unconstrained" << std::endl;
            MongoQuery query = context->data_source->get_query<MongoTestDerivedFromDerivedData>(data_set_d);
            for (Record obj : query->get_cursor<Record>())
            {
                received << *dot::String::format("    key={0} type={1}", obj->get_key(), obj->get_type()->name()) << std::endl;
            }
        }

        std::string to_verify = received.str();
        received.str("");
        Approvals::verify(to_verify);
    }

    TEST_CASE("sort")
    {
        MongoDataSourceTest test = new MongoDataSourceTestImpl;
        UnitTestContextBase context = make_unit_test_context(test, "sort", ".");

        // Saves data in A and B datasets, A is parent of B
        save_complete_data(context);

        // Look in B dataset
        TemporalId data_set_d = context->get_data_set("D", context->data_set);

        received << "query by MongoTestData, sort by record_index descending, then by double_element ascending" << std::endl;
        dot::CursorWrapper<MongoTestData> base_query = context->data_source->get_query<MongoTestData>(data_set_d)
            ->sort_by_descending(make_prop(&MongoTestDataImpl::record_index))
            ->sort_by(make_prop(&MongoTestDataImpl::double_element))
            ->get_cursor<MongoTestData>();

        for (MongoTestData obj : base_query)
        {
            received
                << *dot::String::format(
                    "    record_index={0} double_element={1} key={2} type={3}",
                    obj->record_index, obj->double_element, obj->get_key(), obj->get_type()->name())
                << std::endl;
        }

        std::string to_verify = received.str();
        received.str("");
        Approvals::verify(to_verify);
    }

    TEST_CASE("cutoff_time")
    {
        MongoDataSourceTest test = new MongoDataSourceTestImpl;
        UnitTestContextBase context = make_unit_test_context(test, "cutoff_time", ".");

        // Create datasets
        TemporalId data_set_a = context->create_data_set("A", context->data_set);
        TemporalId data_set_b = context->create_data_set("B", dot::make_list<TemporalId>({ data_set_a }), context->data_set);

        // Create initial version of the records
        TemporalId obj_a0 = save_minimal_record(context, "A", "A", 0, 0);
        TemporalId obj_b0 = save_minimal_record(context, "B", "B", 0, 0);

        // Create second version of the records
        TemporalId obj_a1 = save_minimal_record(context, "A", "A", 0, 1);
        TemporalId obj_b1 = save_minimal_record(context, "B", "B", 0, 1);

        TemporalId cutoff_object_id = context->data_source->create_ordered_object_id();

        // Create third version of the records
        TemporalId obj_a2 = save_minimal_record(context, "A", "A", 0, 2);
        TemporalId obj_b2 = save_minimal_record(context, "B", "B", 0, 2);

        // Create new records that did not exist before
        TemporalId obj_c0 = save_minimal_record(context, "A", "C", 0, 0);
        TemporalId obj_d0 = save_minimal_record(context, "B", "D", 0, 0);

        received << "load records by TemporalId without constraint" << std::endl;
        received << *dot::String::format("    found by TemporalId=A0 = {0}", context->load_or_null<MongoTestData>(obj_a0) != nullptr) << std::endl;
        received << *dot::String::format("    found by TemporalId=A1 = {0}", context->load_or_null<MongoTestData>(obj_a1) != nullptr) << std::endl;
        received << *dot::String::format("    found by TemporalId=A2 = {0}", context->load_or_null<MongoTestData>(obj_a2) != nullptr) << std::endl;
        received << *dot::String::format("    found by TemporalId=C0 = {0}", context->load_or_null<MongoTestData>(obj_c0) != nullptr) << std::endl;

        // Load each record by String key
        {
            MongoTestKey loaded_a0_key = make_mongo_test_key();
            loaded_a0_key->record_id = "A";
            loaded_a0_key->record_index = dot::Nullable<int>(0);
            MongoTestData loaded_a0 = (MongoTestData)context->load_or_null(loaded_a0_key, data_set_b);

            MongoTestKey loaded_c0_key = make_mongo_test_key();
            loaded_c0_key->record_id = "C";
            loaded_c0_key->record_index = dot::Nullable<int>(0);
            MongoTestData loaded_c0 = (MongoTestData)context->load_or_null(loaded_c0_key, data_set_b);

            received << "load records by String key without constraint" << std::endl;
            if (loaded_a0 != nullptr) received << *dot::String::format("    version found for key=A;0: {0}", loaded_a0->version) << std::endl;
            if (loaded_c0 != nullptr) received << *dot::String::format("    version found for key=C;0: {0}", loaded_c0->version) << std::endl;
        }

        // Query for all records
        {
            dot::CursorWrapper<MongoTestData> query = context->data_source->get_query<MongoTestData>(data_set_b)
                ->sort_by(make_prop(&MongoTestDataImpl::record_id))
                ->sort_by(make_prop(&MongoTestDataImpl::record_index))
                ->get_cursor<MongoTestData>();

            received << "query records without constraint" << std::endl;
            for (MongoTestData obj : query)
            {
                dot::String data_set_id = context->load_or_null<DataSet>(obj->data_set)->data_set_id;
                received << *dot::String::format("    key={0} data_set={1} version={2}", obj->get_key(), data_set_id, obj->version) << std::endl;
            }
        }

        // Set revision time constraint
        context->data_source.as<MongoDataSource>()->cutoff_time = cutoff_object_id;

        // Get each record by TemporalId
        received << "load records by TemporalId with revised_before_id constraint" << std::endl;
        received << *dot::String::format("    found by TemporalId=A0 = {0}", context->load_or_null<MongoTestData>(obj_a0) != nullptr) << std::endl;
        received << *dot::String::format("    found by TemporalId=A1 = {0}", context->load_or_null<MongoTestData>(obj_a1) != nullptr) << std::endl;
        received << *dot::String::format("    found by TemporalId=A2 = {0}", context->load_or_null<MongoTestData>(obj_a2) != nullptr) << std::endl;
        received << *dot::String::format("    found by TemporalId=C0 = {0}", context->load_or_null<MongoTestData>(obj_c0) != nullptr) << std::endl;

        // Load each record by String key
        {
            MongoTestKey loaded_a0_key = make_mongo_test_key();
            loaded_a0_key->record_id = "A";
            loaded_a0_key->record_index = dot::Nullable<int>(0);
            MongoTestData loaded_a0 = (MongoTestData)context->load_or_null(loaded_a0_key, data_set_b);

            MongoTestKey loaded_c0_key = make_mongo_test_key();
            loaded_a0_key->record_id = "C";
            loaded_a0_key->record_index = dot::Nullable<int>(0);
            MongoTestData loaded_c0 = (MongoTestData)context->load_or_null(loaded_c0_key, data_set_b);

            received << "load records by String key with revised_before_id constraint" << std::endl;
            if (loaded_a0 != nullptr) received << *dot::String::format("    version found for key=A;0: {0}", loaded_a0->version) << std::endl;
            if (loaded_c0 != nullptr) received << *dot::String::format("    version found for key=C;0: {0}", loaded_c0->version) << std::endl;
        }

        // Query for revised before the cutoff time
        {
            dot::CursorWrapper<MongoTestData> query = context->data_source->get_query<MongoTestData>(data_set_b)
                ->sort_by(make_prop(&MongoTestDataImpl::record_id))
                ->sort_by(make_prop(&MongoTestDataImpl::record_index))
                ->get_cursor<MongoTestData>();

            received << "query records with revised_before_id constraint" << std::endl;
            for (MongoTestData obj : query)
            {
                dot::String data_set_id = context->load_or_null<DataSet>(obj->data_set)->data_set_id;
                received << *dot::String::format("    key={0} data_set={1} version={2}", obj->get_key(), data_set_id, obj->version) << std::endl;
            }
        }

        // Clear revision time constraint before exiting to avoid an error
        // about deleting readonly database. The error occurs because
        // revision time constraint makes the data source readonly.
        context->data_source.as<MongoDataSource>()->cutoff_time = dot::Nullable<TemporalId>();

        std::string to_verify = received.str();
        received.str("");
        Approvals::verify(to_verify);
    }
}
