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

#include <deque>

#include <dot/mongo/declare.hpp>
#include <dot/system/ptr.hpp>
#include <dot/system/object.hpp>

#include <dot/mongo/mongo_db/bson/object_id.hpp>

#include <dot/noda_time/local_date_util.hpp>
#include <dot/noda_time/local_time_util.hpp>
#include <dot/noda_time/local_date_time_util.hpp>
#include <dot/noda_time/local_minute_util.hpp>

namespace dot
{
    /// Base class for mongo query tokens.
    /// It used for wrapping c++ expressions into mongo query tokens.
    class filter_token_base_impl : public ObjectImpl
    {
    };

    using filter_token_base = Ptr<filter_token_base_impl>;

    /// Saves mongo query operator. For example,
    /// (obj->prop > 10)
    /// expression will be translated to
    /// { "prop" : { "$gt": "10" } }
    class operator_wrapper_impl : public filter_token_base_impl
    {
    public:

        /// Constructor from property, operator and value.
        operator_wrapper_impl(String prop_name, String op, Object rhs)
            : prop_name_(prop_name), op_(op), rhs_(rhs)
        {}

        /// Holds property name.
        String prop_name_;

        /// Holds operator name to be applied to property.
        String op_;

        /// Holds operator parameter.
        Object rhs_;
    };

    using operator_wrapper = Ptr<operator_wrapper_impl>;

    /// Holds list of tokens, that will be applied to $and operator.
    /// For example,
    /// (token1 && token2 && token3)
    /// expression will be translated to
    /// "$and": [ token1, token2, token3 ]
    class and_list_impl : public filter_token_base_impl
    {
    public:

        /// Holds list of tokens.
        list<filter_token_base> values_list_;

        /// Default constructor.
        and_list_impl()
            : values_list_(make_list<filter_token_base>())
        {
        }
    };

    /// Holds list of tokens, that will be applied to $or operator.
    /// For example,
    /// (token1 || token2 || token3)
    /// expression will be translated to
    /// "$or": [ token1, token2, token3 ]
    class or_list_impl : public filter_token_base_impl
    {
    public:

        /// Holds list of tokens.
        list<filter_token_base> values_list_;

        /// Default constructor.
        or_list_impl()
            : values_list_(make_list<filter_token_base>())
        {
        }
    };

    using and_list = Ptr<and_list_impl>;
    using or_list = Ptr<or_list_impl>;

    /// Returns tokens wrapped into and_list.
    inline and_list operator &&(filter_token_base lhs, filter_token_base rhs)
    {
        and_list list = new and_list_impl();
        list->values_list_->add(lhs);
        list->values_list_->add(rhs);
        return list;
    }

    /// Returns tokens wrapped into and_list.
    inline and_list operator &&(filter_token_base lhs, and_list rhs)
    {
        rhs->values_list_->add(lhs);
        return rhs;
    }

    /// Returns tokens wrapped into and_list.
    inline and_list operator &&(and_list lhs, filter_token_base rhs)
    {
        lhs->values_list_->add(rhs);
        return lhs;
    }

    /// Returns tokens wrapped into or_list.
    inline or_list operator ||(filter_token_base lhs, filter_token_base rhs)
    {
        or_list list = new or_list_impl();
        list->values_list_->add(lhs);
        list->values_list_->add(rhs);
        return list;
    }

    /// Returns tokens wrapped into or_list.
    inline or_list operator ||(filter_token_base lhs, or_list rhs)
    {
        rhs->values_list_->add(lhs);
        return std::move(rhs);
    }

    /// Returns tokens wrapped into or_list.
    inline or_list operator ||(or_list lhs, filter_token_base rhs)
    {
        lhs->values_list_->add(rhs);
        return lhs;
    }

    namespace detail
    {
        /// Converts specific types into common.
        template <class T>
        struct in_traits
        {
            typedef T type;
        };

        /// Converts std::string into dot::String.
        template <>
        struct in_traits<std::string>
        {
            typedef dot::String type;
        };

        /// Converts char* into dot::String.
        template <>
        struct in_traits<char*>
        {
            typedef dot::String type;
        };

        /// Converts specific const char* into dot::String.
        template <>
        struct in_traits<const char*>
        {
            typedef dot::String type;
        };
    }

    template <class Class, class Prop>
    struct prop_wrapper;

    /// Wrapps document property sequence to access inner document properties.
    template <class LastProp>
    struct props_list
    {
        /// Holds property sequence.
        std::deque<dot::String> props_;

        /// Wraps inner document properties access into props_list.
        template <class ClassR, class PropR>
        props_list<PropR> operator->*(const prop_wrapper<ClassR, PropR> & rhs)
        {
            // Compile time check
            static_assert(std::is_same<typename LastProp::element_type, ClassR>::value, "Wrong ->* sequence. Left operand doesn't have right property.");

            props_list<PropR> ret;
            ret.props_.swap(props_);
            ret.props_.push_back(rhs.prop_->name());
            return ret;
        }

        /// Wraps inner array item access into props_list.
        auto operator[](int rhs)
        {
            using ReturnType = typename std::remove_reference<decltype(std::declval<LastProp>().operator[](rhs))>::type;

            props_list<ReturnType> ret;
            ret.props_.swap(props_);
            ret.props_.push_back(std::to_string(rhs));
            return ret;
        }

        /// Applies operator to props_list and rerurns operator_wrapper.
        template <class T>
        operator_wrapper operator==(T rhs) const
        {
            return new operator_wrapper_impl(get_name(), "$eq", rhs);
        }

        /// Applies operator to props_list and rerurns operator_wrapper.
        template <class T>
        operator_wrapper operator!=(T rhs) const
        {
            return new operator_wrapper_impl(get_name(), "$ne", rhs);
        }

        /// Applies operator to props_list and rerurns operator_wrapper.
        template <class T>
        operator_wrapper operator<(T rhs) const
        {
            return new operator_wrapper_impl(get_name(), "$lt", rhs);
        }

        /// Applies operator to props_list and rerurns operator_wrapper.
        template <class T>
        operator_wrapper operator<=(T rhs) const
        {
            return new operator_wrapper_impl(get_name(), "$lte", rhs);
        }

        /// Applies operator to props_list and rerurns operator_wrapper.
        template <class T>
        operator_wrapper operator>(T rhs) const
        {
            return new operator_wrapper_impl(get_name(), "$gt", rhs);
        }

        /// Applies operator to props_list and rerurns operator_wrapper.
        template <class T>
        operator_wrapper operator>=(T rhs) const
        {
            return new operator_wrapper_impl(get_name(), "$gte", rhs);
        }

        /// Applies "$in" operator to props_list and values list and rerurns operator_wrapper.
        template <class T>
        operator_wrapper in(T const& rhs)
        {
            list<typename detail::in_traits<typename T::value_type>::type> l = make_list<typename detail::in_traits<typename T::value_type>::type>();
            for (typename T::value_type item : rhs)
            {
                l->add(item);
            }
            return new operator_wrapper_impl(get_name(), "$in", l);
        }

        /// Applies "$in" operator to props_list and values list and rerurns operator_wrapper.
        template <class T>
        operator_wrapper in(dot::list<T> rhs)
        {
            return new operator_wrapper_impl(get_name(), "$in", rhs);
        }

        /// Applies "$in" operator to props_list and values list and rerurns operator_wrapper.
        template <class T>
        operator_wrapper in(std::initializer_list<T> const& rhs)
        {
            list<typename detail::in_traits<T>::type> l = make_list<typename detail::in_traits<T>::type>();
            for (T item : rhs)
            {
                l->add(item);
            }
            return new operator_wrapper_impl(get_name(), "$in", l);
        }

    private:

        /// Returns property list joined with dot.
        String get_name() const
        {
            std::stringstream ss;
            ss << *props_.front();
            std::for_each(props_.begin() + 1, props_.end(), [&ss](const dot::String& prop) { ss << "." << *prop; });
            return ss.str();
        }
    };

    /// Wrapps document property to build mongo query.
    template <class Class, class Prop>
    struct prop_wrapper
    {
        /// Holds property name.
        dot::field_info prop_;

        /// Wraps inner document properties access into props_list.
        template <class ClassR, class PropR>
        props_list<PropR> operator->*(const prop_wrapper<ClassR, PropR> & rhs)
        {
            // Compile time check
            static_assert(std::is_same<typename Prop::element_type, ClassR>::value, "Wrong ->* sequence. Left operand doesn't have right property.");
            return props_list<PropR>{{ prop_->name(), rhs.prop_->name() }};
        }

        /// Wraps inner array item access into props_list.
        auto operator[](int rhs)
        {
            using ReturnType = typename std::remove_reference<decltype(std::declval<Prop>().operator[](rhs))>::type;
            return props_list<ReturnType>{{ prop_->name(), std::to_string(rhs) }};
        }

        /// Applies operator to prop_wrapper and rerurns operator_wrapper.
        template <class T>
        operator_wrapper operator==(T rhs) const
        {
            return new operator_wrapper_impl(prop_->name(), "$eq", rhs);
        }

        /// Applies operator to prop_wrapper and rerurns operator_wrapper.
        template <class T>
        operator_wrapper operator!=(T rhs) const
        {
            return new operator_wrapper_impl(prop_->name(), "$ne", rhs);
        }

        /// Applies operator to prop_wrapper and rerurns operator_wrapper.
        template <class T>
        operator_wrapper operator<(T rhs) const
        {
            return new operator_wrapper_impl(prop_->name(), "$lt", rhs);
        }

        /// Applies operator to prop_wrapper and rerurns operator_wrapper.
        template <class T>
        operator_wrapper operator<=(T rhs) const
        {
            return new operator_wrapper_impl(prop_->name(), "$lte", rhs);
        }

        /// Applies operator to prop_wrapper and rerurns operator_wrapper.
        template <class T>
        operator_wrapper operator>(T rhs) const
        {
            return new operator_wrapper_impl(prop_->name(), "$gt", rhs);
        }

        /// Applies operator to prop_wrapper and rerurns operator_wrapper.
        template <class T>
        operator_wrapper operator>=(T rhs) const
        {
            return new operator_wrapper_impl(prop_->name(), "$gte", rhs);
        }

        /// Applies "$in" operator to prop_wrapper and values list and rerurns operator_wrapper.
        template <class T>
        operator_wrapper in(T const& rhs)
        {
            list<typename detail::in_traits<typename T::value_type>::type> l = make_list<typename detail::in_traits<typename T::value_type>::type>();
            for (typename T::value_type item : rhs)
            {
                l->add(item);
            }
            return new operator_wrapper_impl(prop_->name(), "$in", l);
        }

        /// Applies "$in" operator to prop_wrapper and values list and rerurns operator_wrapper.
        template <class T>
        operator_wrapper in(dot::list<T> rhs)
        {
            return new operator_wrapper_impl(prop_->name(), "$in", rhs);
        }

        /// Applies "$in" operator to prop_wrapper and values list and rerurns operator_wrapper.
        template <class T>
        operator_wrapper in(std::initializer_list<T> const& rhs)
        {
            list<typename detail::in_traits<T>::type> l = make_list<typename detail::in_traits<T>::type>();
            for (T item : rhs)
            {
                l->add(item);
            }
            return new operator_wrapper_impl(prop_->name(), "$in", l);
        }
    };

    /// Specialization of prop_wrapper class for object_id.
    template <class Class>
    struct prop_wrapper<Class, dot::object_id>
    {
        /// Applies operator to prop_wrapper and rerurns operator_wrapper.
        operator_wrapper operator==(dot::LocalDateTime rhs) const
        {
            char bytes[12] = { 0 };

            boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
            boost::posix_time::time_duration d = (boost::posix_time::ptime)rhs - epoch;
            int64_t seconds = d.total_seconds();
            std::memcpy(bytes, &seconds, sizeof(seconds));
            return new operator_wrapper_impl("_id", "$eq", object_id(bsoncxx::oid(bytes, 12)));
        }

        /// Applies operator to prop_wrapper and rerurns operator_wrapper.
        operator_wrapper operator<(dot::LocalDateTime rhs) const
        {
            char bytes[12] = { 0 };

            boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
            boost::posix_time::time_duration d = (boost::posix_time::ptime)rhs - epoch;
            int64_t seconds = d.total_seconds();
            std::memcpy(bytes, &seconds, sizeof(seconds));
            return new operator_wrapper_impl("_id", "$lt", object_id(bsoncxx::oid(bytes, 12)));
        }

        /// Applies operator to prop_wrapper and rerurns operator_wrapper.
        operator_wrapper operator==(dot::object_id rhs) const
        {
            return new operator_wrapper_impl("_id", "$eq", rhs);
        }

        /// Applies operator to prop_wrapper and rerurns operator_wrapper.
        operator_wrapper operator<(dot::object_id rhs) const
        {
            return new operator_wrapper_impl("_id", "$lt", rhs);
        }
    };

    /// Wraps given class property into prop_wrapper class.
    template <class Prop, class Class>
    prop_wrapper<Class, Prop> make_prop(Prop Class::*prop_)
    {
        dot::type type = dot::typeof<dot::Ptr<Class>>();
        dot::list<dot::field_info> props = type->get_fields();

        for (dot::field_info const& prop : props)
        {
            dot::Ptr<dot::field_info_impl<Prop, Class>> casted_prop = prop.as<dot::Ptr<dot::field_info_impl<Prop, Class>>>();
            if (!casted_prop.is_empty() && casted_prop->field_ == prop_)
                return prop_wrapper<Class, Prop>{ prop };
        }

        throw dot::Exception("Unregistered property");
    }

    /// Wraps given class property into prop_wrapper class.
    template <class Class>
    prop_wrapper<Class, dot::object_id> make_prop(dot::object_id Class::*prop_)
    {
        return prop_wrapper<Class, dot::object_id>{};
    }
}
