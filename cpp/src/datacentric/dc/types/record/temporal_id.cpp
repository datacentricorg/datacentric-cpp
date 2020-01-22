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

#include <dc/precompiled.hpp>
#include <dc/implement.hpp>
#include <dc/types/record/temporal_id.hpp>
#include <chrono>

namespace dc
{
    TemporalId TemporalId::empty = TemporalId();

    const int TemporalId::oid_bytes_size_ = 12;
    const int TemporalId::oid_timestamp_offset_ = 0;
    const int TemporalId::oid_timestamp_size_ = 4;
    const int TemporalId::oid_other_offset_ = 4;
    const int TemporalId::oid_other_size_ = 8;

    const int TemporalId::bytes_size_ = 16;
    const int TemporalId::timestamp_offset_ = 0;
    const int TemporalId::timestamp_size_ = 8;
    const int TemporalId::other_offset_ = 8;
    const int TemporalId::other_size_ = 8;

    TemporalId::TemporalId()
    {
        bytes_ = dot::make_byte_array(bytes_size_);
    }

    TemporalId::TemporalId(bsoncxx::oid id)
    {
        // Copy iod bytes structure
        bytes_ = dot::make_byte_array(bytes_size_);
        int64_t id_time = id.get_time_t() * 1000;
        bytes_->copy_value(timestamp_offset_, id_time);
        bytes_->copy(other_offset_, id.bytes() + oid_other_offset_, oid_other_size_);
    }

    TemporalId::TemporalId(dot::Object obj)
    {
        bytes_ = ((dot::struct_wrapper<TemporalId>) obj)->bytes_;
    }

    TemporalId::TemporalId(dot::String str)
    {
        if (str->length() != 2 * bytes_size_)
            throw dot::Exception("Passed srting shoud be 32 characters long.");

        unsigned long long p1 = std::stoull(*str->substring(0, bytes_size_), nullptr, 16);
        unsigned long long p2 = std::stoull(*str->substring(bytes_size_, bytes_size_), nullptr, 16);

        bytes_ = dot::make_byte_array(bytes_size_);
        bytes_->copy_value(p1);
        bytes_->copy_value(bytes_size_ / 2, p2);
    }

    TemporalId::TemporalId(const char* bytes, std::size_t len)
    {
        if (len != bytes_size_)
            throw dot::Exception("Passed byte array shoud be 16 bytes long.");

        bytes_ = dot::make_byte_array(bytes, len);
    }

    TemporalId::TemporalId(dot::ByteArray bytes)
    {
        if (bytes->get_length() != bytes_size_)
            throw dot::Exception("Passed byte array shoud be 16 bytes long.");

        bytes_ = bytes;
    }

    TemporalId::TemporalId(dot::LocalDateTime value)
    {
        boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
        boost::posix_time::time_duration d = (boost::posix_time::ptime) value - epoch;
        int64_t milliseconds = d.total_milliseconds();

        bytes_ = dot::make_byte_array(bytes_size_);
        bytes_->copy_value(timestamp_offset_, milliseconds);
    }

    bool TemporalId::is_empty()
    {
        static char empty_bytes[bytes_size_] = { 0 };
        return bytes_->compare(empty_bytes) == 0;
    }

    TemporalId TemporalId::generate_new_id()
    {
        int64_t time_now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        bsoncxx::oid id = bsoncxx::oid();

        dot::ByteArray bytes = dot::make_byte_array(bytes_size_);
        bytes->copy_value(timestamp_offset_, time_now);
        bytes->copy(other_offset_, id.bytes() + oid_other_offset_, oid_other_size_);

        return TemporalId(bytes);
    }

    dot::Nullable<TemporalId> TemporalId::min(dot::Nullable<TemporalId> lhs, dot::Nullable<TemporalId> rhs)
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

    dot::ByteArray TemporalId::to_byte_array()
    {
        return bytes_;
    }

    dot::String TemporalId::to_string() const
    {
        unsigned long long p1, p2;
        p1 = bytes_->to_primitive<unsigned long long>();
        p2 = bytes_->to_primitive<unsigned long long>(bytes_size_ / 2);

        std::ostringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(bytes_size_) << std::nouppercase << p1;
        ss << std::hex << std::setfill('0') << std::setw(bytes_size_) << std::nouppercase << p2;

        return ss.str();
    }

    bool TemporalId::operator==(const TemporalId& rhs) const
    {
        return bytes_->compare(rhs.bytes_) == 0;
    }

    bool TemporalId::operator!=(const TemporalId& rhs) const
    {
        return bytes_->compare(rhs.bytes_) != 0;
    }

    bool TemporalId::operator>=(const TemporalId& rhs) const
    {
        return bytes_->compare(rhs.bytes_) >= 0;
    }

    bool TemporalId::operator>(const TemporalId& rhs) const
    {
        return bytes_->compare(rhs.bytes_) > 0;
    }

    bool TemporalId::operator<=(const TemporalId& rhs) const
    {
        return bytes_->compare(rhs.bytes_) <= 0;
    }

    bool TemporalId::operator<(const TemporalId& rhs) const
    {
        return bytes_->compare(rhs.bytes_) < 0;
    }

    TemporalId::operator dot::Object() const
    {
        return dot::Object(new dot::StructWrapperImpl<TemporalId>(*this));
    }

    void TemporalId::serialize(dot::tree_writer_base writer, dot::Object obj)
    {
        writer->write_start_value();
        writer->write_value(((TemporalId) obj).to_byte_array());
        writer->write_end_value();
    }

    dot::Object TemporalId::deserialize(dot::Object value, dot::Type type)
    {
        dot::Type value_type = value->get_type();

        if (value_type->equals(dot::typeof<dot::ByteArray>()))
        {
            dot::ByteArray arr = (dot::ByteArray) value;
            return TemporalId(arr);
        }
        if (value_type->equals(dot::typeof<dot::ObjectId>()))
        {
            return TemporalId(((dot::ObjectId) value).oid());
        }
        if (value_type->equals(dot::typeof<dot::String>()))
        {
            return TemporalId((dot::String) value);
        }

        throw dot::Exception("Couldn't construct TemporalId from " + value_type->name());
    }

    dot::Object TemporalId::serialize_token(dot::Object obj)
    {
        TemporalId tid = (TemporalId) obj;
        return tid.to_byte_array();
    }
}
