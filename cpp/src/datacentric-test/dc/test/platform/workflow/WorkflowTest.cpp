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


#define DC_INCLUDE_MONGO_DATA_SOURCE


#include <dc/test/implement.hpp>
#include <approvals/ApprovalTests.hpp>
#include <approvals/Catch.hpp>
#include <dc/platform/workflow/WorkflowData.hpp>
#include <dc/platform/workflow/JobKey.hpp>
#include <dc/platform/workflow/PhaseKey.hpp>
#include <dc/platform/workflow/TaskKey.hpp>
#include <dc/platform/workflow/JobTypeKey.hpp>
#include <dc/platform/workflow/LinkedTypeKey.hpp>

#include <dc/serialization/BsonWriter.hpp>
#include <dc/serialization/BsonRecordSerializer.hpp>
#include <dc/serialization/JsonWriter.hpp>
#include <dc/serialization/JsonRecordSerializer.hpp>

//#include <dc/platform/data_source/mongo/MongoFactory.hpp>
#include <dc/platform/data_source/IDataSource.hpp>
#include <dc/implement.hpp>
#include <dc/platform/context/Context.hpp>

#include <dc/platform/data_source/mongo/QueryBuilder.hpp>

#include <dc/platform/dataset/DataSetKey.hpp>
#include <dc/platform/dataset/DataSetData.hpp>

#include <bsoncxx/json.hpp>

namespace dc
{
    TEST_CASE("JsonRecordSerializer")
    {
        WorkflowData wf = new_WorkflowData();
        wf->ID = ObjectId::GenerateNewId();
        wf->DataSet = ObjectId::GenerateNewId();
        wf->WorkflowID = "ABCBB";
        wf->Workflow = new WorkflowKeyImpl();
        wf->Workflow->WorkflowID = "CBA";
        wf->LDate = dot::local_date(2005, 1, 1);
        wf->LTime = dot::local_time(12, 10, 20, 444);
        wf->LDateTime = dot::local_date_time(2005, 1, 1, 12, 10, 20, 444);
        wf->dbl = 1.1;
        wf->Workflows = dot::new_List<WorkflowKey>();
        wf->Workflows->Add(new WorkflowKeyImpl());
        wf->Workflows[0]->WorkflowID = "ABC";

        wf->data = new_WorkflowData();
        wf->data->ID = ObjectId::GenerateNewId();
        wf->data->DataSet = ObjectId::GenerateNewId();
        wf->data->LDate = dot::local_date(2005, 1, 1);

        JsonWriter jsonWriter = new_JsonWriter();
        JsonRecordSerializer serializer = new_JsonRecordSerializer();

        serializer->Serialize(jsonWriter, wf);
        dot::string json = jsonWriter->ToString();

        rapidjson::Document doc;
        doc.Parse(json->data());

        //!! TODO uncomment after fixing seralization of _t and _key
        //WorkflowData wf2 = (WorkflowData) serializer->Deserialize(doc);
        //wf2->ToString();
    }
}

/*
namespace dc
{
    TEST_CASE("Smoke")
    {
        std::cout << "Hello world" << std::endl;

        TaskKey k = new TaskKeyImpl;
        std::string sk = * k->ToString();

        k->Workflow = new_WorkflowKey();
        k->Workflow->WorkflowID = "wid";

        k->Job = new_JobKey();

        k->Job->JobType = new_JobTypeKey();
        k->Job->JobType->JobTypeID = "jtid";

        k->Job->LinkedType = new_LinkedTypeKey();
        k->Job->LinkedType->TypeID = "tid";

        k->Job->LinkedTo = "lto";

        std::string sk2 = *k->ToString();
        TaskKey k2 = new TaskKeyImpl;
        k2->PopulateFrom(sk2);
        std::string sk3 = *k2->ToString();

        WorkflowData wf = new_WorkflowData();
        wf->WorkflowID = "ABCBB";
        wf->Workflow = new WorkflowKeyImpl();
        wf->Workflow->WorkflowID = "CBA";
        wf->LDate = LocalDate(2005, 1, 1);
        wf->LTime = LocalTime(12, 10, 20, 444);
        wf->LDateTime = LocalDateTime(2005, 1, 1, 12, 10, 20, 444);
        wf->Workflows = new_List<WorkflowKey>();
        wf->Workflows->Add(new WorkflowKeyImpl());
        wf->Workflows[0]->WorkflowID = "ABC";

        wf->data = new_WorkflowData();
        wf->data->LDate = LocalDate(2005, 1, 1);

        IObjectEnumerable en = (IObjectEnumerable)(List<WorkflowKey>)wf->Workflows;


        dot::string str = wf->Workflows[0]->WorkflowID;
        REQUIRE(str == "ABC");

        BsonWriter wr = new_BsonWriter();
        wr->WriteStartDocument(wf->type()->name);
        wf->SerializeTo(wr);
        wr->WriteEndDocument(wf->type()->name);
        dot::string s = wr->ToString();
        // Key == dot::string

        wf->Workflow->type();

        Context context = new_Context();
        context->Init("mongodb://localhost:27017/test", "test");
        MongoDataSource ds = (MongoDataSource) (IDataSource) context->DataSource;


        wf->_id = ds->GetId();
        wf->_id = LocalDateTime(2005, 1, 1, 12, 10, 20);
        bool bb2 = wf->_id.GetTimeStamp() == LocalDateTime(2005, 1, 1, 12, 10, 20);
        ds->Save(wf);

        std::string s2 = bsoncxx::to_json((make_prop(&WorkflowDataImpl::WorkflowID) != "ABCC"
            && make_prop(&WorkflowDataImpl::Workflow)->*make_prop(&WorkflowKeyImpl::WorkflowID) == wf->Workflow->WorkflowID
            && make_prop(&WorkflowDataImpl::LDate) == LocalDate(2005, 1, 1)
            && make_prop(&WorkflowDataImpl::LTime) == LocalTime(12, 10, 20, 444)
            && make_prop(&WorkflowDataImpl::LDateTime) == LocalDateTime(2005, 1, 1, 12, 10, 20, 444)
            && make_prop(&WorkflowDataImpl::dbl) == 0.
            && make_prop(&WorkflowDataImpl::_id) == LocalDateTime(2005, 1, 1, 12, 10, 15, 415))
            .operator bsoncxx::v_noabi::document::view_or_value());

        WorkflowData wf2 = wf->LoadOrNull(context);

        List<Data> wf3 = ds->LoadByQuery<WorkflowData>(make_prop(&WorkflowDataImpl::WorkflowID) != "ABCC"
                                                        && make_prop(&WorkflowDataImpl::Workflow) ->* make_prop(&WorkflowKeyImpl::WorkflowID) == wf->Workflow->WorkflowID
                                                        && make_prop(&WorkflowDataImpl::LDate) == LocalDate(2005, 1, 1)
                                                        && make_prop(&WorkflowDataImpl::LTime) == LocalTime(12, 10, 20, 444)
                                                        && make_prop(&WorkflowDataImpl::LDateTime) == LocalDateTime(2005, 1, 1, 12, 10, 20, 444)
                                                        && make_prop(&WorkflowDataImpl::dbl) == 0.
                                                        && make_prop(&WorkflowDataImpl::_id) < LocalDateTime(2005, 1, 1, 12, 10, 21)
            );
        List<WorkflowData> wf4 = wf3.as<List<WorkflowData>>();
        WorkflowData wf5 = (WorkflowData)wf3[0];

        bool bb = wf5->LDateTime == LocalDateTime(2005, 1, 1, 12, 10, 20, 444);

        BsonRecordSerializer sser = new_BsonRecordSerializer();
        BsonWriter writer = new_BsonWriter();
        sser->Serialize(writer, wf5);
        std::string ggg = * writer->ToString();
        std::cout << ggg << std::endl;

        BsonRecordSerializer sser2 = new_BsonRecordSerializer();
        BsonWriter writer2 = new_BsonWriter();
        sser2->Serialize(writer2, wf2);
        std::string ggg2 = *writer2->ToString();
    }

    TEST_CASE("DataSet")
    {
        Context context = new_Context();
        context->Init("mongodb://localhost:27017/test", "test");
        MongoDataSource ds = (MongoDataSource) (IDataSource) context->DataSource;

        // create datasets
        DataSetData dataSet = new_DataSetData();
        dataSet->DataSetID = "Common";
        ds->Save(dataSet);

        DataSetData dataSet1 = new_DataSetData();
        dataSet1->DataSetID = "DataSet1";
        dataSet1->Parents = new_List<DataSetKey>();
        dataSet1->Parents->Add(new_DataSetKey());
        dataSet1->Parents[0]->DataSetID = dataSet->DataSetID;
        ds->Save(dataSet1);

        DataSetData dataSet_ = dataSet->LoadOrNull(context);

        // add workflows to Common dataset
        WorkflowData wf = new_WorkflowData();
        wf->WorkflowID = "DSWokrflow1";
        wf->dbl = 1.0;
        ds->Save(wf);

        WorkflowData wf2 = new_WorkflowData();
        wf2->WorkflowID = "DSWokrflow2";
        wf2->dbl = 2.0;
        ds->Save(wf2);

        // add workflows to DataSet1 dataset
        context->DataSetID = "DataSet1";
        wf->dbl = 3.0;
        ds->Save(wf);

        // load workflows
        context->DataSetID = "Common";
        wf = wf->LoadOrNull(context);
        REQUIRE(wf->dbl == 1.0);
        wf2 = wf2->LoadOrNull(context);
        REQUIRE(wf2->dbl == 2.0);

        context->DataSetID = "DataSet1";
        wf = wf->LoadOrNull(context);
        REQUIRE(wf->dbl == 3.0);
        wf2 = wf2->LoadOrNull(context);
        REQUIRE(wf2->dbl == 2.0);
    }
}
*/