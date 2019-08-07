/*
Copyright (C) 2013-present The DataCentric Authors.

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

#include <dot/noda_time/local_date_util.hpp>
#include <dot/noda_time/local_time_util.hpp>
#include <dot/noda_time/local_date_time_util.hpp>


namespace dc
{
    struct and_list
    {
        std::deque<bsoncxx::document::view_or_value> values_list_;

        operator bsoncxx::document::view_or_value() const
        {
            bsoncxx::builder::core builder(false);
            builder.key_view("$and");
            builder.open_array();
            for (bsoncxx::document::view_or_value const& item : values_list_)
            {
                builder.append(item);
            }
            builder.close_array();

            return builder.extract_document();
        }

    };

    struct or_list
    {
        std::deque<bsoncxx::document::view_or_value> values_list_;

        operator bsoncxx::document::view_or_value() const
        {
            bsoncxx::builder::core builder(false);
            builder.key_view("$or");
            builder.open_array();
            for (bsoncxx::document::view_or_value const& item : values_list_)
            {
                builder.append(item);
            }
            builder.close_array();

            return builder.extract_document();
        }

    };

    inline void append(bsoncxx::builder::core& builder, const dot::string & rhs)
    {
        builder.append(*rhs);
    }

    inline void append(bsoncxx::builder::core& builder, const std::string & rhs)
    {
        builder.append(rhs);
    }

    inline void append(bsoncxx::builder::core& builder, const char * rhs)
    {
        builder.append(rhs);
    }

    inline void append(bsoncxx::builder::core& builder, double rhs)
    {
        builder.append(rhs);
    }

    inline void append(bsoncxx::builder::core& builder, int rhs)
    {
        builder.append(rhs);
    }

    inline void append(bsoncxx::builder::core& builder, dot::local_date rhs)
    {
        builder.append(dot::local_date_util::to_iso_int(rhs));
    }

    inline void append(bsoncxx::builder::core& builder, dot::local_time rhs)
    {
        builder.append(dot::local_time_util::to_iso_int(rhs));
    }

    inline void append(bsoncxx::builder::core& builder, dot::local_date_time rhs)
    {
        builder.append(bsoncxx::types::b_date(dot::local_date_time_util::to_std_chrono(rhs)));
    }

    inline void append(bsoncxx::builder::core& builder, bsoncxx::oid rhs)
    {
        builder.append(rhs);
    }

    template <class T>
    inline void append(bsoncxx::builder::core& builder, T const& rhs)
    {
        bsoncxx::builder::basic::array arr;

        builder.open_array();
        for (auto item : rhs)
        {
            append(builder, item);
        }
        builder.close_array();
    }

    template <class T>
    inline void append(bsoncxx::builder::core& builder, dot::ptr<T> rhs)
    {
        bsoncxx::builder::basic::array arr;

        builder.open_array();
        for (auto item : rhs)
        {
            append(builder, item);
        }
        builder.close_array();
    }


    template <class Key>
    inline void append(bsoncxx::builder::core& builder, Key rhs, typename std::enable_if<std::is_base_of<key_base_impl, typename Key::element_type>::value >::type * p = 0)
    {
        append(builder, rhs->to_string());
    }

    template <class T>
    struct operator_wrapper
    {
        operator_wrapper(const std::string & prop_name, const std::string & op, T rhs)
            : prop_name_(prop_name), op_(op), rhs_(rhs)
        {}

        operator bsoncxx::document::view_or_value() const
        {
            bsoncxx::builder::core builder(false);
            builder.key_view(prop_name_);
            builder.open_document();
            builder.key_view(op_);
            append(builder, rhs_);
            builder.close_document();

            return builder.extract_document();
        }

    private:

        std::string prop_name_;
        std::string op_;
        T rhs_;
    };

    inline and_list operator &&(const bsoncxx::document::view_or_value & lhs, const bsoncxx::document::view_or_value & rhs)
    {
        and_list list;
        list.values_list_.push_back(lhs);
        list.values_list_.push_back(rhs);
        return list;
    }

    inline and_list operator &&(const bsoncxx::document::view_or_value & lhs, and_list rhs)
    {
        rhs.values_list_.push_back(lhs);
        return std::move(rhs);
    }

    inline and_list operator &&(and_list lhs, const bsoncxx::document::view_or_value & rhs)
    {
        lhs.values_list_.push_back(rhs);
        return lhs;
    }

    inline or_list operator ||(const bsoncxx::document::view_or_value & lhs, const bsoncxx::document::view_or_value & rhs)
    {
        or_list list;
        list.values_list_.push_back(lhs);
        list.values_list_.push_back(rhs);
        return list;
    }

    inline or_list operator ||(const bsoncxx::document::view_or_value & lhs, or_list rhs)
    {
        rhs.values_list_.push_back(lhs);
        return std::move(rhs);
    }

    inline or_list operator ||(or_list lhs, const bsoncxx::document::view_or_value & rhs)
    {
        lhs.values_list_.push_back(rhs);
        return lhs;
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
            static_assert(std::is_same<LastProp::element_type, ClassR>::value, "Wrong ->* sequence. Left operand doesn't have right property.");

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
        operator_wrapper<T> operator==(T rhs) const
        {
            return operator_wrapper<T>(get_name(), "$eq", rhs);
        }

        template <class T>
        operator_wrapper<T> operator!=(T rhs) const
        {
            return operator_wrapper<T>(get_name(), "$ne", rhs);
        }

        template <class T>
        operator_wrapper<T> operator<(T rhs) const
        {
            return operator_wrapper<T>(get_name(), "$lt", rhs);
        }

        template <class T>
        operator_wrapper<T> operator<=(T rhs) const
        {
            return operator_wrapper<T>(get_name(), "$lte", rhs);
        }

        template <class T>
        operator_wrapper<T> operator>(T rhs) const
        {
            return operator_wrapper<T>(get_name(), "$gt", rhs);
        }

        template <class T>
        operator_wrapper<T> operator>=(T rhs) const
        {
            return operator_wrapper<T>(get_name(), "$gte", rhs);
        }

        template <class T>
        operator_wrapper<T> in(T rhs)
        {
            return operator_wrapper<T>(get_name(), "$in", rhs);
        }

        template <class T>
        operator_wrapper<std::initializer_list<T>> in(std::initializer_list<T> const& rhs)
        {
            return operator_wrapper<std::initializer_list<T>>(get_name(), "$in", rhs);
        }


    private:

        std::string get_name() const
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
            static_assert(std::is_same<Prop::element_type, ClassR>::value, "Wrong ->* sequence. Left operand doesn't have right property.");
            return props_list<PropR>{{ prop_->name, rhs.prop_->name }};
        }

        auto operator[](int rhs)
        {
            using ReturnType = typename std::remove_reference<decltype(std::declval<Prop>().operator[](rhs))>::type;
            return props_list<ReturnType>{{ prop_->name, std::to_string(rhs) }};
        }

        template <class T>
        operator_wrapper<T> operator==(T rhs) const
        {
            return operator_wrapper<T>(*dot::string(prop_->name), "$eq", rhs);
        }

        template <class T>
        operator_wrapper<T> operator!=(T rhs) const
        {
            return operator_wrapper<T>(*dot::string(prop_->name), "$ne", rhs);
        }

        template <class T>
        operator_wrapper<T> operator<(T rhs) const
        {
            return operator_wrapper<T>(*dot::string(prop_->name), "$lt", rhs);
        }

        template <class T>
        operator_wrapper<T> operator<=(T rhs) const
        {
            return operator_wrapper<T>(*dot::string(prop_->name), "$lte", rhs);
        }

        template <class T>
        operator_wrapper<T> operator>(T rhs) const
        {
            return operator_wrapper<T>(*dot::string(prop_->name), "$gt", rhs);
        }

        template <class T>
        operator_wrapper<T> operator>=(T rhs) const
        {
            return operator_wrapper<T>(*dot::string(prop_->name), "$gte", rhs);
        }

        template <class T>
        operator_wrapper<T> in(T const& rhs)
        {
            return operator_wrapper<T>(*dot::string(prop_->name), "$in", rhs);
        }

        template <class T>
        operator_wrapper<std::initializer_list<T>> in(std::initializer_list<T> const& rhs)
        {
            return operator_wrapper<std::initializer_list<T>>(*dot::string(prop_->name), "$in", rhs);
        }

    };

    template <class Class>
    struct prop_wrapper<Class, dot::object_id>
    {
        operator_wrapper<bsoncxx::oid> operator==(dot::local_date_time rhs) const
        {
            char bytes[12] = { 0 };

            boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
            boost::posix_time::time_duration d = (ptime)rhs - epoch;
            int64_t seconds = d.total_seconds();
            std::memcpy(bytes, &seconds, sizeof(seconds));
            return operator_wrapper<bsoncxx::oid>("_id", "$eq", bsoncxx::oid(bytes, 12));
        }

        operator_wrapper<bsoncxx::oid> operator<(dot::local_date_time rhs) const
        {
            char bytes[12] = { 0 };

            boost::posix_time::ptime epoch(boost::gregorian::date(1970, boost::date_time::Jan, 1));
            boost::posix_time::time_duration d = (ptime)rhs - epoch;
            int64_t seconds = d.total_seconds();
            std::memcpy(bytes, &seconds, sizeof(seconds));
            return operator_wrapper<bsoncxx::oid>("_id", "$lt", bsoncxx::oid(bytes, 12));
        }

        operator_wrapper<bsoncxx::oid> operator==(bsoncxx::oid rhs) const
        {
            return operator_wrapper<bsoncxx::oid>("_id", "$eq", rhs);
        }

        operator_wrapper<bsoncxx::oid> operator<(bsoncxx::oid rhs) const
        {
            return operator_wrapper<bsoncxx::oid>("_id", "$lt", rhs);
        }

        operator_wrapper<bsoncxx::oid> operator==(dot::object_id rhs) const
        {
            return operator_wrapper<bsoncxx::oid>("_id", "$eq", rhs.id_);
        }

        operator_wrapper<bsoncxx::oid> operator<(dot::object_id rhs) const
        {
            return operator_wrapper<bsoncxx::oid>("_id", "$lt", rhs.id_);
        }

    };


    template <class Prop, class Class>
    prop_wrapper<Class, Prop> make_prop(Prop Class::*prop_)
    {
        dot::type_t type = dot::typeof<dot::ptr<Class>>();
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
