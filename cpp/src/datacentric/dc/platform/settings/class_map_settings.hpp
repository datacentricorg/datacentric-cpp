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
#include <dot/system/collections/generic/list.hpp>

namespace dc
{
    /// Settings for mapping namespaces, class names, and method names.
    class DC_CLASS class_map_settings
    {
    public:

        /// Namespace prefixes including dot separators ignored by class mapping (empty by default).
        ///
        /// Thread safety requires that this property is modified as a whole by
        /// assigning a new instance of List; values should not be appended to
        /// an existing list.
        static dot::list<dot::string> ignored_namespace_prefixes()
        {
            return dot::make_list<dot::string>();
        }

        /// Namespace suffixes including dot separators ignored by class mapping (empty by default).
        ///
        /// Thread safety requires that this property is modified as a whole by
        /// assigning a new instance of List; values should not be appended to
        /// an existing list.
        static dot::list<dot::string> ignored_namespace_suffixes()
        {
            return dot::make_list<dot::string>();
        }

        /// Class name prefixes ignored by class mapping (empty by default).
        ///
        /// Thread safety requires that this property is modified as a whole by
        /// assigning a new instance of List; values should not be appended to
        /// an existing list.
        static dot::list<dot::string> ignored_class_name_prefixes()
        {
            return dot::make_list<dot::string>();
        }

        /// Class name prefixes ignored by class mapping.
        ///
        /// Thread safety requires that this property is modified as a whole by
        /// assigning a new instance of List; values should not be appended to
        /// an existing list.
        static dot::list<dot::string> ignored_class_name_suffixes()
        {
            static auto result = dot::make_list<dot::string>({ "Data", "Key" });
            return result;
        }

        /// Method name prefixes ignored by class mapping (empty by default).
        ///
        /// Thread safety requires that this property is modified as a whole by
        /// assigning a new instance of List; values should not be appended to
        /// an existing list.
        static dot::list<dot::string> ignored_method_name_prefixes()
        {
            return dot::make_list<dot::string>();
        }

        /// Method name prefixes ignored by class mapping (empty by default).
        ///
        /// Thread safety requires that this property is modified as a whole by
        /// assigning a new instance of List; values should not be appended to
        /// an existing list.
        static dot::list<dot::string> ignored_method_name_suffixes()
        {
            return dot::make_list<dot::string>();
        }
    };
}
