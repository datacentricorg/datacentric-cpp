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
#include <dot/mongo/serialization/bson_root_class_attribute.hpp>

namespace dc
{
    class RecordImpl; using Record = dot::Ptr<RecordImpl>;
    class ContextBaseImpl; using ContextBase = dot::Ptr<ContextBaseImpl>;

    /// Record objects must derive from this type.
    class DC_CLASS RecordImpl : public virtual DataImpl
    {
        typedef RecordImpl self;

    public: // FIELDS

        /// TemporalId of the record is specific to its version.
        ///
        /// For the record's history to be captured correctly, all
        /// update operations must assign a new TemporalId with the
        /// timestamp that matches update time.
        TemporalId id;

        /// TemporalId of the dataset where the record is stored.
        ///
        /// The records that may be stored in root dataset (including
        /// data source, database, database server, and common dataset
        /// records) must override this property to avoid an error about
        /// dataset not being set for the record.
        TemporalId data_set;

        /// Use context to access resources.
        dot::WeakPtr<ContextBaseImpl> context;

    public: // PROPERTIES

        /// dot::String key consists of semicolon delimited primary key elements:
        ///
        /// key_element1;key_element2
        ///
        /// To avoid serialization format uncertainty, key elements
        /// can have any atomic type except double.
        virtual dot::String get_key() = 0;

    private: // METHODS

        static void serialize_key(dot::tree_writer_base writer, dot::Object obj);

    public: // METHODS

        /// Set context and perform fast initialization or validation
        /// of class data. Must first invoke base.init(context).
        virtual void init(ContextBase context);

        dot::String to_string() { return get_key(); }

    public: // REFLECTION

        static dot::Type typeof();
        dot::Type get_type() override;
    };
}
