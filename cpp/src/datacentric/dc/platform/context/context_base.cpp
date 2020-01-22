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
#include <dc/platform/context/context_base.hpp>
#include <dc/platform/data_source/data_source_data.hpp>
#include <dc/platform/data_source/data_source_key.hpp>

namespace dc
{
    void ContextBaseImpl::set_data_source(DataSource value)
    {
        data_source = value;
    }

    Record ContextBaseImpl::load(TemporalId id, dot::Type data_type)
    {
        return data_source->load(id, data_type);
    }

    Record ContextBaseImpl::load_or_null(TemporalId id, dot::Type data_type)
    {
        return data_source->load_or_null(id, data_type);
    }

    Record ContextBaseImpl::load(Key key, TemporalId load_from)
    {
        return data_source->load(key, load_from);
    }

    Record ContextBaseImpl::load_or_null(Key key, TemporalId load_from)
    {
        return data_source->load_or_null(key, load_from);
    }

    MongoQuery ContextBaseImpl::get_query(TemporalId load_from, dot::Type data_type)
    {
        return data_source->get_query(load_from, data_type);
    }

    void ContextBaseImpl::save_one(Record record)
    {
        data_source->save_one(record, data_set);
    }

    void ContextBaseImpl::save_one(Record record, TemporalId save_to)
    {
        data_source->save_one(record, save_to);
    }

    void ContextBaseImpl::save_many(dot::List<Record> records)
    {
        data_source->save_many(records, data_set);
    }

    void ContextBaseImpl::save_many(dot::List<Record> records, TemporalId save_to)
    {
        data_source->save_many(records, save_to);
    }

    void ContextBaseImpl::delete_record(Key key)
    {
        data_source->delete_record(key, data_set);
    }

    void ContextBaseImpl::delete_record(Key key, TemporalId delete_in)
    {
        data_source->delete_record(key, delete_in);
    }

    void ContextBaseImpl::delete_db()
    {
        data_source->delete_db();
    }

    TemporalId ContextBaseImpl::get_common()
    {
        return data_source->get_common();
    }

    TemporalId ContextBaseImpl::get_data_set(dot::String data_set_id)
    {
        return data_source->get_data_set(data_set_id, data_set);
    }

    TemporalId ContextBaseImpl::get_data_set(dot::String data_set_id, TemporalId load_from)
    {
        return data_source->get_data_set(data_set_id, load_from);
    }

    dot::Nullable<TemporalId> ContextBaseImpl::get_data_set_or_empty(dot::String data_set_id)
    {
        return data_source->get_data_set_or_empty(data_set_id, data_set);
    }

    dot::Nullable<TemporalId> ContextBaseImpl::get_data_set_or_empty(dot::String data_set_id, TemporalId load_from)
    {
        return data_source->get_data_set_or_empty(data_set_id, load_from);
    }

    TemporalId ContextBaseImpl::create_common()
    {
        return data_source->create_common();
    }

    TemporalId ContextBaseImpl::create_common(DataSetFlags flags)
    {
        return data_source->create_common(flags);
    }

    TemporalId ContextBaseImpl::create_data_set(dot::String data_set_id)
    {
        return data_source->create_data_set(data_set_id, data_set);
    }

    TemporalId ContextBaseImpl::create_data_set(dot::String data_set_id, TemporalId save_to)
    {
        return data_source->create_data_set(data_set_id, save_to);
    }

    TemporalId ContextBaseImpl::create_data_set(dot::String data_set_id, dot::List<TemporalId> parentdata_sets)
    {
        return data_source->create_data_set(data_set_id, parentdata_sets, data_set);
    }

    TemporalId ContextBaseImpl::create_data_set(dot::String data_set_id, dot::List<TemporalId> parentdata_sets, TemporalId save_to)
    {
        return data_source->create_data_set(data_set_id, parentdata_sets, save_to);
    }

    TemporalId ContextBaseImpl::create_data_set(dot::String data_set_id, DataSetFlags flags)
    {
        return data_source->create_data_set(data_set_id, flags, data_set);
    }

    TemporalId ContextBaseImpl::create_data_set(dot::String data_set_id, DataSetFlags flags, TemporalId save_to)
    {
        return data_source->create_data_set(data_set_id, flags, save_to);
    }

    TemporalId ContextBaseImpl::create_data_set(dot::String data_set_id, dot::List<TemporalId> parent_data_sets, DataSetFlags flags)
    {
        return data_source->create_data_set(data_set_id, parent_data_sets, flags, data_set);
    }

    TemporalId ContextBaseImpl::create_data_set(dot::String data_set_id, dot::List<TemporalId> parent_data_sets, DataSetFlags flags, TemporalId save_to)
    {
        return data_source->create_data_set(data_set_id, parent_data_sets, flags, save_to);
    }

    void ContextBaseImpl::save_data_set(DataSet value)
    {
        data_source->save_data_set(value, data_set);
    }

    void ContextBaseImpl::save_data_set(DataSet value, TemporalId save_to)
    {
        data_source->save_data_set(value, save_to);
    }
}
