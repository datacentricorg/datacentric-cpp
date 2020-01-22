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

#include <dc/precompiled.hpp>
#include <dc/implement.hpp>
#include <dc/platform/data_source/mongo/mongo_data_source_data.hpp>
#include <dc/serialization/bson_writer.hpp>
#include <dc/serialization/bson_record_serializer.hpp>
#include <dc/platform/context/context_base.hpp>
#include <dc/platform/query/query.hpp>
#include <dc/platform/cursor/cursor_wrapper.hpp>
#include <dc/types/record/delete_marker.hpp>

#include <mongocxx/collection.hpp>
#include <bsoncxx/json.hpp>

namespace dc
{
    record_base mongo_data_source_data_impl::load_or_null(dot::object_id id, dot::type data_type)
    {
        auto revision_time_constraint = get_revision_time_constraint();
        if (revision_time_constraint != nullptr)
        {
            // If revision_time_constraint is not null, return null for any
            // id that is not strictly before the constraint dot::object_id
            if (id >= revision_time_constraint.value()) return nullptr;
        }

        bsoncxx::builder::basic::document filter{};
        filter.append(bsoncxx::builder::basic::kvp("_id", id.oid()));

        bsoncxx::stdx::optional<bsoncxx::document::value> res = get_collection(data_type).find_one(filter.view());
        if (res != bsoncxx::stdx::nullopt)
        {
            bson_record_serializer serializer = make_bson_record_serializer();
            record_base record = (record_base) serializer->deserialize(res->view());
            record->init(context);
            return record;
        }

        return nullptr;
    }

    record_base mongo_data_source_data_impl::reload_or_null(key_base key, dot::object_id load_from)
    {
        // Get lookup list by expanding the list of parents to arbitrary depth
        // with duplicates and cyclic references removed
        dot::hash_set<dot::object_id> lookup_list = get_data_set_lookup_list(load_from);

        // dot::string key in semicolon delimited format used in the lookup
        dot::string key_value = key->to_string();

        // Look for exact match of the key in the specified list of datasets,
        // then order by dataset dot::object_id in descending order
        bsoncxx::builder::basic::document filter{};
        filter.append(bsoncxx::builder::basic::kvp("_key", *key_value));

        // filter by dataset
        bsoncxx::builder::basic::array filter_dataset_list{};
        for (dot::object_id id : lookup_list)
            filter_dataset_list.append(id.oid());

        bsoncxx::builder::basic::document filter_dataset{};
        filter_dataset.append(bsoncxx::builder::basic::kvp("$in", filter_dataset_list.view()));

        filter.append(bsoncxx::builder::basic::kvp("_dataset", filter_dataset.view()));

        bsoncxx::builder::basic::document sort_ds{};
        bsoncxx::builder::basic::document sort_id{};
        sort_ds.append(bsoncxx::builder::basic::kvp("_dataset", -1));
        sort_id.append(bsoncxx::builder::basic::kvp("_id", -1));

        mongocxx::pipeline query;
        query.match(filter.view());
        query.sort(sort_ds.view());
        query.sort(sort_id.view());

        query.limit(1); // Only the first document is needed

        mongocxx::cursor res = get_collection(key->get_type()).aggregate(query);

        if (res.begin() != res.end())
        {
            bson_record_serializer serializer = make_bson_record_serializer();
            record_base result = (record_base) serializer->deserialize(*res.begin());
            result->init(context);

            // Check not only for null but also for the delete marker
            if (!result.is<delete_marker>())
                return result;
        }

        return nullptr;
    }

    void mongo_data_source_data_impl::save(record_base record, dot::object_id save_to)
    {
        check_not_read_only();

        auto collection = get_collection(record->get_type());

        // This method guarantees that dot::object_ids will be in strictly increasing
        // order for this instance of the data source class always, and across
        // all processes and machine if they are not created within the same
        // second.
        dot::object_id object_id = create_ordered_object_id();

        // dot::object_id of the record must be strictly later
        // than dot::object_id of the dataset where it is stored
        if (object_id <= save_to)
            throw dot::exception(dot::string::format(
                "Attempting to save a record with dot::object_id={0} that is later "
                "than dot::object_id={1} of the dataset where it is being saved.", object_id.to_string(), save_to.to_string()));

        // Assign id and data_set, and only then initialize, because
        // initialization code may use record.id and record.data_set
        record->id = object_id;
        record->data_set = save_to;
        record->init(context);

        // Serialize record.
        bson_record_serializer serializer = make_bson_record_serializer();
        bson_writer writer = make_bson_writer();
        serializer->serialize(writer, record);

        // By design, insert will fail if dot::object_id is not unique within the collection
        //collection.insert_one(writer->view());

        bsoncxx::builder::basic::document doc{}; //!! Temporary fix
        doc.append(bsoncxx::builder::basic::kvp("_t", * record->get_type()->name) );
        doc.append(bsoncxx::builder::basic::kvp("_key", * record->get_key()));
        doc.append(bsoncxx::builder::concatenate(writer->view()));

        collection.insert_one(doc.view());
    }

    query mongo_data_source_data_impl::get_query(dot::object_id data_set, dot::type type)
    {
        return make_query(this, data_set, type);
    }

    object_cursor_wrapper mongo_data_source_data_impl::load_by_query(query query)
    {
        // Get lookup list by expanding the list of parents to arbitrary depth
        // with duplicates and cyclic references removed
        dot::hash_set<dot::object_id> lookup_list = get_data_set_lookup_list(query->data_set_);

        // filter by dataset
        bsoncxx::builder::basic::array filter_dataset_list{};
        for (dot::object_id id : lookup_list)
            filter_dataset_list.append(id.oid());

        bsoncxx::builder::basic::document filter_dataset{};
        filter_dataset.append(bsoncxx::builder::basic::kvp("$in", filter_dataset_list.view()));

        bsoncxx::builder::basic::document filter{};

        filter.append(bsoncxx::builder::basic::kvp("_dataset", filter_dataset.view()));
        filter.append(bsoncxx::builder::basic::concatenate(query->where_.view()));

        bsoncxx::builder::basic::document sort{};
        sort.append(bsoncxx::builder::basic::kvp("_key", 1));
        sort.append(bsoncxx::builder::basic::kvp("_dataset", -1));
        sort.append(bsoncxx::builder::basic::kvp("_id", -1));

        mongocxx::pipeline pipeline;
        pipeline.match(filter.view());
        pipeline.sort(sort.view());

        // group by key
        bsoncxx::builder::basic::document gr;
        gr.append(bsoncxx::builder::basic::kvp("_id", "$_key"));

        gr.append(bsoncxx::builder::basic::kvp("doc",
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$first", "$$ROOT"))));
        pipeline.group(gr.view());

        // filter by type
        bsoncxx::builder::basic::document type_filter;

        bsoncxx::builder::basic::array type_list;

        // append given type and delete_marker
        type_list.append(*(dot::string) query->type_->name);

        // append derived types
        dot::list<dot::type> derived_types = dot::type_impl::get_derived_types(query->type_);
        if (derived_types != nullptr)
        {
            for (dot::type der_type : derived_types)
                type_list.append(*(dot::string) der_type->name);
        }

        pipeline.replace_root(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("newRoot", "$doc")));

        bsoncxx::builder::basic::document custom_sort{};
        custom_sort.append(bsoncxx::builder::basic::concatenate(query->sort_.view()));
        custom_sort.append(bsoncxx::builder::basic::kvp("_key", 1));
        custom_sort.append(bsoncxx::builder::basic::kvp("_dataset", -1));
        custom_sort.append(bsoncxx::builder::basic::kvp("_id", -1));

        pipeline.sort(custom_sort.view());

        type_filter.append(bsoncxx::builder::basic::kvp("_t",
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$in", type_list.view()))));
        pipeline.match(type_filter.view());

        if (query->select_.is_empty())
        {
            context_base context = this->context;
            return make_object_cursor_wrapper(std::move(get_collection(query->type_).aggregate(pipeline)),
                [context](const bsoncxx::document::view& item)->dot::object
                {
                    bson_record_serializer serializer = make_bson_record_serializer();
                    record_base record = (record_base)serializer->deserialize(item);

                    record->init(context);
                    return record;
                }
            );
        }
        else
        {
            bsoncxx::builder::basic::document select_list{};
            for (dot::field_info prop : query->select_)
                select_list.append(bsoncxx::builder::basic::kvp((std::string&)*(dot::string) prop->name, 1));
            select_list.append(bsoncxx::builder::basic::kvp("_key", 1));

            pipeline.project(select_list.view());

            context_base context = this->context;
            return make_object_cursor_wrapper(std::move(get_collection(query->type_).aggregate(pipeline)),
                [context, query](const bsoncxx::document::view& item)->dot::object
                {
                    bson_record_serializer serializer = make_bson_record_serializer();
                    dot::object record = serializer->deserialize_tuple(item, query->select_, query->element_type_);
                    return record;
                }
            );

        }
    }

    void mongo_data_source_data_impl::delete_record(key_base key, dot::object_id delete_in)
    {
        // Create delete marker with the specified key
        auto record = make_delete_marker();
        record->get_key() = key->get_value();

        // Get collection
        auto collection = get_collection(key->get_type());

        // This method guarantees that dot::object_ids will be in strictly increasing
        // order for this instance of the data source class always, and across
        // all processes and machine if they are not created within the same
        // second.
        auto object_id = create_ordered_object_id();
        record->id = object_id;

        // Assign dataset and then initialize, as the results of
        // initialization may depend on record.data_set
        record->data_set = delete_in;

        // Serialize record.
        bson_record_serializer serializer = make_bson_record_serializer();
        bson_writer writer = make_bson_writer();
        serializer->serialize(writer, record);

        // By design, insert will fail if dot::object_id is not unique within the collection
        //collection.insert_one(writer->view());

        bsoncxx::builder::basic::document doc{}; //!! Temporary fix
        doc.append(bsoncxx::builder::basic::kvp("_t", *record->get_type()->name));
        doc.append(bsoncxx::builder::basic::kvp("_key", *key->get_value()));
        doc.append(bsoncxx::builder::concatenate(writer->view()));

        collection.insert_one(doc.view());
    }
}
