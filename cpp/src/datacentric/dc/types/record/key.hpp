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

#include <dc/declare.hpp>
#include <dc/types/record/data.hpp>
#include <dot/serialization/deserialize_attribute.hpp>

namespace dc
{
    class key_impl; using key = dot::ptr<key_impl>;
    class data_impl; using data = dot::ptr<data_impl>;

    /// Base class of a foreign key.
    ///
    /// The curiously recurring template pattern (CRTP) key class
    /// Key(TKey,TRecord) is derived from this class.
    ///
    /// Any elements of defined in the type specific key record
    /// become key tokens. Property Value and method ToString() of
    /// the key consists of key tokens with semicolon delimiter.
    class DC_CLASS key_impl : public data_impl
    {
        typedef key_impl self;

    public:

        /// String key consists of semicolon delimited primary key elements:
        ///
        /// KeyElement1;KeyElement2
        ///
        /// To avoid serialization format uncertainty, key elements
        /// can have any atomic type except Double.
        dot::string get_value()
        {
            return to_string();
        }

    public: // METHODS

        /// String key consists of semicolon delimited primary key elements:
        ///
        /// KeyElement1;KeyElement2
        ///
        /// To avoid serialization format uncertainty, key elements
        /// can have any atomic type except Double.
        dot::string to_string();

        /// Populate key elements from semicolon delimited string.
        /// Elements that are themselves keys may use more than
        /// one token.
        ///
        /// If key AKey has two elements, B and C, where
        ///
        /// * B has type BKey which has two string elements, and
        /// * C has type string,
        ///
        /// the semicolon delimited key has the following format:
        ///
        /// BToken1;BToken2;CToken
        ///
        /// To avoid serialization format uncertainty, key elements
        /// can have any atomic type except Double.
        void populate_from(dot::string value);

        temporal_id _id;

    private:

        /// Populate key elements from string stream.
        void populate_from(std::stringstream & value);

        /// Custom deserializator for deserialize_attribute for key type.
        static dot::object deserialize(dot::object value, dot::type type);

        /// Custom serializator for serialize_attribute for key type.
        static void serialize(dot::tree_writer_base writer, dot::object obj);

    public: // REFLECTION

        virtual dot::type get_type() { return typeof(); }

        static dot::type typeof()
        {
            static dot::type result = []()-> dot::type
            {
                dot::type t = dot::make_type_builder<self>("dc", "key", { dot::make_deserialize_class_attribute(&key_impl::deserialize)
                    , dot::make_serialize_class_attribute(&key_impl::serialize) })
                          ->with_method("assign_string", static_cast<void (key_impl::*)(dot::string)>(&key_impl::populate_from), {"value"})
                          ->build();
                return t;
            }();
            return result;
        }
    };
}
