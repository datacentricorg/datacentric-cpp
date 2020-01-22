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
    /// Class implements dot::query.
    /// Holds mongocxx::pipeline.
    /// Has LINQ-like API.
    class DOT_MONGO_CLASS QueryInnerImpl : public QueryImpl::QueryInnerBaseImpl
    {
    public:

        /// Adds filter.
        virtual void where(FilterTokenBase value) override
        {
            flush_sort();
            pipeline_.match(serialize_tokens(value));
        }

        /// Groups docs by key.
        virtual void group_by(dot::FieldInfo key_selectors) override
        {
            flush_sort();

            bsoncxx::builder::basic::document gr;
            gr.append(bsoncxx::builder::basic::kvp("_id", "$" + std::string(*(key_selectors->name()))));

            gr.append(bsoncxx::builder::basic::kvp("doc",
                bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$first", "$$ROOT"))));
            pipeline_.group(gr.view());

            pipeline_.replace_root(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("newRoot", "$doc")));
        }

        /// First sort.
        /// For subsequent sorting use then_by/then_by_descending after sort_by.
        virtual void sort_by(dot::FieldInfo key_selector) override
        {
            if (!sort_.empty())
            {
                throw dot::Exception("Attempt to sort_by while previous sort is not finished. Use then_by instead.");
            }

            sort_.push_back(std::make_pair(key_selector->name(), 1));
        }

        /// First sort.
        /// For subsequent sorting use then_by/then_by_descending after sort_by_descending.
        virtual void sort_by_descending(dot::FieldInfo key_selector) override
        {
            if (!sort_.empty())
            {
                throw dot::Exception("Attempt to sort_by_descending while previous sort is not finished. Use then_by_descending instead.");
            }

            sort_.push_back(std::make_pair(key_selector->name(), -1));
        }

        /// Use in subsequent sorting after sort_by/sort_by_descending.
        virtual void then_by(dot::FieldInfo key_selector) override
        {
            sort_.push_back(std::make_pair(key_selector->name(), 1));
        }

        /// Use in subsequent sorting after sort_by/sort_by_descending.
        virtual void then_by_descending(dot::FieldInfo key_selector) override
        {
            sort_.push_back(std::make_pair(key_selector->name(), -1));
        }

        /// Returns cursor constructed from pipeline and document deserializator.
        virtual ObjectCursorWrapperBase get_cursor() override
        {
            flush_sort();

            return new ObjectCursorWrapperImpl(dynamic_cast<CollectionInner*>(collection_->impl_.get())->collection_.aggregate(pipeline_),
                [](const bsoncxx::document::view& item)->dot::Object
                {
                    BsonRecordSerializer serializer = make_bson_record_serializer();
                    Object record = serializer->deserialize(item);

                    return record;
                }
            );
        }

        /// Returns cursor constructed from pipeline and tuple deserializator.
        virtual ObjectCursorWrapperBase select(dot::List<dot::FieldInfo> props, dot::Type element_type) override
        {
            flush_sort();

            bsoncxx::builder::basic::document selectList{};
            for (dot::FieldInfo prop : props)
                selectList.append(bsoncxx::builder::basic::kvp((std::string&)*(dot::String) prop->name(), 1));
            selectList.append(bsoncxx::builder::basic::kvp("_key", 1));

            pipeline_.project(selectList.view());

            return new ObjectCursorWrapperImpl(dynamic_cast<CollectionInner*>(collection_->impl_.get())->collection_.aggregate(pipeline_),
                [props, element_type](const bsoncxx::document::view& item)->dot::Object
                {
                    BsonRecordSerializer serializer = make_bson_record_serializer();
                    dot::Object record = serializer->deserialize_tuple(item, props, element_type);
                    return record;
                }
            );

        }

        /// Sets up maximum count of documents in query.
        virtual void limit(int32_t limit_size) override
        {
            flush_sort();
            pipeline_.limit(limit_size);
        }

    private:

        /// Applies sort conditions to pipeline.
        /// This method is called from other before
        /// any operation except sort. 
        void flush_sort()
        {
            if (!sort_.empty())
            {
                bsoncxx::builder::basic::document sort_doc{};

                for (std::pair<String, int> const& item : sort_)
                {
                    sort_doc.append(bsoncxx::builder::basic::kvp(std::string(*(item.first)), item.second));
                }

                pipeline_.sort(sort_doc.view());
                sort_.clear();
            }
        }

    public:

        dot::Collection collection_;
        std::deque<std::pair<String, int>> sort_;
        dot::Type type_;
        dot::Type element_type_;
        dot::List<dot::FieldInfo> select_;

        mongocxx::pipeline pipeline_;
    };

    using QueryInner = Ptr<QueryInnerImpl>;

    Query QueryImpl::where(FilterTokenBase value)
    {
        impl_->where(value);
        return this;
    }

    ObjectCursorWrapperBase QueryImpl::get_cursor()
    {
        return impl_->get_cursor();
    }

    Query QueryImpl::group_by(dot::FieldInfo key_selector)
    {
        impl_->group_by(key_selector);
        return this;
    }

    Query QueryImpl::sort_by(dot::FieldInfo key_selector)
    {
        impl_->sort_by(key_selector);
        return this;
    }

    Query QueryImpl::sort_by_descending(dot::FieldInfo key_selector)
    {
        impl_->sort_by_descending(key_selector);
        return this;
    }

    Query QueryImpl::then_by(dot::FieldInfo key_selector)
    {
        impl_->then_by(key_selector);
        return this;
    }

    Query QueryImpl::then_by_descending(dot::FieldInfo key_selector)
    {
        impl_->then_by_descending(key_selector);
        return this;
    }

    ObjectCursorWrapperBase QueryImpl::select(dot::List<dot::FieldInfo> props, dot::Type element_type)
    {
        return impl_->select(props, element_type);
    }

    Query QueryImpl::limit(int32_t limit_size)
    {
        impl_->limit(limit_size);
        return this;
    }

    QueryImpl::QueryImpl(dot::Collection collection, dot::Type type)
    {
        QueryInner impl = new QueryInnerImpl;

        impl->collection_ = collection;
        impl->type_ = type;
        impl->element_type_ = type;

        impl_ = impl;
        type_ = type;
    }
}