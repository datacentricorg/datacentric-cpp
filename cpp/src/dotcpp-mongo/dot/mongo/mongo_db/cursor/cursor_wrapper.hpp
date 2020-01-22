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

#include <dot/mongo/declare.hpp>

namespace dot
{
    class iterator_inner_base_impl; using iterator_inner_base = Ptr<iterator_inner_base_impl>;

    /// Base class for iterator implementation classes.
    /// Derived iterator class is hidden to cpp.
    class DOT_MONGO_CLASS iterator_inner_base_impl : public ObjectImpl
    {
    public:

        virtual ~iterator_inner_base_impl() = default;

        virtual Object operator*() = 0;

        virtual Object operator*() const = 0;

        virtual void operator++() = 0;

        virtual bool operator!=(iterator_inner_base rhs) = 0;

        virtual bool operator==(iterator_inner_base rhs) = 0;
    };

    /// Class representing an input iterator of documents in a MongoDB cursor
    /// result set.
    ///
    /// For iterators of a tailable cursor, calling cursor.begin() may revive an
    /// exhausted iterator so that it no longer compares equal to the
    /// end-iterator.
    template <class T>
    class iterator_wrappper
    {
    public:

        typedef T value_type;

        iterator_wrappper(iterator_inner_base iterator)
            :iterator_(iterator)
        {}

        T operator*()
        {
            return iterator_->operator*();
        }

        T operator*() const
        {
            return iterator_->operator*();
        }

        iterator_wrappper<T>& operator++()
        {
            iterator_->operator++();
            return *this;
        }

        bool operator!=(iterator_wrappper<T> const& rhs)
        {
            return *iterator_ != (rhs.iterator_);
        }

        bool operator==(iterator_wrappper<T> const& rhs)
        {
            return *iterator_ == (rhs.iterator_);
        }

        iterator_inner_base iterator_;
    };


    /// Wrapper class for mongocxx::cursor. Provides non-typed iterator.
    /// Represents a pointer to the result set of a query on a MongoDB server.
    ///
    /// @note By default, cursors timeout after 10 minutes of inactivity.
    class DOT_MONGO_CLASS object_cursor_wrapper_base_impl : public dot::ObjectImpl
    {
    public:

        virtual ~object_cursor_wrapper_base_impl() = default;

        /// A dot::iterator_wrapper<dot::Object> that points to the beginning of any available
        /// results.  If begin() is called more than once, the dot::iterator_wrapper
        /// returned points to the next remaining result, not the result of
        /// the original call to begin().
        ///
        /// For a tailable cursor, when cursor.begin() == cursor.end(), no
        /// documents are available.  Each call to cursor.begin() checks again
        /// for newly-available documents.
        virtual iterator_wrappper<dot::Object> begin() = 0;

        /// A dot::iterator_wrapper<dot::Object> indicating cursor exhaustion, meaning that
        /// no documents are available from the cursor.
        virtual iterator_wrappper<dot::Object> end() = 0;

    };

    using object_cursor_wrapper_base = Ptr<object_cursor_wrapper_base_impl>;

    template <class T>
    class cursor_wrapper_impl;
    template <class T>
    using cursor_wrapper = Ptr<cursor_wrapper_impl<T>>;

    template <class T>
    inline cursor_wrapper<T> make_cursor_wrapper(object_cursor_wrapper_base object_cursor);

    /// Wrapper class for mongocxx::cursor. Provides typed iterator.
    /// Represents a pointer to the result set of a query on a MongoDB server.
    ///
    /// @note By default, cursors timeout after 10 minutes of inactivity.
    template <class T>
    class cursor_wrapper_impl : public dot::ObjectImpl
    {
        template <class T_>
        friend cursor_wrapper<T_> make_cursor_wrapper(object_cursor_wrapper_base object_cursor);

    public:

        /// A dot::iterator_wrapper<dot::Object> that points to the beginning of any available
        /// results.  If begin() is called more than once, the dot::iterator_wrapper
        /// returned points to the next remaining result, not the result of
        /// the original call to begin().
        ///
        /// For a tailable cursor, when cursor.begin() == cursor.end(), no
        /// documents are available.  Each call to cursor.begin() checks again
        /// for newly-available documents.
        inline iterator_wrappper<T> begin()
        {
            return iterator_wrappper<T>(object_cursor_->begin().iterator_);
        }

        /// A dot::iterator_wrapper<dot::Object> indicating cursor exhaustion, meaning that
        /// no documents are available from the cursor.
        inline iterator_wrappper<T> end()
        {
            return iterator_wrappper<T>(object_cursor_->end().iterator_);
        }

    private:

        /// Private ctor from object_cursor_wrapper_base.
        cursor_wrapper_impl(object_cursor_wrapper_base object_cursor)
            : object_cursor_(object_cursor)
        {
        }

        object_cursor_wrapper_base object_cursor_;
    };

    /// Returns typed cursor.
    template <class T>
    inline cursor_wrapper<T> make_cursor_wrapper(object_cursor_wrapper_base object_cursor) { return new cursor_wrapper_impl<T>(object_cursor); }
}
