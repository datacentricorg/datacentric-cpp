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
#include <dot/system/collections/generic/list.hpp>
#include <dot/system/collections/generic/dictionary.hpp>

namespace dc
{
    class index_elements_attribute_impl; using index_elements_attribute = dot::Ptr<index_elements_attribute_impl>;

    /// Use IndexElements attribute to specify database indexes
    /// for the class. A class may have more than one IndexElements
    /// attribute, each for a separate index.
    ///
    /// This method must be set for the root data type of a collection.
    /// Root data type is the type derived directly from TypedRecord.
    ///
    /// The definition String for the index is a comma separated
    /// list of element names. The elements sorted in descending
    /// order are prefixed by -.
    ///
    /// Examples:
    ///
    /// * A is an index on element A in ascending order;
    /// * -A is an index on element A in descending order;
    /// * A,B,-C is an index on elements A and B in ascending
    ///   order and then element C in descending order.
    ///
    /// When collection interface is obtained from a data source,
    /// names of the elements in the index definition are validated
    /// to match element names of the class for which the index is
    /// defined. If the class does not have an an element with the
    /// name specified as part of the definition String, an error
    /// message is given.
    class DC_CLASS index_elements_attribute_impl : public dot::AttributeImpl
    {
        friend index_elements_attribute make_index_elements_attribute(dot::String);
        friend index_elements_attribute make_index_elements_attribute(dot::String, dot::String);

    private:

        dot::String definition_;
        dot::String name_;

    private: // CONSTRUCTORS

        /// Create from the index definition String.
        ///
        /// The definition String for the index is a comma separated
        /// list of element names. The elements sorted in descending
        /// order are prefixed by -.
        ///
        /// Examples:
        ///
        /// * A is an index on element A in ascending order;
        /// * -A is an index on element A in descending order;
        /// * A,B,-C is an index on elements A and B in ascending
        ///   order and then element C in descending order.
        index_elements_attribute_impl(dot::String definition);

        /// Create from the index definition String, and optional
        /// custom name for the index.
        ///
        /// The definition String for the index is a comma separated
        /// list of element names. The elements sorted in descending
        /// order are prefixed by -.
        ///
        /// Examples:
        ///
        /// * A is an index on element A in ascending order;
        /// * -A is an index on element A in descending order;
        /// * A,B,-C is an index on elements A and B in ascending
        ///   order and then element C in descending order.
        ///
        /// By default, the delimited elements String (index
        /// definition) is used as index name. When the default
        /// name exceeds the maximum index name length, use
        /// this optional property to specify a shorter custom
        /// index name.
        index_elements_attribute_impl(dot::String definition, dot::String name);

    public: // METHODS

        /// The definition String for the index is a comma separated
        /// list of element names. The elements sorted in descending
        /// order are prefixed by -.
        ///
        /// Examples:
        ///
        /// * A is an index on element A in ascending order;
        /// * -A is an index on element A in descending order;
        /// * A,B,-C is an index on elements A and B in ascending
        ///   order and then element C in descending order.
        dot::String get_definition();

        /// Custom short name of the index (optional).
        ///
        /// By default, the delimited elements String (index
        /// definition) is used as index name. When the default
        /// name exceeds the maximum index name length, use
        /// this optional property to specify a shorter custom
        /// index name.
        dot::String get_neme();

    public: // STATIC

        /// Get IndexedElements attributes for the class and its
        /// parents as SortedDictionary(IndexDefinition, IndexName).
        static dot::dictionary<dot::String, dot::String> get_attributes_dict(dot::Type record_type);

        /// Parse IndexElements definition String to get an ordered
        /// list of (ElementName,SortOrder) tuples, where ElementName
        /// is name of the property for which the index is defined,
        /// and SortOrder is 1 for ascending, and -1 for descending.
        ///
        /// The parser will also validate that each element
        /// name exists in type TRecord.
        static dot::list<std::tuple<dot::String, int>> parse_definition(dot::String definition, dot::Type record_type);

    public: // REFLECTION

        static dot::Type typeof();
        dot::Type get_type() override;
    };

    inline index_elements_attribute make_index_elements_attribute(dot::String definition)
    {
        return new index_elements_attribute_impl(definition);
    }

    inline index_elements_attribute make_index_elements_attribute(dot::String definition, dot::String name)
    {
        return new index_elements_attribute_impl(definition, name);
    }
}
