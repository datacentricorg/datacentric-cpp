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

    class token_base_impl : public object_impl
    {
    };

    using token_base = ptr<token_base_impl>;

    class operator_wrapper_impl : public token_base_impl
    {
    public:

        operator_wrapper_impl(string prop_name, string op, object rhs)
            : prop_name_(prop_name), op_(op), rhs_(rhs)
        {}

        string prop_name_;
        string op_;
        object rhs_;
    };

    using operator_wrapper = ptr<operator_wrapper_impl>;

    class and_list_impl : public token_base_impl
    {
    public:

        list<token_base> values_list_;

        and_list_impl()
            : values_list_(make_list<token_base>())
        {
        }

    };

    struct or_list_impl : public token_base_impl
    {
        list<token_base> values_list_;

        or_list_impl()
            : values_list_(make_list<token_base>())
        {
        }
    };

    using and_list = ptr<and_list_impl>;
    using or_list = ptr<or_list_impl>;

    inline and_list operator &&(token_base lhs, token_base rhs)
    {
        and_list list = new and_list_impl();
        list->values_list_->add(lhs);
        list->values_list_->add(rhs);
        return list;
    }

    inline and_list operator &&(token_base lhs, and_list rhs)
    {
        rhs->values_list_->add(lhs);
        return rhs;
    }

    inline and_list operator &&(and_list lhs, token_base rhs)
    {
        lhs->values_list_->add(rhs);
        return lhs;
    }

    inline or_list operator ||(token_base lhs, token_base rhs)
    {
        or_list list = new or_list_impl();;
        list->values_list_->add(lhs);
        list->values_list_->add(rhs);
        return list;
    }

    inline or_list operator ||(token_base lhs, or_list rhs)
    {
        rhs->values_list_->add(lhs);
        return std::move(rhs);
    }

    inline or_list operator ||(or_list lhs, token_base rhs)
    {
        lhs->values_list_->add(rhs);
        return lhs;
    }

    namespace detail
    {
        template <class T>
        struct in_traits
        {
            typedef T type;
        };

        template <>
        struct in_traits<std::string>
        {
            typedef dot::string type;
        };

        template <>
        struct in_traits<char*>
        {
            typedef dot::string type;
        };

        template <>
        struct in_traits<const char*>
        {
            typedef dot::string type;
        };
    }

    template <class Class, class Prop>
    struct prop_wrapper;

    template <class LastProp>
    struct props_list
    {
        std::deque<dot::string> props_;

        template <class ClassR, class PropR>
        props_list<PropR> operator->*(const prop_wrapper<ClassR, PropR> & rhs)
        {
            // Compile time check
            static_assert(std::is_same<typename LastProp::element_type, ClassR>::value, "Wrong ->* sequence. Left operand doesn't have right property.");

            props_list<PropR> ret;
            ret.props_.swap(props_);
            ret.props_.push_back(rhs.prop_->name);
            return ret;
        }

        auto operator[](int rhs)
        {
            using ReturnType = typename std::remove_reference<decltype(std::declval<LastProp>().operator[](rhs))>::type;

            props_list<ReturnType> ret;
            ret.props_.swap(props_);
            ret.props_.push_back(std::to_string(rhs));
            return ret;
        }

        template <class T>
        operator_wrapper operator==(T rhs) const
        {
            return new operator_wrapper_impl(get_name(), "$eq", rhs);
        }

        template <class T>
        operator_wrapper operator!=(T rhs) const
        {
            return new operator_wrapper_impl(get_name(), "$ne", rhs);
        }

        template <class T>
        operator_wrapper operator<(T rhs) const
        {
            return new operator_wrapper_impl(get_name(), "$lt", rhs);
        }

        template <class T>
        operator_wrapper operator<=(T rhs) const
        {
            return new operator_wrapper_impl(get_name(), "$lte", rhs);
        }

        template <class T>
        operator_wrapper operator>(T rhs) const
        {
            return new operator_wrapper_impl(get_name(), "$gt", rhs);
        }

        template <class T>
        operator_wrapper operator>=(T rhs) const
        {
            return new operator_wrapper_impl(get_name(), "$gte", rhs);
        }

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

        template <class T>
        operator_wrapper in(dot::list<T> rhs)
        {
            return new operator_wrapper_impl(get_name(), "$in", rhs);
        }

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

        string get_name() const
        {
            std::stringstream ss;
            ss << *props_.front();
            std::for_each(props_.begin() + 1, props_.end(), [&ss](const dot::string& prop) { ss << "." << *prop; });
            return ss.str();
        }

    };

    template <class Class, class Prop>
    struct prop_wrapper
    {
        dot::field_info prop_;

        template <class ClassR, class PropR>
        props_list<PropR> operator->*(const prop_wrapper<ClassR, PropR> & rhs)
        {
            // Compile time check
            static_assert(std::is_same<typename Prop::element_type, ClassR>::value, "Wrong ->* sequence. Left operand doesn't have right property.");
            return props_list<PropR>{{ prop_->name, rhs.prop_->name }};
        }

        auto operator[](int rhs)
        {
            using ReturnType = typename std::remove_reference<decltype(std::declval<Prop>().operator[](rhs))>::type;
            return props_list<ReturnType>{{ prop_->name, std::to_string(rhs) }};
        }

        template <class T>
        operator_wrapper operator==(T rhs) const
        {
            return new operator_wrapper_impl(prop_->name, "$eq", rhs);
        }

        template <class T>
        operator_wrapper operator!=(T rhs) const
        {
            return new operator_wrapper_impl(prop_->name, "$ne", rhs);
        }

        template <class T>
        operator_wrapper operator<(T rhs) const
        {
            return new operator_wrapper_impl(prop_->name, "$lt", rhs);
        }

        template <class T>
        operator_wrapper operator<=(T rhs) const
        {
            return new operator_wrapper_impl(prop_->name, "$lte", rhs);
        }

        template <class T>
        operator_wrapper operator>(T rhs) const
        {
            return new operator_wrapper_impl(prop_->name, "$gt", rhs);
        }

        template <class T>
        operator_wrapper operator>=(T rhs) const
        {
            return new operator_wrapper_impl(prop_->name, "$gte", rhs);
        }

        template <class T>
        operator_wrapper in(T const& rhs)
        {
            list<typename detail::in_traits<typename T::value_type>::type> l = make_list<typename detail::in_traits<typename T::value_type>::type>();
            for (typename T::value_type item : rhs)
            {
                l->add(item);
            }
            return new operator_wrapper_impl(prop_->name, "$in", l);
        }

        template <class T>
        operator_wrapper in(dot::list<T> rhs)
        {
            return new operator_wrapper_impl(prop_->name, "$in", rhs);
        }

        template <class T>
        operator_wrapper in(std::initializer_list<T> const& rhs)
        {
            list<typename detail::in_traits<T>::type> l = make_list<typename detail::in_traits<T>::type>();
            for (T item : rhs)
            {
                l->add(item);
            }
            return new operator_wrapper_impl(prop_->name, "$in", l);
        }

    };

    template <class Class>
    struct prop_wrapper<Class, dot::object_id>
    {
        operator_wrapper operator==(dot::local_date_time rhs) const
        {
            char bytes[12] = { 0 };

            boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
            boost::posix_time::time_duration d = (boost::posix_time::ptime)rhs - epoch;
            int64_t seconds = d.total_seconds();
            std::memcpy(bytes, &seconds, sizeof(seconds));
            return new operator_wrapper_impl("_id", "$eq", object_id(bsoncxx::oid(bytes, 12)));
        }

        operator_wrapper operator<(dot::local_date_time rhs) const
        {
            char bytes[12] = { 0 };

            boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
            boost::posix_time::time_duration d = (boost::posix_time::ptime)rhs - epoch;
            int64_t seconds = d.total_seconds();
            std::memcpy(bytes, &seconds, sizeof(seconds));
            return new operator_wrapper_impl("_id", "$lt", object_id(bsoncxx::oid(bytes, 12)));
        }

        operator_wrapper operator==(dot::object_id rhs) const
        {
            return new operator_wrapper_impl("_id", "$eq", rhs);
        }

        operator_wrapper operator<(dot::object_id rhs) const
        {
            return new operator_wrapper_impl("_id", "$lt", rhs);
        }

    };


    template <class Prop, class Class>
    prop_wrapper<Class, Prop> make_prop(Prop Class::*prop_)
    {
        dot::type type = dot::typeof<dot::ptr<Class>>();
        dot::list<dot::field_info> props = type->get_fields();

        for (dot::field_info const& prop : props)
        {
            dot::ptr<dot::field_info_impl<Prop, Class>> casted_prop = prop.as<dot::ptr<dot::field_info_impl<Prop, Class>>>();
            if (!casted_prop.is_empty() && casted_prop->field_ == prop_)
                return prop_wrapper<Class, Prop>{ prop };

        }

        throw dot::exception("Unregistered property");
    }

    template <class Class>
    prop_wrapper<Class, dot::object_id> make_prop(dot::object_id Class::*prop_)
    {
        return prop_wrapper<Class, dot::object_id>{};
    }
}
