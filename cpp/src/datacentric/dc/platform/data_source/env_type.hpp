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
#include <dot/system/enum.hpp>

namespace dc
{
    /// Specifies instance type.
    ///
    /// Some API functions are restricted based on the instance type.
    class DC_CLASS EnvType : public dot::EnumBase
    {
        typedef EnvType self;

    public:

        enum enum_type
        {
            /// Empty
            empty,

            /// Production environment.
            ///
            /// This environment type is used for live production data.
            ///
            /// For this environment type, database name is semicolon
            /// delimited string with the following format:
            ///
            /// PROD;EnvGroup;EnvName
            ///
            /// Database for this environment type cannot be deleted
            /// (dropped) through an API call.
            prod,

            /// Shared UAT (user acceptance testing) environment.
            ///
            /// For this environment type, database name is semicolon
            /// delimited string with the following format:
            ///
            /// UAT;EnvGroup;EnvName
            ///
            /// Database for this environment type cannot be deleted
            /// (dropped) through an API call.
            uat,

            /// Shared environment used for development.
            ///
            /// For this environment type, database name is semicolon
            /// delimited string with the following format:
            ///
            /// DEV;EnvGroup;EnvName
            ///
            /// Database for this environment type CAN be deleted
            /// (dropped) through an API call.
            dev,

            /// Personal environment of a specific user.
            ///
            /// For this environment type, database name is semicolon
            /// delimited string with the following format:
            ///
            /// USER;EnvGroup;EnvName
            ///
            /// Database for this environment type CAN be deleted
            /// (dropped) through an API call.
            user,

            /// Environment used for unit testing.
            ///
            /// For this environment type, database name is semicolon
            /// delimited string with the following format:
            ///
            /// TEST;EnvGroup;EnvName
            ///
            /// IMPORTANT - Database for the test environment type is
            /// AUTOMATICALLY deleted (dropped) at the start and the end
            /// of each unit test execution. They should not be used for
            /// any purpose other than unit tests.
            test,

            /// Environment type is used to specify a custom database
            /// name that does not use the standard semicolon delimited
            /// format of other environment types.
            ///
            /// For this environment type, EnvGroup must be null and
            /// EnvName is database name.
            ///
            /// Database for this environment type cannot be deleted
            /// (dropped) through an API call.
            custom,
        };

        DOT_ENUM_BEGIN("dc", "EnvType")
            DOT_ENUM_VALUE(empty)
            DOT_ENUM_VALUE(prod)
            DOT_ENUM_VALUE(uat)
            DOT_ENUM_VALUE(dev)
            DOT_ENUM_VALUE(user)
            DOT_ENUM_VALUE(test)
        DOT_ENUM_END()
    };
}
