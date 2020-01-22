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

#pragma once

#include <dot/mongo/declare.hpp>
#include <dot/system/ptr.hpp>
#include <dot/system/type.hpp>
#include <bsoncxx/oid.hpp>
#include <dot/serialization/deserialize_attribute.hpp>

namespace dot
{
    class LocalDateTime;

    /// Repesents 12-byte MongoDB identifier that is unique within the server.
    ///
    /// This type is immutable.
    class DOT_MONGO_CLASS ObjectId
    {
        template <class T>
        friend inline Type typeof();

    public: // STATIC

        /// Represents empty object id, all bytes are zero
        static ObjectId empty;

    public: // CONSTRUCTORS

        /// Create with value ObjectId::empty.
        ObjectId();

        /// Create from MongoDB driver object id type.
        ObjectId(bsoncxx::oid id);

        /// Create from MongoDB driver object id type.
        ObjectId(dot::Object obj);

        /// Create from the hexidecimal string representation produced by to_string().
        ObjectId(dot::String str);

        /// Create from byte array.
        ObjectId(const char* bytes, std::size_t len);

        /// Create from UTC datetime, with all other components of object id empty.
        /// The created Object is less or equal to any other Object with time timestamp
        /// that falls on the same second.
        ///
        /// Error message if the timestamp does not fall on the second // TODO - need to check
        explicit ObjectId(dot::LocalDateTime value);

    public: // METHODS

        /// Return the internal BSON oid type
        bsoncxx::oid oid() const { return id_; }

        bool is_empty();

        static ObjectId generate_new_id();

        /// Returns hexadecimal string representation
        dot::String to_string() const;

    public: // OPERATORS

        /// Equality operator
        bool operator==(const ObjectId& rhs) const;

        /// Inquality operator
        bool operator!=(const ObjectId& rhs) const;

        /// More of equal operator
        bool operator>=(const ObjectId& rhs) const;

        /// More operator
        bool operator>(const ObjectId& rhs) const;

        /// Less or equal operator
        bool operator<=(const ObjectId& rhs) const;

        /// Less operator
        bool operator<(const ObjectId& rhs) const;

        /// Boxing operator
        operator dot::Object() const
        {
            return dot::Object(new dot::struct_wrapper_impl<ObjectId>(*this));
        }

        /// Min method for Nullable ObjectId.
        static dot::Nullable<ObjectId> min(dot::Nullable<ObjectId> lhs, dot::Nullable<ObjectId> rhs);

    private:

        static Object deserialize(Object value, dot::Type type);

    private:
        bsoncxx::oid id_;
    };
}

namespace dot
{
    template <>
    inline Type typeof<dot::ObjectId>()
    {
        static dot::Type type_ = dot::make_type_builder<dot::ObjectId>("dot", "ObjectId", { make_deserialize_class_attribute( &ObjectId::deserialize) })
            ->build();
        return type_;
    }
}

namespace std
{
    /// Implements hash struct used by STL unordered_map for ObjectId.
    template <>
    struct hash<dot::ObjectId>
    {
        size_t operator()(const dot::ObjectId& id) const
        {
            return hash<string>()(id.oid().to_string());
        }
    };

    /// Implements equal_to struct used by STL unordered_map for ObjectId.
    template <>
    struct equal_to<dot::ObjectId>
    {
        bool operator()(const dot::ObjectId& lhs, const dot::ObjectId& rhs) const
        {
            return lhs == rhs;
        }
    };
}
