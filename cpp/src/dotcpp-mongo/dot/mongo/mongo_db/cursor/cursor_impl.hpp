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
    class IteratorInnerImpl; using IteratorInner = Ptr<IteratorInnerImpl>;

    /// Class implements dot::iterator_wrapper methods.
    /// Holds mongocxx::iterator.
    /// Constructs from mongo iterator and function dot::Object(const bsoncxx::document::view&),
    /// this function call bson deserializer to get Object from bson document.
    class DOT_MONGO_CLASS IteratorInnerImpl : public IteratorInnerBaseImpl
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

        virtual bool operator!=(IteratorInnerBase rhs) override
        {
            return iterator_ != rhs.as<IteratorInner>()->iterator_;
        }

        virtual bool operator==(IteratorInnerBase rhs) override
        {
            return iterator_ == rhs.as<IteratorInner>()->iterator_;
        }

        IteratorInnerImpl(mongocxx::cursor::iterator iterator, std::function<dot::Object(const bsoncxx::document::view&)> f)
            : iterator_(iterator)
            , f_(f)
        {
        }

        mongocxx::cursor::iterator iterator_;
        std::function<dot::Object(const bsoncxx::document::view&)> f_;
    };

    /// Class implements dot::ObjectCursorWrapperBase.
    /// Holds mongocxx::cursor.
    /// Constructs from mongo cursor and function dot::Object(const bsoncxx::document::view&),
    /// this function call bson deserializer to get Object from bson document.
    class DOT_MONGO_CLASS ObjectCursorWrapperImpl : public dot::ObjectCursorWrapperBaseImpl
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
        IteratorWrappper<dot::Object> begin()
        {
            return IteratorWrappper<dot::Object>(new IteratorInnerImpl(cursor_->begin(), f_));
        }

        /// A dot::iterator_wrapper<dot::Object> indicating cursor exhaustion, meaning that
        /// no documents are available from the cursor.
        IteratorWrappper<dot::Object> end()
        {
            return IteratorWrappper<dot::Object>(new IteratorInnerImpl(cursor_->end(), f_));
        }

        ObjectCursorWrapperImpl(mongocxx::cursor && cursor, const std::function<dot::Object(const bsoncxx::document::view&)>& f)
            : cursor_(std::make_shared<mongocxx::cursor>(std::move(cursor)))
            , f_(f)
        {
        }

        std::shared_ptr<mongocxx::cursor> cursor_;
        std::function<dot::Object(const bsoncxx::document::view&)> f_;
    };

    using ObjectCursorWrapper = Ptr<ObjectCursorWrapperImpl>;
}
