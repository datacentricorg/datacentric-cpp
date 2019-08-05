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
#include <dc/types/record/RecordFor.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <dc/platform/cursor/CursorWrapper.hpp>

namespace dc
{
    class IQueryImpl;
    using IQuery = dot::ptr<IQueryImpl>;
    template <class Class, class Prop>
    struct prop_wrapper;

    /// <summary>
    /// Holds expressions for Where, Select, and OrderBy/ThenBy
    /// parts of a query.
    /// </summary>
    class DC_CLASS IQueryImpl :public virtual dot::object_impl
    {
    public:

        virtual IQuery Where(bsoncxx::document::view_or_value value) = 0;

        virtual IQuery SortBy(dot::field_info keySelector) = 0;

        template <class Class, class Prop>
        IQuery SortBy(prop_wrapper<Class, Prop> keySelector)
        {
            return SortBy(keySelector.prop_);
        }

        virtual IQuery SortByDescending(dot::field_info keySelector) = 0;

        template <class Class, class Prop>
        IQuery SortByDescending(prop_wrapper<Class, Prop> keySelector)
        {
            return SortByDescending(keySelector.prop_);
        }

        virtual dot::IObjectEnumerable AsEnumerable() = 0;

        template <class TRecord>
        dot::IEnumerable<TRecord> AsEnumerable()
        {
            return AsEnumerable().template as<ObjectCursorWrapper>()->template AsEnumerable<TRecord>();
        }

        virtual dot::IObjectEnumerable Select(dot::List<dot::field_info> props, dot::type_t elementType) = 0;

        template <class Element>
        dot::IEnumerable<Element> Select(dot::List<dot::field_info> props)
        {
            return Select(props, dot::typeof<Element>()).template as<ObjectCursorWrapper>()->template AsEnumerable<Element>();
        }

    };
}

