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
#include <dc/types/record/record.hpp>
#include <dc/platform/dataset/data_set_key.hpp>

namespace dc
{
    class data_set_key_impl; using data_set_key = dot::ptr<data_set_key_impl>;
    class data_set_data_impl; using data_set_data = dot::ptr<data_set_data_impl>;
    class dot::object_id;

    inline data_set_data make_data_set_data();

    /// data_set key is a required field for all stored records.
    /// It is used to separate records into logical groups within the
    /// same DB collection or table.
    class DC_CLASS data_set_data_impl : public record_impl<data_set_key_impl, data_set_data_impl>
    {
        typedef data_set_data_impl self;
        friend data_set_data make_data_set_data();

    public:

        /// dot::object_id of the dataset where the record is stored.
        ///
        /// This override for the data_set_data record sets data_set to
        /// dot::object_id.Empty for the common dataset.
        dot::object_id data_set;

        /// Unique dataset identifier.
        dot::string data_set_id;

        /// Set context and perform initialization or validation of object data.
        ///
        /// All derived classes overriding this method must call base.Init(context)
        /// before executing the the rest of the code in the method override.
        virtual void Init(context_base context);

        /// data_set parents.
        dot::list<dot::object_id> Parents;

        DOT_TYPE_BEGIN("dc", "data_set_data")
            DOT_TYPE_PROP(data_set_id)
            DOT_TYPE_PROP(Parents)
            DOT_TYPE_CTOR(make_data_set_data)
            DOT_TYPE_BASE(record<data_set_key_impl, data_set_data_impl>)
        DOT_TYPE_END()

    protected: // CONSTRUCTORS

        data_set_data_impl() = default;
    };

    inline data_set_data make_data_set_data() { return new data_set_data_impl; }
}
