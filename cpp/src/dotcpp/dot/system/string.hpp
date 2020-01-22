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
#include <fmt/core.h>
#include <dot/detail/const_string_base.hpp>
#include <dot/system/ptr.hpp>
#include <dot/system/char.hpp>

namespace dot
{
    class StringImpl; class String;
    template <class T> class list_impl; template <class T> using list = Ptr<list_impl<T>>;
    template <class T> class Nullable;
    enum class StringSplitOptions;

    /// Immutable String type.
    ///
    /// The String is encoded internally as UTF-8.
    class DOT_CLASS StringImpl : public virtual ObjectImpl, public detail::ConstStringBase
    {
        typedef StringImpl self;
        typedef detail::ConstStringBase base;
        friend String make_string(const std::string& rhs);
        friend String make_string(const char* rhs);

    public: // CONSTRUCTORS

        /// Creates an empty String.
        StringImpl() {}

        /// Initializes from std::String or String literal.
        ///
        /// This constructor is private. Use make_string(str) function instead.
        StringImpl(const std::string& value) : base(value) {}

        /// Create from const char*, null pointer is converted to to empty value.
        StringImpl(const char* value) : base(value) {}

        /// Create from a single 8-bit character.
        StringImpl(char value) : base(std::to_string(value)) {}

    public: // METHODS

        /// Determines whether this instance and a specified Object,
        /// which must also be a String Object, have the same value.
        bool equals(Object obj) override;

        /// Returns the hash code for this String.
        virtual size_t hash_code() override;

        /// Returns this instance of String; no actual conversion is performed.
        virtual String to_string() override;

        /// Determines whether the end of this
        /// String matches the specified String.
        bool ends_with(const String& value);

        /// Determines whether the beginning of this
        /// String matches the specified String.
        bool starts_with(const String& value);

        /// Retrieves a substring which starts at a specified
        /// character position and continues to the end of the String.
        String substring(int start_index);

        /// Retrieves a substring which starts at the specified
        /// character position and has the specified length.
        String substring(int start_index, int length);

        /// Compares this instance with a specified String Object and indicates
        /// whether this instance precedes, follows, or appears in the same position
        /// in the sort order as the specified String.
        int compare_to(const String& str_b) const;

        /// Reports the zero-based index of the first occurrence in this instance of any
        /// character in a specified array of Unicode characters.
        int index_of_any(list<char> any_of);

        /// Returns a new String in which all the characters in the current instance, beginning
        /// at a specified position and continuing through the last position, have been deleted.
        String remove(int start_index);

        /// Returns a new String in which a specified number of characters in the current
        /// instance beginning at a specified position have been deleted.
        String remove(int start_index, int count);

        /// Returns a new String in which all occurrences of a specified String
        /// in the current instance are replaced with another specified String.
        String replace(const String& old_value, const String& new_value) const; // TODO - implement

        /// Returns a new String in which all occurrences of a specified character
        /// in this instance are replaced with another specified character.
        String replace(const char old_char, const char new_char) const;

        /// Returns a String array that contains the substrings of the current String
        /// that are delimited by any of the specified 8-bit characters.
        list<String> split(char separator) const;

        /// Splits a String into substrings that are based on the characters in an array.
        list<String> split(String separator) const;

        /// Returns a String array that contains the substrings in
        /// this String that are delimited by any of the specified strings.
        /// A parameter specifies whether to return empty array elements.
        list<String> split(const list<String>& separator, const StringSplitOptions& options) const; // TODO - implement

        /// Returns a new String in which all leading and trailing occurrences
        /// of a set of specified characters from the current String are removed.
        String trim() const;

        /// Removes all the leading occurrences of a set of characters
        /// specified in an array from the current String.
        String trim_start() const;

        /// Removes all the trailing occurrences of a set of characters
        /// specified in an array from the current String.
        String trim_end() const;

        /// Indicates whether the argument occurs within this String.
        bool contains(const String& s) const;

        /// Returns a copy of this String converted to lowercase.
        String to_lower() const;

        /// Returns a copy of this String converted to uppercase.
        String to_upper() const;

    public: // OPERATORS

        /// Returns a String containing characters from lhs followed by the characters from rhs.
        friend String operator+(const String& lhs, const String& rhs);

        /// Returns a String containing characters from lhs followed by the characters from rhs.
        friend bool operator<(const String& lhs, const String& rhs);

    public: // REFLECTION

        virtual type get_type();
        static type typeof();
    };

    /// Pointer to StringImpl that has additional constructors compared to Ptr(String)
    class DOT_CLASS String : public Ptr<StringImpl>
    {
        typedef Ptr<StringImpl> base;

    public: // CONSTANTS

        /// Empty String.
        static String empty;

    public: // CONSTRUCTORS

        /// Create null String.
        String() : base() {}

        /// Take ownership of raw pointer to template argument type.
        /// This also permits construction from null pointer.
        String(StringImpl* p) : base(p) {}

        /// Create from std::String.
        String(const std::string& rhs) : base(new StringImpl(rhs)) {}

        /// Create from String literal.
        String(const char* rhs) : base(new StringImpl(rhs)) {}

        /// Copy constructor.
        String(const String& rhs) : base(rhs) {}

        /// Supports cast (explicit constructor) from Object.
        ///
        /// Error if Object does is not a boxed int.
        /// Null Object becomes empty Nullable int.
        explicit String(const Ptr<ObjectImpl>& rhs) : base(rhs) {}

    public: // STATIC

        /// Replaces format entries in the specified String with the String
        /// representation of objects in the argument array.
        template <typename ...Args>
        static String format(const String& format_string, const Args& ...args);

        /// Indicates whether the specified String is null or an String.empty String.
        static bool is_null_or_empty(String value);

    public: // OPERATORS

        /// Case sensitive comparison to std::String.
        bool operator==(const std::string& rhs) const { StringImpl& impl = base::operator*(); return impl == rhs; }

        /// Case sensitive comparison to std::String.
        bool operator!=(const std::string& rhs) const { return !operator==(rhs); }

        /// Case sensitive comparison to String literal.
        bool operator==(const char* rhs) const { if (rhs == nullptr) return base::operator==(nullptr); StringImpl& impl = base::operator*(); return impl == rhs; }

        /// Case sensitive comparison to String literal.
        bool operator!=(const char* rhs) const { return !operator==(rhs); }

        /// Case sensitive comparison to String literal.
        bool operator==(const Ptr<StringImpl>& rhs) const { return *this == *rhs; }

        /// Case sensitive comparison to String literal.
        bool operator!=(const Ptr<StringImpl>& rhs) const { return !operator==(rhs); }

        /// Case sensitive comparison to Object.
        bool operator==(const Object& rhs) const;

        /// Case sensitive comparison to Object.
        bool operator!=(const Object& rhs) const { return !operator==(rhs); }

    private: // STATIC

        /// Non-template implementation of String.format.
        static std::string format_impl(fmt::string_view format_str, fmt::format_args args);
    };

    /// Create from std::String or String literal using new
    inline String make_string(const std::string& rhs) { return new StringImpl(rhs); }

    /// Create from std::String or String literal using new
    inline String make_string(const char* rhs) { return new StringImpl(rhs); }

    /// Returns a String containing characters from lhs followed by the characters from rhs.
    inline String operator+(const String& lhs, const String& rhs) { return make_string(*lhs + *rhs); }

    /// Returns a String containing characters from lhs followed by the characters from rhs.
    inline bool operator<(const String& lhs, const String& rhs) { return *lhs < *rhs; }

    /// Helper class for fmt::format arguments conversion
    template <class T>
    struct format_forward {
        static inline auto convert(const T& t) { return t; }
    };

    /// Helper class for fmt::format arguments conversion
    template<>
    struct format_forward<String> {
        static inline std::string convert(const String& s)
        {
            if (s.is_empty())
                return std::string("");
            else
                return *s;
        }

    };

    /// Helper class for fmt::format arguments conversion
    template<class T>
    struct format_forward<Ptr<T>> {
        static inline std::string convert(const Ptr<T>& o) { return *o->to_string(); }
    };

    /// Helper class for fmt::format arguments conversion
    template<class T>
    struct format_forward<Nullable<T>> {
        static inline T convert(const Nullable<T>& o) { return o.value_or_default(); }
    };

    template<typename ...Args>
    String String::format(const String& format_string, const Args& ...args)
    {
        return format_impl(*format_string, fmt::make_format_args(format_forward<Args>::convert(args)...));
    }
}

namespace std
{
    /// Implements hash struct used by STL unordered_map for String.
    template <>
    struct hash<dot::String> : public hash<dot::Ptr<dot::StringImpl>>
    {};

    /// Implements equal_to struct used by STL unordered_map for String.
    template <>
    struct equal_to<dot::String> : public equal_to<dot::Ptr<dot::StringImpl>>
    {};
}
