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

#include <dc/implement.hpp>
#include <dc/serialization/CsvWriter.hpp>

namespace dc
{
    class ICsvWriterImpl; using ICsvWriter = dot::ptr<ICsvWriterImpl>;

    int CsvWriterImpl::rowCount()
    {
        return static_cast<int>(rows_.size());
    }            

    int CsvWriterImpl::colCount()
    {
        return colCount_;
    }
    
    void CsvWriterImpl::writeValue(const std::string& token)
    {
        //!! checkUnlocked();

        // Create row if not currently open
        if (currentRow_ == nullptr)
        {
            rows_.push_back(std::vector<std::string>());
            currentRow_ = &(rows_.back());
        }

        // Append token with escaped delimiter and quotes
        currentRow_->push_back(token); //!!!! Should be emplace_back(CsvUtil.escapeListSeparator(token))

        // Increment col count if less than current row length
        if (colCount_ < currentRow_->size()) colCount_ = currentRow_->size();
    }
    
    void CsvWriterImpl::nextRow()
    {
        //!! checkUnlocked();

        if (currentRow_ != nullptr)
        {
            // Setting current row to null will cause a new row
            // to be created next time a token is added. The
            // current row is already added to the rows_ list.
            currentRow_ = nullptr;
        }
        else
        {
            // Otherwise add empty row
            rows_.push_back(std::vector<std::string>());
        }
    }

    void CsvWriterImpl::testDataSerialization()
    {
        /*!! Move to CsvOutput test?
        // Create and populate a mock data object
        auto data = ComplexMockData::create();
        data->ComplexTypeID = "ComplexTypeID";
        data->StringValue = "StringValue";
        data->DoubleValue = 1.0;
        data->IntValue = 1;
        data->EnumValue = SimpleMockEnum::EnumValue1;
        */

        // Create CSV writer
        ICsvWriter writer = new CsvWriterImpl();
        writer->writeValue("A");
        writer->writeValue("B");
        writer->nextRow();
        writer->writeValue("C");
        writer->writeValue("D");
        writer->writeValue("E");
        
        int rowCount = writer->rowCount();
        int colCount = writer->colCount();
    }
}
