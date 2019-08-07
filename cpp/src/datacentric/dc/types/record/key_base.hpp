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
#include <dc/types/record/data.hpp>

namespace dc
{
    class key_base_impl; using key_base = dot::ptr<key_base_impl>;
    class data_impl; using data = dot::ptr<data_impl>;

    /// Key objects must derive from this type.
    class DC_CLASS key_base_impl : public data_impl
    {
        typedef key_base_impl self;

    public:

        dot::string getValue()
        {
            return to_string();
        }

    public: // METHODS

        dot::string to_string();

        void AssignString(dot::string value);

        dot::object_id _id;

    private:

        void AssignString(std::stringstream & value);

        DOT_TYPE_BEGIN("dc", "KeyBase")
            //DOT_TYPE_BASE(Data)
        DOT_TYPE_END()

    };
}
