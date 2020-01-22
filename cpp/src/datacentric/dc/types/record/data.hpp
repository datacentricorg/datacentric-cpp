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
#include <dc/types/record/temporal_id.hpp>
#include <dot/serialization/tree_writer_base.hpp>
#include <dot/serialization/serialize_attribute.hpp>

namespace dc
{
    class DataImpl; using Data = dot::Ptr<DataImpl>;
    class KeyImpl; using Key = dot::Ptr<KeyImpl>;

    /// Data objects must derive from this Type.
    class DC_CLASS DataImpl : public virtual dot::ObjectImpl
    {
        typedef DataImpl self;
        typedef dot::String t_type;

    public: // METHODS

    public:

        virtual dot::Type get_type();
        static dot::Type typeof();

    protected:

        static const char separator = ';';
    };
}
