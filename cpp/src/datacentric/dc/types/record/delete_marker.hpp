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
#include <dc/types/record/record_base.hpp>

namespace dc
{
    class DeleteMarkerImpl; using DeleteMarker = dot::ptr<DeleteMarkerImpl>;

    inline DeleteMarker make_DeleteMarker();

    /// When returned by the data source, this record has the same
    /// effect as if no record was found. It is used to indicate
    /// a deleted record when audit log must be preserved.
    class DC_CLASS DeleteMarkerImpl : public record_base_impl
    {
        typedef DeleteMarkerImpl self;

        friend DeleteMarker make_DeleteMarker();

    public: // PROPERTIES

        /// dot::string key consists of semicolon delimited primary key elements:
        ///
        /// KeyElement1;KeyElement2
        ///
        /// To avoid serialization format uncertainty, key elements
        /// can have any atomic type except Double.
        virtual dot::string get_key() { return dot::string::empty; }

    public: // CONSTRUCTORS

        DeleteMarkerImpl() = default;

    public:

        DOT_TYPE_BEGIN("dc", "DeleteMarker")
            DOT_TYPE_BASE(record_base)
            DOT_TYPE_CTOR(make_DeleteMarker)
        DOT_TYPE_END()
    };

    inline DeleteMarker make_DeleteMarker() { return new DeleteMarkerImpl(); }
}
