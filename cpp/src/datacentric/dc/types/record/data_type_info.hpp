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
#include <dot/system/type.hpp>
#include <dot/system/string.hpp>
#include <dot/system/collections/generic/dictionary.hpp>
#include <dc/types/record/data_kind_enum.hpp>

namespace dc
{
    class data_type_info_impl; using data_type_info = dot::Ptr<data_type_info_impl>;

    /// Information about a data type obtained through reflection.
    ///
    /// This class can be used to obtain type information for classes
    /// derived from Data class, including through Key or Record classes.
    /// Using it with any other type will result in an error.
    class DC_CLASS data_type_info_impl : public virtual dot::ObjectImpl
    {
    public: // PROPERTIES

        /// Kind of the data type (record, key, or element).
        data_kind_enum data_kind() { return data_kind_; }

        /// Inheritance chain from derived to base, ending
        /// with and inclusive of the RootType. This property
        /// is used to generate the value of _t BSON attribute.
        ///
        /// Root data type is the type that inherits directly
        /// from Key(TKey, TRecord).
        dot::list<dot::String> inheritance_chain() { return inheritance_chain_; }

    public: // METHODS

        /// Name of the database table or collection where the record is stored.
        ///
        /// By convention, this method returns the name of the class one level
        /// below TypedRecord in the inheritance chain, without namespace.
        ///
        /// Error message if called for a type that is not derived from TypedRecord.
        dot::String get_collection_name();

    public: // STATIC

        /// Get cached instance for the specified Object, or create
        /// using  and add to thread static cache if does not exist.
        ///
        /// This Object contains information about the data type
        /// including the list of its elements (public properties
        /// that have one of the supported data types).
        static data_type_info get_or_create(dot::Object value);

        /// Get cached instance for the specified type, or create
        /// using  and add to thread static cache if does not exist.
        ///
        /// This Object contains information about the data type
        /// including the list of its elements (public properties
        /// that have one of the supported data types).
        ///
        /// This overload accepts the value of Type as parameter.
        static data_type_info get_or_create(dot::Type value);

    private: // CONSTRUCTORS

        /// Create using settings from settings.default.class_map.
        ///
        /// This constructor is private because it is only called
        /// from the get_or_create(...) method. Users should rely
        /// on get_or_create(...) method only which uses thread static
        /// cached value if any, and creates the instance only if
        /// it is not yet cached for the thread.
        data_type_info_impl(dot::Type value);

    private: // PRIVATE

        static dot::dictionary<dot::Type, data_type_info>& get_type_dict();

    private: // FIELDS

        data_kind_enum data_kind_ = data_kind_enum::empty;
        dot::list<dot::String> inheritance_chain_;

        dot::Type type_;
        dot::Type root_type_;
        dot::Type root_key_type_;
        dot::Type root_data_type_;
    };
}
