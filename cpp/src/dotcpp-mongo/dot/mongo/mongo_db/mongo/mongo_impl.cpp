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

#include <dot/mongo/precompiled.hpp>
#include <dot/mongo/implement.hpp>
#include <dot/mongo/mongo_db/mongo/client.hpp>
#include <dot/mongo/mongo_db/mongo/collection.hpp>
#include <dot/mongo/mongo_db/mongo/database.hpp>

#include <mongocxx/database.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <dot/mongo/serialization/bson_writer.hpp>
#include <dot/mongo/serialization/bson_record_serializer.hpp>

#include <dot/mongo/mongo_db/cursor/cursor_wrapper.hpp>
#include <dot/mongo/mongo_db/query/query_builder.hpp>

#include <dot/mongo/mongo_db/query/query.hpp>

#include <mongocxx/pipeline.hpp>
#include <bsoncxx/builder/basic/document.hpp>

#include <bsoncxx/json.hpp>

namespace dot
{

    class collection_inner : public collection_impl::collection_inner_base
    {
        friend class query_inner_impl;

    public:

        collection_inner(mongocxx::collection const& collection)
            : collection_(collection)
        {
        }

        virtual void insert_one(object obj) override
        {
            bson_record_serializer serializer = make_bson_record_serializer();
            bson_writer writer = make_bson_writer();
            serializer->serialize(writer, obj);

            collection_.insert_one(writer->view());
        }

    private:

        mongocxx::collection collection_;
    };

    class database_inner : public database_impl::database_inner_base
    {
    public:

        database_inner(mongocxx::database const& database)
            : database_(database)
        {
        }

        virtual collection get_collection(dot::string name) override
        {
            return new collection_impl( std::make_unique<collection_inner>(database_[*name]) );
        }

    private:

        mongocxx::database database_;
    };



    class client_inner : public client_impl::client_inner_base
    {
    public:

        client_inner(string uri)
        {
            static mongocxx::instance instance{};

            client_ = mongocxx::client(mongocxx::uri(*uri));
        }

    protected:

        virtual database get_database(dot::string name) override
        {
            return new database_impl(std::make_unique<database_inner>(client_[*name]) );
        }

        virtual void drop_database(dot::string name) override
        {
            client_[*name].drop();
        }

    private:

        mongocxx::client client_;

    };

    collection_impl::collection_impl(std::unique_ptr<collection_inner_base> && impl)
        : impl_(std::move(impl))
    {
    }

    void collection_impl::insert_one(object obj)
    {
        impl_->insert_one(obj);
    }

    collection database_impl::get_collection(dot::string name)
    {
        return impl_->get_collection(name);
    }

    database_impl::database_impl(std::unique_ptr<database_inner_base> && impl)
        : impl_(std::move(impl))
    {
    }

    client_impl::client_impl(string uri)
    {
        impl_ = std::make_unique<client_inner>(uri);
    }

    database client_impl::get_database(dot::string name)
    {
        return impl_->get_database(name);
    }

    void client_impl::drop_database(dot::string name)
    {
        impl_->drop_database(name);
    }


    class DOT_MONGO_CLASS iterator_inner : public iterator_inner_base
    {
    public:

        virtual object operator*() override
        {
            return f_(*iterator_);
        }

        virtual object operator*() const override
        {
            return f_(*iterator_);
        }

        virtual void operator++() override
        {
            iterator_++;
        }

        virtual bool operator!=(std::shared_ptr<iterator_inner_base> const& rhs) override
        {
            return iterator_ != std::dynamic_pointer_cast<iterator_inner>(rhs)->iterator_;
        }

        virtual bool operator==(std::shared_ptr<iterator_inner_base> const& rhs) override
        {
            return iterator_ == std::dynamic_pointer_cast<iterator_inner>(rhs)->iterator_;
        }

        iterator_inner(mongocxx::cursor::iterator iterator, std::function<dot::object(const bsoncxx::document::view&)> f)
            : iterator_(iterator)
            , f_(f)
        {
        }

        mongocxx::cursor::iterator iterator_;
        std::function<dot::object(const bsoncxx::document::view&)> f_;
    };

    class DOT_MONGO_CLASS object_cursor_wrapper_impl : public dot::object_cursor_wrapper_base_impl
    {
    public:

        /// Returns begin iterator of the underlying std::vector.
        iterator_wrappper<dot::object> begin()
        {
            return iterator_wrappper<dot::object>(std::make_shared<iterator_inner>(cursor_->begin(), f_));
        }

        /// Returns end iterator of the underlying std::vector.
        iterator_wrappper<dot::object> end()
        {
            return iterator_wrappper<dot::object>(std::make_shared<iterator_inner>(cursor_->end(), f_));
        }

        //template <class T>
        //cursor_wrapper<T> to_cursor_wrapper()
        //{
        //    std::function<dot::object(const bsoncxx::document::view&)> f = f_;
        //    return new cursor_wrapper_impl<T>(cursor_, [f](const bsoncxx::document::view& item)->T
        //        {
        //            return T(f(item));
        //        }
        //    );
        //}


        object_cursor_wrapper_impl(mongocxx::cursor && cursor, const std::function<dot::object(const bsoncxx::document::view&)>& f)
            : cursor_(std::make_shared<mongocxx::cursor>(std::move(cursor)))
            , f_(f)
        {
        }

        std::shared_ptr<mongocxx::cursor> cursor_;
        std::function<dot::object(const bsoncxx::document::view&)> f_;
    };

    using object_cursor_wrapper = ptr<object_cursor_wrapper_impl>;

    class DOT_MONGO_CLASS query_inner_impl : public query_impl::query_inner_base_impl
    {
    public:

        virtual void where(filter_token_base value) override
        {
            flush_sort();
            pipeline_.match(serialize_tokens(value));
        }

        virtual void group_by(dot::field_info key_selectors) override
        {
            flush_sort();

            bsoncxx::builder::basic::document gr;
            gr.append(bsoncxx::builder::basic::kvp("_id", "$" + std::string(*(key_selectors->name))));

            gr.append(bsoncxx::builder::basic::kvp("doc",
                bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("$first", "$$ROOT"))));
            pipeline_.group(gr.view());

            pipeline_.replace_root(bsoncxx::builder::basic::make_document(bsoncxx::builder::basic::kvp("newRoot", "$doc")));
        }

        virtual void sort_by(dot::field_info key_selector) override
        {
            if (!sort_.empty())
            {
                throw dot::exception("Attempt to sort_by while previous sort is not finished. Use then_by instead.");
            }

            sort_.push_back(std::make_pair(key_selector->name, 1));
        }

        virtual void sort_by_descending(dot::field_info key_selector) override
        {
            if (!sort_.empty())
            {
                throw dot::exception("Attempt to sort_by_descending while previous sort is not finished. Use then_by_descending instead.");
            }

            sort_.push_back(std::make_pair(key_selector->name, -1));
        }

        virtual void then_by(dot::field_info key_selector) override
        {
            sort_.push_back(std::make_pair(key_selector->name, 1));
        }

        virtual void then_by_descending(dot::field_info key_selector) override
        {
            sort_.push_back(std::make_pair(key_selector->name, -1));
        }

        virtual object_cursor_wrapper_base get_cursor() override
        {
            flush_sort();

            return new object_cursor_wrapper_impl(dynamic_cast<collection_inner*>(collection_->impl_.get())->collection_.aggregate(pipeline_),
                [](const bsoncxx::document::view& item)->dot::object
                {
                    bson_record_serializer serializer = make_bson_record_serializer();
                    object record = serializer->deserialize(item);

                    return record;
                }
                );
        }

        virtual object_cursor_wrapper_base select(dot::list<dot::field_info> props, dot::type element_type) override
        {
            flush_sort();

            bsoncxx::builder::basic::document selectList{};
            for (dot::field_info prop : props)
                selectList.append(bsoncxx::builder::basic::kvp((std::string&)*(dot::string) prop->name, 1));
            selectList.append(bsoncxx::builder::basic::kvp("_key", 1));

            pipeline_.project(selectList.view());

            return new object_cursor_wrapper_impl(dynamic_cast<collection_inner*>(collection_->impl_.get())->collection_.aggregate(pipeline_),
                [props, element_type](const bsoncxx::document::view& item)->dot::object
                {
                    bson_record_serializer serializer = make_bson_record_serializer();
                    dot::object record = serializer->deserialize_tuple(item, props, element_type);
                    return record;
                }
            );

        }

        virtual void limit(int32_t limit_size) override
        {
            flush_sort();
            pipeline_.limit(limit_size);
        }

    private:

        void flush_sort()
        {
            if (!sort_.empty())
            {
                bsoncxx::builder::basic::document sort_doc{};

                for (std::pair<string, int> const& item : sort_)
                {
                    sort_doc.append(bsoncxx::builder::basic::kvp(std::string(*(item.first)), item.second));
                }

                pipeline_.sort(sort_doc.view());
                sort_.clear();
            }
        }

        void append(bsoncxx::builder::core& builder, object value)
        {
            dot::type value_type = value->get_type();

            if (value_type->equals(dot::typeof<dot::string>()))
                builder.append(*(dot::string)value);
            else
            if (value_type->equals(dot::typeof<double>())) // ? TODO check dot::typeof<Double>() dot::typeof<NullableDouble>()
                builder.append((double)value);
            else
            if (value_type->equals(dot::typeof<bool>()))
                builder.append((bool)value);
            else
            if (value_type->equals(dot::typeof<int>()))
                builder.append((int)value);
            else
            if (value_type->equals(dot::typeof<int64_t>()))
                builder.append((int64_t)value);
            else
            if (value_type->equals(dot::typeof<dot::local_date>()))
                builder.append(dot::local_date_util::to_iso_int((dot::local_date)value));
            else
            if (value_type->equals(dot::typeof<dot::local_time>()))
                builder.append(dot::local_time_util::to_iso_int((dot::local_time)value));
            else
            if (value_type->equals(dot::typeof<dot::local_minute>()))
                builder.append(dot::local_minute_util::to_iso_int((dot::local_minute) value));
            else
            if (value_type->equals(dot::typeof<dot::local_date_time>()))
                builder.append(bsoncxx::types::b_date{ dot::local_date_time_util::to_std_chrono((dot::local_date_time)value) });
            else
            if (value_type->equals(dot::typeof<dot::object_id>()))
                builder.append(((dot::struct_wrapper<dot::object_id>)value)->oid());
            else
            if (value_type->is_enum)
                builder.append(*value->to_string());
            else
            if (value.is<dot::list_base>())
            {
                list_base l = value.as<list_base>();
                builder.open_array();
                for (int i = 0; i < l->get_length(); ++i)
                {
                    append(builder, l->get_item(i));
                }
                builder.close_array();
            }
            else
                throw dot::exception("Unknown query token");
        }

        bsoncxx::document::view_or_value serialize_tokens(filter_token_base value)
        {
            if (value.is<operator_wrapper>())
            {
                operator_wrapper wrapper = value.as<operator_wrapper>();
                bsoncxx::builder::core builder(false);
                builder.key_view(*(wrapper->prop_name_));
                builder.open_document();
                builder.key_view(*(wrapper->op_));
                append(builder, wrapper->rhs_);
                builder.close_document();

                return builder.extract_document();
            }
            else if (value.is<and_list>())
            {
                and_list list = value.as<and_list>();

                bsoncxx::builder::core builder(false);
                builder.key_view("$and");
                builder.open_array();
                for (filter_token_base const& item : list->values_list_)
                {
                    builder.append(serialize_tokens(item));
                }
                builder.close_array();

                return builder.extract_document();

            }
            else if (value.is<or_list>())
            {
                or_list list = value.as<or_list>();

                bsoncxx::builder::core builder(false);
                builder.key_view("$or");
                builder.open_array();
                for (filter_token_base const& item : list->values_list_)
                {
                    builder.append(serialize_tokens(item));
                }
                builder.close_array();

                return builder.extract_document();
            }
            else throw dot::exception("Unknown query token");
        }

    public:


        dot::collection collection_;
        std::deque<std::pair<string, int>> sort_;
        dot::type type_;
        dot::type element_type_;
        dot::list<dot::field_info> select_;

        mongocxx::pipeline pipeline_;
    };

    using query_inner = ptr<query_inner_impl>;

    query query_impl::where(filter_token_base value)
    {
        impl_->where(value);
        return this;
    }

    object_cursor_wrapper_base query_impl::get_cursor()
    {
        return impl_->get_cursor();
    }

    query query_impl::group_by(dot::field_info key_selector)
    {
        impl_->group_by(key_selector);
        return this;
    }

    query query_impl::sort_by(dot::field_info key_selector)
    {
        impl_->sort_by(key_selector);
        return this;
    }

    query query_impl::sort_by_descending(dot::field_info key_selector)
    {
        impl_->sort_by_descending(key_selector);
        return this;
    }

    query query_impl::then_by(dot::field_info key_selector)
    {
        impl_->then_by(key_selector);
        return this;
    }

    query query_impl::then_by_descending(dot::field_info key_selector)
    {
        impl_->then_by_descending(key_selector);
        return this;
    }

    object_cursor_wrapper_base query_impl::select(dot::list<dot::field_info> props, dot::type element_type)
    {
        return impl_->select(props, element_type);
        //this->element_type_ = element_type;
        //this->select_ = props;
        //return data_source_->load_by_query(this);
        return nullptr;
    }

    query query_impl::limit(int32_t limit_size)
    {
        impl_->limit(limit_size);
        return this;
    }

    query_impl::query_impl(dot::collection collection, dot::type type)
    {
        query_inner impl = new query_inner_impl;

        impl->collection_ = collection;
        impl->type_ = type;
        impl->element_type_ = type;

        impl_ = impl;
        type_ = type;
    }

}