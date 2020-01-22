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
#include <dc/platform/data_source/mongo/mongo_data_source_base_data.hpp>
#include <dc/platform/data_set/data_set_detail_data.hpp>

namespace dc
{
    class MongoDataSourceImpl; using MongoDataSource = dot::Ptr<MongoDataSourceImpl>;

    /// Data source implementation for MongoDB.
    class DC_CLASS MongoDataSourceImpl : public MongoDataSourceBaseImpl
    {
        typedef MongoDataSourceImpl self;

    public: // METHODS

        /// Load record by its TemporalId and Type.
        ///
        /// Return null if there is no record for the specified TemporalId;
        /// however an exception will be thrown if the record exists but
        /// is not derived from TRecord.
        virtual Record load_or_null(TemporalId id, dot::Type data_type) override;

        /// This method does not use cached value inside the key
        /// and always retrieves a new record from storage. To get
        /// the record cached inside the key instead (if present), use
        /// the caching variant of this method:
        ///
        /// load_or_null(key, load_from)
        ///
        /// Load record by String key from the specified dataset or
        /// its parent. The lookup occurs first in the reverse
        /// chronological order of datasets to one second resolution,
        /// and then in the reverse chronological order of records
        /// within the latest dataset that has at least one record.
        ///
        /// The root dataset has empty TemporalId value that is less
        /// than any other TemporalId value. Accordingly, the root
        /// dataset is the last one in the lookup order of datasets.
        ///
        /// The first record in this lookup order is returned, or null
        /// if no records are found or if delete marker is the first
        /// record.
        ///
        /// Return null if there is no record for the specified TemporalId;
        /// however an exception will be thrown if the record exists but
        /// is not derived from TRecord.
        virtual Record load_or_null(Key key, TemporalId load_from) override;

        /// Save multiple records to the specified dataset. After the method exits,
        /// for each record the property record.DataSet will be set to the value of
        /// the saveTo parameter.
        ///
        /// All Save methods ignore the value of record.DataSet before the
        /// Save method is called. When dataset is not specified explicitly,
        /// the value of dataset from the context, not from the record, is used.
        /// The reason for this behavior is that the record may be stored from
        /// a different dataset than the one where it is used.
        ///
        /// This method guarantees that TemporalIds of the saved records will be in
        /// strictly increasing order.
        virtual void save_many(dot::List<Record> records, TemporalId save_to) override;

        /// Get query for the specified Type.
        ///
        /// After applying query parameters, the lookup occurs first in the
        /// reverse chronological order of datasets to one second resolution,
        /// and then in the reverse chronological order of records
        /// within the latest dataset that has at least one record.
        ///
        /// The root dataset has empty TemporalId value that is less
        /// than any other TemporalId value. Accordingly, the root
        /// dataset is the last one in the lookup order of datasets.
        virtual MongoQuery get_query(TemporalId data_set, dot::Type type) override;

        /// Write a delete marker for the specified data_set and data_key
        /// instead of actually deleting the record. This ensures that
        /// a record in another dataset does not become visible during
        /// lookup in a sequence of datasets.
        ///
        /// To avoid an additional roundtrip to the data store, the delete
        /// marker is written even when the record does not exist.
        virtual void delete_record(Key key, TemporalId delete_in) override;

        /// Apply the final constraints after all prior Where clauses but before OrderBy clause:
        ///
        /// * The constraint on dataset lookup list, restricted by CutoffTime (if not null)
        /// * The constraint on ID being strictly less than CutoffTime (if not null)
        dot::Query apply_final_constraints(dot::Query query, TemporalId load_from);

        /// Get TemporalId of the dataset with the specified name.
        ///
        /// All of the previously requested dataSetIds are cached by
        /// the data source. To load the latest version of the dataset
        /// written by a separate process, clear the cache first by
        /// calling DataSource.ClearDataSetCache() method.
        ///
        /// Returns null if not found.
        virtual dot::Nullable<TemporalId> get_data_set_or_empty(dot::String data_set_id, TemporalId load_from) override;

        /// Save new version of the dataset.
        ///
        /// This method sets Id element of the argument to be the
        /// new TemporalId assigned to the record when it is saved.
        /// The timestamp of the new TemporalId is the current time.
        ///
        /// This method updates in-memory cache to the saved dataset.
        virtual void save_data_set(DataSet data_set_data, TemporalId save_to) override;

        /// Returns enumeration of import datasets for specified dataset data,
        /// including imports of imports to unlimited depth with cyclic
        /// references and duplicates removed.
        ///
        /// The list will not include datasets that are after the value of
        /// CutoffTime if specified, or their imports (including
        /// even those imports that are earlier than the constraint).
        dot::HashSet<TemporalId> get_data_set_lookup_list(TemporalId load_from);

        /// Get detail of the specified dataset.
        ///
        /// Returns null if the details record does not exist.
        ///
        /// The detail is loaded for the dataset specified in the first argument
        /// (detailFor) from the dataset specified in the second argument (loadFrom).
        DataSetDetail get_data_set_detail_or_empty(TemporalId detail_for);

        /// <summary>
        /// CutoffTime should only be used via this method which also takes into
        /// account the CutoffTime set in dataset detail record, and never directly.
        ///
        /// CutoffTime may be set in data source globally, or for a specific dataset
        /// in its details record. If CutoffTime is set for both, this method will
        /// return the earlier of the two values will be used.
        ///
        /// Records with TemporalId that is greater than or equal to CutoffTime
        /// will be ignored by load methods and queries, and the latest available
        /// record where TemporalId is less than CutoffTime will be returned instead.
        ///
        /// CutoffTime applies to both the records stored in the dataset itself,
        /// and the reports loaded through the Imports list.
        /// </summary>
        dot::Nullable<TemporalId> get_cutoff_time(TemporalId data_set_id);

        /// <summary>
        /// Gets ImportsCutoffTime from the dataset detail record.
        /// Returns null if dataset detail record is not found.
        ///
        /// Imported records (records loaded through the Imports list)
        /// where TemporalId is greater than or equal to CutoffTime
        /// will be ignored by load methods and queries, and the latest
        /// available record where TemporalId is less than CutoffTime will
        /// be returned instead.
        ///
        /// This setting only affects records loaded through the Imports
        /// list. It does not affect records stored in the dataset itself.
        ///
        /// Use this feature to freeze Imports as of a given CreatedTime
        /// (part of TemporalId), isolating the dataset from changes to the
        /// data in imported datasets that occur after that time.
        /// </summary>
        dot::Nullable<TemporalId> get_imports_cutoff_time(TemporalId data_set_id);

    private: // METHODS

        /// Get collection with name based on the Type.
        dot::Collection get_or_create_collection(dot::Type data_type);

        /// Builds hashset of import datasets for specified dataset data,
        /// including imports of imports to unlimited depth with cyclic
        /// references and duplicates removed. This method uses cached lookup
        /// list for the import datasets but not for the argument dataset.
        ///
        /// The list will not include datasets that are after the value of
        /// CutoffTime if specified, or their imports (including
        /// even those imports that are earlier than the constraint).
        ///
        /// This overload of the method will return the result hashset.
        ///
        /// This private helper method should not be used directly.
        /// It provides functionality for the public API of this class.
        dot::HashSet<TemporalId> build_data_set_lookup_list(DataSet data_set_data);

        /// Builds hashset of import datasets for specified dataset data,
        /// including imports of imports to unlimited depth with cyclic
        /// references and duplicates removed. This method uses cached lookup
        /// list for the import datasets but not for the argument dataset.
        ///
        /// The list will not include datasets that are after the value of
        /// CutoffTime if specified, or their imports (including
        /// even those imports that are earlier than the constraint).
        ///
        /// This overload of the method will return the result hashset.
        ///
        /// This private helper method should not be used directly.
        /// It provides functionality for the public API of this class.
        void build_data_set_lookup_list(DataSet data_set_data, dot::HashSet<TemporalId> result);

        /// Error message if one of the following is the case:
        ///
        /// * ReadOnly is set for the data source
        /// * ReadOnly is set for the dataset
        /// * CutoffTime is set for the data source
        /// * CutoffTime is set for the dataset
        void check_not_read_only(TemporalId dataSetId);

    public: // FIELDS

        /// Records with TemporalId that is greater than or equal to CutoffTime
        /// will be ignored by load methods and queries, and the latest available
        /// record where TemporalId is less than CutoffTime will be returned instead.
        ///
        /// CutoffTime applies to both the records stored in the dataset itself,
        /// and the reports loaded through the Imports list.
        ///
        /// CutoffTime may be set in data source globally, or for a specific dataset
        /// in its details record. If CutoffTime is set for both, the earlier of the
        /// two values will be used.
        dot::Nullable<TemporalId> cutoff_time;

    private: // FIELDS

        /// Dictionary of collections indexed by Type T.
        dot::Dictionary<dot::Type, dot::Object> collection_dict_ = dot::make_dictionary<dot::Type, dot::Object>();

        /// Dictionary of dataset temporal_ids stored under String data_set_id.
        dot::Dictionary<dot::String, TemporalId> data_set_dict_ = dot::make_dictionary<dot::String, TemporalId>();

        /// Dictionary of datasets and datasets that holds them
        dot::Dictionary<TemporalId, TemporalId> data_set_owners_dict_ = dot::make_dictionary<TemporalId, TemporalId>();

        /// Dictionary of dataset temporal_ids stored under String data_set_id.
        dot::Dictionary<TemporalId, DataSetDetail> data_set_detail_dict_ = dot::make_dictionary<TemporalId, DataSetDetail>();

        /// Dictionary of the expanded list of parent temporal_ids of dataset, including
        /// parents of parents to unlimited depth with cyclic references and duplicates
        /// removed, under TemporalId of the dataset.
        dot::Dictionary<TemporalId, dot::HashSet<TemporalId>> data_set_parent_dict_ = dot::make_dictionary<TemporalId, dot::HashSet<TemporalId>>();
    };

    inline MongoDataSource make_mongo_data_source() { return new MongoDataSourceImpl(); }
}
