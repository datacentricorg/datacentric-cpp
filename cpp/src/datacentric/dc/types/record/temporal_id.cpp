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
    temporal_id temporal_id::empty = temporal_id();

    temporal_id::temporal_id()
    {
        bytes_ = dot::make_byte_array(16);
    }

    temporal_id::temporal_id(bsoncxx::oid id)
    {
        bytes_ = dot::make_byte_array(16);
        memcpy(bytes_->get_data(), id.bytes(), 4);
        memcpy(bytes_->get_data() + 8, id.bytes() + 4, 8);
    }

    temporal_id::temporal_id(dot::object obj)
    {
        bytes_ = ((dot::struct_wrapper<temporal_id>) obj)->bytes_;
    }

    temporal_id::temporal_id(dot::string str)
    {
        if (str->length() != 32)
            throw dot::exception("Passed srting shoud be 32 characters long.");

        unsigned long long p1 = std::stoull(*str->substring(0, 16), nullptr, 16);
        unsigned long long p2 = std::stoull(*str->substring(16, 16), nullptr, 16);

        bytes_ = dot::make_byte_array(16);
        std::memcpy(bytes_->get_data(), &p1, sizeof(p1));
        std::memcpy(bytes_->get_data() + 8, &p2, sizeof(p2));
    }

    temporal_id::temporal_id(const char* bytes, std::size_t len)
    {
        if (len != 16)
            throw dot::exception("Passed byte array shoud be 16 bytes long.");

        bytes_ = dot::make_byte_array(bytes, len);
    }

    temporal_id::temporal_id(dot::byte_array bytes)
    {
        if (bytes->get_length() != 16)
            throw dot::exception("Passed byte array shoud be 16 bytes long.");

        bytes_ = bytes;
    }

    temporal_id::temporal_id(dot::local_date_time value)
    {
        boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
        boost::posix_time::time_duration d = (boost::posix_time::ptime) value - epoch;
        int64_t seconds = d.total_seconds();

        bytes_ = dot::make_byte_array(16);
        std::memcpy(bytes_->get_data(), &seconds, sizeof(seconds));
    }

    bool temporal_id::is_empty()
    {
        static char empty_bytes[16] = { 0 };
        return memcmp(bytes_->get_data(), empty_bytes, bytes_->get_length()) == 0;
    }

    temporal_id temporal_id::generate_new_id()
    {
        //int64_t time_now2 = time(nullptr);
        int64_t time_now = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch()).count();
        bsoncxx::oid id = bsoncxx::oid();

        dot::byte_array bytes = dot::make_byte_array(16);
        memcpy(bytes->get_data(), &time_now, sizeof(time_now));
        memcpy(bytes->get_data() + 8, id.bytes() + 4, 8);

        dot::string s = temporal_id(bytes).to_string();

        return temporal_id(bytes);
    }

    dot::byte_array temporal_id::to_byte_array()
    {
        return bytes_;
    }

    dot::string temporal_id::to_string() const
    {
        unsigned long long p1, p2;
        memcpy(&p1, bytes_->get_data(), sizeof(p1));
        memcpy(&p2, bytes_->get_data() + 8, sizeof(p2));

        std::ostringstream ss;
        ss << std::hex << std::setfill('0') << std::setw(8) << std::nouppercase << p1;
        ss << std::hex << std::setfill('0') << std::setw(8) << std::nouppercase << p2;

        return ss.str();
    }

    bool temporal_id::operator==(const temporal_id& rhs) const
    {
        return memcmp(bytes_->get_data(), rhs.bytes_->get_data(), 16) == 0;
    }

    bool temporal_id::operator!=(const temporal_id& rhs) const
    {
        return memcmp(bytes_->get_data(), rhs.bytes_->get_data(), 16) != 0;
    }

    bool temporal_id::operator>=(const temporal_id& rhs) const
    {
        return memcmp(bytes_->get_data(), rhs.bytes_->get_data(), 16) >= 0;
    }

    bool temporal_id::operator>(const temporal_id& rhs) const
    {
        return memcmp(bytes_->get_data(), rhs.bytes_->get_data(), 16) > 0;
    }

    bool temporal_id::operator<=(const temporal_id& rhs) const
    {
        return memcmp(bytes_->get_data(), rhs.bytes_->get_data(), 16) <= 0;
    }

    bool temporal_id::operator<(const temporal_id& rhs) const
    {
        return memcmp(bytes_->get_data(), rhs.bytes_->get_data(), 16) < 0;
    }

    temporal_id::operator dot::object() const
    {
        return dot::object(new dot::struct_wrapper_impl<temporal_id>(*this));
    }

    void temporal_id::serialize(dot::tree_writer_base writer, dot::object obj)
    {
        writer->write_start_value();
        writer->write_value(((temporal_id) obj).to_byte_array());
        writer->write_end_value();
    }

    dot::object temporal_id::deserialize(dot::object value, dot::type type)
    {
        dot::type value_type = value->get_type();

        if (value_type->equals(dot::typeof<dot::object_id>()))
        {
            return temporal_id(((dot::object_id) value).oid());
        }
        if (value_type->equals(dot::typeof<dot::byte_array>()))
        {
            dot::byte_array arr = (dot::byte_array) value;
            return temporal_id(arr);
        }
        if (value_type->equals(dot::typeof<dot::string>()))
        {
            return temporal_id((dot::string) value);
        }

        throw dot::exception("Couldn't construct temporal_id from " + value_type->name());
    }

    dot::object temporal_id::serialize_token(dot::object obj)
    {
        temporal_id tid = (temporal_id) obj;
        return tid.to_byte_array();
    }
}
