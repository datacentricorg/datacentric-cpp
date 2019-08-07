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

#include <dc/precompiled.hpp>
#include <dc/implement.hpp>
#include <dc/types/record/record_base.hpp>
#include <dc/platform/context/context_base.hpp>

namespace dc
{

    void record_base_impl::Init(context_base context)
    {
        // The line below is an example of calling Init(...) method for base class.
        // It should be uncommented for all classes derived from this class.
        //
        // base.Init(context);

        Context = context;
        if (context == nullptr) throw dot::exception(
            dot::string::format("Null context is passed to the Init(...) method for {0}.", type()->name));
    }

}
