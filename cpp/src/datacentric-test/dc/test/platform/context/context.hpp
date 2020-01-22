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
    class unit_test_context_base_impl; using unit_test_context_base = dot::ptr<unit_test_context_base_impl>;

    /// Extends context_base with approval test functionality.
    class unit_test_context_base_impl : public context_base_impl
    {
        typedef unit_test_context_base_impl self;

    public:

        /// Test database, if accessed during test, is normally
        /// deleted (dropped) on first access and once again on
        /// Dispose().
        ///
        /// If KeepDb property is set to true,
        /// the test database will not be dropped so that its
        /// data can be examined after the test.
        bool KeepDb;
    };

    class unit_test_context_impl; using unit_test_context = dot::ptr<unit_test_context_impl>;

    /// Context for use in test fixtures that do not require MongoDB.
    ///
    /// This class implements unit_test_context_base which extends context_base
    /// with approval test functionality. Attempting to access DataSource
    /// using this context will cause an error.
    ///
    /// For tests that require MongoDB, use UnitTestDataContext.
    class unit_test_context_impl : public unit_test_context_base_impl
    {
    public:

        /// Create with class name, method name, and source file path.
        ///
        /// When ``this'' is passed as the the only argument to the
        /// constructor, the latter two arguments are provided by
        /// the compiler.
        unit_test_context_impl(dot::object classInstance,
            dot::string methodName,
            dot::string sourceFilePath);

        /// Releases resources and calls base.Dispose().
        ///
        /// This method will NOT be called by the garbage
        /// collector, therefore instantiating it inside
        /// the ``using'' clause is essential to ensure
        /// that Dispose() method gets invoked.
        ///
        /// ATTENTION:
        ///
        /// Each class must call base.Dispose() at the end
        /// of its own Dispose() method.
        ~unit_test_context_impl();

    };
}
