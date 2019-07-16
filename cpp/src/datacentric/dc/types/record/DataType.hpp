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
#include <dc/platform/data_source/mongo/ObjectId.hpp>
#include <dot/system/Type.hpp>

namespace dc
{
    class ITreeWriterImpl; using ITreeWriter = dot::Ptr<ITreeWriterImpl>;
    class DataImpl; using Data = dot::Ptr<DataImpl>;
    class KeyTypeImpl; using KeyType = dot::Ptr<KeyTypeImpl>;

    /// <summary>Data objects must derive from this type.</summary>
    class DC_CLASS DataImpl : public virtual dot::ObjectImpl
    {
        typedef DataImpl self;

    public:

        DOT_GET(dot::String, ClassName, { return this->GetType()->Name; })

    public: // METHODS

        /// <summary>Creates dictionary at current writer level.</summary>
        void SerializeTo(ITreeWriter writer);

        DOT_TYPE_BEGIN(".Runtime.Main", "Data")
            ->WithProperty("_t", &self::ClassName)
        DOT_TYPE_END()

    protected:
        static const char separator = ';';

    };
}
