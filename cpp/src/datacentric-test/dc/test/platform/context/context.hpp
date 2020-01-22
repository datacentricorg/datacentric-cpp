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

#include <dc/test/declare.hpp>
#include <dc/platform/context/context_base.hpp>

namespace dc
{
    class unit_test_context_base_impl; using unit_test_context_base = dot::Ptr<unit_test_context_base_impl>;

    /// Extends context_base with approval test functionality.
    class unit_test_context_base_impl : public context_base_impl
    {
        typedef unit_test_context_base_impl self;

    public:

        /// Test database, if accessed during test, is normally
        /// deleted (dropped) on first access and once again on
        /// dispose().
        ///
        /// If keep_db property is set to true,
        /// the test database will not be dropped so that its
        /// data can be examined after the test.
        bool keep_db = true;
    };

    class unit_test_context_impl; using unit_test_context = dot::Ptr<unit_test_context_impl>;

    /// Context for use in test fixtures that do not require MongoDB.
    ///
    /// This class implements unit_test_context_base which extends context_base
    /// with approval test functionality. Attempting to access data_source
    /// using this context will cause an error.
    ///
    /// For tests that require MongoDB, use unit_test_data_context.
    class unit_test_context_impl : public unit_test_context_base_impl
    {
        friend unit_test_context make_unit_test_context(dot::Object, dot::String, dot::String);

    private:

        /// Create with class name, method name, and source file path.
        ///
        /// When ``this'' is passed as the the only argument to the
        /// constructor, the latter two arguments are provided by
        /// the compiler.
        unit_test_context_impl();

    public:
        /// Releases resources and calls base.dispose().
        ///
        /// This method will NOT be called by the garbage
        /// collector, therefore instantiating it inside
        /// the ``using'' clause is essential to ensure
        /// that dispose() method gets invoked.
        ///
        /// ATTENTION:
        ///
        /// Each class must call base.dispose() at the end
        /// of its own dispose() method.
        ~unit_test_context_impl();
    };

    /// Create with class name, method name, and source file path.
    unit_test_context make_unit_test_context(
        dot::Object class_instance,
        dot::String method_name,
        dot::String source_file_path);
}
