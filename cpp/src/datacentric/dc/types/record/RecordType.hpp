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
#include <dc/types/record/DataType.hpp>

namespace dc
{
    class RecordTypeImpl; using RecordType = dot::Ptr<RecordTypeImpl>;
    class IContextImpl; using IContext = dot::Ptr<IContextImpl>;

    /// <summary>Record objects must derive from this type.</summary>
    class DC_CLASS RecordTypeImpl : public virtual DataImpl
    {
        typedef RecordTypeImpl self;

    protected:
        /// <summary>Use context to access resources.</summary>
        IContext context_;

        /// <summary>Backing variable for the ID property.</summary>
        ObjectId id_;

        /// <summary>Backing variable for the DataSet property.</summary>
        ObjectId dataSet_;

    public: // PROPERTIES

        /// <summary>
        /// ObjectId of the record is specific to its version.
        ///
        /// For the record's history to be captured correctly, all
        /// update operations must assign a new ObjectId with the
        /// timestamp that matches update time.
        /// </summary>
        DOT_PROP(ObjectId, ID,
            {
                if (id_.IsEmpty())
                {
                    dot::String key = Key;
                    if (!key.IsEmpty()) throw dot::new_Exception(dot::String::Format("ID property is not set for the record with class name {0} and key {1}.", this->ClassName, this->Key));
                    else throw dot::new_Exception(dot::String::Format("ID property is not set for class {0}.", this->ClassName));
                }
                return id_;
            }
        , { id_ = value; });

        /// <summary>
       /// ObjectId of the dataset where the record is stored.
       ///
       /// The records that may be stored in root dataset (including
       /// data source, database, database server, and Common dataset
       /// records) must override this property to avoid an error about
       /// dataset not being set for the record.
       /// </summary>
       DOT_PROP(ObjectId, DataSet,
           {
               if (dataSet_.IsEmpty())
               {
                   dot::String key = Key;
                   if (!key.IsEmpty()) throw dot::new_Exception(dot::String::Format("DataSet property is not set for the record with class name {0} and key {1}.", this->ClassName, this->Key));
                   else throw dot::new_Exception(dot::String::Format("DataSet property is not set for class {0}.", this->ClassName));
               }
               return dataSet_;
           }
           , { dataSet_ = value; });

        /// <summary>
        /// dot::String key consists of semicolon delimited primary key elements:
        ///
        /// KeyElement1;KeyElement2
        ///
        /// To avoid serialization format uncertainty, key elements
        /// can have any atomic type except Double.
        /// </summary>
        DOT_DECL_PROP(dot::String, Key);

        /// <summary>Use context to access resources.</summary>
        DOT_GET(IContext, Context, {
            // Check that context is set
            if (context_ == nullptr) throw dot::new_Exception(
                dot::String::Format("Init(...) method has not been called for {0}.", GetType()->Name));
            return context_;
            })

    public: // METHODS

        /// <summary>
        /// Set context and perform fast initialization or validation
        /// of class data. Must first invoke base.Init(context).
        /// </summary>
        virtual void Init(IContext context);

        dot::String ToString() { return Key; }

        DOT_TYPE_BEGIN(".Runtime.Main", "RecordType")
            ->WithProperty("_id", &self::ID)
            ->WithProperty("_dataset", &self::DataSet)
            ->WithProperty("_key", &self::Key)
            DOT_TYPE_BASE(Data)
        DOT_TYPE_END()

    };
}
