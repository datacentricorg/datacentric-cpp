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
    /// Verbosity level of log output.
    enum class LogVerbosity : int
    {
        /// Empty
        empty,

        /// No output even for errors.
        Silent,

        /// Error messages only with message arguments truncated to 20 characters.
        ErrorSummary,

        /// Error messages only with message arguments truncated to 255 characters.
        ErrorDetails,

        /// All log entry types with message arguments truncated to 20 characters.
        MessageSummary,

        /// All log entry types with message arguments truncated to 255 characters.
        MessageDetails
    };
}
