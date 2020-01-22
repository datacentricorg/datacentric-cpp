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
    class index_elements_attribute_impl; using index_elements_attribute = dot::ptr<index_elements_attribute_impl>;

    /// Use IndexElements attribute to specify database indexes
    /// for the class. A class may have more than one IndexElements
    /// attribute, each for a separate index.
    ///
    /// This method must be set for the root data type of a collection.
    /// Root data type is the type derived directly from TypedRecord.
    ///
    /// The definition string for the index is a comma separated
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
    /// name specified as part of the definition string, an error
    /// message is given.
    class DC_CLASS index_elements_attribute_impl : public dot::attribute_impl
    {
        friend index_elements_attribute make_index_elements_attribute(dot::string);
        friend index_elements_attribute make_index_elements_attribute(dot::string, dot::string);

    private:

        dot::string definition_;
        dot::string name_;

    private: // CONSTRUCTORS

        /// Create from the index definition string.
        ///
        /// The definition string for the index is a comma separated
        /// list of element names. The elements sorted in descending
        /// order are prefixed by -.
        ///
        /// Examples:
        ///
        /// * A is an index on element A in ascending order;
        /// * -A is an index on element A in descending order;
        /// * A,B,-C is an index on elements A and B in ascending
        ///   order and then element C in descending order.
        index_elements_attribute_impl(dot::string definition);

        /// Create from the index definition string, and optional
        /// custom name for the index.
        ///
        /// The definition string for the index is a comma separated
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
        /// By default, the delimited elements string (index
        /// definition) is used as index name. When the default
        /// name exceeds the maximum index name length, use
        /// this optional property to specify a shorter custom
        /// index name.
        index_elements_attribute_impl(dot::string definition, dot::string name);

    public: // METHODS

        /// The definition string for the index is a comma separated
        /// list of element names. The elements sorted in descending
        /// order are prefixed by -.
        ///
        /// Examples:
        ///
        /// * A is an index on element A in ascending order;
        /// * -A is an index on element A in descending order;
        /// * A,B,-C is an index on elements A and B in ascending
        ///   order and then element C in descending order.
        dot::string get_definition();

        /// Custom short name of the index (optional).
        ///
        /// By default, the delimited elements string (index
        /// definition) is used as index name. When the default
        /// name exceeds the maximum index name length, use
        /// this optional property to specify a shorter custom
        /// index name.
        dot::string get_neme();

    public: // STATIC

        /// Get IndexedElements attributes for the class and its
        /// parents as SortedDictionary(IndexDefinition, IndexName).
        static dot::dictionary<dot::string, dot::string> get_attributes_dict(dot::type record_type);

        /// Parse IndexElements definition string to get an ordered
        /// list of (ElementName,SortOrder) tuples, where ElementName
        /// is name of the property for which the index is defined,
        /// and SortOrder is 1 for ascending, and -1 for descending.
        ///
        /// The parser will also validate that each element
        /// name exists in type TRecord.
        static dot::list<std::tuple<dot::string, int>> parse_definition(dot::string definition, dot::type record_type);

    public: // REFLECTION

        static dot::type typeof();
        dot::type get_type() override;
    };

    inline index_elements_attribute make_index_elements_attribute(dot::string definition)
    {
        return new index_elements_attribute_impl(definition);
    }

    inline index_elements_attribute make_index_elements_attribute(dot::string definition, dot::string name)
    {
        return new index_elements_attribute_impl(definition, name);
    }
}
