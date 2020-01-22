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
    template<class T> class ptr;

    /// Weak smart pointer used for preventing reference loops.
    template <class T>
    class weak_ptr
    {
        template<class R> friend class weak_ptr;
        template<class R> friend class ptr;

    public: // TYPEDEF

        typedef T element_type;
        typedef T* pointer_type;

    private: // FIELDS

        T* ptr_;

    public: // CONSTRUCTORS

        /// Create empty pointer.
        weak_ptr();

        /// Take ownership of raw pointer to template argument type.
        /// This also permits construction from null pointer.
        weak_ptr(T* p);

        /// Copy constructor for the pointer (does not copy T).
        weak_ptr(const weak_ptr<T>& rhs);

    public: // METHODS

        /// Returns true if pointer holds object, and false otherwise.
        bool is_empty() const;

        ptr<T> lock() const;

    public: // OPERATORS

        /// Returns true if the argument contains
        /// pointer to the same instance as self.
        bool operator==(const weak_ptr<T>& rhs) const;

        /// Returns true if the argument does
        /// not contain pointer to the same instance as self.
        bool operator!=(const weak_ptr<T>& rhs) const;

        /// Supports weak_ptr == nullptr.
        bool operator==(nullptr_t) const;

        /// Supports weak_ptr != nullptr.
        bool operator!=(nullptr_t) const;

        /// Take ownership of raw pointer to template argument type.
        /// This also permits assignment of pointer to type derived from T.
        weak_ptr<T>& operator=(T* rhs);

        /// Assign pointer of the same type.
        weak_ptr<T>& operator=(const weak_ptr<T>& rhs);
    };

    template<class T>
    weak_ptr<T>::weak_ptr()
        : ptr_(nullptr)
    {}

    template<class T>
    weak_ptr<T>::weak_ptr(T* p)
        : ptr_(p)
    {}

    template <class T>
    weak_ptr<T>::weak_ptr(const weak_ptr<T>& rhs)
        : ptr_(rhs.ptr_)
    {}

    template <class T>
    bool weak_ptr<T>::is_empty() const
    {
        return !ptr_;
    }

    template <class T>
    ptr<T> weak_ptr<T>::lock() const
    {
        return ptr<T>(ptr_);
    }

    template <class T>
    bool weak_ptr<T>::operator==(const weak_ptr<T>& rhs) const
    {
        return ptr_ == rhs.ptr_;
    }

    template <class T>
    bool weak_ptr<T>::operator!=(const weak_ptr<T>& rhs) const
    {
        return ptr_ != rhs.ptr_;
    }

    template <class T>
    bool weak_ptr<T>::operator==(nullptr_t) const
    {
        return ptr_ == nullptr;
    }

    template <class T>
    bool weak_ptr<T>::operator!=(nullptr_t) const
    {
        return ptr_ != nullptr;
    }

    template <class T>
    weak_ptr<T>& weak_ptr<T>::operator=(T* rhs)
    {
        ptr_ = rhs;
        return *this;
    }

    template <class T>
    weak_ptr<T>& weak_ptr<T>::operator=(const weak_ptr<T>& rhs)
    {
        ptr_ = rhs.ptr_;
        return *this;
    }
}
