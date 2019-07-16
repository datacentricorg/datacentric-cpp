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

#include <dc/implement.hpp>
#include <dc/platform/query/Query.hpp>
#include <dc/platform/context/IContext.hpp>

namespace dc
{
    IQuery QueryImpl::Where(bsoncxx::document::view_or_value value)
    {
        where_.append(bsoncxx::builder::basic::concatenate(value));
        return this;
    }

    dot::IObjectEnumerable QueryImpl::AsEnumerable()
    {
        return dataSource_->LoadByQuery(this);
    }

    IQuery QueryImpl::SortBy(dot::PropertyInfo keySelector)
    {
        sort_.append(bsoncxx::builder::basic::kvp((std::string) * (dot::String)keySelector->Name, 1));
        return this;
    }

    IQuery QueryImpl::SortByDescending(dot::PropertyInfo keySelector)
    {
        sort_.append(bsoncxx::builder::basic::kvp((std::string) * (dot::String)keySelector->Name, -1));
        return this;
    }

    dot::IObjectEnumerable QueryImpl::Select(dot::List<dot::PropertyInfo> props, dot::Type elementType)
    {
        this->elementType_ = elementType;
        this->select_ = props;
        return dataSource_->LoadByQuery(this);
    }

}