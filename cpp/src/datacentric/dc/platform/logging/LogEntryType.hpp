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

namespace dc
{
    /// Enumeration for the log entry type.
    enum class LogEntryType : int
    {
        /// Empty
        empty,

        /// Error entry type is not logged directly for those log types
        /// where the output is subsequently performed by the exception catch block.
        Error,

        /// Warning.
        Warning,

        /// Informational message.
        Message,

        /// Passed test.
        Passed,

        /// Failed test (not necessarily followed by exception).
        Failed,

        /// Result (description is not formatted using {0}, {1} tags).
        Result
    };
}
