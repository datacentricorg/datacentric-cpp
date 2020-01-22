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

#include <dot/declare.hpp>
#include <dot/system/exception.hpp>

namespace dot
{
    template<class T> class Ptr;

    /// Weak smart pointer used for preventing reference loops.
    template <class T>
    class WeakPtr
    {
        template<class R> friend class WeakPtr;
        template<class R> friend class Ptr;

    public: // TYPEDEF

        typedef T element_type;
        typedef T* pointer_type;

    private: // FIELDS

        T* ptr_;

    public: // CONSTRUCTORS

        /// Create empty pointer.
        WeakPtr();

        /// Take ownership of raw pointer to template argument type.
        /// This also permits construction from null pointer.
        WeakPtr(T* p);

        /// Copy constructor for the pointer (does not copy T).
        WeakPtr(const WeakPtr<T>& rhs);

    public: // METHODS

        /// Returns true if pointer holds Object, and false otherwise.
        bool is_empty() const;

        Ptr<T> lock() const;

    public: // OPERATORS

        /// Returns true if the argument contains
        /// pointer to the same instance as self.
        bool operator==(const WeakPtr<T>& rhs) const;

        /// Returns true if the argument does
        /// not contain pointer to the same instance as self.
        bool operator!=(const WeakPtr<T>& rhs) const;

        /// Supports WeakPtr == nullptr.
        bool operator==(nullptr_t) const;

        /// Supports WeakPtr != nullptr.
        bool operator!=(nullptr_t) const;

        /// Take ownership of raw pointer to template argument type.
        /// This also permits assignment of pointer to type derived from T.
        WeakPtr<T>& operator=(T* rhs);

        /// Assign pointer of the same type.
        WeakPtr<T>& operator=(const WeakPtr<T>& rhs);
    };

    template<class T>
    WeakPtr<T>::WeakPtr()
        : ptr_(nullptr)
    {}

    template<class T>
    WeakPtr<T>::WeakPtr(T* p)
        : ptr_(p)
    {}

    template <class T>
    WeakPtr<T>::WeakPtr(const WeakPtr<T>& rhs)
        : ptr_(rhs.ptr_)
    {}

    template <class T>
    bool WeakPtr<T>::is_empty() const
    {
        return !ptr_;
    }

    template <class T>
    Ptr<T> WeakPtr<T>::lock() const
    {
        return Ptr<T>(ptr_);
    }

    template <class T>
    bool WeakPtr<T>::operator==(const WeakPtr<T>& rhs) const
    {
        return ptr_ == rhs.ptr_;
    }

    template <class T>
    bool WeakPtr<T>::operator!=(const WeakPtr<T>& rhs) const
    {
        return ptr_ != rhs.ptr_;
    }

    template <class T>
    bool WeakPtr<T>::operator==(nullptr_t) const
    {
        return ptr_ == nullptr;
    }

    template <class T>
    bool WeakPtr<T>::operator!=(nullptr_t) const
    {
        return ptr_ != nullptr;
    }

    template <class T>
    WeakPtr<T>& WeakPtr<T>::operator=(T* rhs)
    {
        ptr_ = rhs;
        return *this;
    }

    template <class T>
    WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr<T>& rhs)
    {
        ptr_ = rhs.ptr_;
        return *this;
    }
}
