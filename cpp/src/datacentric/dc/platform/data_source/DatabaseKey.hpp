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
#include <dc/types/record/record.hpp>
#include <dc/platform/data_source/InstanceType.hpp>
#include <dc/platform/data_source/DatabaseData.hpp>

namespace dc
{
    class db_name_key_impl; using db_name_key = dot::ptr<db_name_key_impl>;
    class db_name_data_impl; using db_name_data = dot::ptr<db_name_data_impl>;

    /// This class enforces strict naming conventions
    /// for database naming. While format of the resulting database
    /// name is specific to data store type, it always consists
    /// of three tokens: instance_type, instance_name, and env_name.
    /// The meaning of instance_name and env_name tokens depends on
    /// the value of instance_type enumeration.
    ///
    /// This record is stored in root dataset.
    class DC_CLASS db_name_key_impl : public root_key_impl<db_name_key_impl, db_name_data_impl>
    {
        typedef db_name_key_impl self;
        typedef instance_type instance_type_;

    public: // PROPERTIES

        /// Instance type enumeration.
        ///
        /// Some API functions are restricted based on the instance type.
        instance_type_ instance_type;

        /// The meaning of instance name depends on the instance type.
        ///
        /// \begin{itemize}
        /// \item
        /// For PROD, UAT, and DEV instance types, instance name
        /// identifies the endpoint.
        ///
        /// \item
        /// For USER instance type, instance name is user alias.
        ///
        /// \item
        /// For TEST instance type, instance name is the name of
        /// the unit test class (test fixture).
        /// \end{itemize}
        dot::string instance_name;

        /// The meaning of environment name depends on the instance type.
        ///
        /// \begin{itemize}
        /// \item
        /// For PROD, UAT, DEV, and USER instance types, it is the
        /// name of the user environment selected in the client.
        ///
        /// \item
        /// For TEST instance type, it is the test method name.
        /// \end{itemize}
        dot::string env_name;

        DOT_TYPE_BEGIN("dc", "DbNameKey")
            DOT_TYPE_PROP(instance_type)
            DOT_TYPE_PROP(instance_name)
            DOT_TYPE_PROP(env_name)
        DOT_TYPE_END()


    };
}
