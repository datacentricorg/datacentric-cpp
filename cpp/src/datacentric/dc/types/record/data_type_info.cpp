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

#include <dc/precompiled.hpp>
#include <dc/implement.hpp>
#include <dc/types/record/data_type_info.hpp>
#include <dc/types/record/key.hpp>
#include <dc/types/record/record.hpp>

namespace dc
{
    dot::String DataTypeInfoImpl::get_collection_name()
    {
        if (data_kind_ != DataKindEnum::record && data_kind_ != DataKindEnum::key)
            throw dot::Exception(dot::String::format(
                "get_collection_name() method is called for {0} "
                "that is not derived from TypedRecord.", type_->name()));

        return root_data_type_->name();
    }

    DataTypeInfo DataTypeInfoImpl::get_or_create(dot::Object value)
    {
        return get_or_create(value->get_type());
    }

    DataTypeInfo DataTypeInfoImpl::get_or_create(dot::Type value)
    {
        dot::Dictionary<dot::Type, DataTypeInfo> dict_ = DataTypeInfoImpl::get_type_dict();

        // Check if a cached instance exists in dictionary
        DataTypeInfo result;
        if (dict_->try_get_value(value, result))
        {
            // Return if found
            return result;
        }
        else
        {
            // Otherwise create and add to dictionary
            result = new DataTypeInfoImpl(value);
            dict_->add(value, result);
            return result;
        }
    }

    DataTypeInfoImpl::DataTypeInfoImpl(dot::Type value)
    {
        type_ = value;

        // Populate the inheritance chain from parent to base,
        // stop when one of the base classes is reached or
        // there is no base class
        dot::List<dot::Type> inheritance_chain = dot::make_list<dot::Type>();
        dot::Type current_type = value;

        while (current_type->get_base_type() != nullptr)
        {
            // Add type to the inheritance chain
            inheritance_chain->add(current_type);

            dot::Type base_type = current_type->get_base_type();
            if (base_type->equals(dot::typeof<Data>()))
            {
                if (root_type_ == nullptr)
                {
                    data_kind_ = DataKindEnum::element;
                    root_type_ = current_type;
                }
            }
            else if (base_type->equals(dot::typeof<Key>()))
            {
                if (root_type_ == nullptr)
                {
                    data_kind_ = DataKindEnum::key;
                    root_type_ = current_type->get_generic_arguments()[0];
                    root_key_type_ = current_type->get_generic_arguments()[0];
                    root_data_type_ = current_type->get_generic_arguments()[1];

                    if (inheritance_chain->count() > 2)
                        throw dot::Exception(dot::String::format(
                            "Key Type {0} must be derived directly from TypedKey<TKey, TRecord> and sealed "
                            "because key classes cannot have an inheritance hierarchy, only Data classes can.",
                            value->name()));
                }
            }
            else if (base_type->equals(dot::typeof<Record>()))
            {
                if (root_type_ == nullptr)
                {
                    data_kind_ = DataKindEnum::record;
                    root_type_ = current_type->get_generic_arguments()[1];
                    root_key_type_ = current_type->get_generic_arguments()[0];
                    root_data_type_ = current_type->get_generic_arguments()[1];
                }
            }

            current_type = current_type->get_base_type();
        }

        // Add the root class to the inheritance chain
        if (current_type != nullptr)
        {
            inheritance_chain->add(current_type);
        }

        // Error message if the type is not derived from one of the permitted base classes
        if (data_kind_ == DataKindEnum::empty)
            throw dot::Exception(dot::String::format(
                "Data Type {0} is not derived from Data, TypedKey<TKey, TRecord>, or TypedRecord<TKey, TRecord>.", value->name()));

        inheritance_chain_ = dot::make_list<dot::String>();
        for (dot::Type t : inheritance_chain)
            inheritance_chain_->add(t->name());
    }

    dot::Dictionary<dot::Type, DataTypeInfo>& DataTypeInfoImpl::get_type_dict()
    {
        static dot::Dictionary<dot::Type, DataTypeInfo> dict_ = dot::make_dictionary<dot::Type, DataTypeInfo>();
        return dict_;
    }
}
