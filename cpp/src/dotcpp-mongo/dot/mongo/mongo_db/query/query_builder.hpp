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
    class FilterTokenBaseImpl : public ObjectImpl
    {
    };

    using FilterTokenBase = Ptr<FilterTokenBaseImpl>;

    /// Saves mongo query operator. For example,
    /// (obj->prop > 10)
    /// expression will be translated to
    /// { "prop" : { "$gt": "10" } }
    class OperatorWrapperImpl : public FilterTokenBaseImpl
    {
    public:

        /// Constructor from property, operator and value.
        OperatorWrapperImpl(String prop_name, String op, Object rhs)
            : prop_name_(prop_name), op_(op), rhs_(rhs)
        {}

        /// Holds property name.
        String prop_name_;

        /// Holds operator name to be applied to property.
        String op_;

        /// Holds operator parameter.
        Object rhs_;
    };

    using OperatorWrapper = Ptr<OperatorWrapperImpl>;

    /// Holds list of tokens, that will be applied to $and operator.
    /// For example,
    /// (token1 && token2 && token3)
    /// expression will be translated to
    /// "$and": [ token1, token2, token3 ]
    class AndListImpl : public FilterTokenBaseImpl
    {
    public:

        /// Holds list of tokens.
        List<FilterTokenBase> values_list_;

        /// Default constructor.
        AndListImpl()
            : values_list_(make_list<FilterTokenBase>())
        {
        }
    };

    /// Holds list of tokens, that will be applied to $or operator.
    /// For example,
    /// (token1 || token2 || token3)
    /// expression will be translated to
    /// "$or": [ token1, token2, token3 ]
    class OrListImpl : public FilterTokenBaseImpl
    {
    public:

        /// Holds list of tokens.
        List<FilterTokenBase> values_list_;

        /// Default constructor.
        OrListImpl()
            : values_list_(make_list<FilterTokenBase>())
        {
        }
    };

    using AndList = Ptr<AndListImpl>;
    using OrList = Ptr<OrListImpl>;

    /// Returns tokens wrapped into AndList.
    inline AndList operator &&(FilterTokenBase lhs, FilterTokenBase rhs)
    {
        AndList list = new AndListImpl();
        list->values_list_->add(lhs);
        list->values_list_->add(rhs);
        return list;
    }

    /// Returns tokens wrapped into AndList.
    inline AndList operator &&(FilterTokenBase lhs, AndList rhs)
    {
        rhs->values_list_->add(lhs);
        return rhs;
    }

    /// Returns tokens wrapped into AndList.
    inline AndList operator &&(AndList lhs, FilterTokenBase rhs)
    {
        lhs->values_list_->add(rhs);
        return lhs;
    }

    /// Returns tokens wrapped into OrList.
    inline OrList operator ||(FilterTokenBase lhs, FilterTokenBase rhs)
    {
        OrList list = new OrListImpl();
        list->values_list_->add(lhs);
        list->values_list_->add(rhs);
        return list;
    }

    /// Returns tokens wrapped into OrList.
    inline OrList operator ||(FilterTokenBase lhs, OrList rhs)
    {
        rhs->values_list_->add(lhs);
        return std::move(rhs);
    }

    /// Returns tokens wrapped into OrList.
    inline OrList operator ||(OrList lhs, FilterTokenBase rhs)
    {
        lhs->values_list_->add(rhs);
        return lhs;
    }

    namespace detail
    {
        /// Converts specific types into common.
        template <class T>
        struct InTraits
        {
            typedef T type;
        };

        /// Converts std::string into dot::String.
        template <>
        struct InTraits<std::string>
        {
            typedef dot::String type;
        };

        /// Converts char* into dot::String.
        template <>
        struct InTraits<char*>
        {
            typedef dot::String type;
        };

        /// Converts specific const char* into dot::String.
        template <>
        struct InTraits<const char*>
        {
            typedef dot::String type;
        };
    }

    template <class Class, class Prop>
    struct PropWrapper;

    /// Wrapps document property sequence to access inner document properties.
    template <class LastProp>
    struct PropsList
    {
        /// Holds property sequence.
        std::deque<dot::String> props_;

        /// Wraps inner document properties access into PropsList.
        template <class ClassR, class PropR>
        PropsList<PropR> operator->*(const PropWrapper<ClassR, PropR> & rhs)
        {
            // Compile time check
            static_assert(std::is_same<typename LastProp::element_type, ClassR>::value, "Wrong ->* sequence. Left operand doesn't have right property.");

            PropsList<PropR> ret;
            ret.props_.swap(props_);
            ret.props_.push_back(rhs.prop_->name());
            return ret;
        }

        /// Wraps inner array item access into PropsList.
        auto operator[](int rhs)
        {
            using ReturnType = typename std::remove_reference<decltype(std::declval<LastProp>().operator[](rhs))>::type;

            PropsList<ReturnType> ret;
            ret.props_.swap(props_);
            ret.props_.push_back(std::to_string(rhs));
            return ret;
        }

        /// Applies operator to PropsList and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper operator==(T rhs) const
        {
            return new OperatorWrapperImpl(get_name(), "$eq", rhs);
        }

        /// Applies operator to PropsList and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper operator!=(T rhs) const
        {
            return new OperatorWrapperImpl(get_name(), "$ne", rhs);
        }

        /// Applies operator to PropsList and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper operator<(T rhs) const
        {
            return new OperatorWrapperImpl(get_name(), "$lt", rhs);
        }

        /// Applies operator to PropsList and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper operator<=(T rhs) const
        {
            return new OperatorWrapperImpl(get_name(), "$lte", rhs);
        }

        /// Applies operator to PropsList and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper operator>(T rhs) const
        {
            return new OperatorWrapperImpl(get_name(), "$gt", rhs);
        }

        /// Applies operator to PropsList and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper operator>=(T rhs) const
        {
            return new OperatorWrapperImpl(get_name(), "$gte", rhs);
        }

        /// Applies "$in" operator to PropsList and values list and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper in(T const& rhs)
        {
            List<typename detail::InTraits<typename T::value_type>::type> l = make_list<typename detail::InTraits<typename T::value_type>::type>();
            for (typename T::value_type item : rhs)
            {
                l->add(item);
            }
            return new OperatorWrapperImpl(get_name(), "$in", l);
        }

        /// Applies "$in" operator to PropsList and values list and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper in(dot::List<T> rhs)
        {
            return new OperatorWrapperImpl(get_name(), "$in", rhs);
        }

        /// Applies "$in" operator to PropsList and values list and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper in(std::initializer_list<T> const& rhs)
        {
            List<typename detail::InTraits<T>::type> l = make_list<typename detail::InTraits<T>::type>();
            for (T item : rhs)
            {
                l->add(item);
            }
            return new OperatorWrapperImpl(get_name(), "$in", l);
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
    struct PropWrapper
    {
        /// Holds property name.
        dot::FieldInfo prop_;

        /// Wraps inner document properties access into PropsList.
        template <class ClassR, class PropR>
        PropsList<PropR> operator->*(const PropWrapper<ClassR, PropR> & rhs)
        {
            // Compile time check
            static_assert(std::is_same<typename Prop::element_type, ClassR>::value, "Wrong ->* sequence. Left operand doesn't have right property.");
            return PropsList<PropR>{{ prop_->name(), rhs.prop_->name() }};
        }

        /// Wraps inner array item access into PropsList.
        auto operator[](int rhs)
        {
            using ReturnType = typename std::remove_reference<decltype(std::declval<Prop>().operator[](rhs))>::type;
            return PropsList<ReturnType>{{ prop_->name(), std::to_string(rhs) }};
        }

        /// Applies operator to PropWrapper and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper operator==(T rhs) const
        {
            return new OperatorWrapperImpl(prop_->name(), "$eq", rhs);
        }

        /// Applies operator to PropWrapper and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper operator!=(T rhs) const
        {
            return new OperatorWrapperImpl(prop_->name(), "$ne", rhs);
        }

        /// Applies operator to PropWrapper and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper operator<(T rhs) const
        {
            return new OperatorWrapperImpl(prop_->name(), "$lt", rhs);
        }

        /// Applies operator to PropWrapper and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper operator<=(T rhs) const
        {
            return new OperatorWrapperImpl(prop_->name(), "$lte", rhs);
        }

        /// Applies operator to PropWrapper and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper operator>(T rhs) const
        {
            return new OperatorWrapperImpl(prop_->name(), "$gt", rhs);
        }

        /// Applies operator to PropWrapper and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper operator>=(T rhs) const
        {
            return new OperatorWrapperImpl(prop_->name(), "$gte", rhs);
        }

        /// Applies "$in" operator to PropWrapper and values list and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper in(T const& rhs)
        {
            List<typename detail::InTraits<typename T::value_type>::type> l = make_list<typename detail::InTraits<typename T::value_type>::type>();
            for (typename T::value_type item : rhs)
            {
                l->add(item);
            }
            return new OperatorWrapperImpl(prop_->name(), "$in", l);
        }

        /// Applies "$in" operator to PropWrapper and values list and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper in(dot::List<T> rhs)
        {
            return new OperatorWrapperImpl(prop_->name(), "$in", rhs);
        }

        /// Applies "$in" operator to PropWrapper and values list and rerurns OperatorWrapper.
        template <class T>
        OperatorWrapper in(std::initializer_list<T> const& rhs)
        {
            List<typename detail::InTraits<T>::type> l = make_list<typename detail::InTraits<T>::type>();
            for (T item : rhs)
            {
                l->add(item);
            }
            return new OperatorWrapperImpl(prop_->name(), "$in", l);
        }
    };

    /// Specialization of PropWrapper class for ObjectId.
    template <class Class>
    struct PropWrapper<Class, dot::ObjectId>
    {
        /// Applies operator to PropWrapper and rerurns OperatorWrapper.
        OperatorWrapper operator==(dot::LocalDateTime rhs) const
        {
            char bytes[12] = { 0 };

            boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
            boost::posix_time::time_duration d = (boost::posix_time::ptime)rhs - epoch;
            int64_t seconds = d.total_seconds();
            std::memcpy(bytes, &seconds, sizeof(seconds));
            return new OperatorWrapperImpl("_id", "$eq", ObjectId(bsoncxx::oid(bytes, 12)));
        }

        /// Applies operator to PropWrapper and rerurns OperatorWrapper.
        OperatorWrapper operator<(dot::LocalDateTime rhs) const
        {
            char bytes[12] = { 0 };

            boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
            boost::posix_time::time_duration d = (boost::posix_time::ptime)rhs - epoch;
            int64_t seconds = d.total_seconds();
            std::memcpy(bytes, &seconds, sizeof(seconds));
            return new OperatorWrapperImpl("_id", "$lt", ObjectId(bsoncxx::oid(bytes, 12)));
        }

        /// Applies operator to PropWrapper and rerurns OperatorWrapper.
        OperatorWrapper operator==(dot::ObjectId rhs) const
        {
            return new OperatorWrapperImpl("_id", "$eq", rhs);
        }

        /// Applies operator to PropWrapper and rerurns OperatorWrapper.
        OperatorWrapper operator<(dot::ObjectId rhs) const
        {
            return new OperatorWrapperImpl("_id", "$lt", rhs);
        }
    };

    /// Wraps given class property into PropWrapper class.
    template <class Prop, class Class>
    PropWrapper<Class, Prop> make_prop(Prop Class::*prop_)
    {
        dot::Type type = dot::typeof<dot::Ptr<Class>>();
        dot::List<dot::FieldInfo> props = type->get_fields();

        for (dot::FieldInfo const& prop : props)
        {
            dot::Ptr<dot::FieldInfoImpl<Prop, Class>> casted_prop = prop.as<dot::Ptr<dot::FieldInfoImpl<Prop, Class>>>();
            if (!casted_prop.is_empty() && casted_prop->field_ == prop_)
                return PropWrapper<Class, Prop>{ prop };
        }

        throw dot::Exception("Unregistered property");
    }

    /// Wraps given class property into PropWrapper class.
    template <class Class>
    PropWrapper<Class, dot::ObjectId> make_prop(dot::ObjectId Class::*prop_)
    {
        return PropWrapper<Class, dot::ObjectId>{};
    }
}
