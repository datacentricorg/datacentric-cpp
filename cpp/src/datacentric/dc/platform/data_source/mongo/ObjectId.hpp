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

#pragma once

#include <dc/declare.hpp>
#include <dot/system/ptr.hpp>
#include <bsoncxx/oid.hpp>

namespace dc
{
    class DC_CLASS ObjectId
    {

    public:

        ObjectId();

        ObjectId(bsoncxx::oid id);

        ObjectId(dot::object obj);

        ObjectId(dot::string str);

        explicit ObjectId(const char* bytes, std::size_t len);

        explicit ObjectId(dot::local_date_time value);

        bool operator==(const ObjectId& rhs) const;

        bool operator!=(const ObjectId& rhs) const;

        bool operator>=(const ObjectId& rhs) const;

        bool operator<=(const ObjectId& rhs) const;

        bool is_empty();

        bool operator<(const ObjectId& rhs) const;

        //local_date_time GetTimeStamp();

        static ObjectId GenerateNewId();

        dot::string to_string();

        bsoncxx::oid _id;

        static ObjectId Empty;

        operator dot::object()
        {
            if (_id != Empty._id)
                return dot::object(new dot::struct_wrapper_impl<ObjectId>(*this));
            else return dot::object();
        }

    };

}

namespace dot
{
    template <>
    inline type_t typeof<dc::ObjectId>()
    {
        static dot::type_t type_ = dot::make_type_builder<dc::ObjectId>("Mongo", "ObjectId")->build();
        return type_;
    }

}

namespace std
{
    /// <summary>Implements hash struct used by STL unordered_map for ObjectId.</summary>
    template <>
    struct hash<dc::ObjectId>
    {
        size_t operator()(const dc::ObjectId& id) const
        {
            return hash<string>()(id._id.to_string());
        }
    };

    /// <summary>Implements equal_to struct used by STL unordered_map for ObjectId.</summary>
    template <>
    struct equal_to<dc::ObjectId>
    {
        bool operator()(const dc::ObjectId& lhs, const dc::ObjectId& rhs) const
        {
            return lhs == rhs;
        }
    };
}
