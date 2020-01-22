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
#include <dc/platform/context/context_base.hpp>
#include <dc/types/record/delete_marker.hpp>

#include <dot/mongo/mongo_db/mongo/collection.hpp>
#include <dot/mongo/mongo_db/query/query.hpp>

namespace dc
{
    record_base mongo_data_source_data_impl::load_or_null(dot::object_id id, dot::type dataType)
    {
        auto revisionTimeConstraint = get_revision_time_constraint();
        if (revisionTimeConstraint != nullptr)
        {
            // If RevisionTimeConstraint is not null, return null for any
            // id that is not strictly before the constraint dot::object_id
            if (id >= revisionTimeConstraint.value()) return nullptr;
        }

        dot::query query = dot::make_query(get_collection(dataType), dataType);
        dot::object_cursor_wrapper_base cursor = query->where(new dot::operator_wrapper_impl("_id", "$eq", id))
            ->limit(1)
            ->get_cursor()
            ;

        if (cursor->begin() != cursor->end())
        {
            dot::object obj = *(cursor->begin());
            if (!obj.is<delete_marker>())
                return (record_base)obj;
        }

        return nullptr;
    }

    record_base mongo_data_source_data_impl::reload_or_null(key_base key, dot::object_id loadFrom)
    {
        // Get lookup list by expanding the list of parents to arbitrary depth
        // with duplicates and cyclic references removed

        dot::hash_set<dot::object_id> lookup_set = get_data_set_lookup_list(loadFrom);
        dot::list<dot::object_id> lookup_list = dot::make_list<dot::object_id>(std::vector<dot::object_id>(lookup_set->begin(), lookup_set->end()));

        // dot::string key in semicolon delimited format used in the lookup
        dot::string key_value = key->to_string();

        dot::type record_type = dot::typeof<record_base>();

        dot::query query = dot::make_query(get_collection(key->get_type()), key->get_type());
        dot::object_cursor_wrapper_base cursor = query
            ->where(new dot::operator_wrapper_impl("_key", "$eq", key_value))
            ->where(new dot::operator_wrapper_impl("_dataset", "$in", lookup_list))
            ->sort_by_descending(record_type->get_field("_dataset"))
            ->then_by_descending(record_type->get_field("_id"))
            ->limit(1)
            ->get_cursor()
            ;

        if (cursor->begin() != cursor->end())
        {
            dot::object obj = *(cursor->begin());
            if (!obj.is<delete_marker>())
                return (record_base)obj;
        }

        return nullptr;

    }

    void mongo_data_source_data_impl::save(record_base record, dot::object_id saveTo)
    {
        check_not_read_only();

        auto collection = get_collection(record->get_type());

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

        // Assign id and data_set, and only then initialize, because
        // initialization code may use record.id and record.data_set
        record->id = objectId;
        record->data_set = saveTo;
        record->init(Context);

        collection->insert_one(record);
    }

    mongo_query mongo_data_source_data_impl::get_query(dot::object_id dataSet, dot::type type)
    {
        return make_mongo_query(get_collection(type), type, this, dataSet);
    }


    void mongo_data_source_data_impl::delete_record(key_base key, dot::object_id deleteIn)
    {
        dot::collection collection = get_collection(key->get_type());

        delete_marker record = make_delete_marker(key);

        dot::object_id objectId = create_ordered_object_id();

        // dot::object_id of the record must be strictly later
        // than dot::object_id of the dataset where it is stored
        if (objectId <= deleteIn)
            throw dot::exception(dot::string::format(
                "Attempting to save a record with dot::object_id={0} that is later "
                "than dot::object_id={1} of the dataset where it is being saved.", objectId.to_string(), deleteIn.to_string()));

        // Assign id and data_set, and only then initialize, because
        // initialization code may use record.id and record.data_set
        record->id = objectId;
        record->data_set = deleteIn;

        collection->insert_one(record);

    }
}
