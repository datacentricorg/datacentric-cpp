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

    class DOT_MONGO_CLASS iterator_inner_base
    {
    public:

        virtual object operator*() = 0;

        virtual object operator*() const = 0;

        virtual void operator++() = 0;

        virtual bool operator!=(std::shared_ptr<iterator_inner_base> const& rhs) = 0;

        virtual bool operator==(std::shared_ptr<iterator_inner_base> const& rhs) = 0;
    };

    template <class T>
    class iterator_wrappper
    {
    public:

        typedef T value_type;

        iterator_wrappper(std::shared_ptr<iterator_inner_base> iterator)
            :iterator_(iterator)
        {}

        T operator*()
        {
            return T(iterator_->operator*());
        }

        T operator*() const
        {
            return T(iterator_->operator*());
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

        std::shared_ptr<iterator_inner_base> iterator_;
    };


    class DOT_MONGO_CLASS object_cursor_wrapper_base_impl : public dot::object_impl
    {
    public:

        /// Returns begin iterator of the underlying std::vector.
        virtual iterator_wrappper<dot::object> begin() = 0;

        /// Returns end iterator of the underlying std::vector.
        virtual iterator_wrappper<dot::object> end() = 0;

    };

    using object_cursor_wrapper_base = ptr<object_cursor_wrapper_base_impl>;

    template <class T>
    class cursor_wrapper_impl;
    template <class T>
    using cursor_wrapper = ptr<cursor_wrapper_impl<T>>;

    template <class T>
    inline cursor_wrapper<T> make_cursor_wrapper(object_cursor_wrapper_base object_cursor);

    template <class T>
    class cursor_wrapper_impl : public dot::object_impl
    {
        template <class T_>
        friend cursor_wrapper<T_> make_cursor_wrapper(object_cursor_wrapper_base object_cursor);

    public:

        /// Returns begin iterator of the underlying std::vector.
        inline iterator_wrappper<T> begin()
        {
            return iterator_wrappper<T>(object_cursor_->begin().iterator_);
        }

        /// Returns end iterator of the underlying std::vector.
        inline iterator_wrappper<T> end()
        {
            return iterator_wrappper<T>(object_cursor_->end().iterator_);
        }

    private:

        cursor_wrapper_impl(object_cursor_wrapper_base object_cursor)
            : object_cursor_(object_cursor)
        {
        }

        object_cursor_wrapper_base object_cursor_;
    };

    template <class T>
    inline cursor_wrapper<T> make_cursor_wrapper(object_cursor_wrapper_base object_cursor) { return new cursor_wrapper_impl<T>(object_cursor); }
}