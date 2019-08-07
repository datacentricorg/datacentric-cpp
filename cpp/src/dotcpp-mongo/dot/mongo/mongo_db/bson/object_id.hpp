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

#include <dot/declare.hpp>
#include <dot/system/ptr.hpp>
#include <dot/system/type.hpp>
#include <bsoncxx/oid.hpp>

namespace dot
{
    class local_date_time;

    /// Repesents 12-byte MongoDB identifier that is unique within the server.
    ///
    /// This type is immutable.
    class DOT_CLASS object_id
    {
    public: // STATIC

        /// Represents empty object id, all bytes are zero
        static object_id Empty;

    public: // CONSTRUCTORS

        /// <summary>Create with value object_id::empty.</summary>
        object_id();

        /// <summary>Create from MongoDB driver object id type.</summary>
        object_id(bsoncxx::oid id);

        /// <summary>Create from MongoDB driver object id type.</summary>
        object_id(dot::object obj);

        /// <summary>Create from the hexidecimal string representation produced by to_string().</summary>
        object_id(dot::string str);

        /// <summary>Create from byte array.</summary>
        object_id(const char* bytes, std::size_t len);

        /// Create from UTC datetime, with all other components of object id empty.
        /// The created object is less or equal to any other object with time timestamp
        /// that falls on the same second.
        ///
        /// Error message if the timestamp does not fall on the second // TODO - need to check
        explicit object_id(dot::local_date_time value);

    public: // METHODS

        /// Return the internal BSON oid type
        bsoncxx::oid oid() const { return _id; }

        bool is_empty();

        static object_id GenerateNewId();

        /// Returns hexadecimal string representation
        dot::string to_string() const;

    public: // OPERATORS

        /// Equality operator
        bool operator==(const object_id& rhs) const;

        /// Inquality operator
        bool operator!=(const object_id& rhs) const;

        /// More of equal operator
        bool operator>=(const object_id& rhs) const;

        /// Less or equal operator
        bool operator<=(const object_id& rhs) const;

        /// Less operator
        bool operator<(const object_id& rhs) const;

        /// More operator
        bool operator>(const object_id& rhs) const;

        /// Boxing operator
        operator dot::object() const
        {
            if (_id != Empty._id)
                return dot::object(new dot::struct_wrapper_impl<object_id>(*this));
            else return dot::object();
        }

    private:
        bsoncxx::oid _id;
    };
}

namespace dot
{
    template <>
    inline type_t typeof<dot::object_id>()
    {
        static dot::type_t type_ = dot::make_type_builder<dot::object_id>("Mongo", "object_id")->build();
        return type_;
    }
}

namespace std
{
    /// <summary>Implements hash struct used by STL unordered_map for object_id.</summary>
    template <>
    struct hash<dot::object_id>
    {
        size_t operator()(const dot::object_id& id) const
        {
            return hash<string>()(id.oid().to_string());
        }
    };

    /// <summary>Implements equal_to struct used by STL unordered_map for object_id.</summary>
    template <>
    struct equal_to<dot::object_id>
    {
        bool operator()(const dot::object_id& lhs, const dot::object_id& rhs) const
        {
            return lhs == rhs;
        }
    };
}
