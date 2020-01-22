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
#include <dot/implement.hpp>
#include <dot/system/type.hpp>
#include <dot/system/object.hpp>
#include <dot/detail/struct_wrapper.hpp>
#include <dot/system/string.hpp>
#include <dot/system/nullable.hpp>
#include <dot/noda_time/local_minute.hpp>
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_date.hpp>
#include <dot/noda_time/local_date_time.hpp>

namespace dot
{
    object::object(nullptr_t) : base(nullptr) {}

    object::object(const ptr<object_impl>& p) : base(p) {}

    object::object(object_impl* value) : base(value) {}

    object::object(const string& value) : base(value) {}

    object::object(const char* value) : base(string(value)) {}

    object::object(bool value) : base(new bool_impl(value)) {}

    object::object(double value) : base(new double_impl(value)) {}

    object::object(int value) : base(new int_impl(value)) {}

    object::object(int64_t value) : base(new long_impl(value)) {}

    object::object(char value) : base(new char_impl(value)) {}

    object::object(const LocalMinute & value) : base(new struct_wrapper_impl<LocalMinute>(value)) {}

    object::object(const LocalTime & value) : base(new struct_wrapper_impl<LocalTime>(value)) {}

    object::object(const LocalDate & value) : base(new struct_wrapper_impl<LocalDate>(value)) {}

    object::object(const LocalDateTime & value) : base(new struct_wrapper_impl<LocalDateTime>(value)) {}

    bool object::operator==(nullptr_t) const { return base::operator==(nullptr); }

    bool object::operator!=(nullptr_t) const { return base::operator!=(nullptr); }

    object& object::operator=(nullptr_t) { base::operator=(nullptr); return *this; }

    object& object::operator=(const ptr<object_impl>& p) { base::operator=(p); return *this; }

    object& object::operator=(const string& value) { base::operator=(value); return *this; }

    object& object::operator=(const char* value) { base::operator=(string(value)); return *this; }

    object& object::operator=(bool value) { base::operator=(new bool_impl(value)); return *this; }

    object& object::operator=(double value) { base::operator=(new double_impl(value)); return *this; }

    object& object::operator=(int value) { base::operator=(new int_impl(value)); return *this; }

    object& object::operator=(int64_t value) { base::operator=(new long_impl(value)); return *this; }

    object& object::operator=(char value) { base::operator=(new char_impl(value)); return *this; }

    object& object::operator=(const LocalMinute& value) { base::operator=(new struct_wrapper_impl<LocalMinute>(value)); return *this; }

    object& object::operator=(const LocalTime& value) { base::operator=(new struct_wrapper_impl<LocalTime>(value)); return *this; }

    object& object::operator=(const LocalDate& value) { base::operator=(new struct_wrapper_impl<LocalDate>(value)); return *this; }

    object& object::operator=(const LocalDateTime& value) { base::operator=(new struct_wrapper_impl<LocalDateTime>(value)); return *this; }

    object::operator bool() const { return ptr<bool_impl>(*this)->value_; }

    object::operator double() const { return ptr<double_impl>(*this)->value_; }

    object::operator int() const { return ptr<int_impl>(*this)->value_; }

    object::operator int64_t() const { return ptr<long_impl>(*this)->value_; }

    object::operator char() const { return ptr<char_impl>(*this)->value_; }

    object::operator LocalMinute() const { return *ptr<struct_wrapper_impl<LocalMinute>>(*this); }

    object::operator LocalTime() const { return *ptr<struct_wrapper_impl<LocalTime>>(*this); }

    object::operator LocalDate() const { return *ptr<struct_wrapper_impl<LocalDate>>(*this); }

    object::operator LocalDateTime() const { return *ptr<struct_wrapper_impl<LocalDateTime>>(*this); }

    object::operator string() const { return *ptr<string_impl>(this->as<string>()); }

    bool object::reference_equals(object obj_a, object obj_b)
    {
        return &(*obj_a) == &(*obj_b);
    }
}
