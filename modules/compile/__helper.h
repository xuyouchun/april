
#ifndef __COMPILE_HELPER_H__
#define __COMPILE_HELPER_H__

namespace X_ROOT_NS::modules::compile {

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // Converts to eobject.
        template<typename etype_t> struct __to_eobject_t
        {
            static etype_t to_eobject(etype_t & eobject)
            {
                return eobject;
            }
        };

        // Converts to eobject for pointer.
        template<typename etype_t> struct __to_eobject_t<etype_t *>
        {
            typedef std::remove_pointer_t<etype_t> __etype_t;
            static __etype_t * to_eobject(etype_t & eobject)
            {
                return &eobject;
            }
        };

        template<typename etype_t>
        using __itype_t = typename std::remove_reference_t<std::remove_pointer_t<etype_t>>::itype_t;

        //-------- ---------- ---------- ---------- ----------

        // Fake ast node.
        template<typename _etype_t, ast_value_t _value, typename _itype_t = __itype_t<_etype_t>>
        class __fake_ast_node_t : public single_ast_node_t
                                , public eobject_ast_t<_itype_t>
        {
            typedef std::remove_reference_t<_etype_t> * etype_t;
            typedef std::remove_reference_t<_etype_t> __etype_t;

        public:

            // Constructor.
            __fake_ast_node_t() = default;
            __fake_ast_node_t(__etype_t && eobject, const code_unit_t * code_unit = nullptr)
                : __eobject(eobject)
            {
                this->code_unit = code_unit;
            }

            // Returns ast value.
            virtual ast_value_t value() const override
            {
                return (ast_value_t)_value;
            }

            // Returns eobject.
            virtual _itype_t to_eobject() override
            {
                return &__eobject;
            }

            __etype_t * operator -> () { return &__eobject; }
            const __etype_t * operator -> () const { return &__eobject; }

            __etype_t * operator & () { return &__eobject; }
            const __etype_t * operator & () const { return &__eobject; }

            __etype_t & operator * () { return __eobject; }
            const __etype_t & operator * () const { return __eobject; }

        private:
            __etype_t __eobject;
        };

        //-------- ---------- ---------- ---------- ----------

        // Fake ast node.
        template<typename _etype_t, ast_value_t _value, typename _itype_t>
        class __fake_ast_node_t<_etype_t *, _value, _itype_t>
                                : public single_ast_node_t
                                , public eobject_ast_t<_itype_t>
        {
            typedef _etype_t * etype_t;
            typedef _etype_t __etype_t;

        public:

            // Constructors.
            __fake_ast_node_t() = default;
            __fake_ast_node_t(__etype_t * eobject, const code_unit_t * code_unit = nullptr)
                : __eobject(eobject)
            {
                this->code_unit = code_unit;
            }

            // Returns ast value.
            virtual ast_value_t value() const override
            {
                return (ast_value_t)_value;
            }

            // Returns eobject.
            virtual _itype_t to_eobject() override
            {
                return __eobject;
            }

            __etype_t * operator -> () { return &__eobject; }
            const __etype_t * operator -> () const { return &__eobject; }

            __etype_t * operator & () { return &__eobject; }
            const __etype_t * operator & () const { return &__eobject; }

            __etype_t & operator * () { return __eobject; }
            const __etype_t & operator * () const { return __eobject; }

        private:
            __etype_t * __eobject;
        };

        //-------- ---------- ---------- ---------- ----------

        // Fake asts node.
        template<typename _etype_t, ast_value_t _value>
        class __fake_asts_node_t : public single_ast_node_t
                                 , public eobjects_ast_t<_etype_t>
        {
        public:
            typedef _etype_t etype_t;

            // Constructors.
            __fake_asts_node_t() = default;
            __fake_asts_node_t(
                std::initializer_list<etype_t> il, const code_unit_t * code_unit = nullptr)
                : __eobjects(il)
            {
                this->code_unit = code_unit;
            }

            // Returns ast value.
            virtual ast_value_t value() const override
            {
                return (ast_value_t)_value;
            }

            // Returns eobject count.
            virtual size_t eobject_count() override
            {
                return __eobjects.size();
            }

            // Returns eobject at specified index.
            virtual etype_t eobject_at(size_t index) override
            {
                return __eobjects[index];
            }

        private:
            al::svector_t<etype_t, 2> __eobjects;
        };
    }

    template<typename _etype_t, ast_value_t _value, typename _itype_t = __itype_t<_etype_t>>
    using fake_ast_node_t = __fake_ast_node_t<_etype_t, _value, _itype_t>;

    template<typename _etype_t, ast_value_t _value>
    using fake_asts_node_t = __fake_asts_node_t<_etype_t, _value>;

    // Converts to fake ast node.
    template<ast_value_t _value, typename _etype_t>
    auto to_fake_ast(_etype_t && eobject, memory_t * memory,
                                          const code_unit_t * code_unit = nullptr)
    {
        typedef std::remove_reference_t<_etype_t> etype_t;
        return memory_t::new_obj<fake_ast_node_t<etype_t, _value>>(
            memory, std::forward<etype_t>(eobject), code_unit
        );
    }

    ////////// ////////// ////////// ////////// //////////

    // Base class for building a ast node.
    template<typename _ast_node_t, typename _self_t>
    class x_ast_builder_base_t : public tast_builder_t<_ast_node_t>
    {
    public:
        typedef _self_t                     __self_t;
        typedef _ast_node_t                 __ast_node_t;
        typedef tast_builder_t<_ast_node_t> __base_t;

        // Constructor.
        template<typename ... args_t>
        x_ast_builder_base_t(ast_context_t & context, lang_ast_build_args_t & args)
            : tast_builder_t<_ast_node_t>(context, args)
        { }

    protected:
        typedef ast_builder_completed_args_t __completed_args_t;

        // Log message.
        template<typename ... args_t>
        void __log(args_t && ... args)
        {
            this->__context.logger.log(std::forward<args_t>(args) ...);
        }

        // Compileted.
        virtual void completed(__completed_args_t & args) override final
        {
            this->__node->code_unit = args.code_unit;
            this->on_completed(args);
            this->__node->on_commit();
        }

        virtual void on_completed(__completed_args_t & args) { }

        // Converts to sid.
        template<typename obj_t>
        sid_t __to_sid(const obj_t & obj)
        {
            return this->__get_spool().to_sid(_str(obj));
        }

        // Converts to name.
        template<typename obj_t>
        name_t __to_name(obj_t && obj)
        {
            return name_t(__to_sid(std::forward<obj_t>(obj)));
        }

        // Converts to a multi-name.
        mname_t * __to_mname(const string_t & s)
        {
            return mname_t::parse(to_mname_operate_context(__get_xpool()), s);
        }

        // Returns memory management.
        memory_t * __get_memory() const
        {
            return this->__context.compile_context.get_memory();
        }

        // Returns string pool.
        al::spool_t & __get_spool()
        {
            return this->__get_xpool().spool;
        }

        // Returns xpool.
        xpool_t & __get_xpool()
        {
            return this->__context.compile_context.global_context.xpool;
        }

        // Creates a new fake ast node.
        template<typename _etype_t, ast_value_t _value, typename ... args_t>
        fake_ast_node_t<_etype_t, _value> * __new_fake_ast(args_t && ... args)
        {
            return this->template __new_obj<fake_ast_node_t<_etype_t, _value>>(
                std::forward<args_t>(args) ...
            );
        }

        // Creates a new fake asts node.
        template<typename _etype_t, ast_value_t _value, typename ... args_t>
        fake_asts_node_t<_etype_t, _value> * __new_fake_asts(args_t && ... args)
        {
            return this->template __new_obj<fake_asts_node_t<_etype_t, _value>>(
                std::forward<args_t>(args) ...
            );
        }

        // Converts to a fake ast node.
        template<ast_value_t _value, typename _etype_t>
        auto __to_ast(_etype_t && eobject, const code_unit_t * code_unit = nullptr)
        {
            typedef std::remove_reference_t<_etype_t> etype_t;

            return to_fake_ast<_value, etype_t>(
                std::forward<etype_t>(eobject), __get_memory(), code_unit
            );
        }

        // Creates a new ast node.
        template<typename ast_node_t, typename ... args_t>
        ast_node_t * __new_ast(args_t && ... args)
        {
            return this->template __new_obj<ast_node_t>(
                this->__context, std::forward<args_t>(args) ...
            );
        }

        // Creates a new ast node and commits it.
        template<typename ast_node_t, typename ... args_t>
        ast_node_t * __new_ast_with_commit(args_t && ... args)
        {
            ast_node_t * node = __new_ast<ast_node_t>(std::forward<args_t>(args) ...);
            node->on_commit();

            return node;
        }

        // Returns ast context.
        ast_context_t & __get_context()
        {
            return this->__context;
        }

        // Combines code elements.
        code_element_t * __combine(code_element_t * from, code_element_t * to)
        {
            return combine(__get_memory(), from, to);
        }

        // Returns cache.
        template<typename cache_t> cache_t & __cache()
        {
            global_context_t & gctx = this->__context.compile_context.global_context;
            return *gctx.template from_cache<cache_t>(0);
        }

        // Returns language cache.
        template<typename cache_t> cache_t & __lang_cache()
        {
            global_context_t & gctx = this->__context.compile_context.global_context;
            lang_id_t lang_id = this->__context.lang->get_id();

            return *gctx.template from_cache<cache_t>(lang_id);
        }

        // Returns value of specified language and key.
        template<typename cache_t, typename load_func_t,
            typename key_t   = typename cache_t::key_t,
            typename value_t = typename cache_t::value_t
        >
        value_t __lang_cache_of(const key_t & key, load_func_t load_func)
        {
            return __lang_cache<cache_t>().get(key, std::function<value_t ()>(load_func));
        }

        // Returns value of specified language and key.
        // Auto loads when not found.
        template<typename cache_t, typename load_func_t,
            typename key_t   = typename cache_t::key_t,
            typename value_t = typename cache_t::value_t
        >
        value_t __cache_of(const key_t & key, load_func_t load_func)
        {
            return __cache<cache_t>().get(key, std::function<value_t ()>(load_func));
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // A simple ast build element list.
    class array_ast_build_elements_t : public ast_build_elements_t
    {
    public:

        array_ast_build_elements_t() = default;

        array_ast_build_elements_t(const std::initializer_list<analyze_element_t> & elements)
        {
            for (const analyze_element_t & element : elements)
            {
                __elements.push_back(element);
            }
        }

        // Appends an element.
        void append(const analyze_element_t & element)
        {
            __elements.push_back(element);
        }

        template<size_t _n>
        void append_list(const analyze_element_t (&elements)[_n])
        {
            for (const analyze_element_t & element : elements)
            {
                __elements.push_back(element);
            }
        }
        
        // Reads next element.
        virtual analyze_element_t * next() override
        {
            if (__index >= __elements.size())
                return nullptr;

            return std::addressof(__elements[__index++]);
        }

    private:
        std::vector<analyze_element_t> __elements;
        size_t __index = 0;
    };

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::compile

#endif // __COMPILE_HELPER_H__

