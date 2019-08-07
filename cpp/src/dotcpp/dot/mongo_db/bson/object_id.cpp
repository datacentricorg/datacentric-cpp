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
#include <dc/platform/data_source/mongo/ObjectId.hpp>
#include <dot/system/string.hpp>
#include <dot/noda_time/local_date_time.hpp>

namespace dc
{

    ObjectId::ObjectId()
    {
        static const char Empty[12] = { 0 };
        _id = bsoncxx::oid(Empty, 12);
    }

    ObjectId::ObjectId(bsoncxx::oid id) : _id(id) {}

    ObjectId::ObjectId(dot::object obj)
    {
        _id = ((dot::struct_wrapper<ObjectId>)obj)->_id;
    }

    ObjectId::ObjectId(dot::string str)
        : _id(*str)
    {}

    ObjectId::ObjectId(const char* bytes, std::size_t len)
        : _id(bytes, len)
    {
    }

    ObjectId::ObjectId(dot::local_date_time value)
    {
        char bytes[12] = { 0 };

        boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
        boost::posix_time::time_duration d = (boost::posix_time::ptime)value - epoch;
        int64_t seconds = d.total_seconds();
        std::memcpy(bytes, &seconds, sizeof(seconds));
        _id = bsoncxx::oid(bytes, 12);
    }

    bool ObjectId::operator==(const ObjectId& rhs) const
    {
        return _id == rhs._id;
    }

    bool ObjectId::operator!=(const ObjectId& rhs) const
    {
        return _id != rhs._id;
    }

    bool ObjectId::operator>=(const ObjectId& rhs) const
    {
        return _id >= rhs._id;
    }

    bool ObjectId::operator<=(const ObjectId& rhs) const
    {
        return _id <= rhs._id;
    }

    bool ObjectId::is_empty()
    {
        return *_id.bytes() == 0; // TODO check
    }

    bool ObjectId::operator<(const ObjectId& rhs) const
    {
        return _id < rhs._id;
    }

    //local_date_time ObjectId::GetTimeStamp()
    //{
    //    ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
    //    int64_t seconds;
    //    std::memcpy(&seconds, _id.bytes(), sizeof(seconds));
    //
    //    boost::posix_time::time_duration d = boost::posix_time::seconds(seconds);
    //    return epoch + d;
    //}

    ObjectId ObjectId::GenerateNewId()
    {
        return bsoncxx::oid();
    }

    dot::string ObjectId::to_string()
    {
        return _id.to_string();
    }

    ObjectId ObjectId::Empty = ObjectId();

}