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
#include <dot/system/Ptr.hpp>
#include <dc/types/record/RecordFor.hpp>
#include <dc/platform/data_source/InstanceType.hpp>
#include <dc/platform/data_source/DatabaseData.hpp>

namespace dc
{
    class DbNameKeyImpl; using DbNameKey = dot::Ptr<DbNameKeyImpl>;
    class DbNameDataImpl; using DbNameData = dot::Ptr<DbNameDataImpl>;

    /// <summary>
    /// This class enforces strict naming conventions
    /// for database naming. While format of the resulting database
    /// name is specific to data store type, it always consists
    /// of three tokens: InstanceType, InstanceName, and EnvName.
    /// The meaning of InstanceName and EnvName tokens depends on
    /// the value of InstanceType enumeration.
    ///
    /// This record is stored in root dataset.
    /// </summary>
    class DC_CLASS DbNameKeyImpl : public RootKeyForImpl<DbNameKeyImpl, DbNameDataImpl>
    {
        typedef DbNameKeyImpl self;
        typedef InstanceType InstanceType_;

    public: // PROPERTIES

        /// <summary>
        /// Instance type enumeration.
        ///
        /// Some API functions are restricted based on the instance type.
        /// </summary>
        InstanceType_ InstanceType;

        /// <summary>
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
        /// </summary>
        dot::String InstanceName;

        /// <summary>
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
        /// </summary>
        dot::String EnvName;

        DOT_TYPE_BEGIN(".Runtime.Main", "DbNameKey")
            DOT_TYPE_PROP(InstanceType)
            DOT_TYPE_PROP(InstanceName)
            DOT_TYPE_PROP(EnvName)
        DOT_TYPE_END()


    };
}
