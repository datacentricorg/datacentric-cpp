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
    Object::Object(nullptr_t) : base(nullptr) {}

    Object::Object(const Ptr<ObjectImpl>& p) : base(p) {}

    Object::Object(ObjectImpl* value) : base(value) {}

    Object::Object(const String& value) : base(value) {}

    Object::Object(const char* value) : base(String(value)) {}

    Object::Object(bool value) : base(new BoolImpl(value)) {}

    Object::Object(double value) : base(new DoubleImpl(value)) {}

    Object::Object(int value) : base(new IntImpl(value)) {}

    Object::Object(int64_t value) : base(new LongImpl(value)) {}

    Object::Object(char value) : base(new CharImpl(value)) {}

    Object::Object(const LocalMinute & value) : base(new StructWrapperImpl<LocalMinute>(value)) {}

    Object::Object(const LocalTime & value) : base(new StructWrapperImpl<LocalTime>(value)) {}

    Object::Object(const LocalDate & value) : base(new StructWrapperImpl<LocalDate>(value)) {}

    Object::Object(const LocalDateTime & value) : base(new StructWrapperImpl<LocalDateTime>(value)) {}

    bool Object::operator==(nullptr_t) const { return base::operator==(nullptr); }

    bool Object::operator!=(nullptr_t) const { return base::operator!=(nullptr); }

    Object& Object::operator=(nullptr_t) { base::operator=(nullptr); return *this; }

    Object& Object::operator=(const Ptr<ObjectImpl>& p) { base::operator=(p); return *this; }

    Object& Object::operator=(const String& value) { base::operator=(value); return *this; }

    Object& Object::operator=(const char* value) { base::operator=(String(value)); return *this; }

    Object& Object::operator=(bool value) { base::operator=(new BoolImpl(value)); return *this; }

    Object& Object::operator=(double value) { base::operator=(new DoubleImpl(value)); return *this; }

    Object& Object::operator=(int value) { base::operator=(new IntImpl(value)); return *this; }

    Object& Object::operator=(int64_t value) { base::operator=(new LongImpl(value)); return *this; }

    Object& Object::operator=(char value) { base::operator=(new CharImpl(value)); return *this; }

    Object& Object::operator=(const LocalMinute& value) { base::operator=(new StructWrapperImpl<LocalMinute>(value)); return *this; }

    Object& Object::operator=(const LocalTime& value) { base::operator=(new StructWrapperImpl<LocalTime>(value)); return *this; }

    Object& Object::operator=(const LocalDate& value) { base::operator=(new StructWrapperImpl<LocalDate>(value)); return *this; }

    Object& Object::operator=(const LocalDateTime& value) { base::operator=(new StructWrapperImpl<LocalDateTime>(value)); return *this; }

    Object::operator bool() const { return Ptr<BoolImpl>(*this)->value_; }

    Object::operator double() const { return Ptr<DoubleImpl>(*this)->value_; }

    Object::operator int() const { return Ptr<IntImpl>(*this)->value_; }

    Object::operator int64_t() const { return Ptr<LongImpl>(*this)->value_; }

    Object::operator char() const { return Ptr<CharImpl>(*this)->value_; }

    Object::operator LocalMinute() const { return *Ptr<StructWrapperImpl<LocalMinute>>(*this); }

    Object::operator LocalTime() const { return *Ptr<StructWrapperImpl<LocalTime>>(*this); }

    Object::operator LocalDate() const { return *Ptr<StructWrapperImpl<LocalDate>>(*this); }

    Object::operator LocalDateTime() const { return *Ptr<StructWrapperImpl<LocalDateTime>>(*this); }

    Object::operator String() const { return *Ptr<StringImpl>(this->as<String>()); }

    bool Object::reference_equals(Object obj_a, Object obj_b)
    {
        return &(*obj_a) == &(*obj_b);
    }
}
