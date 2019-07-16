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

    class MongoTestKeyImpl; using MongoTestKey = dot::Ptr<MongoTestKeyImpl>;
    class MongoTestDataImpl; using MongoTestData = dot::Ptr<MongoTestDataImpl>;

    MongoTestKey new_MongoTestKey();

    /// <summary>Key class.</summary>
    class MongoTestKeyImpl : public KeyForImpl<MongoTestKeyImpl, MongoTestDataImpl>
    {
        typedef MongoTestKeyImpl self;

    public:

        DOT_AUTO_PROP(dot::String, RecordID)
        DOT_AUTO_PROP(dot::Nullable<int>, RecordIndex)

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
    class MongoTestDataImpl : public RecordForImpl<MongoTestKeyImpl, MongoTestDataImpl>
    {
        typedef MongoTestDataImpl self;

    public:

        DOT_AUTO_PROP(dot::String, RecordID)
        DOT_AUTO_PROP(dot::Nullable<int>, RecordIndex)
        DOT_AUTO_PROP(dot::Nullable<double>, DoubleElement)
        DOT_AUTO_PROP(dot::Nullable<dot::LocalDate>, LocalDateElement)
        DOT_AUTO_PROP(dot::Nullable<dot::LocalTime>, LocalTimeElement)
        DOT_AUTO_PROP(dot::Nullable<dot::LocalMinute>, LocalMinuteElement)
        DOT_AUTO_PROP(dot::Nullable<dot::LocalDateTime>, LocalDateTimeElement)
        DOT_AUTO_PROP(MongoTestEnum, EnumValue)
        DOT_AUTO_PROP(dot::Nullable<int>, Version)

        DOT_TYPE_BEGIN(".Runtime.Test", "MongoTestData")
            DOT_TYPE_PROP(RecordID)
            DOT_TYPE_PROP(RecordIndex)
            DOT_TYPE_PROP(DoubleElement)
            DOT_TYPE_PROP(LocalDateElement)
            DOT_TYPE_PROP(LocalTimeElement)
            DOT_TYPE_PROP(LocalMinuteElement)
            DOT_TYPE_PROP(LocalDateTimeElement)
            DOT_TYPE_PROP(EnumValue)
            DOT_TYPE_PROP(Version)
            DOT_TYPE_BASE(RecordFor<MongoTestKeyImpl, MongoTestDataImpl>)
            DOT_TYPE_CTOR(new_MongoTestData)
        DOT_TYPE_END()
    };

    inline MongoTestData new_MongoTestData() { return new MongoTestDataImpl; }

    class ElementSampleDataImpl; using ElementSampleData = dot::Ptr<ElementSampleDataImpl>;

    ElementSampleData new_ElementSampleData();

    /// <summary>Element data class.</summary>
    class ElementSampleDataImpl : public DataImpl
    {
        typedef ElementSampleDataImpl self;
    public:
        DOT_AUTO_PROP(dot::Nullable<double>, DoubleElement3)
        DOT_AUTO_PROP(dot::String, StringElement3)

        DOT_TYPE_BEGIN(".Runtime.Test", "ElementSampleData")
            DOT_TYPE_PROP(DoubleElement3)
            DOT_TYPE_PROP(StringElement3)
            DOT_TYPE_BASE(Data)
            DOT_TYPE_CTOR(new_ElementSampleData)
        DOT_TYPE_END()
    };

    inline ElementSampleData new_ElementSampleData() { return new ElementSampleDataImpl; }

    class MongoTestDerivedDataImpl; using MongoTestDerivedData = dot::Ptr<MongoTestDerivedDataImpl>;

    MongoTestDerivedData new_MongoTestDerivedData();

    /// <summary>Derived data class.</summary>
    class MongoTestDerivedDataImpl : public MongoTestDataImpl
    {
        typedef MongoTestDerivedDataImpl self;
    public:
        DOT_AUTO_PROP(dot::Nullable<double>, DoubleElement2)
        DOT_AUTO_PROP(dot::String, StringElement2)
        DOT_AUTO_PROP(dot::Array1D<dot::String>, ArrayOfString)
        DOT_AUTO_PROP(dot::List<dot::String>, ListOfString)
        DOT_AUTO_PROP(dot::Array1D<double>, ArrayOfDouble)
        DOT_AUTO_PROP(dot::Array1D<dot::Nullable<double>>, ArrayOfNullableDouble)
        DOT_AUTO_PROP(dot::List<double>, ListOfDouble)
        DOT_AUTO_PROP(dot::List<dot::Nullable<double>>, ListOfNullableDouble)
        DOT_AUTO_PROP(ElementSampleData, DataElement)
        DOT_AUTO_PROP(dot::List<ElementSampleData>, DataElementList)
        DOT_AUTO_PROP(MongoTestKey, KeyElement)
        DOT_AUTO_PROP(dot::List<MongoTestKey>, KeyElementList)

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

    class MongoTestOtherDerivedDataImpl; using MongoTestOtherDerivedData = dot::Ptr<MongoTestOtherDerivedDataImpl>;

    MongoTestOtherDerivedData new_MongoTestOtherDerivedData();

    /// <summary>Other derived data class.</summary>
    class MongoTestOtherDerivedDataImpl : public MongoTestDataImpl
    {
        typedef MongoTestOtherDerivedDataImpl self;

    public:

        DOT_AUTO_PROP(dot::Nullable<double>, OtherDoubleElement2)
        DOT_AUTO_PROP(dot::String, OtherStringElement2)

        DOT_TYPE_BEGIN(".Runtime.Test", "MongoTestOtherDerivedData")
            DOT_TYPE_PROP(OtherDoubleElement2)
            DOT_TYPE_PROP(OtherStringElement2)
            DOT_TYPE_BASE(MongoTestData)
            DOT_TYPE_CTOR(new_MongoTestOtherDerivedData)
        DOT_TYPE_END()
    };

    inline MongoTestOtherDerivedData new_MongoTestOtherDerivedData() { return new MongoTestOtherDerivedDataImpl; }

    class MongoTestDerivedFromDerivedDataImpl; using MongoTestDerivedFromDerivedData = dot::Ptr<MongoTestDerivedFromDerivedDataImpl>;

    MongoTestDerivedFromDerivedData new_MongoTestDerivedFromDerivedData();

    /// <summary>Next level in the inheritance chain.</summary>
    class MongoTestDerivedFromDerivedDataImpl : public MongoTestDerivedDataImpl
    {
        typedef MongoTestDerivedFromDerivedDataImpl self;

    public:

        DOT_AUTO_PROP(dot::Nullable<double>, OtherDoubleElement3)
        DOT_AUTO_PROP(dot::String, OtherStringElement3)

        DOT_TYPE_BEGIN(".Runtime.Test", "MongoTestDerivedFromDerivedData")
            DOT_TYPE_PROP(OtherDoubleElement3)
            DOT_TYPE_PROP(OtherStringElement3)
            DOT_TYPE_BASE(MongoTestDerivedData)
            DOT_TYPE_CTOR(new_MongoTestDerivedFromDerivedData)
        DOT_TYPE_END()
    };

    inline MongoTestDerivedFromDerivedData new_MongoTestDerivedFromDerivedData() { return new MongoTestDerivedFromDerivedDataImpl; }
}
