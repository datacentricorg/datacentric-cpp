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
//#include <dc/platform/context/IContext.hpp>
#include <mongocxx/cursor.hpp>
#include <bsoncxx/types.hpp>

namespace dc
{

    class ObjectCursorWrapperImpl; using ObjectCursorWrapper = dot::ptr<ObjectCursorWrapperImpl>;
    template <class T> class CursorWrapperImpl;
    template <class T> using CursorWrapper = dot::ptr<CursorWrapperImpl<T>>;

    template <class T>
    inline ObjectCursorWrapper new_ObjectCursorWrapper(mongocxx::cursor && cursor, const std::function<T(const bsoncxx::document::view&)>& f);

    template <class T>
    class IteratorWrappper
    {
    public:

        typedef T value_type;

        IteratorWrappper(const mongocxx::cursor::iterator & iterator, const std::function<T(const bsoncxx::document::view&)>& f)
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

        IteratorWrappper& operator++()
        {
            //String currKey = (*iterator_)["_key"].get_utf8().value.to_string();
            //while (!iterator_->empty() && dot::string((*iterator_)["_key"].get_utf8().value.to_string()) == currKey)
                ++iterator_;
            return *this;
        }

        bool operator!=(IteratorWrappper const& rhs)
        {
            return iterator_ != rhs.iterator_;
        }

        bool operator==(IteratorWrappper const& rhs)
        {
            return iterator_ == rhs.iterator_;
        }

        std::function<T(const bsoncxx::document::view&)> f_;
        mongocxx::cursor::iterator iterator_;
    };


    class ObjectCursorWrapperImpl : public dot::IObjectEnumerableImpl//, public IEnumerableImpl<T>
    {
        friend ObjectCursorWrapper new_ObjectCursorWrapper(mongocxx::cursor &&, const std::function<dot::object(const bsoncxx::document::view&)>&);

    public:

        /// <summary>Returns forward begin object iterator.</summary>
        virtual dot::detail::std_object_iterator_wrapper object_begin()
        {
            return dot::detail::make_obj_iterator(IteratorWrappper<dot::object>(cursor_->begin(), f_));
        }

        /// <summary>Returns forward end object iterator.</summary>
        virtual dot::detail::std_object_iterator_wrapper object_end()
        {
            return dot::detail::make_obj_iterator(IteratorWrappper<dot::object>(cursor_->end(), f_));
        }

        /// <summary>Returns random access begin iterator of the underlying std::vector.</summary>
        typename mongocxx::cursor::iterator begin() { return cursor_->begin(); }

        /// <summary>Returns random access end iterator of the underlying std::vector.</summary>
        typename mongocxx::cursor::iterator end() { return cursor_->end(); }

        template <class T>
        inline dot::IEnumerable<T> AsEnumerable()
        {
            std::function<dot::object(const bsoncxx::document::view&)> f = f_;
            return new CursorWrapperImpl<T>(cursor_, [f](const bsoncxx::document::view& item)->T
                {
                    return T(f(item));
                }
            );
        }

    private:

        ObjectCursorWrapperImpl(mongocxx::cursor && cursor, const std::function<dot::object(const bsoncxx::document::view&)>& f)
            : cursor_(std::make_shared<mongocxx::cursor>(std::move(cursor)))
            , f_(f)
        {
        }

        std::shared_ptr<mongocxx::cursor> cursor_;
        std::function<dot::object(const bsoncxx::document::view&)> f_;
    };

    template <class T>
    class CursorWrapperImpl : public dot::IEnumerableImpl<T>
    {
        friend class ObjectCursorWrapperImpl;

    public:

        /// <summary>Returns an enumerator that iterates through the collection.</summary>
        virtual dot::IEnumerator<T> GetEnumerator()
        {
            return new_Enumerator(IteratorWrappper<T>(cursor_->begin(), f_), IteratorWrappper<T>(cursor_->end(), f_));
        }

        ///// <summary>Returns random access begin iterator of the underlying std::vector.</summary>
        //typename mongocxx::cursor::iterator begin() { return cursor_->begin(); }
        //
        ///// <summary>Returns random access end iterator of the underlying std::vector.</summary>
        //typename mongocxx::cursor::iterator end() { return cursor_->end(); }

    private:

        CursorWrapperImpl(std::shared_ptr<mongocxx::cursor> cursor, const std::function<T(const bsoncxx::document::view&)>& f)
            : cursor_(cursor)
            , f_(f)
        {
        }

        std::shared_ptr<mongocxx::cursor> cursor_;
        std::function<T(const bsoncxx::document::view&)> f_;
    };



    inline ObjectCursorWrapper new_ObjectCursorWrapper(mongocxx::cursor && cursor, const std::function<dot::object(const bsoncxx::document::view&)>& f)
    {
        return new ObjectCursorWrapperImpl(std::move(cursor), f);
    }

}