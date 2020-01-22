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
    class delete_marker_impl; using delete_marker = dot::ptr<delete_marker_impl>;

    inline delete_marker make_delete_marker();

    /// When returned by the data source, this record has the same
    /// effect as if no record was found. It is used to indicate
    /// a deleted record when audit log must be preserved.
    class DC_CLASS delete_marker_impl : public record_base_impl
    {
        typedef delete_marker_impl self;

        friend delete_marker make_delete_marker();

    public: // PROPERTIES

        /// dot::string key consists of semicolon delimited primary key elements:
        ///
        /// key_element1;key_element2
        ///
        /// To avoid serialization format uncertainty, key elements
        /// can have any atomic type except double.
        virtual dot::string get_key() { return dot::string::empty; }

    public: // CONSTRUCTORS

        delete_marker_impl() = default;

    public:

        DOT_TYPE_BEGIN("dc", "delete_marker")
            DOT_TYPE_BASE(record_base)
            DOT_TYPE_CTOR(make_delete_marker)
        DOT_TYPE_END()
    };

    inline delete_marker make_delete_marker() { return new delete_marker_impl(); }
}
