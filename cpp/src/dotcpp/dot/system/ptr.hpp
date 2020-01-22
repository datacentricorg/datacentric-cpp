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
#include <dot/detail/reference_counter.hpp>
#include <dot/system/exception.hpp>
#include <dot/system/weak_ptr.hpp>

namespace dot
{
    class Exception;

    /// Reference counted smart pointer based on std::shared_ptr
    /// with emulation of selected features of .NET references including
    /// type conversion using 'is' and 'as'.
    template <class T>
    class Ptr
    {
        template<class R> friend class Ptr;

    public: // TYPEDEF

        typedef T element_type;
        typedef T* pointer_type;

    private: // FIELDS

        T* ptr_;

    public: // CONSTRUCTORS

        /// Create empty pointer.
        Ptr();

        /// Take ownership of raw pointer to template argument type.
        /// This also permits construction from null pointer.
        Ptr(T* p);

        /// Conversion from WeakPtr to Ptr.
        Ptr(const WeakPtr<T>& rhs);

        /// Implicit conversion from derived to base pointer (does not use dynamic cast).
        template <class R>
        Ptr(const Ptr<R>& rhs, typename std::enable_if<std::is_base_of<T, R>::value>::type* p = 0);

        /// Explicit conversion from base to derived or sibling pointer (uses dynamic cast).
        ///
        /// Error message if the cast fails because the two types are unrelated.
        template <class R>
        explicit Ptr(const Ptr<R>& rhs, typename std::enable_if<!std::is_base_of<T, R>::value>::type* p = 0);

        /// Copy constructor for the pointer (does not copy T).
        Ptr(const Ptr<T>& rhs);

    public: // DESTRUCTOR

        /// Decrements reference count if not empty.
        ~Ptr();

    public: // METHODS

        /// Dynamic cast to type R, returns 0 if the cast fails.
        template <class R>
        R as() const;

        /// Returns true if pointer holds Object of type R, and false otherwise.
        template <class R>
        bool is() const;

        /// Returns true if pointer holds Object, and false otherwise.
        bool is_empty() const;

        WeakPtr<T> to_weak() const;

    public: // OPERATORS

        /// Pointer dereference.
        T& operator*() const;

        /// Pointer dereference.
        T* operator->() const;

        /// Returns true if the argument contains
        /// pointer to the same instance as self.
        bool operator==(const Ptr<T>& rhs) const;

        /// Returns true if the argument does
        /// not contain pointer to the same instance as self.
        bool operator!=(const Ptr<T>& rhs) const;

        /// Supports Ptr == nullptr.
        bool operator==(nullptr_t) const;

        /// Supports Ptr != nullptr.
        bool operator!=(nullptr_t) const;

        /// Take ownership of raw pointer to template argument type.
        /// This also permits assignment of pointer to type derived from T.
        Ptr<T>& operator=(T* rhs);

        /// Assign pointer to template argument base type. // TODO - use SFINAE to detemine if dynamic cast is needed
        template <class R>
        Ptr<T>& operator=(const Ptr<R>& rhs);

        /// Assign pointer of the same type.
        Ptr<T>& operator=(const Ptr<T>& rhs);

        /// Const indexer operator for arrays.
        template <class I>
        decltype(auto) operator[](I const& i) const;

        /// Non-const indexer operator for arrays.
        template <class I>
        decltype(auto) operator[](I const& i);
    };

    template <class T>
    Ptr<T>::Ptr()
        : ptr_(nullptr)
    {}

    template <class T>
    Ptr<T>::Ptr(T* p)
        : ptr_(p)
    {
        if (ptr_) ptr_->increment_reference_count();
    }

    template<class T>
    Ptr<T>::Ptr(const WeakPtr<T>& rhs)
        : ptr_(rhs.ptr_)
    {
        if (ptr_) ptr_->increment_reference_count();
    }

    template <class T>
    template <class R>
    Ptr<T>::Ptr(const Ptr<R>& rhs, typename std::enable_if<std::is_base_of<T, R>::value>::type* p)
        : ptr_(rhs.ptr_)
    {
        if (ptr_) ptr_->increment_reference_count();
    }

    template <class T>
    template <class R>
    Ptr<T>::Ptr(const Ptr<R>& rhs, typename std::enable_if<!std::is_base_of<T, R>::value>::type* p)
        : ptr_(nullptr)
    {
        // If argument is null, ptr_ should also remain null
        if (rhs.ptr_)
        {
            // Perform dynamic cast from base to derived
            T* p = dynamic_cast<T*>(rhs.ptr_);

            // Check that dynamic cast succeeded
            if (!p)
                throw dot::Exception("Cast cannot be performed."); // TODO Use typeof(...) and get_type() to provide specific types in the error message

            // Current pointer now contains the result of dynamic_cast
            ptr_ = p;

            // Increment reference count
            ptr_->increment_reference_count();
        }
    }

    template <class T>
    Ptr<T>::Ptr(const Ptr<T>& rhs)
        : ptr_(rhs.ptr_)
    {
        if (ptr_) ptr_->increment_reference_count();
    }

    template <class T>
    Ptr<T>::~Ptr()
    {
        if (ptr_) ptr_->decrement_reference_count();
    }

    template <class T>
    template <class R>
    R Ptr<T>::as() const
    {
        typename R::pointer_type p = dynamic_cast<typename R::pointer_type>(ptr_);
        return p;
    }

    template <class T>
    template <class R>
    bool Ptr<T>::is() const
    {
        return dynamic_cast<typename R::pointer_type>(ptr_);
    }

    template <class T>
    bool Ptr<T>::is_empty() const
    {
        return !ptr_;
    }

    template<class T>
    WeakPtr<T> Ptr<T>::to_weak() const
    {
        return WeakPtr<T>(ptr_);
    }

    template <class T>
    T& Ptr<T>::operator*() const
    {
        if (!ptr_)
            throw dot::Exception("Pointer is not initialized");
        return *ptr_;
    }

    template <class T>
    T* Ptr<T>::operator->() const
    {
        if (!ptr_)
            throw dot::Exception("Pointer is not initialized");
        return ptr_;
    }

    template <class T>
    bool Ptr<T>::operator==(const Ptr<T>& rhs) const
    {
        return ptr_ == rhs.ptr_; // TODO check when comparison is performed by value
    }

    template <class T>
    bool Ptr<T>::operator!=(const Ptr<T>& rhs) const
    {
        return ptr_ != rhs.ptr_; // TODO check when comparison is performed by value
    }

    template <class T>
    bool Ptr<T>::operator==(nullptr_t) const
    {
        return ptr_ == nullptr;
    }

    template <class T>
    bool Ptr<T>::operator!=(nullptr_t) const
    {
        return ptr_ != nullptr;
    }

    template <class T>
    Ptr<T>& Ptr<T>::operator=(T* rhs)
    {
        if (ptr_) ptr_->decrement_reference_count();
        if (rhs) rhs->increment_reference_count();
        ptr_ = rhs;
        return *this;
    }

    template <class T>
    template <class R>
    Ptr<T>& Ptr<T>::operator=(const Ptr<R>& rhs)
    {
        if (ptr_) ptr_->decrement_reference_count();
        if (rhs.ptr_) rhs.ptr_->increment_reference_count();
        ptr_ = rhs.ptr_;
        return *this;
    }

    template <class T>
    Ptr<T>& Ptr<T>::operator=(const Ptr<T>& rhs)
    {
        if (ptr_) ptr_->decrement_reference_count();
        if (rhs.ptr_) rhs.ptr_->increment_reference_count();
        ptr_ = rhs.ptr_;
        return *this;
    }

    template <class T>
    template <class I>
    decltype(auto) Ptr<T>::operator[](I const& i) const
    {
        return (*ptr_)[i];
    }

    template <class T>
    template <class I>
    decltype(auto) Ptr<T>::operator[](I const& i)
    {
        return (*ptr_)[i];
    }

    /// Implements begin() used by STL and similar algorithms.
    template <class T>
    auto begin(dot::Ptr<T> & obj)
    {
        return obj->begin();
    }

    /// Implements end() used by STL and similar algorithms.
    template <class T>
    auto end(dot::Ptr<T> & obj)
    {
        return obj->end();
    }

    /// Implements begin() used by STL and similar algorithms.
    template <class T>
    auto begin(dot::Ptr<T> const& obj)
    {
        return obj->begin();
    }

    /// Implements end() used by STL and similar algorithms.
    template <class T>
    auto end(dot::Ptr<T> const& obj)
    {
        return obj->end();
    }
}

namespace std
{
    /// Implements hash struct used by STL unordered_map for Ptr.
    template <typename T>
    struct hash<dot::Ptr<T>>
    {
        size_t operator()(const dot::Ptr<T>& obj) const
        {
            return obj->hash_code();
        }
    };

    /// Implements equal_to struct used by STL unordered_map for Ptr.
    template <typename T>
    struct equal_to<dot::Ptr<T>>
    {
        bool operator()(const dot::Ptr<T>& lhs, const dot::Ptr<T>& rhs) const
        {
            return lhs->equals(rhs);
        }
    };
}
