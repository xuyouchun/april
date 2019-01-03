#include <rt.h>

namespace X_ROOT_NS { namespace modules { namespace rt {

    using namespace core;

    typedef rt_error_code_t __e_t;
    template<__tidx_t tidx> using __entity_t = rt_entity_t<tidx>;

    #define __AlwaysInline  X_ALWAYS_INLINE

    ////////// ////////// ////////// ////////// //////////

    #define __FormatError(s) _E(__e_t::assembly_format_error, s)

    ////////// ////////// ////////// ////////// //////////

    template<typename loader_t, __lv_t lv, __lv_t rlv = lv>
    class __rt_assembly_loader_super_t { };

    //-------- ---------- ---------- ---------- ----------

    struct __mt_template_t
    {
        template<__tidx_t tidx> using mt_t = rt_mt_t<tidx>;
    };

    ////////// ////////// ////////// ////////// //////////

    template<typename loader_t, __lv_t rlv>
    class __rt_assembly_loader_super_t<loader_t, 0, rlv>
            : public assembly_layout_t<rt_spool_t, rlv, __mt_template_t>
    {
        typedef __rt_assembly_loader_super_t<loader_t, 0, rlv> __self_t;
        typedef assembly_layout_t<rt_spool_t, rlv, __mt_template_t> __super_t;

        template<__tidx_t tidx>
        using __mt_t = typename __mt_template_t::template mt_t<tidx>;

    public:

        template<__tidx_t tidx> using mt_t = __mt_t<tidx>;

        __rt_assembly_loader_super_t(rt_context_t & ctx, xistream_t & stream,
                rt_assembly_t & assembly, rt_assembly_ref_loader_t * ref_loader)
            : __super_t(ctx.spool), __stream(stream), __res_reader(this->__heap, ctx.spool)
            , __ctx(ctx), __assembly(assembly), __ref_loader(ref_loader)
        {
            __init_mt_items();
        }

        void load()
        {
            read_metadata<rlv, __table_count, __mt_template_t>(__stream, this->__mt_objects, __heap);
            __read_assembly();
            __init_search_datas();
        }

        rt_method_t * get_entry_point()
        {
            return __entry_point;
        }

        rt_sid_t to_sid(res_t res)
        {
            return __to_sid(res);
        }

        rt_bytes_t read_bytes(res_t res)
        {
            if(res == res_t::null)
                return rt_bytes_t();

            size_t length;
            const byte_t * bytes = this->__heap.read_block(res, &length);
            return rt_bytes_t { bytes, length };
        }

        template<__tidx_t tidx> __entity_t<tidx> * entity_at(ref_t ref)
        {
            return __entity_at<tidx>(ref);
        }

        template<__tidx_t tidx> rt_mt_t<tidx> & metadata_at(ref_t ref)
        {
            return __metadata_at<tidx>(ref.index);
        }

        rt_general_type_t * get_type(rt_sid_t ns, rt_sid_t name, int generic_param_count)
        {
            auto it = __general_type_map.find(__general_type_key_t(ns, name, generic_param_count));
            if(it == __general_type_map.end())
                return nullptr;

            return __entity_at<__tidx_t::type>(**it);
        }

        template<__tidx_t tidx> ref_t get_ref(__entity_t<tidx> * entity)
        {
            // TODO
            return ref_t(0);
        }

        template<typename itor_t, typename cmp_t>
        bool __binary_find(itor_t begin, itor_t end, itor_t * out_r, cmp_t f)
        {
            if(begin == end)
                return false;

            itor_t it = begin + (end - begin) / 2;
            auto r = f(**it);

            if(r < 0)
                return __binary_find(begin, it, out_r, f);

            if(r > 0)
                return __binary_find(it + 1, end, out_r, f);

            al::assign_value(out_r, it);
            return true;
        }

        template<typename itor_t, typename cmp_t>
        itor_t binary_find(itor_t begin, itor_t end, cmp_t f)
        {
            byte_t r[sizeof(itor_t)];
            return __binary_find(begin, end, (itor_t *)r, f)? *(itor_t *)r : end;
        }

        template<typename container_t, typename cmp_t>
        auto binary_find(const container_t & container, cmp_t f)
        {
            return binary_find(std::begin(container), std::end(container), f);
        }

        template<typename cmp_t> rt_general_type_t * search_host(cmp_t f)
        {
            auto & type_mgr = __mt_manager<__tidx_t::type>();
            typedef rt_mt_t<__tidx_t::type> mt_t;
            auto it = binary_find(type_mgr, f);

            if(it == type_mgr.end())
                return nullptr;

            return __entity_at<__tidx_t::type>(**it);
        }

        template<__tidx_t tidx> mt_manager_t<tidx, __mt_t<tidx>> & mt_manager()
        {
            return __mt_manager<tidx>();
        }

    private:
        static const size_t __table_count = __self_t::value_of(__ln_t::table_count);

        rt_context_t  & __ctx;
        xistream_t    & __stream;
        rt_assembly_t & __assembly;
        mt_heap_t       __heap;

        res_reader_t<rt_spool_t> __res_reader;
        rt_assembly_ref_loader_t * __ref_loader;
        al::heap_t<void *[]> __rt_heap;

        rt_method_t   * __entry_point = nullptr;

        std::map<res_t, rt_string_t> __string_map;

        typedef std::tuple<rt_sid_t, rt_sid_t, int> __general_type_key_t;
        al::fixed_map_t<__general_type_key_t, rt_mt_t<__tidx_t::type> *> __general_type_map;

        template<typename entity_t>
        struct __t_mt_item_t
        {
            entity_t ** mt_ptr;
            size_t count;

            typedef entity_t ** iterator;
            typedef entity_t *  value_type;

            entity_t ** begin() const { return mt_ptr; }
            entity_t ** end()   const { return mt_ptr + count; }
        };

        template<__tidx_t tidx>
        struct __tidx_mt_item_t : __t_mt_item_t<rt_entity_t<tidx>> { };

        typedef __t_mt_item_t<void> __mt_item_t;
        __mt_item_t __mt_items[__table_count];

        void __init_mt_items()
        {
            for(__mt_item_t * it = __mt_items, * it_end = it + X_ARRAY_SIZE(__mt_items);
                it < it_end; it++)
            {
                it->mt_ptr = nullptr;
            }
        }

        template<__tidx_t tidx> mt_manager_t<tidx, __mt_t<tidx>> & __mt_manager()
        {
            return __super_t::template __mt_manager<tidx>();
        }

        rt_sid_t __to_sid(res_t res)
        {
            return __res_reader.read_sid(res);
        }

        //-------- ---------- ---------- ---------- ----------

        template<typename t, typename ... args_t> t * __new_obj(args_t && ... args)
        {
            return memory_t::new_obj<t>(__ctx.memory, std::forward<args_t>(args) ...);
        }

        rt_sid_t __assembly_name()
        {
            return __to_sid(__assembly.mt->name);
        }

        template<__tidx_t tidx> __entity_t<tidx> * __entity_at(int index)
        {
            return __entity_at<tidx>(__metadata_at<tidx>(index));
        }

        template<__tidx_t tidx> __entity_t<tidx> * __entity_at(rt_mt_t<tidx> & mt)
        {
            typedef __entity_t<tidx> entity_t;

            if(mt.rt_object == nullptr)
            {
                mt.rt_object = rt_mt_t<tidx>::new_entity(__ctx.memory);
                mt.rt_object->mt = &mt;
            }

            return mt.rt_object;
        }

        template<__tidx_t tidx> __entity_t<tidx> * __entity_at(ref_t ref)
        {
            if(ref == ref_t::null)
                return nullptr;

            return __entity_at<tidx>(ref.index);
        }

        template<__tidx_t tidx> rt_mt_t<tidx> & __metadata_at(int index)
        {
            auto & mgr = __mt_manager<tidx>();
            if(index >= mgr.count())
            {
                throw __FormatError(_F(_T("'%1%:%2%' ref index '%3%' overflow"),
                    __assembly_name(), tidx, index
                ));
            }

            return mgr[index];
        }

        //-------- ---------- ---------- ---------- ----------

        void __read_assembly()
        {
            auto & assembly_mgr = __mt_manager<__tidx_t::assembly>();
            if(assembly_mgr.count() == 0)
                throw __FormatError(_T("no assembly information"));

            if(assembly_mgr.count() >= 2)
                throw __FormatError(_T("multiply assembly information"));

            rt_mt_t<__tidx_t::assembly> & mt_assembly = assembly_mgr[0];
            __entry_point = __entity_at<__tidx_t::method>(mt_assembly.entry_point);

            __assembly.mt = &mt_assembly;
        }

        //-------- ---------- ---------- ---------- ----------

        void __init_search_datas()
        {
            // init type search map
            auto & type_mgr = __mt_manager<__tidx_t::type>();
            __general_type_map.init(type_mgr.count());

            for(int index = 0, end = index + type_mgr.count(); index < end; index++)
            {
                rt_mt_t<__tidx_t::type> & mt = type_mgr[index];
                __general_type_map.append(__general_type_key_t(
                        __to_sid(mt.namespace_), __to_sid(mt.name), (int)mt.generic_params.count
                    ), &mt
                );
            }

            __general_type_map.commit();            
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // layout 1

    template<typename reader_t, __lv_t rlv>
    class __rt_assembly_loader_super_t<reader_t, 1, rlv>
        : public __rt_assembly_loader_super_t<reader_t, 0, rlv>
    {
        typedef __rt_assembly_loader_super_t<reader_t, 0, rlv> __super_t;

    public:
        using __super_t::__super_t;
    };

    ////////// ////////// ////////// ////////// //////////

    template<__lv_t lv>
    class __rt_assembly_loader_t
        : public __rt_assembly_loader_super_t<__rt_assembly_loader_t<lv>, lv>
    {
    public:
        typedef __rt_assembly_loader_super_t<__rt_assembly_loader_t<lv>, lv> __super_t;

        using __super_t::__super_t;
    };

    ////////// ////////// ////////// ////////// //////////

    template<__lv_t lv>
    class __rt_assembly_t : public rt_assembly_t
    {
        typedef rt_assembly_t __super_t;
        typedef __rt_assembly_loader_t<lv> __loader_t;
        template<__tidx_t tidx> using __mt_t = typename __loader_t::template mt_t<tidx>;

    public:
        __rt_assembly_t(rt_context_t & ctx, xistream_t & stream,
                            rt_assembly_ref_loader_t * ref_loader)
            : __loader(ctx, stream, *this, ref_loader)
        { }

        virtual rt_sid_t to_sid(res_t s) override
        {
            return __loader.to_sid(s);
        }

        virtual rt_sid_t get_name() override
        {
            mt_assembly_t * mt = this->mt;
            return __loader.to_sid(mt->name);
        }

        virtual rt_method_t * get_entry_point() override
        {
            return __loader.get_entry_point();
        }

        virtual rt_bytes_t get_method_body(rt_method_t * method) override
        {
            _A(method != nullptr);

            return __loader.read_bytes(method->mt->body);
        }

        virtual rt_sid_t get_name(rt_type_t * type) override
        {
            _A(type != nullptr);

            return type->get_name(this);
        }

        virtual rt_sid_t get_name(rt_method_t * method) override
        {
            _A(method != nullptr);

            return __to_sid(method->mt->name);
        }

        virtual rt_sid_t get_name(rt_field_t * field) override
        {
            _A(field != nullptr);

            return __to_sid(field->mt->name);
        }

        virtual rt_method_ref_t * get_method_ref(ref_t ref) override
        {
            return __entity_at<__tidx_t::method_ref>(ref);
        }

        virtual rt_field_ref_t * get_field_ref(ref_t ref) override
        {
            return __entity_at<__tidx_t::field_ref>(ref);
        }

        virtual rt_type_ref_t * get_type_ref(ref_t ref) override
        {
            return __entity_at<__tidx_t::type_ref>(ref);
        }

        virtual rt_assembly_ref_t * get_assembly_ref(ref_t ref) override
        {
            return __entity_at<__tidx_t::assembly_ref>(ref);
        }

        virtual rt_sid_t get_name(rt_assembly_ref_t * assembly_ref) override
        {
            _A(assembly_ref != nullptr);

            return __to_sid(assembly_ref->mt->name);
        }

        virtual rt_sid_t get_package(rt_assembly_ref_t * assembly_ref) override
        {
            _A(assembly_ref != nullptr);

            return __to_sid(assembly_ref->mt->package);
        }

        virtual rt_general_type_t * get_type(ref_t ref) override
        {
            return __entity_at<__tidx_t::type>(ref);
        }

        virtual rt_general_type_t * get_type(rt_sid_t ns, rt_sid_t name,
                                    int generic_param_count) override
        {
            return __loader.get_type(ns, name, generic_param_count);
        }

        virtual rt_sid_t get_name(rt_general_type_t * type) override
        {
            _A(type != nullptr);

            return __to_sid(type->mt->name);
        }

        virtual rt_array_type_t * get_array_type(ref_t ref) override
        {
            return __entity_at<__tidx_t::array_type>(ref);
        }

        virtual rt_generic_type_t * get_generic_type(ref_t ref) override
        {
            return __entity_at<__tidx_t::generic_type>(ref);
        }

        virtual rt_method_t * get_method(ref_t ref) override
        {
            return __entity_at<__tidx_t::method>(ref);
        }

        virtual rt_field_t * get_field(ref_t ref) override
        {
            return __entity_at<__tidx_t::field>(ref);
        }

        virtual rt_param_t  * get_param(ref_t ref) override
        {
            return __entity_at<__tidx_t::param>(ref);
        }

        virtual rt_super_type_t * get_super_type(ref_t ref) override
        {
            return __entity_at<__tidx_t::super_type>(ref);
        }

        virtual rt_nest_type_t  * get_nest_type(ref_t ref) override
        {
            return __entity_at<__tidx_t::nest_type>(ref);
        }

        virtual bool each_methods(ref_t methods, __each_method_func_t f) override
        {
            return __mt_manager<__tidx_t::method>().each(methods, f);
        }

        virtual bool each_fields(ref_t fields, __each_field_func_t f) override
        {
            return __mt_manager<__tidx_t::field>().each(fields, f);
        }

        virtual bool each_params(ref_t params, __each_param_func_t f) override
        {
            return __mt_manager<__tidx_t::param>().each(params, f);
        }

        virtual rt_general_type_t * get_host_by_method_ref(ref_t method_ref) override
        {
            return __loader.search_host([index=method_ref.index](
                                        const rt_mt_t<__tidx_t::type> & mt) {
                ref_t m = mt.methods;
                return index < m.index? -1 : index >= m.index + m.count? 1 : 0;
            });
        }

        virtual rt_general_type_t * get_host_by_method(rt_method_t * method) override
        {
            _A(method != nullptr);

            return get_host_by_method_ref(__get_ref<__tidx_t::method>(method));
        }

        virtual rt_general_type_t * get_host_by_field_ref(ref_t field_ref) override
        {
            return __loader.search_host([index=field_ref.index](
                                        const rt_mt_t<__tidx_t::type> & mt) {
                ref_t f = mt.fields;
                return index < f.index? -1 : index >= f.index + f.count? 1 : 0;
            });
        }

        virtual rt_general_type_t * get_host_by_field(rt_field_t * field) override
        {
            _A(field != nullptr);

            return get_host_by_field_ref(__get_ref<__tidx_t::field>(field));
        }

        virtual rt_method_ref_param_t * get_method_ref_param(ref_t ref) override
        {
            return __entity_at<__tidx_t::method_ref_param>(ref);
        }

        void load()
        {
            __loader.load();
        }

    private:
        __loader_t __loader;
        std::map<rt_sid_t, rt_general_type_t *> __types;

        template<__tidx_t tidx> __entity_t<tidx> * __entity_at(ref_t ref)
        {
            return __loader.template entity_at<tidx>(ref);
        }

        template<__tidx_t tidx> rt_mt_t<tidx> & __metadata_at(ref_t ref)
        {
            return __loader.template metadata_at<tidx>(ref);
        }

        template<__tidx_t tidx> mt_manager_t<tidx, __mt_t<tidx>> & __mt_manager()
        {
            return __loader.template mt_manager<tidx>();
        }

        rt_sid_t __to_sid(res_t name)
        {
            return __loader.to_sid(name);
        }

        template<__tidx_t tidx> ref_t __get_ref(__entity_t<tidx> * entity)
        {
            return __loader.template get_ref<tidx>(entity);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    template<__lv_t lv>
    __rt_assembly_t<lv> * __load_rt_assembly(rt_context_t & ctx, xistream_t & stream,
                                           rt_assembly_ref_loader_t * ref_loader)
    {
        __rt_assembly_t<lv> * assembly = memory_t::new_obj<__rt_assembly_t<1>>(
            ctx.memory, ctx, stream, ref_loader
        );

        assembly->load();
        return assembly;
    }

    rt_assembly_t * load_assembly(rt_context_t & ctx, xistream_t & stream,
                                    rt_assembly_ref_loader_t * ref_loader)
    {
        mt_guide_t guide;
        assembly_read_guide(stream, &guide);

        rt_assembly_t * assembly;

        switch(guide.layout)
        {
            case 1:
                assembly = __load_rt_assembly<1>(ctx, stream, ref_loader);
                break;

            default:
                throw _ED(assembly_error_code_t::unsupported_layout, guide.layout);
        }

        return assembly;
    }

    rt_assembly_t * load_assembly(rt_context_t & ctx, const string_t & path,
                                    rt_assembly_ref_loader_t * ref_loader)
    {
        file_xistream_t fs;
        fs.open(path);

        return load_assembly(ctx, fs, ref_loader);
    }

    ////////// ////////// ////////// ////////// //////////

} } }

