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

namespace dc
{
    class class_info_impl; using class_info = dot::ptr<class_info_impl>;

    /// Provides the result of applying class map settings to a class.
    class DC_CLASS class_info_impl : public virtual dot::object_impl
    {
        typedef class_info_impl self;
        //typedef dot::type type_;

    public: // PROPERTIES

        /// Type for which class info is provided.
        dot::type type;

        /// Namespace before mapping.
        dot::string raw_namespace;

        /// Namespace after mapping.
        dot::string mapped_namespace;

        /// Class name without namespace before mapping.
        dot::string raw_class_name;

        /// Class name without namespace after mapping.
        dot::string mapped_class_name;

        /// Fully qualified class name before mapping.
        dot::string raw_full_name;

        /// Fully qualified class name after mapping.
        dot::string mapped_full_name;

    public: // METHODS

        /// Returns fully qualified class name.
        virtual dot::string to_string() override;

    public: // STATIC

        /// Get cached instance for the specified object, or create
        /// using settings from Settings.Default.ClassMap
        /// and add to thread static cache if does not exist.
        ///
        /// This object contains information about the
        /// class including its name, namespace, etc.
        static class_info get_or_create(dot::object value);

        /// Get cached instance for the specified type, or create
        /// using settings from Settings.Default.ClassMap
        /// and add to thread static cache if does not exist.
        ///
        /// This object contains information about the
        /// class including its name, namespace, etc.
        static class_info get_or_create(dot::type type);

    private: // CONSTRUCTORS

        /// Create using settings from Settings.Default.ClassMap.
        ///
        /// This constructor is private because it is only called
        /// from the GetOrCreate(...) method. Users should rely
        /// on GetOrCreate(...) method only which uses thread static
        /// cached value if any, and creates the instance only if
        /// it is not yet cached for the thread.
        class_info_impl(dot::type type);

    private: // PRIVATE

        static dot::dictionary<dot::type, class_info>& get_type_dict();
    };
}
