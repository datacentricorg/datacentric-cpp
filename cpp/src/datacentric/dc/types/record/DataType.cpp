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
#include <dc/types/record/DataType.hpp>
#include <dc/serialization/ITreeWriter.hpp>
#include <dot/system/reflection/Activator.hpp>
#include <dc/types/record/KeyType.hpp>


namespace dc
{

    void SerializeTo(dot::IObjectEnumerable obj, dot::string elementName, ITreeWriter writer)
    {
        // Write start element tag
        writer->WriteStartArrayElement(elementName);

        // Iterate over sequence elements
        for (dot::object item : obj)
        {
            // Write array item start tag
            writer->WriteStartArrayItem();

            if (item.IsEmpty())
            {
                writer->WriteStartValue();
                writer->WriteValue(item);
                writer->WriteEndValue();
                writer->WriteEndArrayItem();
                continue;
            }

            // Serialize based on type of the item
            dot::type_t itemType = item->GetType();

            if (itemType->Equals(dot::typeof<dot::string>())
                || itemType->Equals(dot::typeof<double>())
                || itemType->Equals(dot::typeof<bool>())
                || itemType->Equals(dot::typeof<int>())
                || itemType->Equals(dot::typeof<int64_t>())
                || itemType->Equals(dot::typeof<dot::local_date>())
                || itemType->Equals(dot::typeof<dot::local_date_time>())
                || itemType->Equals(dot::typeof<dot::local_time>())
                || itemType->Equals(dot::typeof<dot::local_minute>())
                || itemType->IsEnum
                || itemType->Equals(dot::typeof<ObjectId>())
                )
            {
                writer->WriteStartValue();
                writer->WriteValue(item);
                writer->WriteEndValue();
            }
            else
            if (!itemType->GetInterface("IObjectEnumerable").IsEmpty())
            {
                throw dot::exception(dot::string::Format("Serialization is not supported for element {0} "
                    "which is collection containing another collection.", elementName));
            }
            else
            if (item.is<Data>())
            {
                if (itemType->Name->EndsWith("Key"))
                {
                    // Embedded as string key
                    writer->WriteStartValue();
                    writer->WriteValue(item->ToString());
                    writer->WriteEndValue();
                }
                else
                {
                    ((Data)item)->SerializeTo(writer);
                }
            }
            else
            {
                throw dot::exception(dot::string::Format(
                    "Element type {0} is not supported for tree serialization.", itemType->Name));
            }


            // Write array item end tag
            writer->WriteEndArrayItem();
        }

        // Write matching end element tag
        writer->WriteEndArrayElement(elementName);
    }


    void DataImpl::SerializeTo(ITreeWriter writer)
    {
        // Write start tag
        writer->WriteStartDict();

        // Iterate over the list of elements
        dot::Array1D<dot::field_info> innerElementInfoList = GetType()->GetProperties();
        for (dot::field_info innerElementInfo : innerElementInfoList)
        {
            // Get element name and value
            dot::string innerElementName = innerElementInfo->Name;
            dot::object innerElementValue = innerElementInfo->GetValue(this);

            if (innerElementValue.IsEmpty())
            {
                continue;
            }

            dot::type_t elementType = innerElementValue->GetType();

            if (   elementType->Equals(dot::typeof<dot::string>())
                || elementType->Equals(dot::typeof<double>())
                || elementType->Equals(dot::typeof<bool>())
                || elementType->Equals(dot::typeof<int>())
                || elementType->Equals(dot::typeof<int64_t>())
                || elementType->Equals(dot::typeof<dot::local_date>())
                || elementType->Equals(dot::typeof<dot::local_date_time>())
                || elementType->Equals(dot::typeof<dot::local_time>())
                || elementType->Equals(dot::typeof<dot::local_minute>())
                || elementType->IsEnum
                || elementType->Equals(dot::typeof<ObjectId>())
                )
            {
                writer->WriteValueElement(innerElementName, innerElementValue);
            }
            else
            if (!elementType->GetInterface("IObjectEnumerable").IsEmpty())
            {
                dc::SerializeTo((dot::IObjectEnumerable)innerElementValue, innerElementName, writer);
            }
            else
            if (innerElementValue.is<Data>())
            {
                if (innerElementValue->GetType()->Name->EndsWith("Key"))
                {
                    // Embedded as string key
                    writer->WriteValueElement(innerElementName, innerElementValue->ToString());
                }
                else
                {
                    // Embedded as data
                    writer->WriteStartElement(innerElementName);
                    ((Data)innerElementValue)->SerializeTo(writer);
                    writer->WriteEndElement(innerElementName);
                }
            }
            else
            {
                throw dot::exception(dot::string::Format("Element type {0} is not supported for tree serialization.", innerElementInfo->PropertyType));
            }
        }

        // Write end tag
        writer->WriteEndDict();
    }

}
