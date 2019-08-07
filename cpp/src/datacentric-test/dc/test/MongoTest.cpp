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
#include <boost/optional.hpp>

#include <approvals/ApprovalTests.hpp>
#include <approvals/Catch.hpp>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>


namespace dc
{

    struct TestStruct
    {
        int i;
        double d;
    };

    TEST_CASE("Smoke")
    {

        TestStruct ts{15, 42.42};

        mongocxx::instance inst{};
        mongocxx::client conn{ mongocxx::uri{} };
        auto coll = conn["test"]["coll"];
        coll.drop();

        mongocxx::cursor cursor = coll.find(bsoncxx::builder::stream::document{}.view());
        REQUIRE(cursor.begin() == cursor.end());

        bsoncxx::builder::stream::document doc;
        doc << "i" << ts.i << "d" << ts.d;

        REQUIRE(coll.insert_one(doc.view())->result().inserted_count() == 1);

        bsoncxx::builder::stream::document key;
        key << "i" << ts.i;

        auto val = coll.find_one(key.view());
        REQUIRE((bool)val);

        TestStruct ts_loaded;
        ts_loaded.i = val->view()["i"].get_int32();
        ts_loaded.d = val->view()["d"].get_double();

        REQUIRE(ts.d == ts_loaded.d);
        REQUIRE(ts.i == ts_loaded.i);

    }
}
