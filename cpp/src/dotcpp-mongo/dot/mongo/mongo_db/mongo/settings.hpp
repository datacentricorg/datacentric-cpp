/*
Copyright (C) 2015-present The DotCpp Authors.

This file is part of .C++, a native C++ implementation of
popular .NET class library APIs developed to facilitate
code reuse between C# and C++.

    http://github.com/dotcpp/dotcpp (source)
    http://dotcpp.org (documentation)

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

namespace dot
{
    /// Represents a discriminator convention for a type.
    enum class DiscriminatorConvention
    {
        empty,
        scalar,       /// Type represents by scalar String with actual type.
        hierarchical, /// Type represents by String vector with actual type and base type up to root type.
        default_convention = scalar
    };


    /// Class contains settings for mongo client, e.g. discriminator convention for a type.
    class DOT_MONGO_CLASS MongoClientSettings
    {
    private:

        /// Returns reference to static discriminator convention value.
        static DiscriminatorConvention& discriminator_convention_ref()
        {
            static dot::DiscriminatorConvention convention_ = dot::DiscriminatorConvention::empty;
            return convention_;
        }

    public:

        /// Returns static discriminator convention value.
        /// If value is not set by set_discriminator_convention method
        /// it will be set by default.
        static dot::DiscriminatorConvention get_discriminator_convention()
        {
            dot::DiscriminatorConvention& convention = discriminator_convention_ref();
            if (convention == dot::DiscriminatorConvention::empty)
            {
                convention = dot::DiscriminatorConvention::default_convention;
            }

            return convention;
        }

        /// Sets static discriminator convention value.
        /// If value has already been set, Throws exception.
        static void set_discriminator_convention(dot::DiscriminatorConvention convention)
        {
            dot::DiscriminatorConvention& convention_ref = discriminator_convention_ref();
            if (convention_ref == dot::DiscriminatorConvention::empty)
            {
                convention_ref = convention;
            }
            else
            {
                throw dot::Exception("DiscriminatorConvention is already set.");
            }
        }
    };
}
