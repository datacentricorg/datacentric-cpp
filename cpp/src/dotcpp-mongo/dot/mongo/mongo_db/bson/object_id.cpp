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
    object_id object_id::Empty = object_id();

    object_id::object_id()
    {
        static const char Empty[12] = { 0 };
        _id = bsoncxx::oid(Empty, 12);
    }

    object_id::object_id(bsoncxx::oid id) : _id(id) {}

    object_id::object_id(dot::object obj)
    {
        _id = ((struct_wrapper<object_id>)obj)->_id;
    }

    object_id::object_id(dot::string str)
        : _id(*str)
    {}

    object_id::object_id(const char* bytes, std::size_t len)
        : _id(bytes, len)
    {
    }

    object_id::object_id(dot::local_date_time value)
    {
        char bytes[12] = { 0 };

        boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
        boost::posix_time::time_duration d = (boost::posix_time::ptime)value - epoch;
        int64_t seconds = d.total_seconds();
        std::memcpy(bytes, &seconds, sizeof(seconds));
        _id = bsoncxx::oid(bytes, 12);
    }

    bool object_id::is_empty()
    {
        return *_id.bytes() == 0; // TODO check
    }

    object_id object_id::GenerateNewId()
    {
        return bsoncxx::oid();
    }

    dot::string object_id::to_string() const
    {
        return _id.to_string();
    }

    bool object_id::operator==(const object_id& rhs) const
    {
        return _id == rhs._id;
    }

    bool object_id::operator!=(const object_id& rhs) const
    {
        return _id != rhs._id;
    }

    bool object_id::operator>=(const object_id& rhs) const
    {
        return _id >= rhs._id;
    }

    bool object_id::operator<=(const object_id& rhs) const
    {
        return _id <= rhs._id;
    }

    bool object_id::operator<(const object_id& rhs) const
    {
        return _id < rhs._id;
    }
}
