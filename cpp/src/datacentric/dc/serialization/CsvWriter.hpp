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
#include <dc/serialization/ICsvWriter.hpp>

namespace dc
{
    /// <summary>Interface used by IXmlInputImpl to read XML data.</summary>
    class DC_CLASS CsvWriterImpl : public ICsvWriterImpl
    {
        std::vector<std::vector<std::string>> rows_;
        std::vector<std::string>* currentRow_ = nullptr;
        int colCount_ = 0;

    public: //  METHODS

        /// <summary>(ICsvWriterImpl) Number of rows written so far.</summary>
        virtual int rowCount() override;

        /// <summary>(ICsvWriterImpl) Number of values in the longest row written so far.</summary>
        virtual int colCount() override;

        /// <summary>(ICsvWriterImpl) Write a single value escaping the list delimiter and double quotes.</summary>
        virtual void writeValue(const std::string& token) override;

        /// <summary>(ICsvWriterImpl) Advance to the next row.</summary>
        virtual void nextRow() override;

    public: // TESTS

        /// <summary>Smoke test of serialization</summary>
        void testDataSerialization(); //!! Add parameter for coverage
        void testCsvWriter(); //!! Add parameter for coverage
    };
}
