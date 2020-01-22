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
#include <dc/types/record/temporal_id.hpp>

namespace dc
{
    temporal_id temporal_id::empty = temporal_id();

    temporal_id::temporal_id()
    {
        static const char empty_buffer[12] = { 0 };
        id_ = bsoncxx::oid(empty_buffer, 12);
    }

    temporal_id::temporal_id(bsoncxx::oid id) : id_(id) {}

    temporal_id::temporal_id(dot::object obj)
    {
        id_ = ((dot::struct_wrapper<temporal_id>)obj)->id_;
    }

    temporal_id::temporal_id(dot::string str)
        : id_(*str)
    {}

    temporal_id::temporal_id(const char* bytes, std::size_t len)
        : id_(bytes, len)
    {
    }

    temporal_id::temporal_id(dot::local_date_time value)
    {
        char bytes[12] = { 0 };

        boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
        boost::posix_time::time_duration d = (boost::posix_time::ptime)value - epoch;
        int64_t seconds = d.total_seconds();
        std::memcpy(bytes, &seconds, sizeof(seconds));
        id_ = bsoncxx::oid(bytes, 12);
    }

    bool temporal_id::is_empty()
    {
        return *id_.bytes() == 0; // TODO check
    }

    temporal_id temporal_id::generate_new_id()
    {
        return bsoncxx::oid();
    }

    dot::byte_array temporal_id::to_byte_array()
    {
        return new dot::byte_array_impl(id_.bytes(), 12);
    }

    dot::string temporal_id::to_string() const
    {
        return id_.to_string();
    }

    bool temporal_id::operator==(const temporal_id& rhs) const
    {
        return id_ == rhs.id_;
    }

    bool temporal_id::operator!=(const temporal_id& rhs) const
    {
        return id_ != rhs.id_;
    }

    bool temporal_id::operator>=(const temporal_id& rhs) const
    {
        return id_ >= rhs.id_;
    }

    bool temporal_id::operator>(const temporal_id& rhs) const
    {
        return id_ > rhs.id_;
    }

    bool temporal_id::operator<=(const temporal_id& rhs) const
    {
        return id_ <= rhs.id_;
    }

    bool temporal_id::operator<(const temporal_id& rhs) const
    {
        return id_ < rhs.id_;
    }

    void temporal_id::serialize(dot::tree_writer_base writer, dot::object obj)
    {
        writer->write_start_value();
        //writer->write_value((dot::object_id) ((temporal_id) obj).oid());
        writer->write_value(((temporal_id) obj).to_byte_array());
        writer->write_end_value();
    }

    dot::object temporal_id::deserialize(dot::object value, dot::type type)
    {
        dot::type value_type = value->get_type();

        if (value_type->equals(dot::typeof<dot::object_id>()))
        {
            return temporal_id(((dot::object_id) value).oid());
        }
        if (value_type->equals(dot::typeof<dot::byte_array>()))
        {
            dot::byte_array arr = (dot::byte_array) value;
            return temporal_id(arr->get_data(), arr->count());
        }
        if (value_type->equals(dot::typeof<dot::string>()))
        {
            return temporal_id((dot::string) value);
        }

        throw dot::exception("Couldn't construct temporal_id from " + value_type->name());
    }
}
