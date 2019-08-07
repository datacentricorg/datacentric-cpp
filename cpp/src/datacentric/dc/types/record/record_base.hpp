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
#include <dc/types/record/data.hpp>

namespace dc
{
    class record_base_impl; using record_base = dot::ptr<record_base_impl>;
    class context_base_impl; using context_base = dot::ptr<context_base_impl>;

    /// Record objects must derive from this type.
    class DC_CLASS record_base_impl : public virtual data_impl
    {
        typedef record_base_impl self;

    public: // FIELDS

        /// dot::object_id of the record is specific to its version.
        ///
        /// For the record's history to be captured correctly, all
        /// update operations must assign a new dot::object_id with the
        /// timestamp that matches update time.
        dot::object_id ID;

        /// dot::object_id of the dataset where the record is stored.
        ///
        /// The records that may be stored in root dataset (including
        /// data source, database, database server, and common dataset
        /// records) must override this property to avoid an error about
        /// dataset not being set for the record.
        dot::object_id data_set;

        /// Use context to access resources.
        context_base Context;

    public: // PROPERTIES

        /// dot::string key consists of semicolon delimited primary key elements:
        ///
        /// KeyElement1;KeyElement2
        ///
        /// To avoid serialization format uncertainty, key elements
        /// can have any atomic type except Double.
        virtual dot::string get_key() = 0;

    public: // METHODS

        /// Set context and perform fast initialization or validation
        /// of class data. Must first invoke base.Init(context).
        virtual void Init(context_base context);

        dot::string to_string() { return get_key(); }

        DOT_TYPE_BEGIN("dc", "RecordBase")
            ->with_field("_id", &self::ID)
            ->with_field("_dataset", &self::data_set)
          //  ->WithProperty("_key", &self::Key)
            DOT_TYPE_BASE(data)
        DOT_TYPE_END()

    };
}
