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
#include <dot/system/type.hpp>
#include <dot/system/byte_array.hpp>
#include <dot/serialization/serialize_attribute.hpp>
#include <dot/serialization/deserialize_attribute.hpp>
#include <dot/mongo/mongo_db/bson/object_id.hpp>
#include <dot/mongo/serialization/filter_token_serialization_attribute.hpp>

namespace dc
{
    /// A 16-byte, unique, ordered identifier that consists of two int64_t
    /// elements:
    ///
    /// * First element representing creation time recorded as the number
    ///   of ticks since the Unix epoch (1970).
    /// * Second element is randomized with the inclusion of machine-specific
    ///   information to make the identifier unique in combination with the
    ///   first element.
    ///
    /// temporal_id has the same size as GUID and can be stored in a
    /// data type designed for GUID.
    ///
    /// temporal_id is unique in the absence of collision for its randomized
    /// part, which by design has extremely low probability (similar to GUID).
    /// It has the following ordering guarantees:
    ///
    /// * When generated in the same process, temporal_ids are strictly ordered
    ///   irrespective of how fast they are generated.
    /// * When generated independently, temporal_ids are ordered if generated
    ///   more than one operating system clock event apart. While the
    ///   underlying tick data type has 100ns resolution, the operating
    ///   system clock more typically has one event per 10-20 ms.
    ///
    /// Because temporal_id does not rely on auto-incremented database field,
    /// it can be used with distributed, web scale databases where getting
    /// a strictly increasing auto-incremented identifier would cause a
    /// performance hit.
    class DC_CLASS temporal_id
    {
        template <class T>
        friend inline dot::type dot::typeof();

    public: // STATIC

        /// Represents empty object id, all bytes are zero
        static temporal_id empty;

    public: // CONSTRUCTORS

        /// Create with value temporal_id::empty.
        temporal_id();

        /// Create from MongoDB driver object id type.
        temporal_id(bsoncxx::oid id);

        /// Create from MongoDB driver object id type.
        temporal_id(dot::object obj);

        /// Create from the hexidecimal string representation produced by to_string().
        temporal_id(dot::string str);

        /// Create from byte array.
        temporal_id(const char* bytes, std::size_t len);

        /// Create from byte_array.
        temporal_id(dot::byte_array bytes);

        /// Create from UTC datetime, with all other components of object id empty.
        /// The created object is less or equal to any other object with time timestamp
        /// that falls on the same second.
        ///
        /// Error message if the timestamp does not fall on the second // TODO - need to check
        explicit temporal_id(dot::local_date_time value);

    public: // METHODS

        /// Check if temporal_id is empty.
        bool is_empty();

        /// Generates new temporal_id.
        static temporal_id generate_new_id();

        /// Min method for nullable temporal_id.
        static dot::nullable<temporal_id> min(dot::nullable<temporal_id> lhs, dot::nullable<temporal_id> rhs);

        /// Converts temporal_id to byte_array.
        dot::byte_array to_byte_array();

        /// Returns hexadecimal string representation
        dot::string to_string() const;

    public: // OPERATORS

        /// Equality operator
        bool operator==(const temporal_id& rhs) const;

        /// Inquality operator
        bool operator!=(const temporal_id& rhs) const;

        /// More of equal operator
        bool operator>=(const temporal_id& rhs) const;

        /// More operator
        bool operator>(const temporal_id& rhs) const;

        /// Less or equal operator
        bool operator<=(const temporal_id& rhs) const;

        /// Less operator
        bool operator<(const temporal_id& rhs) const;

        /// Boxing operator
        operator dot::object() const;

    private:

        static void serialize(dot::tree_writer_base writer, dot::object obj);
        static dot::object deserialize(dot::object value, dot::type type);
        static dot::object serialize_token(dot::object obj);

    private:

        /// Bytes size and structure of object_id.
        static const int oid_bytes_size_;
        static const int oid_timestamp_offset_;
        static const int oid_timestamp_size_;
        static const int oid_other_offset_;
        static const int oid_other_size_;

        /// Bytes size and structure of temporal_id.
        static const int bytes_size_;
        static const int timestamp_offset_;
        static const int timestamp_size_;
        static const int other_offset_;
        static const int other_size_;

        dot::byte_array bytes_;
    };
}

namespace dot
{
    template <>
    inline type typeof<dc::temporal_id>()
    {
        static dot::type type_ = dot::make_type_builder<dc::temporal_id>("dc", "temporal_id", {
                make_serialize_class_attribute(&dc::temporal_id::serialize),
                make_deserialize_class_attribute(&dc::temporal_id::deserialize),
                make_filter_token_serialization_attribute(&dc::temporal_id::serialize_token) })
            ->build();
        return type_;
    }
}

namespace std
{
    /// Implements hash struct used by STL unordered_map for temporal_id.
    template <>
    struct hash<dc::temporal_id>
    {
        size_t operator()(const dc::temporal_id& id) const
        {
            return hash<string>()(*id.to_string());
        }
    };

    /// Implements equal_to struct used by STL unordered_map for temporal_id.
    template <>
    struct equal_to<dc::temporal_id>
    {
        bool operator()(const dc::temporal_id& lhs, const dc::temporal_id& rhs) const
        {
            return lhs == rhs;
        }
    };
}
