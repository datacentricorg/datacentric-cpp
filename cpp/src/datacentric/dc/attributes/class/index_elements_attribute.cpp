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
#include <dc/attributes/class/index_elements_attribute.hpp>
#include <dc/types/record/key.hpp>
#include <dc/types/record/record.hpp>

namespace dc
{
    index_elements_attribute_impl::index_elements_attribute_impl(dot::String definition)
        : definition_(definition)
    {}

    index_elements_attribute_impl::index_elements_attribute_impl(dot::String definition, dot::String name)
        : definition_(definition)
        , name_(name)
    {}

    dot::String index_elements_attribute_impl::get_definition()
    {
        return definition_;
    }

    dot::String index_elements_attribute_impl::get_neme()
    {
        return name_;
    }

    dot::Dictionary<dot::String, dot::String> index_elements_attribute_impl::get_attributes_dict(dot::Type record_type)
    {
        // The dictionary uses definition as key and name as value;
        // the name is the same as definition unless specified in
        // the attribute explicitly.
        dot::Dictionary<dot::String, dot::String> index_dict = dot::make_dictionary<dot::String, dot::String>();

        // Create a list of index definition strings for the class,
        // including index definitions of its base classes, eliminating
        // duplicate definitions.
        dot::Type class_type = record_type;
        while (!class_type->equals(dot::typeof<record>()) && !class_type->equals(dot::typeof<key>()))
        {
            if (class_type == nullptr)
                throw dot::Exception(dot::String::format(
                    "Data source cannot get collection for type {0} "
                    "because it is not derived from type Record.",
                    record_type->name()));

            // Get class attributes with inheritance
            dot::List<dot::Attribute> class_attributes = class_type->get_custom_attributes(dot::typeof<index_elements_attribute>(), true);
            for (dot::Attribute attr : class_attributes)
            {
                index_elements_attribute class_attribute = (index_elements_attribute) attr;
                dot::String definition = class_attribute->get_definition();
                dot::String name = class_attribute->get_neme();

                // Validate definition and specify default value for the name if null or empty
                if (dot::String::is_null_or_empty(definition))
                    throw dot::Exception("Empty index definition in IndexAttribute.");
                if (dot::String::is_null_or_empty(name)) name = definition;

                // Remove + prefix from definition if specified
                if (definition->starts_with("+")) definition = definition->substring(1, definition->length() - 1);

                dot::String existing_name;
                if (index_dict->try_get_value(definition, existing_name))
                {
                    // Already included, check that the name matches, error message otherwise
                    if (name != existing_name)
                        throw dot::Exception(dot::String::format(
                            "The same index definition {0} is provided with two different "
                            "custom index names {1} and {2} in the inheritance chain for class {3}.",
                            definition, name, existing_name, record_type->name()));
                }
                else
                {
                    // Not yet included, add
                    index_dict->add(definition, name);
                }
            }

            // Continue to base type
            class_type = class_type->get_base_type();
        }

        return index_dict;
    }

    dot::List<std::tuple<dot::String, int>> index_elements_attribute_impl::parse_definition(dot::String definition, dot::Type record_type)
    {
        dot::List<std::tuple<dot::String, int>> result = dot::make_list<std::tuple<dot::String, int>>();

        // Validation of the definition String
        if (dot::String::is_null_or_empty(definition))
            throw dot::Exception("Empty index definition String in IndexElements attribute.");
        if (definition->contains("+"))
            throw dot::Exception(dot::String::format(
                "Index definition String {0} in IndexElements contains "
                "one or more + tokens. Only - but not + tokens are permitted.", definition));

        // Parse comma separated index definition String into tokens
        // and iterate over each token
        dot::List<dot::String> tokens = definition->split(',');
        for (dot::String token : tokens)
        {
            // Trim leading and trailing whitespace from the token
            dot::String element_name = token->trim();

            // Set descending sort order if the token starts from -
            int sort_order = 0;
            if (element_name->starts_with("-"))
            {
                // Descending sort order
                sort_order = -1;

                // Remove leading - and trim any whitespace between - and element name
                element_name = element_name->substring(1)->trim_start();
            }
            else
            {
                // Ascending sort order
                sort_order = 1;
            }

            // Check that element name is not empty
            if (element_name->length() == 0)
                throw dot::Exception(dot::String::format(
                    "Empty element name in comma separated index definition String {0}.", definition));

            // Check that element name does not contain whitespace
            if (element_name->contains(" "))
                throw dot::Exception(dot::String::format(
                    "Element name {0} in comma separated index definition String {1} contains whitespace.",
                    element_name, definition));

            // Check that element is present in TRecord as public property with both getter and setter
            dot::FieldInfo property_info = record_type->get_field(element_name);
            if (property_info == nullptr)
                throw dot::Exception(dot::String::format(
                    "Property {0} not found in {1} or its parents, "
                    "or is not a public property with both getter and setter defined.",
                    element_name, record_type->name()));

            // Add element and its sort order to the result
            result->add({ element_name, sort_order });
        }

        if (result->get_length() == 0)
            throw dot::Exception(dot::String::format(
                "No index elements are found definition String {0}.", definition));

        return result;
    }

    dot::Type index_elements_attribute_impl::typeof()
    {
        static dot::Type result = []()->dot::Type
        {
            dot::Type t = dot::make_type_builder<index_elements_attribute_impl>("dc", "index_elements_attribute")
                ->build();
            return t;
        }();
        return result;
    }

    dot::Type index_elements_attribute_impl::get_type()
    {
        return typeof();
    }
}
