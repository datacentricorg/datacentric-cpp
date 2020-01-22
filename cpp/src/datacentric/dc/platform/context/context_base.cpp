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
    void context_base_impl::set_data_source(data_source_data value)
    {
        data_source = value;
    }

    record context_base_impl::load(temporal_id id, dot::type data_type)
    {
        return data_source->load(id, data_type);
    }

    record context_base_impl::load_or_null(temporal_id id, dot::type data_type)
    {
        return data_source->load_or_null(id, data_type);
    }

    record context_base_impl::load(key key, temporal_id load_from)
    {
        return data_source->load(key, load_from);
    }

    record context_base_impl::load_or_null(key key, temporal_id load_from)
    {
        return data_source->load_or_null(key, load_from);
    }

    mongo_query context_base_impl::get_query(temporal_id load_from, dot::type data_type)
    {
        return data_source->get_query(load_from, data_type);
    }

    void context_base_impl::save_one(record record)
    {
        data_source->save_one(record, data_set);
    }

    void context_base_impl::save_one(record record, temporal_id save_to)
    {
        data_source->save_one(record, save_to);
    }

    void context_base_impl::save_many(dot::list<record> records)
    {
        data_source->save_many(records, data_set);
    }

    void context_base_impl::save_many(dot::list<record> records, temporal_id save_to)
    {
        data_source->save_many(records, save_to);
    }

    void context_base_impl::delete_record(key key)
    {
        data_source->delete_record(key, data_set);
    }

    void context_base_impl::delete_record(key key, temporal_id delete_in)
    {
        data_source->delete_record(key, delete_in);
    }

    void context_base_impl::delete_db()
    {
        data_source->delete_db();
    }

    temporal_id context_base_impl::get_common()
    {
        return data_source->get_common();
    }

    temporal_id context_base_impl::get_data_set(dot::string data_set_id)
    {
        return data_source->get_data_set(data_set_id, data_set);
    }

    temporal_id context_base_impl::get_data_set(dot::string data_set_id, temporal_id load_from)
    {
        return data_source->get_data_set(data_set_id, load_from);
    }

    temporal_id context_base_impl::get_data_set_or_empty(dot::string data_set_id)
    {
        return data_source->get_data_set_or_empty(data_set_id, data_set);
    }

    temporal_id context_base_impl::get_data_set_or_empty(dot::string data_set_id, temporal_id load_from)
    {
        return data_source->get_data_set_or_empty(data_set_id, load_from);
    }

    temporal_id context_base_impl::create_common()
    {
        return data_source->create_common();
    }

    temporal_id context_base_impl::create_common(data_set_flags flags)
    {
        return data_source->create_common(flags);
    }

    temporal_id context_base_impl::create_data_set(dot::string data_set_id)
    {
        return data_source->create_data_set(data_set_id, data_set);
    }

    temporal_id context_base_impl::create_data_set(dot::string data_set_id, temporal_id save_to)
    {
        return data_source->create_data_set(data_set_id, save_to);
    }

    temporal_id context_base_impl::create_data_set(dot::string data_set_id, dot::list<temporal_id> parentdata_sets)
    {
        return data_source->create_data_set(data_set_id, parentdata_sets, data_set);
    }

    temporal_id context_base_impl::create_data_set(dot::string data_set_id, dot::list<temporal_id> parentdata_sets, temporal_id save_to)
    {
        return data_source->create_data_set(data_set_id, parentdata_sets, save_to);
    }

    temporal_id context_base_impl::create_data_set(dot::string data_set_id, data_set_flags flags)
    {
        return data_source->create_data_set(data_set_id, flags, data_set);
    }

    temporal_id context_base_impl::create_data_set(dot::string data_set_id, data_set_flags flags, temporal_id save_to)
    {
        return data_source->create_data_set(data_set_id, flags, save_to);
    }

    temporal_id context_base_impl::create_data_set(dot::string data_set_id, dot::list<temporal_id> parent_data_sets, data_set_flags flags)
    {
        return data_source->create_data_set(data_set_id, parent_data_sets, flags, data_set);
    }

    temporal_id context_base_impl::create_data_set(dot::string data_set_id, dot::list<temporal_id> parent_data_sets, data_set_flags flags, temporal_id save_to)
    {
        return data_source->create_data_set(data_set_id, parent_data_sets, flags, save_to);
    }

    void context_base_impl::save_data_set(data_set_data value)
    {
        data_source->save_data_set(value, data_set);
    }

    void context_base_impl::save_data_set(data_set_data value, temporal_id save_to)
    {
        data_source->save_data_set(value, save_to);
    }
}
