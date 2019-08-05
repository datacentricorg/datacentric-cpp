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


#include <dot/implement.hpp>
#include <dot/system/type.hpp>
#include <dot/system/objectimpl.hpp>
#include <dot/system/reflection/MethodInfo.hpp>
#include <dot/system/reflection/ConstructorInfo.hpp>
#include <dot/system/array1d.hpp>
#include <dot/system/collections/generic/list.hpp>
#include <dot/system/string.hpp>

namespace dot
{
    /// <summary>Built type_t from the current object.</summary>
    type_t type_builder_impl::build()
    {
        type_->fill(this);

        // Fill derived types map
        type_t base_type = base_;
        while (base_type != nullptr)
        {
            auto iter = type_impl::get_derived_types_map().find(base_type->full_name());
            if (iter == type_impl::get_derived_types_map().end())
            {
                iter = type_impl::get_derived_types_map().insert({base_type->full_name(), make_list<type_t>()}).first;
            }
            else if (iter->second == nullptr)
            {
                iter->second = make_list<type_t>();
            }
            iter->second->add(type_);
            base_type = base_type->base_type();
        }

        return type_;
    }

    /// <summary>
    /// Fill data from builder.
    /// </summary>
    void type_impl::fill(const type_builder& data)
    {
        if (!data->base_.IsEmpty() && data->base_->get_methods()->count())
        {
            if (data->methods_.IsEmpty())
            {
                data->methods_ = make_list<method_info>();
            }

            array<method_info> base_methods = data->base_->get_methods();
            list<method_info> new_methods = make_list<method_info>();
            for (method_info meth_info_data : base_methods)
            {

                new_methods->add(meth_info_data);
            }

            for (method_info meth_info_data : data->methods_)
            {
                new_methods->add(meth_info_data);
            }

            data->methods_ = new_methods;
        }

        if (!data->methods_.IsEmpty())
        {
            this->methods_ = make_array<method_info>(data->methods_->count());
            int i = 0;
            for (method_info meth_info_data : data->methods_)
            {
                this->methods_[i++] = meth_info_data;
            }
        }
        else
            this->methods_ = make_array<method_info>(0);

        if (!data->ctors_.IsEmpty())
        {
            this->ctors_ = make_array<constructor_info>(data->ctors_->count());
            int i = 0;
            for (constructor_info ctor_info_data : data->ctors_)
            {
                this->ctors_[i++] = ctor_info_data;
            }
        }
        else
            this->ctors_ = make_array<constructor_info>(0);

        if (!data->interfaces_.IsEmpty())
        {
            this->interfaces_ = make_array<type_t>(data->interfaces_->count());
            int i = 0;
            for (type_t interface : data->interfaces_)
            {
                this->interfaces_[i++] = interface;
            }
        }
        else
            this->interfaces_ = make_array<type_t>(0);

        if (!data->generic_args_.IsEmpty())
        {
            this->generic_args_ = make_array<type_t>(data->generic_args_->count());
            int i = 0;
            for (type_t arg : data->generic_args_)
            {
                this->generic_args_[i++] = arg;
            }
        }
        else
            this->generic_args_ = make_array<type_t>(0);

        this->base_ = data->base_;
        this->is_class = data->is_class_;
        this->is_enum = data->is_enum_;
    }

    /// <summary>
    /// Create from builder.
    ///
    /// This constructor is private. Use type_builder->Build() method instead.
    /// </summary>
    type_impl::type_impl(string nspace, string name)
    {
        this->name_space = nspace;
        this->name = name;
    }


    type_builder_impl::type_builder_impl(string name_space, string name, string cpp_name)
        : full_name_(name_space + "." + name)
    {
        type_ = new type_impl(name_space, name);
        type_impl::get_type_map()[full_name_] = type_;
        type_impl::get_type_map()[name] = type_;
        type_impl::get_type_map()[cpp_name] = type_;
    }

    method_info type_impl::get_method(string name)
    {
        if (methods_.IsEmpty()) return nullptr;

        for (auto method : methods_)
        {
            if (method->name == name)
                return method;
        }

        return nullptr;
    }

    type_t type_impl::get_interface(string name)
    {
        if (interfaces_.IsEmpty()) return nullptr;

        for (auto interface : interfaces_)
        {
            if (interface->name == name)
                return interface;
        }

        return nullptr;
    }

    bool type_impl::equals(object obj)
    {
        if (obj.is<type_t>())
            return this->full_name() == ((type_t)obj)->full_name();

        return false;
    }

    size_t type_impl::hash_code()
    {
        return this->full_name()->hash_code();
    }
}
