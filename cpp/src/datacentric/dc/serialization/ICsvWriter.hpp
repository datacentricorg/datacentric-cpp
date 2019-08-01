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
    class DC_CLASS ICsvWriterImpl : public virtual dot::object_impl
    {
    public: // METHODS

        /// <summary>(ICsvWriterImpl) Number of rows written so far.</summary>
        virtual int rowCount() = 0;

        /// <summary>(ICsvWriterImpl) Number of values in the longest row written so far.</summary>
        virtual int colCount() = 0;

        /// <summary>(ICsvWriterImpl) Write a single value escaping the list delimiter and double quotes.</summary>
        virtual void writeValue(const std::string& token) = 0;

        /// <summary>(ICsvWriterImpl) Advance to the next row.</summary>
        virtual void nextRow() = 0;
    };
}
