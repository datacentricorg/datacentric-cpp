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

#include <dot/precompiled.hpp>
#define FMT_HEADER_ONLY
#include <fmt/format.h>
#include <dot/implement.hpp>
#include <dot/system/string.hpp>
#include <dot/system/object.hpp>
#include <dot/system/nullable.hpp>
#include <dot/system/type.hpp>

namespace dot
{
    string string::empty = make_string("");

    dot::type string_impl::typeof()
    {
        static dot::type result = []()->dot::type
        {
            dot::type t = dot::make_type_builder<string_impl>("dot", "string")
                ->build();
            return t;
        }();
        return result;
    }

    dot::type string_impl::get_type()
    {
        return typeof();
    }


    bool string_impl::equals(object obj)
    {
        if (this == &(*obj)) return true;

        if (obj.is<string>())
        {
            return *this == *obj.as<string>();
        }

        return false;
    }

    size_t string_impl::hash_code()
    {
        return std::hash<std::string>()(*this);
    }

    string string_impl::to_string()
    {
        return this;
    }

    bool string_impl::ends_with(const string& value)
    {
        int p = length() - value->length();
        if (p >= 0 && substr(p, value->length()) == *value)
            return true;
        return false;
    }

    bool string_impl::starts_with(const string& value)
    {
        int p = length() - value->length();
        if (p >= 0 && substr(0, value->length()) == *value)
            return true;
        return false;
    }

    string string_impl::substring(int start_index)
    {
        return make_string(this->substr(start_index));
    }

    string string_impl::substring(int start_index, int length)
    {
        return make_string(this->substr(start_index, length));
    }

    int string_impl::compare_to(const string& str_b) const
    {
        return this->compare(*str_b);
    }

    int string_impl::index_of_any(list<char> any_of)
    {
        size_t pos = find_first_of(any_of->data(), 0, any_of->size());
        if (pos != std::string::npos)
            return pos;
        return -1;
    }

    string string_impl::remove(int start_index)
    {
        return make_string(*this)->erase(start_index);
    }

    string string_impl::remove(int start_index, int count)
    {
        return make_string(*this)->erase(start_index, count);
    }

    string string_impl::replace(const char old_char, const char new_char) const
    {
        string make_str = *this;
        std::replace(make_str->begin(), make_str->end(), old_char, new_char);
        return make_str;
    }

    list<string> string_impl::split(char separator) const
    {
        list<string> result = make_list<string>();

        std::size_t current, previous = 0;
        current = this->find(separator);
        while (current != std::string::npos) {
            result->add(this->substr(previous, current - previous));
            previous = current + 1;
            current = this->find(separator, previous);
        }
        result->add(this->substr(previous, current - previous));

        return result;
    }

    list<string> string_impl::split(string separator) const
    {
        list<string> result = make_list<string>();

        std::size_t current, previous = 0;
        current = this->find_first_of(*separator);
        while (current != std::string::npos) {
            result->add(this->substr(previous, current - previous));
            previous = current + 1;
            current = this->find_first_of(*separator, previous);
        }
        result->add(this->substr(previous, current - previous));

        return result;
    }

    string string_impl::trim() const
    {
        string result = make_string(*this);

        // Trim end
        result->erase(std::find_if(result->rbegin(), result->rend(), [](char ch) {
            return !char_impl::is_white_space(ch);
        }).base(), result->end());

        // Trim start
        result->erase(result->begin(), std::find_if(result->begin(), result->end(), [](char ch) {
            return !char_impl::is_white_space(ch);
        }));

        return result;
    }

    string string_impl::trim_start() const
    {
        string result = make_string(*this);
        result->erase(result->begin(), std::find_if(result->begin(), result->end(), [](char ch) {
            return !char_impl::is_white_space(ch);
        }));
        return result;
    }

    string string_impl::trim_end() const
    {
        string result = make_string(*this);
        result->erase(std::find_if(result->rbegin(), result->rend(), [](char ch) {
            return !char_impl::is_white_space(ch);
        }).base(), result->end());
        return result;
    }

    bool string_impl::contains(const string& s) const
    {
        return this->find(*s) != std::string::npos;
    }

    string string_impl::to_lower() const
    {
        string result = make_string(*this);
        std::transform(result->begin(), result->end(), result->begin(),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    string string_impl::to_upper() const
    {
        string result = make_string(*this);
        std::transform(result->begin(), result->end(), result->begin(),
            [](unsigned char c) { return std::toupper(c); });
        return result;
    }

    bool string::is_null_or_empty(string value)
    {
        if (value == nullptr || value->empty())
            return true;
        return false;
    }

    bool string::operator==(const object& rhs) const
    {
        // If rhs is null, return false. Otherwise, check if
        // the other object is a string. If yes, compare by value.
        // If no, return false.
        if (rhs == nullptr)
        {
            return false;
        }
        else
        {
            string rhs_str = rhs.as<string>();
            if (rhs_str != nullptr) return operator==(rhs_str);
            else return false;
        }
    }

    std::string string::format_impl(fmt::string_view format_str, fmt::format_args args)
    {
        return fmt::vformat(format_str, args);
    }
}
