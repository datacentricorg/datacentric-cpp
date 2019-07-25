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
#include <dc/types/record/RecordType.hpp>

namespace dc
{
    class DeleteMarkerImpl; using DeleteMarker = dot::Ptr<DeleteMarkerImpl>;

    inline DeleteMarker new_DeleteMarker();

    /// <summary>
    /// When returned by the data source, this record has the same
    /// effect as if no record was found. It is used to indicate
    /// a deleted record when audit log must be preserved.
    /// </summary>
    class DC_CLASS DeleteMarkerImpl : public RecordTypeImpl
    {
        typedef DeleteMarkerImpl self;

        friend DeleteMarker new_DeleteMarker();

    public: // PROPERTIES

        /// <summary>
        /// dot::String key consists of semicolon delimited primary key elements:
        ///
        /// KeyElement1;KeyElement2
        ///
        /// To avoid serialization format uncertainty, key elements
        /// can have any atomic type except Double.
        /// </summary>
        dot::String Key;

    public: // CONSTRUCTORS

        DeleteMarkerImpl() = default;

    public:

        DOT_TYPE_BEGIN(".Runtime.Main", "DeleteMarker")
            DOT_TYPE_BASE(RecordType)
            DOT_TYPE_CTOR(new_DeleteMarker)
        DOT_TYPE_END()
    };

    inline DeleteMarker new_DeleteMarker() { return new DeleteMarkerImpl(); }
}
