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

#include <unordered_map>
#include <dot/system/exception.hpp>
#include <dot/system/collections/generic/list.hpp>

namespace dot
{
    template <class TKey, class TValue> class DictionaryImpl;
    template <class TKey, class TValue> using Dictionary = Ptr<DictionaryImpl<TKey, TValue>>;

    /// Represents a collection of keys and values.
    template <class TKey, class TValue>
    class DictionaryImpl
        : public virtual ObjectImpl
        , public std::unordered_map<TKey, TValue>
    {
        typedef DictionaryImpl<TKey, TValue> self;
        typedef std::unordered_map<TKey, TValue> base;

        template <class key_t_, class value_t_>
        friend Dictionary<key_t_, value_t_> make_dictionary();

    private: // CONSTRUCTORS

        /// Initializes a new instance of Dictionary.
        ///
        /// This constructor is private. Use make_dictionary() function instead.
        DictionaryImpl() : base() {}

    public: // PROPERTIES

        /// Gets the number of key/value pairs contained in the Dictionary.
        int count() { return this->size(); }

        /// Gets a collection containing the keys in the Dictionary.
        List<TKey> keys()
        {
            List<TKey> list = make_list<TKey>();
            for (auto& x : *this) list->add(x.first);
            return list;
        }

        /// Gets a collection containing the values in the Dictionary.
        List<TValue> values()
        {
            List<TValue> list = make_list<TValue>();
            for (auto& x : *this) list->add(x.second);
            return list;
        }

    public: // METHODS

        /// Adds the specified key and value to the Dictionary.
        void add(const TKey& key, const TValue& value)
        {
            this->add(std::pair<TKey, TValue>(key, value));
        }

        /// Adds the specified value to the collection_base with the specified key.
        void add(const std::pair<TKey, TValue>& key_value_pair)
        {
            auto res = this->insert(key_value_pair);
            if (!res.second)
                throw Exception("An element with the same key already exists in the Dictionary");
        }

        /// Determines whether the Dictionary contains the specified key.
        bool contains_key(const TKey& key)
        {
            return this->find(key) != this->end();
        }

        /// Determines whether the Dictionary contains a specific value.
        virtual bool contains_value(const TValue& value)
        {
            for (auto& x : *this)
            {
                if (std::equal_to<TValue>()(x.second, value))
                    return true;
            }
            return false;
        }

        /// Removes the value with the specified key from the Dictionary.
        bool remove(const TKey& key)
        {
            return this->erase(key) != 0;
        }

        /// Gets the value associated with the specified key.
        bool try_get_value(const TKey& key, TValue& value)
        {
            auto iter = this->find(key);
            if (iter != this->end())
            {
                value = iter->second;
                return true;
            }
            return false;
        }

    public: // OPERATORS

        /// Gets or sets the value associated with the specified key.
        TValue& operator[](const TKey& key)
        {
            return base::operator[](key);
        }
    };

    /// Initializes a new instance of Dictionary.
    template <class TKey, class TValue>
    inline Dictionary<TKey, TValue> make_dictionary() { return new DictionaryImpl<TKey, TValue>(); }
}
