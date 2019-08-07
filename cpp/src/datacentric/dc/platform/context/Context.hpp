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
#include <dc/platform/context/IContext.hpp>

namespace dc
{
    class ContextImpl; using Context = dot::ptr<ContextImpl>;

    /// Context defines dataset and provides access to data,
    /// logging, and other supporting functionality.
    class DC_CLASS ContextImpl : public IContextImpl
    {
        typedef ContextImpl self;

    public: // CONSTRUCTORS

        ContextImpl() = default;

    public: // METHODS

        /// Initialize connection.
        void Init(dot::string connectionString, dot::string environment)
        {
            //MongoFactory fact = new MongoFactoryImpl;
            //MongoDataSource ds = (MongoDataSource)fact->Create(this, connectionString, environment);
            //SetDataSource(ds);
        }
    };

    inline Context make_Context() { return new ContextImpl; }
}

