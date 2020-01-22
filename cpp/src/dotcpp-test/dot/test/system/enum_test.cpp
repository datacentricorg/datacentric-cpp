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

#include <dot/test/implement.hpp>
#include <approvals/ApprovalTests.hpp>
#include <approvals/Catch.hpp>
#include <dot/system/object.hpp>
#include <dot/system/enum_impl.hpp>
#include <dot/system/text/string_builder.hpp>
#include <dot/test/system/enum_test_apples_sample.hpp>
#include <dot/test/system/enum_test_colors_sample.hpp>

namespace dot
{
    TEST_CASE("smoke")
    {
        StringBuilder received = make_string_builder();

        // Serialization
        received->append_line("serialization");
        //received->indent++;

        // Serialize
        if (true)
        {
            ApplesSample value = ApplesSample::red;
            String serialized_value = to_string(value);
            received->append_line(dot::String::format("serialized={0}", serialized_value));
        }

        // Serialize Nullable enum
        if (true)
        {
            // Establish that null.to_string() is String.empty, not null
            Nullable<ApplesSample> nullable_value = nullptr;
            String serialized_value = to_string(nullable_value);
            received->append_line(String::format("nullable_serialized(null).is_null={0}", serialized_value == nullptr));
            received->append_line(String::format("nullable_serialized(null).is_empty={0}", serialized_value == String::empty));
        }
        if (true)
        {
            Nullable<ApplesSample> nullable_value = ApplesSample::red;
            String serialized_value = to_string(nullable_value);
            received->append_line(String::format("nullable_serialized(red)={0}", serialized_value));
        }

        // Deserialization
        if (true)
        {
            dot::String str = "red";
            ApplesSample result;
            bool success = ToStringImpl<ApplesSample>::try_parse(str, result);
            received->append_line(dot::String::format("string={0} try_parse={1} value={2}", str, success, to_string(result)));
        }

        if (true)
        {
            dot::String str = "blue";
            ApplesSample result;
            bool success = ToStringImpl<ApplesSample>::try_parse(str, result);
            received->append_line(dot::String::format("string={0} try_parse={1} value={2}", str, success, to_string(result)));
        }

        // Boxing
        received->append_line("boxing");
        //received->indent++;

        // Check Type of boxed enum
        dot::Object boxed = ApplesSample::red;
        // received->append_line(dot::String::format("Type(boxed)={0}", boxed->get_type()));

        // Unbox to the correct Type
        ApplesSample unboxed = boxed.to_enum<ApplesSample>();
        received->append_line(dot::String::format("boxed={0} unboxed={1}", boxed->to_string(), to_string(unboxed)));

        // Establish that unboxing DOES enforce enum Type, unlike in C#.
        //
        // For added safety, this behavior has been intentionally made different
        // from C# where unboxing DOES NOT enforce enum Type. Similar code in
        // C# would have converted the enum based on its integer value.
        CHECK_THROWS((EnumImpl<ColorsSample>)boxed);

        Approvals::verify(*received);
    }
}
