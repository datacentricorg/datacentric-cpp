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
    enum class discriminator_convention
    {
        empty,
        scalar,       /// Type represents by scalar string with actual type.
        hierarchical, /// Type represents by string vector with actual type and base type up to root type.
        default_convention = scalar
    };


    /// Class contains settings for mongo client, e.g. discriminator convention for a type.
    class DOT_MONGO_CLASS mongo_client_settings
    {
    private:

        /// Returns reference to static discriminator convention value.
        static discriminator_convention& discriminator_convention_ref()
        {
            static dot::discriminator_convention convention_ = dot::discriminator_convention::empty;
            return convention_;
        }

    public:

        /// Returns static discriminator convention value.
        /// If value is not set by set_discriminator_convention method
        /// it will be set by default.
        static dot::discriminator_convention get_discriminator_convention()
        {
            dot::discriminator_convention& convention = discriminator_convention_ref();
            if (convention == dot::discriminator_convention::empty)
            {
                convention = dot::discriminator_convention::default_convention;
            }

            return convention;

        }

        /// Sets static discriminator convention value.
        /// If value has already been set, Throws exception.
        static void set_discriminator_convention(dot::discriminator_convention convention)
        {
            dot::discriminator_convention& convention_ref = discriminator_convention_ref();
            if (convention_ref == dot::discriminator_convention::empty)
            {
                convention_ref = convention;
            }
            else
            {
                throw dot::exception("discriminator_convention is already set.");
            }
        }
    };
}
