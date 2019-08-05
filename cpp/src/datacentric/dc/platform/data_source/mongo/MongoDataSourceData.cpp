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

#include <dc/implement.hpp>
#include <dc/platform/data_source/mongo/MongoDataSourceData.hpp>
#include <dc/serialization/BsonWriter.hpp>
#include <dc/serialization/BsonRecordSerializer.hpp>
#include <dc/platform/context/IContext.hpp>
#include <dc/platform/query/Query.hpp>
#include <dc/platform/cursor/CursorWrapper.hpp>
#include <dc/types/record/DeleteMarker.hpp>

#include <mongocxx/collection.hpp>

#include <bsoncxx/json.hpp>

namespace dc
{
    RecordType MongoDataSourceDataImpl::LoadOrNull(ObjectId id, dot::type_t dataType)
    {
        auto revisionTimeConstraint = GetRevisionTimeConstraint();
        if (revisionTimeConstraint != nullptr)
        {
            // If RevisionTimeConstraint is not null, return null for any
            // ID that is not strictly before the constraint ObjectId
            if (id >= revisionTimeConstraint.getValue()) return nullptr;
        }

        bsoncxx::builder::basic::document filter{};
        filter.append(bsoncxx::builder::basic::kvp("_id", id._id));

        bsoncxx::stdx::optional<bsoncxx::document::value> res = GetCollection(dataType).find_one(filter.view());
        if (res != bsoncxx::stdx::nullopt)
        {
            BsonRecordSerializer serializer = new_BsonRecordSerializer();
            RecordType record = (RecordType) serializer->Deserialize(res->view());
            record->Init(Context);
            return record;
        }

        return nullptr;
    }

    RecordType MongoDataSourceDataImpl::ReloadOrNull(KeyType key, ObjectId loadFrom)
    {
        // Get lookup list by expanding the list of parents to arbitrary depth
        // with duplicates and cyclic references removed
        dot::IEnumerable<ObjectId> lookupList = GetDataSetLookupList(loadFrom);

        // dot::string key in semicolon delimited format used in the lookup
        dot::string keyValue = key->ToString();

        // Look for exact match of the key in the specified list of datasets,
        // then order by dataset ObjectId in descending order
        bsoncxx::builder::basic::document filter{};
        filter.append(bsoncxx::builder::basic::kvp("_key", *keyValue));

        // filter by dataset
        bsoncxx::builder::basic::array filterDatasetList{};
        for (ObjectId id : lookupList)
            filterDatasetList.append(id._id);

        bsoncxx::builder::basic::document filterDataset{};
        filterDataset.append(bsoncxx::builder::basic::kvp("$in", filterDatasetList.view()));

        filter.append(bsoncxx::builder::basic::kvp("_dataset", filterDataset.view()));

        bsoncxx::builder::basic::document sort_ds{};
        bsoncxx::builder::basic::document sort_id{};
        sort_ds.append(bsoncxx::builder::basic::kvp("_dataset", -1));
        sort_id.append(bsoncxx::builder::basic::kvp("_id", -1));

        mongocxx::pipeline query;
        query.match(filter.view());
        query.sort(sort_ds.view());
        query.sort(sort_id.view());

        query.limit(1); // Only the first document is needed

        mongocxx::cursor res = GetCollection(key->GetType()).aggregate(query);

        std::string s = bsoncxx::to_json(query.view());

        if (res.begin() != res.end())
        {
            BsonRecordSerializer serializer = new_BsonRecordSerializer();
            RecordType result = (RecordType) serializer->Deserialize(*res.begin());
            result->Init(Context);

            // Check not only for null but also for the delete marker
            if (!result.is<DeleteMarker>())
                return result;
        }

        return nullptr;
    }

    void MongoDataSourceDataImpl::Save(RecordType record, ObjectId saveTo)
    {
        CheckNotReadOnly();

        auto collection = GetCollection(record->GetType());

        // This method guarantees that ObjectIds will be in strictly increasing
        // order for this instance of the data source class always, and across
        // all processes and machine if they are not created within the same
        // second.
        ObjectId objectId = CreateOrderedObjectId();

        // ObjectId of the record must be strictly later
        // than ObjectId of the dataset where it is stored
        if (objectId <= saveTo)
            throw dot::new_Exception(dot::string::Format(
                "Attempting to save a record with ObjectId={0} that is later "
                "than ObjectId={1} of the dataset where it is being saved.", objectId.ToString(), saveTo.ToString()));

        // Assign ID and DataSet, and only then initialize, because
        // initialization code may use record.ID and record.DataSet
        record->ID = objectId;
        record->DataSet = saveTo;
        record->Init(Context);

        // Serialize record.
        BsonRecordSerializer serializer = new_BsonRecordSerializer();
        BsonWriter writer = new_BsonWriter();
        serializer->Serialize(writer, record);

        // By design, insert will fail if ObjectId is not unique within the collection
        //collection.insert_one(writer->view());

        bsoncxx::builder::basic::document doc{}; //!! Temporary fix
        doc.append(bsoncxx::builder::basic::kvp("_t", * record->GetType()->Name) );
        doc.append(bsoncxx::builder::basic::kvp("_key", * record->getKey()));
        doc.append(bsoncxx::builder::concatenate(writer->view()));

        collection.insert_one(doc.view());
    }

    IQuery MongoDataSourceDataImpl::GetQuery(ObjectId dataSet, dot::type_t type)
    {
        return new_Query(this, dataSet, type);
    }

    dot::IObjectEnumerable MongoDataSourceDataImpl::LoadByQuery(IQuery query)
    {
        Query q = (Query)query;

        // Get lookup list by expanding the list of parents to arbitrary depth
        // with duplicates and cyclic references removed
        dot::IEnumerable<ObjectId> lookupList = GetDataSetLookupList(q->dataSet_);

        // filter by dataset
        bsoncxx::builder::basic::array filterDatasetList{};
        for (ObjectId id : lookupList)
            filterDatasetList.append(id._id);

        bsoncxx::builder::basic::document filterDataset{};
        filterDataset.append(bsoncxx::builder::basic::kvp("$in", filterDatasetList.view()));

        bsoncxx::builder::basic::document filter{};

        filter.append(bsoncxx::builder::basic::kvp("_dataset", filterDataset.view()));
        filter.append(bsoncxx::builder::basic::concatenate(q->where_.view()));

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
        typeList.append(*(dot::string) q->type_->Name);

        // append derived types
        dot::List<dot::type_t> derivedTypes = dot::type_tImpl::GetDerivedTypes(q->type_);
        if (derivedTypes != nullptr)
        {
            for (dot::type_t derType : derivedTypes)
                typeList.append(*(dot::string) derType->Name);
        }

        pipeline.replace_root(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("newRoot", "$doc")));

        bsoncxx::builder::basic::document custom_sort{};
        custom_sort.append(bsoncxx::builder::basic::concatenate(q->sort_.view()));
        custom_sort.append(bsoncxx::builder::basic::kvp("_key", 1));
        custom_sort.append(bsoncxx::builder::basic::kvp("_dataset", -1));
        custom_sort.append(bsoncxx::builder::basic::kvp("_id", -1));

        pipeline.sort(custom_sort.view());

        typeFilter.append(bsoncxx::builder::basic::kvp("_t",
            bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$in", typeList.view()))));
        pipeline.match(typeFilter.view());

        std::string s = bsoncxx::to_json(pipeline.view());

        if (q->select_.IsEmpty())
        {
            IContext context = this->Context;
            return new_ObjectCursorWrapper(std::move(GetCollection(q->type_).aggregate(pipeline)),
                [context](const bsoncxx::document::view& item)->dot::object
                {
                    BsonRecordSerializer serializer = new_BsonRecordSerializer();
                    RecordType record = (RecordType)serializer->Deserialize(item);

                    record->Init(context);
                    return record;
                }
            );
        }
        else
        {
            bsoncxx::builder::basic::document selectList{};
            for (dot::field_info prop : q->select_)
                selectList.append(bsoncxx::builder::basic::kvp((std::string&)*(dot::string) prop->Name, 1));
            selectList.append(bsoncxx::builder::basic::kvp("_key", 1));

            pipeline.project(selectList.view());

            IContext context = this->Context;
            return new_ObjectCursorWrapper(std::move(GetCollection(q->type_).aggregate(pipeline)),
                [context, q](const bsoncxx::document::view& item)->dot::object
                {
                    BsonRecordSerializer serializer = new_BsonRecordSerializer();
                    dot::object record = serializer->DeserializeTuple(item, q->select_, q->elementType_);
                    return record;
                }
            );

        }
    }

    void MongoDataSourceDataImpl::Delete(KeyType key, ObjectId deleteIn)
    {
        // Create delete marker with the specified key
        auto record = new_DeleteMarker();
        record->getKey() = key->getValue();

        // Get collection
        auto collection = GetCollection(key->GetType());

        // This method guarantees that ObjectIds will be in strictly increasing
        // order for this instance of the data source class always, and across
        // all processes and machine if they are not created within the same
        // second.
        auto objectId = CreateOrderedObjectId();
        record->ID = objectId;

        // Assign dataset and then initialize, as the results of
        // initialization may depend on record.DataSet
        record->DataSet = deleteIn;

        // Serialize record.
        BsonRecordSerializer serializer = new_BsonRecordSerializer();
        BsonWriter writer = new_BsonWriter();
        serializer->Serialize(writer, record);

        // By design, insert will fail if ObjectId is not unique within the collection
        //collection.insert_one(writer->view());

        bsoncxx::builder::basic::document doc{}; //!! Temporary fix
        doc.append(bsoncxx::builder::basic::kvp("_t", *record->GetType()->Name));
        doc.append(bsoncxx::builder::basic::kvp("_key", *key->getValue()));
        doc.append(bsoncxx::builder::concatenate(writer->view()));

        collection.insert_one(doc.view());

    }
}
