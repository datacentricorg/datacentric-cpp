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

#include <dot/mongo/precompiled.hpp>
#include <dot/mongo/implement.hpp>
#include <dot/mongo/mongo_db/bson/object_id.hpp>
#include <dot/system/string.hpp>
#include <dot/noda_time/local_date_time.hpp>
#include "dot/system/object.hpp"

namespace dot
{
    object_id object_id::empty = object_id();

    object_id::object_id()
    {
        static const char empty_buffer[12] = { 0 };
        id_ = bsoncxx::oid(empty_buffer, 12);
    }

    object_id::object_id(bsoncxx::oid id) : id_(id) {}

    object_id::object_id(dot::object obj)
    {
        id_ = ((struct_wrapper<object_id>)obj)->id_;
    }

    object_id::object_id(dot::string str)
        : id_(*str)
    {}

    object_id::object_id(const char* bytes, std::size_t len)
        : id_(bytes, len)
    {
    }

    object_id::object_id(dot::local_date_time value)
    {
        char bytes[12] = { 0 };

        boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
        boost::posix_time::time_duration d = (boost::posix_time::ptime)value - epoch;
        int64_t seconds = d.total_seconds();
        std::memcpy(bytes, &seconds, sizeof(seconds));
        id_ = bsoncxx::oid(bytes, 12);
    }

    bool object_id::is_empty()
    {
        return *id_.bytes() == 0; // TODO check
    }

    object_id object_id::generate_new_id()
    {
        return bsoncxx::oid();
    }

    dot::string object_id::to_string() const
    {
        return id_.to_string();
    }

    bool object_id::operator==(const object_id& rhs) const
    {
        return id_ == rhs.id_;
    }

    bool object_id::operator!=(const object_id& rhs) const
    {
        return id_ != rhs.id_;
    }

    bool object_id::operator>=(const object_id& rhs) const
    {
        return id_ >= rhs.id_;
    }

    bool object_id::operator<=(const object_id& rhs) const
    {
        return id_ <= rhs.id_;
    }

    bool object_id::operator<(const object_id& rhs) const
    {
        return id_ < rhs.id_;
    }
}
