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

#include <dot/detail/traits.hpp>
#include <dot/detail/reflection_macro.hpp>
#include <dot/system/attribute.hpp>
#include <dot/system/object.hpp>
#include <dot/system/string.hpp>
#include <dot/system/nullable.hpp>
#include <dot/system/collections/generic/list.hpp>
#include <dot/system/reflection/constructor_info.hpp>
#include <dot/system/reflection/method_info.hpp>
#include <dot/system/reflection/parameter_info.hpp>
#include <dot/system/reflection/field_info.hpp>
#include <dot/noda_time/local_date.hpp>
#include <dot/noda_time/local_time.hpp>
#include <dot/noda_time/local_minute.hpp>
#include <dot/noda_time/local_date_time.hpp>

namespace dot
{
    class TypeImpl; using Type = Ptr<TypeImpl>;
    class TypeBuilderImpl; using TypeBuilder = Ptr<TypeBuilderImpl>;
    class StringImpl; class String;
    class MethodInfoImpl; using MethodInfo = Ptr<MethodInfoImpl>;
    class ConstructorInfoImpl; using constqructor_info = Ptr<ConstructorInfoImpl>;
    template <class T> class ListImpl; template <class T> using List = Ptr<ListImpl<T>>;
    template <class TClass, class ... Args> class MemberConstructorInfoImpl;

    template <class T> Type typeof();

    namespace detail
    {
        /// Struct for ParameterInfo.
        struct TypeMethodArgument
        {
            String name;
            List<Attribute> custom_attributes;

            /// Constructor from parameter name.
            TypeMethodArgument(const char* name)
                : name(name)
                , custom_attributes(make_list<Attribute>())
            {}

            /// Constructor from parameter name and custom attributes.
            template <class ...Args>
            TypeMethodArgument(String name, Args ...attr)
                : name(name)
                , custom_attributes(make_list<Attribute>({attr...}))
            {}
        };
    }

    template <class T>
    inline TypeBuilder make_type_builder(String nspace, String name, const std::initializer_list<Attribute>& custom_attributes = {});

    /// builder for Type.
    class DOT_CLASS TypeBuilderImpl final : public virtual ObjectImpl
    {
        template <class>
        friend TypeBuilder make_type_builder(String nspace, String name, const std::initializer_list<Attribute>& custom_attributes);
        friend class TypeImpl;

    private:
        String full_name_;
        List<MethodInfo> methods_;
        List<ConstructorInfo> ctors_;
        List<FieldInfo> fields_;
        Type type_;
        Type base_;
        List<Type> interfaces_;
        List<Type> generic_args_;
        List<Attribute> custom_attributes_;
        bool is_class_;
        bool is_enum_ = false;

    public: // METHODS

        /// Add public field of the current Type.
        template <class TClass, class fld>
        TypeBuilder with_field(String name, fld TClass::*prop, const std::initializer_list<Attribute>& custom_attributes = {})
        {
            if (fields_.is_empty())
            {
                fields_ = make_list<FieldInfo>();
            }
            fields_->add(make_field_info<fld, TClass>(name, type_, dot::typeof<fld>(), prop, make_list(custom_attributes)));
            return this;
        }

        /// Add public member method of the current Type.
        template <class TClass, class ReturnType, class ... Args>
        TypeBuilder with_method(String name, ReturnType(TClass::*mth) (Args ...), const std::initializer_list<detail::TypeMethodArgument>& arguments, const std::initializer_list<Attribute>& custom_attributes = {})
        {
            const int args_count = sizeof...(Args);
            if (args_count != arguments.size())
                throw Exception("Wrong number of parameters for method " + full_name_);

            if (methods_.is_empty())
            {
                methods_ = make_list<MethodInfo>();
            }

            List<ParameterInfo> parameters = make_list<ParameterInfo>(sizeof...(Args));
            std::vector<Type> param_types = { dot::typeof<Args>()... };

            int i = 0;
            for (detail::TypeMethodArgument mehtod_arg : arguments)
            {
                parameters[i] = make_parameter_info(mehtod_arg.name, param_types[i], i, mehtod_arg.custom_attributes);
                i++;
            }

            MethodInfo method_info = new MemberMethodInfoImpl<TClass, ReturnType, Args...>(name, type_, dot::typeof<ReturnType>(), mth, make_list(custom_attributes));
            method_info->parameters_ = parameters;

            methods_->add(method_info);

            return this;
        }

        /// Add public static method of the current Type.
        template <class ReturnType, class ... Args>
        TypeBuilder with_method(String name, ReturnType(*mth) (Args ...), const std::initializer_list<detail::TypeMethodArgument>& arguments, const std::initializer_list<Attribute>& custom_attributes = {})
        {
            const int args_count = sizeof...(Args);
            if (args_count != arguments.size())
                throw Exception("Wrong number of parameters for method " + full_name_);

            if (methods_.is_empty())
            {
                methods_ = make_list<MethodInfo>();
            }

            List<ParameterInfo> parameters = make_list<ParameterInfo>(sizeof...(Args));
            std::vector<Type> param_types = { dot::typeof<Args>()... };

            int i = 0;
            for (detail::TypeMethodArgument mehtod_arg : arguments)
            {
                parameters[i] = make_parameter_info(mehtod_arg.name, param_types[i], i, mehtod_arg.custom_attributes);
                i++;
            }

            MethodInfo method_info = new StaticMethodInfoImpl<ReturnType, Args...>(name, type_, dot::typeof<ReturnType>(), mth, make_list(custom_attributes));
            method_info->parameters_ = parameters;

            methods_->add(method_info);

            return this;
        }

        /// Add public constructor of the current Type.
        template <class TClass, class ... Args>
        TypeBuilder with_constructor(TClass(*ctor)(Args...), const std::initializer_list<detail::TypeMethodArgument>& arguments, const std::initializer_list<Attribute>& custom_attributes = {})
        {
            const int args_count = sizeof...(Args);
            if (args_count != arguments.size())
                throw Exception("Wrong number of parameters for method " + full_name_);

            if (ctors_.is_empty())
            {
                ctors_ = make_list<ConstructorInfo>();
            }

            List<ParameterInfo> parameters = make_list<ParameterInfo>(sizeof...(Args));
            std::vector<Type> param_types = { dot::typeof<Args>()... };

            int i = 0;
            for (detail::TypeMethodArgument mehtod_arg : arguments)
            {
                parameters[i] = make_parameter_info(mehtod_arg.name, param_types[i], i, mehtod_arg.custom_attributes);
                i++;
            }

            ConstructorInfo ctor_info = new MemberConstructorInfoImpl<TClass, Args...>(type_, ctor, make_list(custom_attributes));
            ctor_info->parameters_ = parameters;

            ctors_->add(ctor_info);

            return this;
        }

        /// Set is_enum to true.
        TypeBuilder is_enum()
        {
            this->is_class_ = false;
            this->is_enum_ = true;
            return this;
        }

        /// Add base Type of the current Type.
        template <class TClass>
        TypeBuilder with_base()
        {
            if (!(this->base_.is_empty()))
                throw Exception("Base already defined in class " + full_name_);

            this->base_ = dot::typeof<TClass>();
            return this;
        }

        /// Add interface Type of the current Type.
        template <class TClass>
        TypeBuilder with_interface()
        {
            if (this->interfaces_.is_empty())
                this->interfaces_ = make_list<Type>();

            this->interfaces_->add(dot::typeof<TClass>());
            return this;
        }

        /// Add generic argument Type of the current Type.
        template <class TClass>
        TypeBuilder with_generic_argument()
        {
            if (this->generic_args_.is_empty())
                this->generic_args_ = make_list<Type>();

            this->generic_args_->add(dot::typeof<TClass>());
            return this;
        }

        /// Build Type Object from this Type builder.
        Type build();

    private: // CONSTRUCTORS

        /// Create an empty instance of TypeBuilder.
        ///
        /// This constructor is private. Use make_type_builder() function instead.
        TypeBuilderImpl(String nspace, String name, String cppname);
    };

    /// Create an empty instance of TypeBuilder.
    template <class T>
    inline TypeBuilder make_type_builder(String nspace, String name, const std::initializer_list<Attribute>& custom_attributes)
    {
        TypeBuilder td = new TypeBuilderImpl(nspace, name, typeid(T).name());
        td->is_class_ = std::is_base_of<ObjectImpl, T>::value;
        td->custom_attributes_ = make_list(custom_attributes);
        return td;
    }

    /// Represents Type declarations: class types, interface types, array types, value types, enumeration types,
    /// Type parameters, generic Type definitions, and open or closed constructed generic types.
    ///
    /// Type is the root of the system.reflection functionality and is the primary way to access metadata.
    /// Use the members of Type to get information about a Type declaration, about the members of a Type
    /// (such as the constructors, methods, fields, properties, and events of a class), as well as the module
    /// and the assembly in which the class is deployed.
    ///
    /// The Type Object associated with a particular Type can be obtained in the following ways:
    ///
    /// \begin{itemize}
    ///
    /// \item The instance Object.get_type method returns a Type Object that represents the Type of an instance.
    /// Because all managed types derive from Object, the get_type method can be called on an instance of any Type.
    ///
    /// \item The typeof method obtains the Type Object for the argument Type.
    ///
    /// \end{itemize}
    class DOT_CLASS TypeImpl final : public virtual ObjectImpl
    {
        friend class TypeBuilderImpl;
        template <class T>
        friend struct type_traits;

        typedef TypeImpl self;

    private: // FIELDS

        String name_;
        String name_space_;
        bool is_class_;
        bool is_enum_;
        List<MethodInfo> methods_;
        List<ConstructorInfo> ctors_;
        List<Type> interfaces_;
        List<Type> generic_args_;
        Type base_;
        List<FieldInfo> fields_;
        List<Attribute> custom_attributes_;

    public: // PROPERTIES

        /// Gets the name of the current Type, excluding namespace.
        String name() const { return name_; }

        /// Gets the fully qualified name of the Type, including its namespace but not its assembly.
        String name_space() const { return name_space_; }

        /// Gets the fully qualified name of the Type, including its namespace but not its assembly.
        String full_name() const { return String::format("{0}.{1}", this->name_space(), this->name()); }

        /// Gets the base Type if current Type.
        Type get_base_type() { return base_; }

        /// Gets a value indicating whether the system.Type is a class or a delegate; that is, not a value Type or interface.
        bool is_class() const { return is_class_; }

        /// Gets a value indicating whether the current system.Type represents an enumeration.
        bool is_enum() const { return is_enum_; }

        /// Gets a collection that contains this member's custom attributes.
        List<Attribute> get_custom_attributes(bool inherit);

        /// Gets a collection that contains this member's custom attributes that are assignable to specified attribute Type.
        List<Attribute> get_custom_attributes(Type attribute_type, bool inherit);

    public: // METHODS

        /// Returns methods of the current Type.
        List<MethodInfo> get_methods() { return methods_; }

        /// Returns constructors of the current Type.
        List<ConstructorInfo> get_constructors() { return ctors_; }

        /// Returns fields of the current Type.
        List<FieldInfo> get_fields() { return fields_; }

        /// Returns interfaces of the current Type.
        List<Type> get_interfaces() { return interfaces_; }

        /// Returns interfaces of the current Type.
        List<Type> get_generic_arguments() { return generic_args_; }

        /// Searches for the public method with the specified name.
        MethodInfo get_method(String name);

        /// Searches for the interface with the specified name.
        Type get_interface(String name);

        /// Searches for the field with the specified name.
        FieldInfo get_field(String name);

        /// A String representing the name of the current Type.
        virtual String to_string() override { return full_name(); }

        /// Get Type Object for the name.
        static Type get_type_of(String name) { return get_type_map()[name]; }

        /// Get derived types list for the name.
        static List<Type> get_derived_types(String name) { return get_derived_types_map()[name]; }

        /// Get derived types list for the Type.
        static List<Type> get_derived_types(Type t) { return get_derived_types_map()[t->full_name()]; }

        /// Determines whether the current Type derives from the specified Type.
        bool is_subclass_of(Type c);

        /// Determines whether an instance of a specified Type can be assigned to a variable of the current Type.
        bool is_assignable_from(Type c);

        bool equals(Object obj) override;

        virtual size_t hash_code() override;

    private: // METHODS

        /// Fill data from builder.
        void fill(const TypeBuilder& data);

        static std::map<String, Type>& get_type_map()
        {
            static std::map<String, Type> map_;
            return map_;
        }

        static std::map<String, List<Type>>& get_derived_types_map()
        {
            static std::map<String, List<Type>> map_;
            return map_;
        }

    private: // CONSTRUCTORS

        /// Create from builder.
        ///
        /// This constructor is private. Use TypeBuilder->build() method instead.
        TypeImpl(String nspace, String name);
    };
}

#include <dot/system/reflection/method_info_impl.hpp>
#include <dot/system/reflection/constructor_info_impl.hpp>

namespace dot
{
    /// Initializes a new instance of the Type class for untyped instance of Object.
    inline Type ObjectImpl::get_type()
    {
        return typeof();
    }

    inline Type ObjectImpl::typeof()
    {
        static Type type_ = make_type_builder<ObjectImpl>("dot", "Object")->build();
        return type_;
    }

    template <class T> Type ListImpl<T>::typeof()
    {
        static Type type_ = make_type_builder<ListImpl<T>>("dot", "List`1")
            //DOT_TYPE_CTOR(make_list<T>)
            ->with_constructor(static_cast<List<T>(*)()>(&make_list<T>), { })
            DOT_TYPE_GENERIC_ARGUMENT(T)
            ->template with_interface<dot::ListBase>()
            ->build();
        return type_;
    }

    template <class T>
    struct type_traits
    {
        static Type typeof()
        {
            String cppname = typeid(typename T::element_type).name(); // TODO - is it faster to use typeid rather than String as key?
            auto p = TypeImpl::get_type_map().find(cppname);
            if (p == TypeImpl::get_type_map().end())
            {
                return T::element_type::typeof();
            }

            return p->second;
        }
    };

    template <>
    struct type_traits<double>
    {
        static Type typeof()
        {
            static Type type_ = make_type_builder<double>("dot", "double")->build();
            return type_;
        }
    };

    template <>
    struct type_traits<int64_t>
    {
        static Type typeof()
        {
            static Type type_ = make_type_builder<int64_t>("dot", "int64_t")->build();
            return type_;
        }
    };

    template <>
    struct type_traits<int>
    {
        static Type typeof()
        {
            static Type type_ = make_type_builder<int>("dot", "int")->build();
            return type_;
        }
    };

    template <>
    struct type_traits<void>
    {
        static Type typeof()
        {
            static Type type_ = make_type_builder<void>("dot", "void")->build();
            return type_;
        }
    };

    template <>
    struct type_traits<bool>
    {
        static Type typeof()
        {
            static Type type_ = make_type_builder<bool>("dot", "bool")->build();
            return type_;
        }
    };

    template <>
    struct type_traits<char>
    {
        static Type typeof()
        {
            static Type type_ = make_type_builder<char>("dot", "char")->build();
            return type_;
        }
    };

    template <>
    struct type_traits<LocalDate>
    {
        static Type typeof()
        {
            static Type type_ = make_type_builder<LocalDate>("dot", "LocalDate")->build();
            return type_;
        }
    };

    template <>
    struct type_traits<LocalTime>
    {
        static Type typeof()
        {
            static Type type_ = make_type_builder<LocalTime>("dot", "LocalTime")->build();
            return type_;
        }
    };

    template <>
    struct type_traits<LocalMinute>
    {
        static Type typeof()
        {
            static Type type_ = make_type_builder<LocalTime>("dot", "LocalMinute")->build();
            return type_;
        }
    };

    template <>
    struct type_traits<LocalDateTime>
    {
        static Type typeof()
        {
            static Type type_ = make_type_builder<LocalDateTime>("dot", "LocalDateTime")->build();
            return type_;
        }
    };

    template <>
    struct type_traits<Period>
    {
        static Type typeof()
        {
            static Type type_ = make_type_builder<LocalDateTime>("dot", "Period")->build();
            return type_;
        }
    };

    template <class T>
    struct type_traits<Nullable<T>>
    {
        static Type typeof()
        {
            static Type type_ = make_type_builder<Nullable<T>>("dot", "Nullable<" + dot::typeof<T>()->name() + ">")
                ->template with_generic_argument<T>()
                ->build();
            return type_;
        }
    };

    template <>
    struct type_traits<std::tuple<>>
    {
        static Type typeof()
        {
            static Type type_ = make_type_builder<std::tuple<>>("dot", "tuple<>")
                ->build();
            return type_;
        }
    };

    template <class ... T>
    struct type_traits<std::tuple<T...>>
    {
        static Type typeof()
        {
            static TypeBuilder builder =
            []()
            {
                TypeBuilder type_builder = make_type_builder<std::tuple<T...>>("dot", "tuple<" + get_name<T...>() + ">");
                set_generic_args<T ...>(type_builder);
                type_builder->with_method("get_item", &get_item, { "tuple", "index" })
                    ->with_method("set_item", &set_item, { "tuple", "index", "value" })
                    ->with_constructor(&contructor, {})
                    ;
                return type_builder;
            }();

            static Type type_ = builder->build();
            return type_;
        }

    private:

        static Object contructor()
        {
            return Object(std::tuple<T...>());
        }

        struct dummy{};

        template <int I, typename Dummy = dummy>
        struct get_item_impl
        {
            static Object impl(Object tuple, int index)
            {
                if (I == index) return std::get<I>(*(struct_wrapper<std::tuple<T...>>)tuple);
                    else return get_item_impl<I + 1>::impl(tuple, index);
            }
        };

        template <typename Dummy>
        struct get_item_impl<sizeof...(T), Dummy>
        {
            static Object impl(Object tuple, int index)
            {
                throw Exception("Tuple index out of bounds");
            }
        };

        static Object get_item(Object tuple, int index)
        {
            return get_item_impl<0>::impl(tuple, index);
        }

        template <int I, typename Dummy = dummy>
        struct set_item_impl
        {
            static void impl(Object tuple, int index, Object value)
            {
                if (I == index) std::get<I>(*(struct_wrapper<std::tuple<T...>>)tuple) = (std::tuple_element_t<I, std::tuple<T...>>)value;
                else set_item_impl<I + 1>::impl(tuple, index, value);
            }
        };

        template <typename Dummy>
        struct set_item_impl<sizeof...(T), Dummy>
        {
            static void impl(Object tuple, int index, Object value)
            {
                throw Exception("Tuple index out of bounds");
            }
        };

        static void set_item(Object tuple, int index, Object value)
        {
            set_item_impl<0>::impl(tuple, index, value);
        }


        template <class Head, class Second, class ... Tail>
        static String get_name()
        {
            return dot::typeof<Head>()->name() + get_name<Second, Tail...>();
        }

        template <class Head>
        static String get_name()
        {
            return dot::typeof<Head>()->name();
        }

        template <class Head, class Second, class ... Tail>
        static TypeBuilder set_generic_args(TypeBuilder tb)
        {
            return set_generic_args<Second, Tail ... >(tb->with_generic_argument<Head>());
        }

        template <class Head>
        static TypeBuilder set_generic_args(TypeBuilder tb)
        {
            return tb->with_generic_argument<Head>();
        }
    };

    /// Get Type Object for the argument.
    template <class T>
    Type typeof()
    {
        return type_traits<T>::typeof();
    }
}
