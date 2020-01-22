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

#include <dot/system/ptr.hpp>
#include <dot/system/collections/list_base.hpp>

namespace dot
{
    template <class T> class ListImpl; template <class T> using List = Ptr<ListImpl<T>>;
    class TypeBuilderImpl; using TypeBuilder = Ptr<TypeBuilderImpl>;

    /// Represents a strongly typed collection of objects that can be accessed by index.
    template <class T>
    class ListImpl : public virtual ObjectImpl, public std::vector<T>, public ListBaseImpl
    {
        template <class R> friend List<R> make_list();
        template <class R> friend List<R> make_list(const std::vector<R> & obj);
        template <class R> friend List<R> make_list(std::vector<R>&& obj);
        template <class R> friend List<R> make_list(const std::initializer_list<R> & obj);
        template <class R> friend List<R> make_list(int size);

        typedef ListImpl<T> self;
        typedef std::vector<T> base;

    private: // CONSTRUCTORS

        /// Create empty list with default initial capacity.
        ///
        /// This constructor is private. Use make_list(...) function instead.
        ListImpl() {}

        /// Construct from vector using deep copy semantics.
        ///
        /// This constructor is private. Use make_list(...) function instead.
        explicit ListImpl(const std::vector<T>& obj) : base(obj) {}

        /// Construct from vector using move semantics.
        ///
        /// This constructor is private. Use make_list(...) function instead.
        explicit ListImpl(std::vector<T>&& obj) : base(obj) {}

        /// Construct from initializer list.
        ///
        /// This constructor is private. Use make_list(...) function instead.
        explicit ListImpl(const std::initializer_list<T>& obj) : base(obj) {}

        /// Construct from int.
        ///
        /// This constructor is private. Use make_list(...) function instead.
        explicit ListImpl(int size) : base(size) {}

    public: // METHODS

        /// The number of items contained in the list.
        int count() { return this->size(); }

        /// Set total number of elements the internal data structure can hold without resizing.
        void set_capacity(int value) { this->reserve(value); }

        /// Adds an Object to the end of the list.
        void add(const T& item) { this->push_back(item); }

        /// Determines whether an element is in the list.
        virtual bool contains(const T& item)
        {
            return std::find(this->begin(), this->end(), item) != this->end();
        }

        /// Adds the elements of the specified collection to the end of the list.
        void add_range(const std::initializer_list<T>& collection)
        {
            this->insert(this->end(), collection.begin(), collection.end());
        }

        /// Adds the elements of the specified collection to the end of the list.
        void add_range(List<T> collection)
        {
            this->insert(this->end(), collection->begin(), collection->end());
        }

        /// Removes the first occurrence of a specific Object from the list.
        bool remove(const T& item)
        {
            auto iter = std::find(this->begin(), this->end(), item);
            if (iter != this->end())
            {
                this->erase(iter);
                return true;
            }
            return false;
        }

    public: // VIRTUAL METHODS

        /// Get Object from collection by index.
        virtual Object get_item(int index) override
        {
            return this->operator[](index);
        }

        /// Set Object from collection by index.
        virtual void set_item(int index, Object value) override
        {
            this->operator[](index) = (T)value;
        }

        /// Get length of collection.
        virtual int get_length() override
        {
            return this->size();
        }

        /// Add Object to end of collection.
        virtual void add_object(Object item) override
        {
            add((T)item);
        }

    public: // OPERATORS

        /// Gets or sets the element at the specified index (const version).
        virtual const T& operator[](int i) const { return base::operator[](i); }

        /// Gets or sets the element at the specified index (non-const version).
        virtual T& operator[](int i) { return base::operator[](i); }

    public: // REFLECTION

        virtual Type get_type() { return typeof(); }
        static Type typeof();
    };

    /// Create empty list with default initial capacity.
    template <class T>
    List<T> make_list() { return new ListImpl<T>(); }

    /// Construct from vector using deep copy semantics.
    template <class T>
    List<T> make_list(const std::vector<T> & obj) { return new ListImpl<T>(obj); }

    /// Construct from vector using move semantics.
    template <class T>
    List<T> make_list(std::vector<T>&& obj) { return new ListImpl<T>(std::forward<std::vector<T>>(obj)); }

    /// Construct from initializer list.
    template <class T>
    List<T> make_list(const std::initializer_list<T> & obj) { return new ListImpl<T>(obj); }

    /// Construct from int.
    template <class T>
    List<T> make_list(int size) { return new ListImpl<T>(size); }
}
