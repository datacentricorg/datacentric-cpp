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
    class data_impl; using data = dot::Ptr<data_impl>;
    class key_impl; using key = dot::Ptr<key_impl>;

    /// Data objects must derive from this type.
    class DC_CLASS data_impl : public virtual dot::ObjectImpl
    {
        typedef data_impl self;
        typedef dot::String t_type;

    public: // METHODS

    public:

        virtual dot::type get_type();
        static dot::type typeof();

    protected:

        static const char separator = ';';
    };
}
