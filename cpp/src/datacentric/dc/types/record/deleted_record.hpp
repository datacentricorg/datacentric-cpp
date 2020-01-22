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

namespace dc
{
    class DeletedRecordImpl; using DeletedRecord = dot::Ptr<DeletedRecordImpl>;

    inline DeletedRecord make_deleted_record(Key key);
    inline DeletedRecord make_deleted_record();

    /// When returned by the data source, this record has the same
    /// effect as if no record was found. It is used to indicate
    /// a deleted record when audit log must be preserved.
    class DC_CLASS DeletedRecordImpl : public RecordImpl
    {
        typedef DeletedRecordImpl self;

        friend DeletedRecord make_deleted_record(Key key);
        friend DeletedRecord make_deleted_record();

    public: // PROPERTIES

        /// dot::String key consists of semicolon delimited primary key elements:
        ///
        /// key_element1;key_element2
        ///
        /// To avoid serialization format uncertainty, key elements
        /// can have any atomic type except double.
        virtual dot::String get_key();

        dot::String key_;

    private: // CONSTRUCTORS

        DeletedRecordImpl(Key key);

        DeletedRecordImpl();

    public:

        DOT_TYPE_BEGIN("dc", "DeletedRecord")
            DOT_TYPE_BASE(Record)
            ->with_constructor(static_cast<DeletedRecord (*)(Key)>(&make_deleted_record), { "key" })
            ->with_constructor(static_cast<DeletedRecord(*) ()>(&make_deleted_record), {})
            ->with_field("_key", &DeletedRecordImpl::key_)
        DOT_TYPE_END()
    };

    inline DeletedRecord make_deleted_record(Key key) { return new DeletedRecordImpl(key); }
    inline DeletedRecord make_deleted_record() { return new DeletedRecordImpl(); }
}
