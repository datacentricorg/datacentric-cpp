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
#include <dot/system/ptr.hpp>
#include <dc/platform/data_source/instance_type.hpp>
#include <dc/platform/data_source/data_source_data.hpp>
#include <dc/types/record/temporal_id.hpp>
#include <dot/mongo/mongo_db/mongo/client.hpp>

namespace dc
{
    class mongo_data_source_base_data_impl; using mongo_data_source_base_data = dot::ptr<mongo_data_source_base_data_impl>;
    class context_base_impl; using context_base = dot::ptr<context_base_impl>;
    class key_impl; using key = dot::ptr<key_impl>;
    class data_impl; using data = dot::ptr<data_impl>;

    /// Abstract base class for data source implementations based on MongoDB.
    ///
    /// This class provides functionality shared by all MongoDB data source types.
    class DC_CLASS mongo_data_source_base_data_impl : public data_source_data_impl
    {
        typedef mongo_data_source_base_data_impl self;

    protected: // FIELDS

        /// Prohibited characters in database name.
        static dot::list<char> prohibited_db_name_symbols_;

        /// Maximum length of the database on Mongo server including delimiters.
        static int max_db_name_length_;

        const bool use_scalar_discriminator_convention_ = false;

    protected: // FIELDS

        /// Type of instance controls the ability to do certain
        /// actions such as deleting (dropping) the database.
        instance_type instance_type_;

        /// Full name of the database on Mongo server including delimiters.
        dot::string db_name_;

        /// Interface to Mongo client in MongoDB C++ driver.
        dot::client client_;

        /// Interface to Mongo database in MongoDB C++ driver.
        dot::database db_;

        /// Previous temporal_id returned by create_ordered_object_id() method.
        temporal_id prev_object_id_ = temporal_id::empty;

    public: // METHODS

        /// Set context and perform initialization or validation of object data.
        ///
        /// All derived classes overriding this method must call base.init(context)
        /// before executing the the rest of the code in the method override.
        void init(context_base context) override;

        /// The returned temporal_ids have the following order guarantees:
        ///
        /// * For this data source instance, to arbitrary resolution; and
        /// * Across all processes and machines, to one second resolution
        ///
        /// One second resolution means that two temporal_ids created within
        /// the same second by different instances of the data source
        /// class may not be ordered chronologically unless they are at
        /// least one second apart.
        virtual temporal_id create_ordered_object_id() override;

        /// Permanently deletes (drops) the database with all records
        /// in it without the possibility to recover them later.
        ///
        /// This method should only be used to free storage. For
        /// all other purposes, methods that preserve history should
        /// be used.
        ///
        /// ATTENTION - THIS METHOD WILL DELETE ALL DATA WITHOUT
        /// THE POSSIBILITY OF RECOVERY. USE WITH CAUTION.
        virtual void delete_db() override;

    protected: // PROTECTED

        mongo_data_source_base_data_impl()
        {
            // TODO uncomment
            //if (use_scalar_discriminator_convention_)
            //{
            //    // Set discriminator convention to scalar. For this convention,
            //    // BSON element _t is a single string value equal to GetType().Name,
            //    // rather than the list of names for the entire inheritance chain.
            //    dot::mongo_client_settings::set_discriminator_convention(dot::discriminator_convention::scalar);
            //}
            //else
            //{
            //    // Set discriminator convention to hierarchical. For this convention,
            //    // BSON element _t is either an array of GetType().Name values for ell
            //    // types in the inheritance chain, or a single string value for a chain
            //    // of length 1.
            //    //
            //    // Choosing root type to be Record ensures that _t is always an array.
            //    dot::mongo_client_settings::set_discriminator_convention(dot::discriminator_convention::hierarchical);
            //}
        }
    };
}
