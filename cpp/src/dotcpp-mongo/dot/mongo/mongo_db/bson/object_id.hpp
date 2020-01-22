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
    class DOT_MONGO_CLASS object_id
    {
        template <class T>
        friend inline Type typeof();

    public: // STATIC

        /// Represents empty object id, all bytes are zero
        static object_id empty;

    public: // CONSTRUCTORS

        /// Create with value object_id::empty.
        object_id();

        /// Create from MongoDB driver object id type.
        object_id(bsoncxx::oid id);

        /// Create from MongoDB driver object id type.
        object_id(dot::Object obj);

        /// Create from the hexidecimal string representation produced by to_string().
        object_id(dot::String str);

        /// Create from byte array.
        object_id(const char* bytes, std::size_t len);

        /// Create from UTC datetime, with all other components of object id empty.
        /// The created Object is less or equal to any other Object with time timestamp
        /// that falls on the same second.
        ///
        /// Error message if the timestamp does not fall on the second // TODO - need to check
        explicit object_id(dot::LocalDateTime value);

    public: // METHODS

        /// Return the internal BSON oid type
        bsoncxx::oid oid() const { return id_; }

        bool is_empty();

        static object_id generate_new_id();

        /// Returns hexadecimal string representation
        dot::String to_string() const;

    public: // OPERATORS

        /// Equality operator
        bool operator==(const object_id& rhs) const;

        /// Inquality operator
        bool operator!=(const object_id& rhs) const;

        /// More of equal operator
        bool operator>=(const object_id& rhs) const;

        /// More operator
        bool operator>(const object_id& rhs) const;

        /// Less or equal operator
        bool operator<=(const object_id& rhs) const;

        /// Less operator
        bool operator<(const object_id& rhs) const;

        /// Boxing operator
        operator dot::Object() const
        {
            return dot::Object(new dot::struct_wrapper_impl<object_id>(*this));
        }

        /// Min method for Nullable object_id.
        static dot::Nullable<object_id> min(dot::Nullable<object_id> lhs, dot::Nullable<object_id> rhs);

    private:

        static Object deserialize(Object value, dot::Type type);

    private:
        bsoncxx::oid id_;
    };
}

namespace dot
{
    template <>
    inline Type typeof<dot::object_id>()
    {
        static dot::Type type_ = dot::make_type_builder<dot::object_id>("dot", "object_id", { make_deserialize_class_attribute( &object_id::deserialize) })
            ->build();
        return type_;
    }
}

namespace std
{
    /// Implements hash struct used by STL unordered_map for object_id.
    template <>
    struct hash<dot::object_id>
    {
        size_t operator()(const dot::object_id& id) const
        {
            return hash<string>()(id.oid().to_string());
        }
    };

    /// Implements equal_to struct used by STL unordered_map for object_id.
    template <>
    struct equal_to<dot::object_id>
    {
        bool operator()(const dot::object_id& lhs, const dot::object_id& rhs) const
        {
            return lhs == rhs;
        }
    };
}
