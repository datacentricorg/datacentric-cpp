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
#include <dc/platform/data_source/env_type.hpp>
#include <dc/platform/data_source/data_source_data.hpp>
#include <dc/platform/data_source/mongo/mongo_server_data.hpp>
#include <dc/types/record/temporal_id.hpp>
#include <dot/mongo/mongo_db/mongo/client.hpp>

namespace dc
{
    class MongoDataSourceBaseImpl; using MongoDataSourceBase = dot::Ptr<MongoDataSourceBaseImpl>;
    class ContextBaseImpl; using ContextBase = dot::Ptr<ContextBaseImpl>;
    class KeyImpl; using Key = dot::Ptr<KeyImpl>;
    class DataImpl; using Data = dot::Ptr<DataImpl>;

    /// Abstract base class for data source implementations based on MongoDB.
    ///
    /// This class provides functionality shared by all MongoDB data source types.
    class DC_CLASS MongoDataSourceBaseImpl : public DataSourceImpl
    {
        typedef MongoDataSourceBaseImpl self;

    protected: // FIELDS

        /// Prohibited characters in database name.
        static dot::List<char> prohibited_db_name_symbols_;

        /// Maximum length of the database on Mongo server including delimiters.
        static int max_db_name_length_;

        const bool use_scalar_discriminator_convention_ = false;

    protected: // FIELDS

        /// Type of instance controls the ability to do certain
        /// actions such as deleting (dropping) the database.
        EnvType env_type_;

        /// Full name of the database on Mongo server including delimiters.
        dot::String db_name_;

        /// Interface to Mongo client in MongoDB C++ driver.
        dot::Client client_;

        /// Interface to Mongo database in MongoDB C++ driver.
        dot::Database db_;

        /// Previous TemporalId returned by create_ordered_object_id() method.
        TemporalId prev_object_id_ = TemporalId::empty;

    public: // PROPERTIES

        /// Specifies Mongo server for this data source.
        ///
        /// Defaults to local server on the standard port if not specified.
        ///
        /// Server URI specified here must refer to the entire server, not
        /// an individual database.
        MongoServerKey mongo_server;

    public: // METHODS

        /// Set context and perform initialization or validation of Object data.
        ///
        /// All derived classes overriding this method must call base.init(context)
        /// before executing the the rest of the code in the method override.
        void init(ContextBase context) override;

        /// The returned temporal_ids have the following order guarantees:
        ///
        /// * For this data source instance, to arbitrary resolution; and
        /// * Across all processes and machines, to one second resolution
        ///
        /// One second resolution means that two temporal_ids created within
        /// the same second by different instances of the data source
        /// class may not be ordered chronologically unless they are at
        /// least one second apart.
        virtual TemporalId create_ordered_object_id() override;

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

        MongoDataSourceBaseImpl()
        {
            // TODO uncomment
            //if (use_scalar_discriminator_convention_)
            //{
            //    // Set discriminator convention to scalar. For this convention,
            //    // BSON element _t is a single String value equal to GetType().Name,
            //    // rather than the list of names for the entire inheritance chain.
            //    dot::MongoClientSettings::set_discriminator_convention(dot::DiscriminatorConvention::scalar);
            //}
            //else
            //{
            //    // Set discriminator convention to hierarchical. For this convention,
            //    // BSON element _t is either an array of GetType().Name values for ell
            //    // types in the inheritance chain, or a single String value for a chain
            //    // of length 1.
            //    //
            //    // Choosing root type to be Record ensures that _t is always an array.
            //    dot::MongoClientSettings::set_discriminator_convention(dot::DiscriminatorConvention::hierarchical);
            //}
        }
    };
}
