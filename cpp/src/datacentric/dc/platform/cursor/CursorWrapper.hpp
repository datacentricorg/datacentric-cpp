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
//#include <dc/platform/context/context_base.hpp>
#include <mongocxx/cursor.hpp>
#include <bsoncxx/types.hpp>

namespace dc
{

    class object_cursor_wrapper_impl; using object_cursor_wrapper = dot::ptr<object_cursor_wrapper_impl>;
    template <class T> class cursor_wrapper_impl;
    template <class T> using cursor_wrapper = dot::ptr<cursor_wrapper_impl<T>>;

    template <class T>
    inline object_cursor_wrapper make_object_cursor_wrapper(mongocxx::cursor && cursor, const std::function<T(const bsoncxx::document::view&)>& f);

    template <class T>
    class iterator_wrappper
    {
    public:

        typedef T value_type;

        iterator_wrappper(const mongocxx::cursor::iterator & iterator, const std::function<T(const bsoncxx::document::view&)>& f)
            :iterator_(iterator)
            ,f_(f)
        {}

        T operator*()
        {
            return f_(*iterator_);
        }

        T operator*() const
        {
            return f_(*iterator_);
        }

        iterator_wrappper& operator++()
        {
            //String currKey = (*iterator_)["_key"].get_utf8().value.to_string();
            //while (!iterator_->empty() && dot::string((*iterator_)["_key"].get_utf8().value.to_string()) == currKey)
                ++iterator_;
            return *this;
        }

        bool operator!=(iterator_wrappper const& rhs)
        {
            return iterator_ != rhs.iterator_;
        }

        bool operator==(iterator_wrappper const& rhs)
        {
            return iterator_ == rhs.iterator_;
        }

        std::function<T(const bsoncxx::document::view&)> f_;
        mongocxx::cursor::iterator iterator_;
    };


    class object_cursor_wrapper_impl : public dot::object_impl
    {
        friend object_cursor_wrapper make_object_cursor_wrapper(mongocxx::cursor &&, const std::function<dot::object(const bsoncxx::document::view&)>&);

    public:

        /// Returns begin iterator of the underlying std::vector.
        iterator_wrappper<dot::object> begin() { return iterator_wrappper<dot::object>(cursor_->begin(), f_); }

        /// Returns end iterator of the underlying std::vector.
        iterator_wrappper<dot::object> end() { return iterator_wrappper<dot::object>(cursor_->end(), f_); }

        template <class T>
        inline cursor_wrapper<T> to_cursor_wrapper()
        {
            std::function<dot::object(const bsoncxx::document::view&)> f = f_;
            return new cursor_wrapper_impl<T>(cursor_, [f](const bsoncxx::document::view& item)->T
                {
                    return T(f(item));
                }
            );
        }

    private:

        object_cursor_wrapper_impl(mongocxx::cursor && cursor, const std::function<dot::object(const bsoncxx::document::view&)>& f)
            : cursor_(std::make_shared<mongocxx::cursor>(std::move(cursor)))
            , f_(f)
        {
        }

        std::shared_ptr<mongocxx::cursor> cursor_;
        std::function<dot::object(const bsoncxx::document::view&)> f_;
    };

    template <class T>
    class cursor_wrapper_impl : public dot::object_impl
    {
        friend class object_cursor_wrapper_impl;

    public:

        /// Returns begin iterator of the underlying std::vector.
        iterator_wrappper<T> begin() { return iterator_wrappper<T>(cursor_->begin(), f_); }

        /// Returns end iterator of the underlying std::vector.
        iterator_wrappper<T> end() { return iterator_wrappper<T>(cursor_->end(), f_); }

    private:

        cursor_wrapper_impl(std::shared_ptr<mongocxx::cursor> cursor, const std::function<T(const bsoncxx::document::view&)>& f)
            : cursor_(cursor)
            , f_(f)
        {
        }

        std::shared_ptr<mongocxx::cursor> cursor_;
        std::function<T(const bsoncxx::document::view&)> f_;
    };



    inline object_cursor_wrapper make_object_cursor_wrapper(mongocxx::cursor && cursor, const std::function<dot::object(const bsoncxx::document::view&)>& f)
    {
        return new object_cursor_wrapper_impl(std::move(cursor), f);
    }

}