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
#include <dot/system/type.hpp>

namespace dc
{
    class ITreeWriterImpl; using ITreeWriter = dot::ptr<ITreeWriterImpl>;
    class data_impl; using data = dot::ptr<data_impl>;
    class key_base_impl; using key_base = dot::ptr<key_base_impl>;

    /// <summary>Data objects must derive from this type.</summary>
    class DC_CLASS data_impl : public virtual dot::object_impl
    {
        typedef data_impl self;

    public: // METHODS

        /// <summary>Creates dictionary at current writer level.</summary>
        void SerializeTo(ITreeWriter writer);

        DOT_TYPE_BEGIN("DataCentric", "Data")
        DOT_TYPE_END()

    protected:

        static const char separator = ';';
    };
}
