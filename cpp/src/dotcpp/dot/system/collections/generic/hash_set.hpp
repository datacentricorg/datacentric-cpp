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

#include <unordered_set>
#include <dot/system/exception.hpp>
#include <dot/system/collections/generic/list.hpp>

namespace dot
{
    template <class T> class HashSetImpl;
    template <class T> using HashSet = Ptr<HashSetImpl<T>>;

    /// Represents a set of values.
    template <class T>
    class HashSetImpl : public virtual ObjectImpl, public std::unordered_set<T>
    {
        typedef HashSetImpl<T> self;
        typedef std::unordered_set<T> base;

        template <class R> friend HashSet<R> make_hash_set();
        template <class R> friend HashSet<R> make_hash_set(List<R> collection);

    protected: // CONSTRUCTORS

        /// Initializes a new instance of the HashSet class that is empty
        /// and uses the default equality comparer for the set type.
        HashSetImpl() = default;

        /// Initializes a new instance of the HashSet class that uses the default
        /// equality comparer for the set type, contains elements copied from the specified
        /// collection, and has sufficient capacity to accommodate the number of elements copied.
        explicit HashSetImpl(List<T> collection)
        {
            for (T const & item : collection)
                this->add(item);
        }

    public: // PROPERTIES

        /// Gets the number of elements that are contained in the set.
        int count() { return this->size(); }

    public: // METHODS

        /// Adds the specified element to a set.
        bool add(const T& item)
        {
            std::pair<typename base::iterator, bool> res = this->insert(item);
            return res.second;
        }

        /// Determines whether a HashSet Object contains the specified element.
        bool contains(const T& item)
        {
            auto iter = this->find(item);
            return iter != this->end();
        }

        /// Removes the specified element from a HashSet Object.
        bool remove(const T& item)
        {
            return this->erase(item) != 0;
        }

        /// Sets the capacity of a HashSet Object to the actual number of elements
        /// it contains,rounded up to a nearby, implementation-specific value.
        void trim_excess()
        {
            this->reserve(this->size());
        }

        /// Searches the set for a given value and returns the equal value it finds, if any.
        bool try_get_value(const T& equal_value, T& actual_value)
        {
            auto iter = this->find(equal_value);
            if (iter != this->end())
            {
                actual_value = *iter;
                return true;
            }
            return false;
        }

        /// Removes all elements in the specified collection from the current HashSet Object.
        void except_with(List<T> other)
        {
            for (T const& item : other)
            {
                this->remove(item);
            }
        }

        /// Modifies the current HashSet Object to contain only elements
        /// that are present in that Object and in the specified collection.
        void intersect_with(List<T> other)
        {
            List<T> left = make_list<T>();
            for (T const& item : other)
            {
                if (this->contains(item))
                    left->add(item);
            }

            this->clear();
            for (T const& item : left) this->add(item);
        }
    };

    template <class T>
    inline HashSet<T> make_hash_set() { return new HashSetImpl<T>(); }

    template <class T>
    inline HashSet<T> make_hash_set(List<T> collection) { return new HashSetImpl<T>(collection); }
}
