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
    String String::empty = make_string("");

    dot::type StringImpl::typeof()
    {
        static dot::type result = []()->dot::type
        {
            dot::type t = dot::make_type_builder<StringImpl>("dot", "String")
                ->build();
            return t;
        }();
        return result;
    }

    dot::type StringImpl::get_type()
    {
        return typeof();
    }


    bool StringImpl::equals(Object obj)
    {
        if (this == &(*obj)) return true;

        if (obj.is<String>())
        {
            return *this == *obj.as<String>();
        }

        return false;
    }

    size_t StringImpl::hash_code()
    {
        return std::hash<std::string>()(*this);
    }

    String StringImpl::to_string()
    {
        return this;
    }

    bool StringImpl::ends_with(const String& value)
    {
        int p = length() - value->length();
        if (p >= 0 && substr(p, value->length()) == *value)
            return true;
        return false;
    }

    bool StringImpl::starts_with(const String& value)
    {
        int p = length() - value->length();
        if (p >= 0 && substr(0, value->length()) == *value)
            return true;
        return false;
    }

    String StringImpl::substring(int start_index)
    {
        return make_string(this->substr(start_index));
    }

    String StringImpl::substring(int start_index, int length)
    {
        return make_string(this->substr(start_index, length));
    }

    int StringImpl::compare_to(const String& str_b) const
    {
        return this->compare(*str_b);
    }

    int StringImpl::index_of_any(list<char> any_of)
    {
        size_t pos = find_first_of(any_of->data(), 0, any_of->size());
        if (pos != std::string::npos)
            return pos;
        return -1;
    }

    String StringImpl::remove(int start_index)
    {
        return make_string(*this)->erase(start_index);
    }

    String StringImpl::remove(int start_index, int count)
    {
        return make_string(*this)->erase(start_index, count);
    }

    String StringImpl::replace(const char old_char, const char new_char) const
    {
        String make_str = *this;
        std::replace(make_str->begin(), make_str->end(), old_char, new_char);
        return make_str;
    }

    list<String> StringImpl::split(char separator) const
    {
        list<String> result = make_list<String>();

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

    list<String> StringImpl::split(String separator) const
    {
        list<String> result = make_list<String>();

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

    String StringImpl::trim() const
    {
        String result = make_string(*this);

        // Trim end
        result->erase(std::find_if(result->rbegin(), result->rend(), [](char ch) {
            return !CharImpl::is_white_space(ch);
        }).base(), result->end());

        // Trim start
        result->erase(result->begin(), std::find_if(result->begin(), result->end(), [](char ch) {
            return !CharImpl::is_white_space(ch);
        }));

        return result;
    }

    String StringImpl::trim_start() const
    {
        String result = make_string(*this);
        result->erase(result->begin(), std::find_if(result->begin(), result->end(), [](char ch) {
            return !CharImpl::is_white_space(ch);
        }));
        return result;
    }

    String StringImpl::trim_end() const
    {
        String result = make_string(*this);
        result->erase(std::find_if(result->rbegin(), result->rend(), [](char ch) {
            return !CharImpl::is_white_space(ch);
        }).base(), result->end());
        return result;
    }

    bool StringImpl::contains(const String& s) const
    {
        return this->find(*s) != std::string::npos;
    }

    String StringImpl::to_lower() const
    {
        String result = make_string(*this);
        std::transform(result->begin(), result->end(), result->begin(),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    }

    String StringImpl::to_upper() const
    {
        String result = make_string(*this);
        std::transform(result->begin(), result->end(), result->begin(),
            [](unsigned char c) { return std::toupper(c); });
        return result;
    }

    bool String::is_null_or_empty(String value)
    {
        if (value == nullptr || value->empty())
            return true;
        return false;
    }

    bool String::operator==(const Object& rhs) const
    {
        // If rhs is null, return false. Otherwise, check if
        // the other Object is a String. If yes, compare by value.
        // If no, return false.
        if (rhs == nullptr)
        {
            return false;
        }
        else
        {
            String rhs_str = rhs.as<String>();
            if (rhs_str != nullptr) return operator==(rhs_str);
            else return false;
        }
    }

    std::string String::format_impl(fmt::string_view format_str, fmt::format_args args)
    {
        return fmt::vformat(format_str, args);
    }
}
