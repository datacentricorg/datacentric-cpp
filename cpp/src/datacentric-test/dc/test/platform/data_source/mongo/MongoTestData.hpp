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

#include <dc/test/implement.hpp>
#include <dc/platform/context/IContext.hpp>
#include <dc/types/record/RecordFor.hpp>
#include <dot/system/Enum.hpp>

namespace dc
{
    /// <summary>Enum type.</summary>
    class MongoTestEnum : public dot::Enum
    {
        typedef MongoTestEnum self;

    public:

        enum enum_type
        {
            Empty,
            EnumValue1,
            EnumValue2
        };

        DOT_ENUM_BEGIN(".Runtime.Test", "MongoTestEnum")
            DOT_ENUM_VALUE(Empty)
            DOT_ENUM_VALUE(EnumValue1)
            DOT_ENUM_VALUE(EnumValue2)
        DOT_ENUM_END()
    };

    class MongoTestKeyImpl; using MongoTestKey = dot::ptr<MongoTestKeyImpl>;
    class MongoTestDataImpl; using MongoTestData = dot::ptr<MongoTestDataImpl>;

    MongoTestKey new_MongoTestKey();

    /// <summary>Key class.</summary>
    class MongoTestKeyImpl : public KeyForImpl<MongoTestKeyImpl, MongoTestDataImpl>
    {
        typedef MongoTestKeyImpl self;

    public:

        dot::string RecordID;
        dot::Nullable<int> RecordIndex;

        DOT_TYPE_BEGIN(".Runtime.Test", "MongoTestKey")
            DOT_TYPE_PROP(RecordID)
            DOT_TYPE_PROP(RecordIndex)
            DOT_TYPE_BASE(KeyFor<MongoTestKeyImpl, MongoTestDataImpl>)
            DOT_TYPE_CTOR(new_MongoTestKey)
        DOT_TYPE_END()
    };

    inline MongoTestKey new_MongoTestKey() { return new MongoTestKeyImpl; }

    MongoTestData new_MongoTestData();

    /// <summary>Base data class.</summary>
    class MongoTestDataImpl : public record_for_impl<MongoTestKeyImpl, MongoTestDataImpl>
    {
        typedef MongoTestDataImpl self;

    public:

        dot::string RecordID;
        dot::Nullable<int> RecordIndex;
        dot::Nullable<double> DoubleElement;
        dot::Nullable<dot::local_date> local_dateElement;
        dot::Nullable<dot::local_time> local_timeElement;
        dot::Nullable<dot::local_minute> local_minuteElement;
        dot::Nullable<dot::local_date_time> local_date_timeElement;
        MongoTestEnum EnumValue;
        dot::Nullable<int> Version;

        DOT_TYPE_BEGIN(".Runtime.Test", "MongoTestData")
            DOT_TYPE_PROP(RecordID)
            DOT_TYPE_PROP(RecordIndex)
            DOT_TYPE_PROP(DoubleElement)
            DOT_TYPE_PROP(local_dateElement)
            DOT_TYPE_PROP(local_timeElement)
            DOT_TYPE_PROP(local_minuteElement)
            DOT_TYPE_PROP(local_date_timeElement)
            DOT_TYPE_PROP(EnumValue)
            DOT_TYPE_PROP(Version)
            DOT_TYPE_BASE(record_for<MongoTestKeyImpl, MongoTestDataImpl>)
            DOT_TYPE_CTOR(new_MongoTestData)
        DOT_TYPE_END()
    };

    inline MongoTestData new_MongoTestData() { return new MongoTestDataImpl; }

    class ElementSampleDataImpl; using ElementSampleData = dot::ptr<ElementSampleDataImpl>;

    ElementSampleData new_ElementSampleData();

    /// <summary>Element data class.</summary>
    class ElementSampleDataImpl : public DataImpl
    {
        typedef ElementSampleDataImpl self;
    public:
        dot::Nullable<double> DoubleElement3;
        dot::string StringElement3;

        DOT_TYPE_BEGIN(".Runtime.Test", "ElementSampleData")
            DOT_TYPE_PROP(DoubleElement3)
            DOT_TYPE_PROP(StringElement3)
            DOT_TYPE_BASE(Data)
            DOT_TYPE_CTOR(new_ElementSampleData)
        DOT_TYPE_END()
    };

    inline ElementSampleData new_ElementSampleData() { return new ElementSampleDataImpl; }

    class MongoTestDerivedDataImpl; using MongoTestDerivedData = dot::ptr<MongoTestDerivedDataImpl>;

    MongoTestDerivedData new_MongoTestDerivedData();

    /// <summary>Derived data class.</summary>
    class MongoTestDerivedDataImpl : public MongoTestDataImpl
    {
        typedef MongoTestDerivedDataImpl self;
    public:
        dot::Nullable<double> DoubleElement2;
        dot::string StringElement2;
        dot::array<dot::string> ArrayOfString;
        dot::List<dot::string> ListOfString;
        dot::array<double> ArrayOfDouble;
        dot::array<dot::Nullable<double>> ArrayOfNullableDouble;
        dot::List<double> ListOfDouble;
        dot::List<dot::Nullable<double>> ListOfNullableDouble;
        ElementSampleData DataElement;
        dot::List<ElementSampleData> DataElementList;
        MongoTestKey KeyElement;
        dot::List<MongoTestKey> KeyElementList;

        DOT_TYPE_BEGIN(".Runtime.Test", "MongoTestDerivedData")
            DOT_TYPE_PROP(DoubleElement2)
            DOT_TYPE_PROP(StringElement2)
            DOT_TYPE_PROP(ArrayOfString)
            DOT_TYPE_PROP(ListOfString)
            DOT_TYPE_PROP(ArrayOfDouble)
            DOT_TYPE_PROP(ArrayOfNullableDouble)
            DOT_TYPE_PROP(ListOfDouble)
            DOT_TYPE_PROP(ListOfNullableDouble)
            DOT_TYPE_PROP(DataElement)
            DOT_TYPE_PROP(DataElementList)
            DOT_TYPE_PROP(KeyElement)
            DOT_TYPE_PROP(KeyElementList)
            DOT_TYPE_BASE(MongoTestData)
            DOT_TYPE_CTOR(new_MongoTestDerivedData)
        DOT_TYPE_END()
    };

    inline MongoTestDerivedData new_MongoTestDerivedData() { return new MongoTestDerivedDataImpl; }

    class MongoTestOtherDerivedDataImpl; using MongoTestOtherDerivedData = dot::ptr<MongoTestOtherDerivedDataImpl>;

    MongoTestOtherDerivedData new_MongoTestOtherDerivedData();

    /// <summary>Other derived data class.</summary>
    class MongoTestOtherDerivedDataImpl : public MongoTestDataImpl
    {
        typedef MongoTestOtherDerivedDataImpl self;

    public:

        dot::Nullable<double> OtherDoubleElement2;
        dot::string OtherStringElement2;

        DOT_TYPE_BEGIN(".Runtime.Test", "MongoTestOtherDerivedData")
            DOT_TYPE_PROP(OtherDoubleElement2)
            DOT_TYPE_PROP(OtherStringElement2)
            DOT_TYPE_BASE(MongoTestData)
            DOT_TYPE_CTOR(new_MongoTestOtherDerivedData)
        DOT_TYPE_END()
    };

    inline MongoTestOtherDerivedData new_MongoTestOtherDerivedData() { return new MongoTestOtherDerivedDataImpl; }

    class MongoTestDerivedFromDerivedDataImpl; using MongoTestDerivedFromDerivedData = dot::ptr<MongoTestDerivedFromDerivedDataImpl>;

    MongoTestDerivedFromDerivedData new_MongoTestDerivedFromDerivedData();

    /// <summary>Next level in the inheritance chain.</summary>
    class MongoTestDerivedFromDerivedDataImpl : public MongoTestDerivedDataImpl
    {
        typedef MongoTestDerivedFromDerivedDataImpl self;

    public:

        dot::Nullable<double> OtherDoubleElement3;
        dot::string OtherStringElement3;

        DOT_TYPE_BEGIN(".Runtime.Test", "MongoTestDerivedFromDerivedData")
            DOT_TYPE_PROP(OtherDoubleElement3)
            DOT_TYPE_PROP(OtherStringElement3)
            DOT_TYPE_BASE(MongoTestDerivedData)
            DOT_TYPE_CTOR(new_MongoTestDerivedFromDerivedData)
        DOT_TYPE_END()
    };

    inline MongoTestDerivedFromDerivedData new_MongoTestDerivedFromDerivedData() { return new MongoTestDerivedFromDerivedDataImpl; }
}
