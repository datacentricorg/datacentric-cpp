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

#include <dot/precompiled.hpp>
#include <dot/implement.hpp>
#include <dot/system/type.hpp>
#include <dot/system/object_impl.hpp>
#include <dot/system/reflection/method_info.hpp>
#include <dot/system/reflection/constructor_info.hpp>
#include <dot/system/collections/generic/list.hpp>
#include <dot/system/string.hpp>

namespace dot
{
    TypeBuilderImpl::TypeBuilderImpl(String name_space, String name, String cpp_name)
        : full_name_(name_space + "." + name)
    {
        type_ = new TypeImpl(name_space, name);
        TypeImpl::get_type_map()[full_name_] = type_;
        TypeImpl::get_type_map()[name] = type_;
        TypeImpl::get_type_map()[cpp_name] = type_;
    }

    Type TypeBuilderImpl::build()
    {
        type_->fill(this);

        // Fill derived types map
        Type base_type = base_;
        while (base_type != nullptr)
        {
            auto iter = TypeImpl::get_derived_types_map().find(base_type->full_name());
            if (iter == TypeImpl::get_derived_types_map().end())
            {
                iter = TypeImpl::get_derived_types_map().insert({base_type->full_name(), make_list<Type>()}).first;
            }
            else if (iter->second == nullptr)
            {
                iter->second = make_list<Type>();
            }
            iter->second->add(type_);
            base_type = base_type->get_base_type();
        }

        return type_;
    }

    void TypeImpl::fill(const TypeBuilder& data)
    {
        if (!data->base_.is_empty() && !data->base_->get_methods().is_empty() && data->base_->get_methods()->count())
        {
            if (data->methods_.is_empty())
            {
                data->methods_ = make_list<MethodInfo>();
            }

            list<MethodInfo> base_methods = data->base_->get_methods();
            list<MethodInfo> make_methods = make_list<MethodInfo>();
            for (MethodInfo meth_info_data : base_methods)
            {
                make_methods->add(meth_info_data);
            }

            for (MethodInfo meth_info_data : data->methods_)
            {
                auto iter = std::find_if(make_methods->begin(), make_methods->end(), [&meth_info_data](MethodInfo mi)
                {
                    if (mi->name() == meth_info_data->name())
                        return true;
                    return false;
                });

                if (iter != make_methods->end())
                    *iter = meth_info_data;
                else
                    make_methods->add(meth_info_data);
            }

            data->methods_ = make_methods;
        }

        if (!data->methods_.is_empty())
        {
            this->methods_ = make_list<MethodInfo>(data->methods_->count());
            int i = 0;
            for (MethodInfo meth_info_data : data->methods_)
            {
                this->methods_[i++] = meth_info_data;
            }
        }
        else
        {
            this->methods_ = make_list<MethodInfo>(0);
        }

        if (!data->ctors_.is_empty())
        {
            this->ctors_ = make_list<ConstructorInfo>(data->ctors_->count());
            int i = 0;
            for (ConstructorInfo ctor_info_data : data->ctors_)
            {
                this->ctors_[i++] = ctor_info_data;
            }
        }
        else
        {
            this->ctors_ = make_list<ConstructorInfo>(0);
        }

        if (!data->base_.is_empty() && !data->base_->get_fields().is_empty() && data->base_->get_fields()->count())
        {
            if (data->fields_.is_empty())
            {
                data->fields_ = make_list<FieldInfo>();
            }

            list<FieldInfo> base_fields = data->base_->get_fields();
            list<FieldInfo> make_fields = make_list<FieldInfo>();
            for (FieldInfo field_info_data : base_fields)
            {
                make_fields->add(field_info_data);
            }

            for (FieldInfo field_info_data : data->fields_)
            {
                auto iter = std::find_if(make_fields->begin(), make_fields->end(), [&field_info_data](FieldInfo fi)
                {
                    if (fi->name() == field_info_data->name())
                        return true;
                    return false;
                });

                if (iter != make_fields->end())
                    *iter = field_info_data;
                else
                    make_fields->add(field_info_data);
            }

            data->fields_ = make_fields;
        }

        if (!data->fields_.is_empty())
        {
            this->fields_ = make_list<FieldInfo>(data->fields_->count());
            int i = 0;
            for (FieldInfo ctor_info_data : data->fields_)
            {
                this->fields_[i++] = ctor_info_data;
            }
        }
        else
        {
            this->fields_ = make_list<FieldInfo>(0);
        }

        if (!data->interfaces_.is_empty())
        {
            this->interfaces_ = make_list<Type>(data->interfaces_->count());
            int i = 0;
            for (Type interface : data->interfaces_)
            {
                this->interfaces_[i++] = interface;
            }
        }
        else
        {
            this->interfaces_ = make_list<Type>(0);
        }

        if (!data->generic_args_.is_empty())
        {
            this->generic_args_ = make_list<Type>(data->generic_args_->count());
            int i = 0;
            for (Type arg : data->generic_args_)
            {
                this->generic_args_[i++] = arg;
            }
        }
        else
        {
            this->generic_args_ = make_list<Type>(0);
        }

        if (!data->custom_attributes_.is_empty())
        {
            this->custom_attributes_ = make_list<Attribute>(data->custom_attributes_->count());
            int i = 0;
            for (Attribute arg : data->custom_attributes_)
            {
                this->custom_attributes_[i++] = arg;
            }
        }
        else
        {
            this->custom_attributes_ = make_list<Attribute>(0);
        }

        this->base_ = data->base_;
        this->is_class_ = data->is_class_;
        this->is_enum_ = data->is_enum_;
    }

    TypeImpl::TypeImpl(String nspace, String name)
        : name_space_(nspace)
        , name_(name)
    {}

    list<Attribute> TypeImpl::get_custom_attributes(bool inherit)
    {
        if (!inherit)
        {
            return custom_attributes_;
        }

        list<Attribute> ret;
        if (base_.is_empty())
        {
            ret = make_list<Attribute>();
        }
        else
        {
            ret = base_->get_custom_attributes(true);
        }

        for (Attribute item : custom_attributes_)
        {
            ret->add(item);
        }

        return ret;
    }

    list<Attribute> TypeImpl::get_custom_attributes(Type attribute_type, bool inherit)
    {
        list<Attribute> ret = make_list<Attribute>();

        for (Attribute item : get_custom_attributes(inherit))
        {
            if (item->get_type()->equals(attribute_type))
                ret->add(item);
        }
        return ret;
    }

    MethodInfo TypeImpl::get_method(String name)
    {
        if (methods_.is_empty()) return nullptr;

        for (auto method : methods_)
        {
            if (method->name() == name)
                return method;
        }

        return nullptr;
    }

    Type TypeImpl::get_interface(String name)
    {
        if (interfaces_.is_empty()) return nullptr;

        for (auto interface : interfaces_)
        {
            if (interface->name() == name)
                return interface;
        }

        return nullptr;
    }

    FieldInfo TypeImpl::get_field(String name)
    {
        if (fields_.is_empty()) return nullptr;

        for (auto field : fields_)
        {
            if (field->name() == name)
                return field;
        }

        return nullptr;

    }

    bool TypeImpl::is_subclass_of(Type c)
    {
        // Search for c type within base types
        Type base = get_base_type();

        while (base != nullptr)
        {
            if (base->equals(c))
                return true;
            base = base->get_base_type();
        }

        return false;
    }

    bool TypeImpl::is_assignable_from(Type c)
    {
        if (c == nullptr)
            return false;

        if (c->equals(this) || c->is_subclass_of(this))
            return true;

        if (c->get_interfaces() != nullptr)
        {
            for (Type intf : c->get_interfaces())
            {
                if (intf->equals(this))
                    return true;
            }
        }

        return false;
    }

    bool TypeImpl::equals(Object obj)
    {
        if (obj.is<Type>())
            return this->full_name() == ((Type)obj)->full_name();

        return false;
    }

    size_t TypeImpl::hash_code()
    {
        return this->full_name()->hash_code();
    }

    list<Attribute> MemberInfoImpl::get_custom_attributes(dot::Type attr_type, bool)
    {
        list<Attribute> attrs = dot::make_list<Attribute>();
        for (Attribute item : custom_attributes_)
        {
            if (item->get_type()->equals(attr_type))
            {
                attrs->add(item);
            }
        }
        return attrs;
    }
}
