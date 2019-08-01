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
#include <dot/system/Type.hpp>
#include <dot/system/String.hpp>
#include <dot/system/collections/generic/Dictionary.hpp>

namespace dc
{
    class ClassInfoImpl; using ClassInfo = dot::ptr<ClassInfoImpl>;

    /// <summary>Provides the result of applying class map settings to a class.</summary>
    class DC_CLASS ClassInfoImpl : public virtual dot::object_impl
    {
        typedef ClassInfoImpl self;
        typedef dot::type_t Type_;

    public: // PROPERTIES

        /// <summary>Type for which class info is provided.</summary>
        Type_ Type;

        /// <summary>Namespace before mapping.</summary>
        dot::string RawNamespace;

        /// <summary>Namespace after mapping.</summary>
        dot::string MappedNamespace;

        /// <summary>Class name without namespace before mapping.</summary>
        dot::string RawClassName;

        /// <summary>Class name without namespace after mapping.</summary>
        dot::string MappedClassName;

        /// <summary>Fully qualified class name before mapping.</summary>
        dot::string RawFullName;

        /// <summary>Fully qualified class name after mapping.</summary>
        dot::string MappedFullName;

    public: // METHODS

        /// <summary>Returns fully qualified class name.</summary>
        virtual dot::string ToString() override;

    public: // STATIC

        /// <summary>
        /// Get cached instance for the specified object, or create
        /// using settings from Settings.Default.ClassMap
        /// and add to thread static cache if does not exist.
        ///
        /// This object contains information about the
        /// class including its name, namespace, etc.
        /// </summary>
        static ClassInfo GetOrCreate(dot::object value);

        /// <summary>
        /// Get cached instance for the specified type, or create
        /// using settings from Settings.Default.ClassMap
        /// and add to thread static cache if does not exist.
        ///
        /// This object contains information about the
        /// class including its name, namespace, etc.
        /// </summary>
        static ClassInfo GetOrCreate(Type_ type);

    private: // CONSTRUCTORS

        /// <summary>
        /// Create using settings from Settings.Default.ClassMap.
        ///
        /// This constructor is private because it is only called
        /// from the GetOrCreate(...) method. Users should rely
        /// on GetOrCreate(...) method only which uses thread static
        /// cached value if any, and creates the instance only if
        /// it is not yet cached for the thread.
        /// </summary>
        ClassInfoImpl(Type_ type);

    private: // PRIVATE

        static dot::Dictionary<Type_, ClassInfo>& GetTypeDict();
    };
}
