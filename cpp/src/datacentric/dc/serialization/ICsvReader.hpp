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
    /// <summary>Interface used by IXmlInputImpl to read XML data.</summary>
    class DC_CLASS ICsvReader : public virtual dot::object_impl
    {
    public: // METHODS

        /// <summary>(ICsvReader) Returns true at the end of line (row).</summary>
        virtual bool isEol() = 0;

        /// <summary>(ICsvReader) Returns true at the end of file.</summary>
        virtual bool isEof() = 0;

        /// <summary>(ICsvReader) Read atomic value (error message at the end of line or file).</summary>
        virtual std::string readValue() = 0;
    };
}

