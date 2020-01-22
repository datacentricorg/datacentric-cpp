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
    /// TemporalId has the same size as GUID and can be stored in a
    /// data type designed for GUID.
    ///
    /// TemporalId is unique in the absence of collision for its randomized
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
    /// Because TemporalId does not rely on auto-incremented database field,
    /// it can be used with distributed, web scale databases where getting
    /// a strictly increasing auto-incremented identifier would cause a
    /// performance hit.
    class DC_CLASS TemporalId
    {
        template <class T>
        friend inline dot::Type dot::typeof();

    public: // STATIC

        /// Represents empty object id, all bytes are zero
        static TemporalId empty;

    public: // CONSTRUCTORS

        /// Create with value TemporalId::empty.
        TemporalId();

        /// Create from MongoDB driver object id type.
        TemporalId(bsoncxx::oid id);

        /// Create from MongoDB driver object id type.
        TemporalId(dot::Object obj);

        /// Create from the hexidecimal String representation produced by to_string().
        TemporalId(dot::String str);

        /// Create from byte array.
        TemporalId(const char* bytes, std::size_t len);

        /// Create from ByteArray.
        TemporalId(dot::ByteArray bytes);

        /// Create from UTC datetime, with all other components of object id empty.
        /// The created Object is less or equal to any other Object with time timestamp
        /// that falls on the same second.
        ///
        /// Error message if the timestamp does not fall on the second // TODO - need to check
        explicit TemporalId(dot::LocalDateTime value);

    public: // METHODS

        /// Check if TemporalId is empty.
        bool is_empty();

        /// Generates new TemporalId.
        static TemporalId generate_new_id();

        /// Min method for Nullable TemporalId.
        static dot::Nullable<TemporalId> min(dot::Nullable<TemporalId> lhs, dot::Nullable<TemporalId> rhs);

        /// Converts TemporalId to ByteArray.
        dot::ByteArray to_byte_array();

        /// Returns hexadecimal String representation
        dot::String to_string() const;

    public: // OPERATORS

        /// Equality operator
        bool operator==(const TemporalId& rhs) const;

        /// Inquality operator
        bool operator!=(const TemporalId& rhs) const;

        /// More of equal operator
        bool operator>=(const TemporalId& rhs) const;

        /// More operator
        bool operator>(const TemporalId& rhs) const;

        /// Less or equal operator
        bool operator<=(const TemporalId& rhs) const;

        /// Less operator
        bool operator<(const TemporalId& rhs) const;

        /// Boxing operator
        operator dot::Object() const;

    private:

        static void serialize(dot::tree_writer_base writer, dot::Object obj);
        static dot::Object deserialize(dot::Object value, dot::Type type);
        static dot::Object serialize_token(dot::Object obj);

    private:

        /// Bytes size and structure of ObjectId.
        static const int oid_bytes_size_;
        static const int oid_timestamp_offset_;
        static const int oid_timestamp_size_;
        static const int oid_other_offset_;
        static const int oid_other_size_;

        /// Bytes size and structure of TemporalId.
        static const int bytes_size_;
        static const int timestamp_offset_;
        static const int timestamp_size_;
        static const int other_offset_;
        static const int other_size_;

        dot::ByteArray bytes_;
    };
}

namespace dot
{
    template <>
    inline Type typeof<dc::TemporalId>()
    {
        static dot::Type type_ = dot::make_type_builder<dc::TemporalId>("dc", "TemporalId", {
                make_serialize_class_attribute(&dc::TemporalId::serialize),
                make_deserialize_class_attribute(&dc::TemporalId::deserialize),
                make_filter_token_serialization_attribute(&dc::TemporalId::serialize_token) })
            ->build();
        return type_;
    }
}

namespace std
{
    /// Implements hash struct used by STL unordered_map for TemporalId.
    template <>
    struct hash<dc::TemporalId>
    {
        size_t operator()(const dc::TemporalId& id) const
        {
            return hash<string>()(*id.to_string());
        }
    };

    /// Implements equal_to struct used by STL unordered_map for TemporalId.
    template <>
    struct equal_to<dc::TemporalId>
    {
        bool operator()(const dc::TemporalId& lhs, const dc::TemporalId& rhs) const
        {
            return lhs == rhs;
        }
    };
}
