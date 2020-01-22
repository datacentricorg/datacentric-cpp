/*
Copyright (C) 2015-present The DotCpp Authors.

This file is part of .C++, a native C++ implementation of
popular .NET class library APIs developed to facilitate
code reuse between C# and C++.

    http://github.com/dotcpp/dotcpp (source)
    http://dotcpp.org (documentation)

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
    ObjectId ObjectId::empty = ObjectId();

    ObjectId::ObjectId()
    {
        static const char empty_buffer[12] = { 0 };
        id_ = bsoncxx::oid(empty_buffer, 12);
    }

    ObjectId::ObjectId(bsoncxx::oid id) : id_(id) {}

    ObjectId::ObjectId(dot::Object obj)
    {
        id_ = ((struct_wrapper<ObjectId>)obj)->id_;
    }

    ObjectId::ObjectId(dot::String str)
        : id_(*str)
    {}

    ObjectId::ObjectId(const char* bytes, std::size_t len)
        : id_(bytes, len)
    {
    }

    ObjectId::ObjectId(dot::LocalDateTime value)
    {
        char bytes[12] = { 0 };

        boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
        boost::posix_time::time_duration d = (boost::posix_time::ptime)value - epoch;
        int64_t seconds = d.total_seconds();
        std::memcpy(bytes, &seconds, sizeof(seconds));
        id_ = bsoncxx::oid(bytes, 12);
    }

    bool ObjectId::is_empty()
    {
        return *id_.bytes() == 0; // TODO check
    }

    ObjectId ObjectId::generate_new_id()
    {
        return bsoncxx::oid();
    }

    dot::String ObjectId::to_string() const
    {
        return id_.to_string();
    }

    bool ObjectId::operator==(const ObjectId& rhs) const
    {
        return id_ == rhs.id_;
    }

    bool ObjectId::operator!=(const ObjectId& rhs) const
    {
        return id_ != rhs.id_;
    }

    bool ObjectId::operator>=(const ObjectId& rhs) const
    {
        return id_ >= rhs.id_;
    }

    bool ObjectId::operator>(const ObjectId& rhs) const
    {
        return id_ > rhs.id_;
    }

    bool ObjectId::operator<=(const ObjectId& rhs) const
    {
        return id_ <= rhs.id_;
    }

    bool ObjectId::operator<(const ObjectId& rhs) const
    {
        return id_ < rhs.id_;
    }

    dot::Nullable<ObjectId> ObjectId::min(dot::Nullable<ObjectId> lhs, dot::Nullable<ObjectId> rhs)
    {
        if (lhs != nullptr && rhs != nullptr)
        {
            // Neither is null, returns the smaller value
            if (lhs.value() < rhs.value()) return lhs;
            else return rhs;
        }
        else if (lhs == nullptr)
        {
            // Also covers the case when both are null
            return rhs;
        }
        else
        {
            return lhs;
        }
    }

    Object ObjectId::deserialize(Object value, dot::Type type)
    {
        dot::Type value_type = value->get_type();

        if (value_type->equals(dot::typeof<ObjectId>()))
        {
            return value;
        }
        if (value_type->equals(dot::typeof<String>()))
        {
            return ObjectId((String)value);
        }

        throw dot::Exception("Couldn't construct ObjectId from " + value_type->name());
        return Object();
    }
}
