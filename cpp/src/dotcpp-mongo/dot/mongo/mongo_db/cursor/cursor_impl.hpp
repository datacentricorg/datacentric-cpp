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
    class iterator_inner_impl; using iterator_inner = Ptr<iterator_inner_impl>;

    /// Class implements dot::iterator_wrapper methods.
    /// Holds mongocxx::iterator.
    /// Constructs from mongo iterator and function dot::Object(const bsoncxx::document::view&),
    /// this function call bson deserializer to get Object from bson document.
    class DOT_MONGO_CLASS iterator_inner_impl : public iterator_inner_base_impl
    {
    public:

        virtual Object operator*() override
        {
            return f_(*iterator_);
        }

        virtual Object operator*() const override
        {
            return f_(*iterator_);
        }

        virtual void operator++() override
        {
            iterator_++;
        }

        virtual bool operator!=(iterator_inner_base rhs) override
        {
            return iterator_ != rhs.as<iterator_inner>()->iterator_;
        }

        virtual bool operator==(iterator_inner_base rhs) override
        {
            return iterator_ == rhs.as<iterator_inner>()->iterator_;
        }

        iterator_inner_impl(mongocxx::cursor::iterator iterator, std::function<dot::Object(const bsoncxx::document::view&)> f)
            : iterator_(iterator)
            , f_(f)
        {
        }

        mongocxx::cursor::iterator iterator_;
        std::function<dot::Object(const bsoncxx::document::view&)> f_;
    };

    /// Class implements dot::object_cursor_wrapper_base.
    /// Holds mongocxx::cursor.
    /// Constructs from mongo cursor and function dot::Object(const bsoncxx::document::view&),
    /// this function call bson deserializer to get Object from bson document.
    class DOT_MONGO_CLASS object_cursor_wrapper_impl : public dot::object_cursor_wrapper_base_impl
    {
    public:

        /// A dot::iterator_wrapper<dot::Object> that points to the beginning of any available
        /// results.  If begin() is called more than once, the dot::iterator_wrapper
        /// returned points to the next remaining result, not the result of
        /// the original call to begin().
        ///
        /// For a tailable cursor, when cursor.begin() == cursor.end(), no
        /// documents are available.  Each call to cursor.begin() checks again
        /// for newly-available documents.
        iterator_wrappper<dot::Object> begin()
        {
            return iterator_wrappper<dot::Object>(new iterator_inner_impl(cursor_->begin(), f_));
        }

        /// A dot::iterator_wrapper<dot::Object> indicating cursor exhaustion, meaning that
        /// no documents are available from the cursor.
        iterator_wrappper<dot::Object> end()
        {
            return iterator_wrappper<dot::Object>(new iterator_inner_impl(cursor_->end(), f_));
        }

        object_cursor_wrapper_impl(mongocxx::cursor && cursor, const std::function<dot::Object(const bsoncxx::document::view&)>& f)
            : cursor_(std::make_shared<mongocxx::cursor>(std::move(cursor)))
            , f_(f)
        {
        }

        std::shared_ptr<mongocxx::cursor> cursor_;
        std::function<dot::Object(const bsoncxx::document::view&)> f_;
    };

    using object_cursor_wrapper = Ptr<object_cursor_wrapper_impl>;
}