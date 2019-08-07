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
#include <dc/serialization/BsonWriter.hpp>
#include <dc/serialization/BsonRecordSerializer.hpp>
#include <dc/platform/context/context_base.hpp>
#include <dc/platform/query/Query.hpp>
#include <dc/platform/cursor/CursorWrapper.hpp>
#include <dc/types/record/DeleteMarker.hpp>

#include <mongocxx/collection.hpp>

#include <bsoncxx/json.hpp>

namespace dc
{
    record_base mongo_data_source_data_impl::load_or_null(dot::object_id id, dot::type_t dataType)
    {
        auto revisionTimeConstraint = get_revision_time_constraint();
        if (revisionTimeConstraint != nullptr)
        {
            // If RevisionTimeConstraint is not null, return null for any
            // ID that is not strictly before the constraint dot::object_id
            if (id >= revisionTimeConstraint.value()) return nullptr;
        }

        bsoncxx::builder::basic::document filter{};
        filter.append(bsoncxx::builder::basic::kvp("_id", id.oid()));

        bsoncxx::stdx::optional<bsoncxx::document::value> res = GetCollection(dataType).find_one(filter.view());
        if (res != bsoncxx::stdx::nullopt)
        {
            BsonRecordSerializer serializer = make_BsonRecordSerializer();
            record_base record = (record_base) serializer->Deserialize(res->view());
            record->Init(Context);
            return record;
        }

        return nullptr;
    }

    record_base mongo_data_source_data_impl::reload_or_null(key_base key, dot::object_id loadFrom)
    {
        // Get lookup list by expanding the list of parents to arbitrary depth
        // with duplicates and cyclic references removed
        dot::hash_set<dot::object_id> lookup_list = get_data_set_lookup_list(loadFrom);

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

        mongocxx::cursor res = GetCollection(key->type()).aggregate(query);

        std::string s = bsoncxx::to_json(query.view());

        if (res.begin() != res.end())
        {
            BsonRecordSerializer serializer = make_BsonRecordSerializer();
            record_base result = (record_base) serializer->Deserialize(*res.begin());
            result->Init(Context);

            // Check not only for null but also for the delete marker
            if (!result.is<DeleteMarker>())
                return result;
        }

        return nullptr;
    }

    void mongo_data_source_data_impl::save(record_base record, dot::object_id saveTo)
    {
        check_not_read_only();

        auto collection = GetCollection(record->type());

        // This method guarantees that dot::object_ids will be in strictly increasing
        // order for this instance of the data source class always, and across
        // all processes and machine if they are not created within the same
        // second.
        dot::object_id objectId = create_ordered_object_id();

        // dot::object_id of the record must be strictly later
        // than dot::object_id of the dataset where it is stored
        if (objectId <= saveTo)
            throw dot::exception(dot::string::format(
                "Attempting to save a record with dot::object_id={0} that is later "
                "than dot::object_id={1} of the dataset where it is being saved.", objectId.to_string(), saveTo.to_string()));

        // Assign ID and data_set, and only then initialize, because
        // initialization code may use record.ID and record.data_set
        record->ID = objectId;
        record->data_set = saveTo;
        record->Init(Context);

        // Serialize record.
        BsonRecordSerializer serializer = make_BsonRecordSerializer();
        BsonWriter writer = make_BsonWriter();
        serializer->Serialize(writer, record);

        // By design, insert will fail if dot::object_id is not unique within the collection
        //collection.insert_one(writer->view());

        bsoncxx::builder::basic::document doc{}; //!! Temporary fix
        doc.append(bsoncxx::builder::basic::kvp("_t", * record->type()->name) );
        doc.append(bsoncxx::builder::basic::kvp("_key", * record->get_key()));
        doc.append(bsoncxx::builder::concatenate(writer->view()));

        collection.insert_one(doc.view());
    }

    query mongo_data_source_data_impl::get_query(dot::object_id dataSet, dot::type_t type)
    {
        return make_query(this, dataSet, type);
    }

    object_cursor_wrapper mongo_data_source_data_impl::load_by_query(query query)
    {
        // Get lookup list by expanding the list of parents to arbitrary depth
        // with duplicates and cyclic references removed
        dot::hash_set<dot::object_id> lookupList = get_data_set_lookup_list(query->data_set_);

        // filter by dataset
        bsoncxx::builder::basic::array filterDatasetList{};
        for (dot::object_id id : lookupList)
            filterDatasetList.append(id.oid());

        bsoncxx::builder::basic::document filterDataset{};
        filterDataset.append(bsoncxx::builder::basic::kvp("$in", filterDatasetList.view()));

        bsoncxx::builder::basic::document filter{};

        filter.append(bsoncxx::builder::basic::kvp("_dataset", filterDataset.view()));
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
        bsoncxx::builder::basic::document typeFilter;

        bsoncxx::builder::basic::array typeList;

        // append given type and DeleteMarker
        typeList.append(*(dot::string) query->type_->name);

        // append derived types
        dot::list<dot::type_t> derivedTypes = dot::type_impl::get_derived_types(query->type_);
        if (derivedTypes != nullptr)
        {
            for (dot::type_t derType : derivedTypes)
                typeList.append(*(dot::string) derType->name);
        }

        pipeline.replace_root(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("newRoot", "$doc")));

        bsoncxx::builder::basic::document custom_sort{};
        custom_sort.append(bsoncxx::builder::basic::concatenate(query->sort_.view()));
        custom_sort.append(bsoncxx::builder::basic::kvp("_key", 1));
        custom_sort.append(bsoncxx::builder::basic::kvp("_dataset", -1));
        custom_sort.append(bsoncxx::builder::basic::kvp("_id", -1));

        pipeline.sort(custom_sort.view());

        typeFilter.append(bsoncxx::builder::basic::kvp("_t",
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$in", typeList.view()))));
        pipeline.match(typeFilter.view());

        std::string s = bsoncxx::to_json(pipeline.view());

        if (query->select_.is_empty())
        {
            context_base context = this->Context;
            return make_object_cursor_wrapper(std::move(GetCollection(query->type_).aggregate(pipeline)),
                [context](const bsoncxx::document::view& item)->dot::object
                {
                    BsonRecordSerializer serializer = make_BsonRecordSerializer();
                    record_base record = (record_base)serializer->Deserialize(item);

                    record->Init(context);
                    return record;
                }
            );
        }
        else
        {
            bsoncxx::builder::basic::document selectList{};
            for (dot::field_info prop : query->select_)
                selectList.append(bsoncxx::builder::basic::kvp((std::string&)*(dot::string) prop->name, 1));
            selectList.append(bsoncxx::builder::basic::kvp("_key", 1));

            pipeline.project(selectList.view());

            context_base context = this->Context;
            return make_object_cursor_wrapper(std::move(GetCollection(query->type_).aggregate(pipeline)),
                [context, query](const bsoncxx::document::view& item)->dot::object
                {
                    BsonRecordSerializer serializer = make_BsonRecordSerializer();
                    dot::object record = serializer->DeserializeTuple(item, query->select_, query->element_type_);
                    return record;
                }
            );

        }
    }

    void mongo_data_source_data_impl::delete_record(key_base key, dot::object_id deleteIn)
    {
        // Create delete marker with the specified key
        auto record = make_DeleteMarker();
        record->get_key() = key->getValue();

        // Get collection
        auto collection = GetCollection(key->type());

        // This method guarantees that dot::object_ids will be in strictly increasing
        // order for this instance of the data source class always, and across
        // all processes and machine if they are not created within the same
        // second.
        auto objectId = create_ordered_object_id();
        record->ID = objectId;

        // Assign dataset and then initialize, as the results of
        // initialization may depend on record.data_set
        record->data_set = deleteIn;

        // Serialize record.
        BsonRecordSerializer serializer = make_BsonRecordSerializer();
        BsonWriter writer = make_BsonWriter();
        serializer->Serialize(writer, record);

        // By design, insert will fail if dot::object_id is not unique within the collection
        //collection.insert_one(writer->view());

        bsoncxx::builder::basic::document doc{}; //!! Temporary fix
        doc.append(bsoncxx::builder::basic::kvp("_t", *record->type()->name));
        doc.append(bsoncxx::builder::basic::kvp("_key", *key->getValue()));
        doc.append(bsoncxx::builder::concatenate(writer->view()));

        collection.insert_one(doc.view());

    }
}
