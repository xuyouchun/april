#include <rt.h>
#include "utils.h"

namespace X_ROOT_NS::modules::rt {

    using namespace core;

    typedef rt_error_code_t __e_t;
    template<__tidx_t tidx> using __entity_t = rt_entity_t<tidx>;

    #define __AlwaysInline  X_ALWAYS_INLINE

    ////////// ////////// ////////// ////////// //////////

    template<typename loader_t, __lv_t lv, __lv_t rlv = lv>
    class __rt_assembly_loader_super_t { };

    //-------- ---------- ---------- ---------- ----------

    struct __mt_template_t
    {
        template<__tidx_t tidx> using mt_t = rt_mt_t<tidx>;
    };

    ////////// ////////// ////////// ////////// //////////

    // Base class of assembly loader.
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

        // Constructor.
        __rt_assembly_loader_super_t(rt_context_t & ctx, xistream_t & stream,
                rt_assembly_t & assembly)
            : __super_t(ctx.spool), __stream(stream), __res_reader(this->__heap, ctx.spool)
            , __ctx(ctx), __assembly(assembly)
        {
            __init_mt_items();
        }

        // Loads assembly.
        void load()
        {
            read_metadata<rlv, __table_count, __mt_template_t>(
                __stream, this->__mt_objects, &__heap
            );

            __read_assembly();
            __init_search_datas();
        }

        // Gets entry point.
        rt_method_t * get_entry_point()
        {
            return __entry_point;
        }

        // Returns sid of resource.
        rt_sid_t to_sid(res_t res)
        {
            return __to_sid(res);
        }

        // Reads bytes from resource.
        rt_bytes_t read_bytes(res_t res)
        {
            if (res == res_t::null)
                return rt_bytes_t();

            size_t length;
            const byte_t * bytes = this->__heap.read_block(res, &length);
            return rt_bytes_t { bytes, length };
        }

        // Returns entity of specified ref.
        template<__tidx_t tidx, typename ... _args_t>
        __entity_t<tidx> * entity_at(ref_t ref, _args_t && ... args)
        {
            return __entity_at<tidx>(ref, std::forward<_args_t>(args) ...);
        }

        // Returns metadata of specified ref.
        template<__tidx_t tidx>
        rt_mt_t<tidx> & metadata_at(ref_t ref)
        {
            return __metadata_at<tidx>(ref.index);
        }

        // Gets type by namespace, name and generic param count.
        rt_general_type_t * get_type(rt_sid_t ns, rt_sid_t name, int generic_param_count)
        {
            auto it = __general_type_map.find(__general_type_key_t(ns, name, generic_param_count));
            if (it == __general_type_map.end())
                return nullptr;

            return __entity_at<__tidx_t::type>(*it);
        }

        // A binary search algorithm for searching host type by member.
        template<typename itor_t, typename cmp_t>
        bool __binary_find(itor_t begin, itor_t end, itor_t * out_r, cmp_t f)
        {
            if (begin == end)
                return false;

            itor_t it = begin + (end - begin) / 2;
            auto r = f(**it);

            if (r < 0)
                return __binary_find(begin, it, out_r, f);

            if (r > 0)
                return __binary_find(it + 1, end, out_r, f);

            al::assign_value(out_r, it);
            return true;
        }

        // A binary search algorithm for searching host type by member.
        template<typename itor_t, typename cmp_t>
        itor_t binary_find(itor_t begin, itor_t end, cmp_t f)
        {
            byte_t r[sizeof(itor_t)];
            return __binary_find(begin, end, (itor_t *)r, f)? *(itor_t *)r : end;
        }

        // A binary search algorithm for searching host type by member.
        template<typename container_t, typename cmp_t>
        auto binary_find(const container_t & container, cmp_t f)
        {
            return binary_find(std::begin(container), std::end(container), f);
        }

        // Searches host.
        template<typename cmp_t> rt_general_type_t * search_host(cmp_t f)
        {
            auto & type_mgr = __mt_manager<__tidx_t::type>();
            typedef rt_mt_t<__tidx_t::type> mt_t;
            auto it = binary_find(type_mgr, f);

            if (it == type_mgr.end())
                return nullptr;

            ref_t ref(it - type_mgr.begin());
            return __entity_at<__tidx_t::type>(**it, ref);
        }

        // Gets metadata manager.
        template<__tidx_t tidx> mt_manager_t<tidx, __mt_t<tidx>> & mt_manager()
        {
            return __mt_manager<tidx>();
        }

    private:
        static const size_t __table_count = __self_t::value_of(__ln_t::table_count);

        rt_context_t  & __ctx;                      // Runtime context.
        xistream_t    & __stream;                   // Stream.
        rt_assembly_t & __assembly;                 // Runtime assembly.
        mt_heap_t       __heap;                     // Heap.

        res_reader_t<rt_spool_t> __res_reader;      // Resource reader.
        al::heap_t<void *[]> __rt_heap;             // Runtime heap.

        rt_method_t   * __entry_point = nullptr;    // Entry point of assembly, the Main method.

        std::map<res_t, rt_string_t> __string_map;  // String map.

        typedef std::tuple<rt_sid_t, rt_sid_t, int> __general_type_key_t;
        al::fixed_map_t<__general_type_key_t, int> __general_type_map;

        // Metadata item with entity type.
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

        // Initialize metadata items.
        void __init_mt_items()
        {
            for (__mt_item_t * it = __mt_items, * it_end = it + X_ARRAY_SIZE(__mt_items);
                it < it_end; it++)
            {
                it->mt_ptr = nullptr;
            }
        }

        // Gets metadata manager.
        template<__tidx_t tidx> mt_manager_t<tidx, __mt_t<tidx>> & __mt_manager()
        {
            return __super_t::template __mt_manager<tidx>();
        }

        // Gets sid of resource.
        rt_sid_t __to_sid(res_t res)
        {
            return __res_reader.read_sid(res);
        }

        //-------- ---------- ---------- ---------- ----------

        // Creates a new object.
        template<typename t, typename ... args_t> t * __new_obj(args_t && ... args)
        {
            return memory_t::new_obj<t>(__ctx.memory, std::forward<args_t>(args) ...);
        }

        // Gets assembly name.
        rt_sid_t __assembly_name()
        {
            return __to_sid(__assembly.mt->name);
        }

        // Gets entity at specified index.
        template<__tidx_t tidx, typename ... _args_t>
        __entity_t<tidx> * __entity_at(int index, _args_t && ... args)
        {
            return __entity_at<tidx>(
                __metadata_at<tidx>(index), ref_t(index), std::forward<_args_t>(args) ...
            );
        }

        // Gets entity by metadata.
        template<__tidx_t tidx, typename ... _args_t>
        __entity_t<tidx> * __entity_at(rt_mt_t<tidx> & mt, ref_t ref, _args_t && ... args)
        {
            typedef __entity_t<tidx> entity_t;

            if constexpr (sizeof ... (args) == 0)
            {
                if (mt.rt_object == nullptr)
                    mt.rt_object = rt_mt_t<tidx>::new_entity(__ctx, ref, &mt, &__assembly);

                return mt.rt_object;
            }
            else
            {
                return rt_mt_t<tidx>::new_entity(
                    __ctx, ref, &mt, &__assembly, std::forward<_args_t>(args) ...
                );
            }
        }

        // Returns entity at ref.
        template<__tidx_t tidx, typename ... _args_t>
        __entity_t<tidx> * __entity_at(ref_t ref, _args_t && ... args)
        {
            if (ref == ref_t::null)
                return nullptr;

            return __entity_at<tidx>(ref.index, std::forward<_args_t>(args) ...);
        }

        // Returns metadata at index.
        template<__tidx_t tidx> rt_mt_t<tidx> & __metadata_at(int index)
        {
            auto & mgr = __mt_manager<tidx>();
            if (index >= mgr.count())
            {
                throw __RtAssemblyErrorF("'%1%:%2%' ref index '%3%' overflow",
                    __assembly_name(), tidx, index
                );
            }

            return mgr[index];
        }

        //-------- ---------- ---------- ---------- ----------

        // Reads assembly.
        void __read_assembly()
        {
            auto & assembly_mgr = __mt_manager<__tidx_t::assembly>();
            if (assembly_mgr.count() == 0)
                throw __RtAssemblyError(_T("no assembly information"));

            if (assembly_mgr.count() >= 2)
                throw __RtAssemblyError(_T("multiply assembly information"));

            rt_mt_t<__tidx_t::assembly> & mt_assembly = assembly_mgr[0];
            __assembly.mt = &mt_assembly;

            __entry_point = __entity_at<__tidx_t::method>(mt_assembly.entry_point);
        }

        //-------- ---------- ---------- ---------- ----------

        // Init searching data.
        void __init_search_datas()
        {
            // init type search map
            auto & type_mgr = __mt_manager<__tidx_t::type>();
            __general_type_map.init(type_mgr.count());

            for (int index = 0, end = index + type_mgr.count(); index < end; index++)
            {
                rt_mt_t<__tidx_t::type> & mt = type_mgr[index];
                __general_type_map.append(__general_type_key_t(
                        __to_sid(mt.namespace_), __to_sid(mt.name), (int)mt.generic_params.count
                    ), index
                );
            }

            __general_type_map.commit();            
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // layout 1

    // Base class of assembly loader for version 1.
    template<typename reader_t, __lv_t rlv>
    class __rt_assembly_loader_super_t<reader_t, 1, rlv>
        : public __rt_assembly_loader_super_t<reader_t, 0, rlv>
    {
        typedef __rt_assembly_loader_super_t<reader_t, 0, rlv> __super_t;

    public:
        using __super_t::__super_t;
    };

    ////////// ////////// ////////// ////////// //////////

    // Assembly loader for version 1.
    template<__lv_t lv>
    class __rt_assembly_loader_t
        : public __rt_assembly_loader_super_t<__rt_assembly_loader_t<lv>, lv>
    {
    public:
        typedef __rt_assembly_loader_super_t<__rt_assembly_loader_t<lv>, lv> __super_t;

        using __super_t::__super_t;
    };

    ////////// ////////// ////////// ////////// //////////

    // Runtime assembly.
    template<__lv_t lv>
    class __rt_assembly_t : public rt_assembly_t
    {
        typedef rt_assembly_t __super_t;
        typedef __rt_assembly_loader_t<lv> __loader_t;
        template<__tidx_t tidx> using __mt_t = typename __loader_t::template mt_t<tidx>;

    public:

        // Constructor.
        __rt_assembly_t(rt_context_t & ctx, xistream_t & stream)
            : __loader(ctx, stream, *this)
        { }

        // Converts string to sid.
        virtual rt_sid_t to_sid(res_t s) override
        {
            return __loader.to_sid(s);
        }

        // Converts string to sid.
        virtual rt_sid_t to_sid(const string_t & s) override
        {
            return __to_sid(s);
        }

        // Returns name.
        virtual rt_sid_t get_name() override
        {
            mt_assembly_t * mt = this->mt;
            return __loader.to_sid(mt->name);
        }

        // Returns entry point of assembly. the main method.
        virtual rt_method_t * get_entry_point() override
        {
            return __loader.get_entry_point();
        }

        // Gets method body.
        virtual rt_bytes_t get_method_body(rt_method_t * method) override
        {
            _A(method != nullptr);

            return __loader.read_bytes(method->mt->body);
        }

        // Gets name of field.
        virtual rt_sid_t get_name(rt_field_t * field) override
        {
            _A(field != nullptr);

            return __to_sid((*field)->name);
        }

        // Gets name of method.
        virtual rt_sid_t get_name(rt_method_t * method) override
        {
            _A(method != nullptr);

            return __to_sid((*method)->name);
        }

        // Gets method ref.
        virtual rt_method_ref_t * get_method_ref(ref_t ref) override
        {
            return __entity_at<__tidx_t::method_ref>(ref);
        }

        // Gets field ref.
        virtual rt_field_ref_t * get_field_ref(ref_t ref) override
        {
            return __entity_at<__tidx_t::field_ref>(ref);
        }

        // Gets type ref.
        virtual rt_type_ref_t * get_type_ref(ref_t ref) override
        {
            return __entity_at<__tidx_t::type_ref>(ref);
        }

        // Gets assembly ref.
        virtual rt_assembly_ref_t * get_assembly_ref(ref_t ref) override
        {
            return __entity_at<__tidx_t::assembly_ref>(ref);
        }

        // Gets name of assembly ref.
        virtual rt_sid_t get_name(rt_assembly_ref_t * assembly_ref) override
        {
            _A(assembly_ref != nullptr);

            return __to_sid(assembly_ref->mt->name);
        }

        // Gets package name of assembly ref.
        virtual rt_sid_t get_package(rt_assembly_ref_t * assembly_ref) override
        {
            _A(assembly_ref != nullptr);

            return __to_sid(assembly_ref->mt->package);
        }

        // Gets general type of ref.
        virtual rt_general_type_t * get_type(ref_t ref) override
        {
            return __entity_at<__tidx_t::type>(ref);
        }

        // Gets general type of specified namespace, name and generic param count.
        virtual rt_general_type_t * get_type(rt_sid_t ns, rt_sid_t name,
                                    int generic_param_count) override
        {
            return __loader.get_type(ns, name, generic_param_count);
        }

        // Gets general type by specified namespace, name and generic param count.
        virtual rt_general_type_t * get_type(const string_t & ns, const string_t & name,
                                    int generic_param_count) override
        {
            return __loader.get_type(__to_sid(ns), __to_sid(name), generic_param_count);
        }

        // Gets name of general type.
        virtual rt_sid_t get_name(rt_general_type_t * type) override
        {
            _A(type != nullptr);

            return __to_sid(type->mt->name);
        }

        // Gets array type metadata.
        virtual rt_mt_t<__tidx_t::array_type> * mt_of_array(ref_t ref) override
        {
            return &__metadata_at<__tidx_t::array_type>(ref);
        }

        // Gets generic type metadata.
        virtual rt_mt_t<__tidx_t::generic_type> * mt_of_generic_type(ref_t ref) override
        {
            return &__metadata_at<__tidx_t::generic_type>(ref);
        }

        // Gets generic field metadata.
        virtual rt_mt_t<__tidx_t::generic_field> * mt_of_generic_field(ref_t ref) override
        {
            return &__metadata_at<__tidx_t::generic_field>(ref);
        }

        // Gets position field metadata.
        virtual rt_mt_t<__tidx_t::position_field> * mt_of_position_field(ref_t ref) override
        {
            return &__metadata_at<__tidx_t::position_field>(ref);
        }

        // Gets generic method metadata.
        virtual rt_mt_t<__tidx_t::generic_method> * mt_of_generic_method(ref_t ref) override
        {
            return &__metadata_at<__tidx_t::generic_method>(ref);
        }

        /*
        // Gets generic type of ref.
        virtual rt_generic_type_t * get_generic_type(ref_t ref) override
        {
            return __entity_at<__tidx_t::generic_type>(ref);
        }
        */

        // Gets method of ref.
        virtual rt_method_t * get_method(ref_t ref) override
        {
            return __entity_at<__tidx_t::method>(ref);
        }

        // Gets field of ref.
        virtual rt_field_t * get_field(ref_t ref) override
        {
            return __entity_at<__tidx_t::field>(ref);
        }

        // Gets param of ref.
        virtual rt_param_t  * get_param(ref_t ref) override
        {
            return __entity_at<__tidx_t::param>(ref);
        }

        // Gets generic method of ref.
        virtual rt_generic_method_t * get_generic_method(ref_t ref,
                                    const generic_param_manager_t * gp_mgr) override
        {
            return __entity_at<__tidx_t::generic_method>(ref, gp_mgr);
        }

        // Gets generic argument of ref.
        virtual rt_generic_argument_t * get_generic_argument(ref_t ref) override
        {
            return __entity_at<__tidx_t::generic_argument>(ref);
        }

        // Gets generic param.
        virtual rt_generic_param_t * get_generic_param(ref_t ref) override
        {
            return __entity_at<__tidx_t::generic_param>(ref);
        }

        // Gets base type of ref.
        virtual rt_super_type_t * get_super_type(ref_t ref) override
        {
            return __entity_at<__tidx_t::super_type>(ref);
        }

        // Gets nest type of ref.
        virtual rt_nest_type_t  * get_nest_type(ref_t ref) override
        {
            return __entity_at<__tidx_t::nest_type>(ref);
        }

        // Enums methods.
        virtual bool each_methods(ref_t methods, __each_method_func_t f) override
        {
            return __mt_manager<__tidx_t::method>().each(methods, f);
        }

        // Enums fields.
        virtual bool each_fields(ref_t fields, __each_field_func_t f) override
        {
            return __mt_manager<__tidx_t::field>().each(fields, f);
        }

        // Enums params.
        virtual bool each_params(ref_t params, __each_param_func_t f) override
        {
            return __mt_manager<__tidx_t::param>().each(params, f);
        }

        // Gets host type by method ref.
        virtual rt_general_type_t * get_host_by_method_ref(ref_t method_ref) override
        {
            return __loader.search_host([index=method_ref.index](
                                        const rt_mt_t<__tidx_t::type> & mt) {
                ref_t m = mt.methods;
                return index < m.index? -1 : index >= m.index + m.count? 1 : 0;
            });
        }

        // Gets host type by field ref.
        virtual rt_general_type_t * get_host_by_field_ref(ref_t field_ref) override
        {
            return __loader.search_host([index=field_ref.index](
                                        const rt_mt_t<__tidx_t::type> & mt) {
                ref_t f = mt.fields;
                return index < f.index? -1 : index >= f.index + f.count? 1 : 0;
            });
        }

        // Gets host by nest type ref.
        virtual rt_general_type_t * get_host_by_nest_type_ref(ref_t nest_type_ref) override
        {
            auto it = __nest_type_map.find(nest_type_ref);
            if (it == __nest_type_map.end())
                return nullptr;

            rt_general_type_t * t = __loader.search_host([index=*it](
                                        const rt_mt_t<__tidx_t::type> & mt) {
                ref_t f = mt.nest_types;
                return index < f.index? -1 : index >= f.index + f.count? 1 : 0;
            });

            return t;
        }

        // Gets method ref param by ref.
        virtual rt_method_ref_param_t * get_method_ref_param(ref_t ref) override
        {
            return __entity_at<__tidx_t::method_ref_param>(ref);
        }

        // Loads assembly.
        void load()
        {
            __loader.load();
            __init_search_datas();
        }

    private:
        __loader_t __loader;
        std::map<rt_sid_t, rt_general_type_t *> __types;
        al::fixed_map_t<ref_t, int> __nest_type_map;

        // Init search datas.
        void __init_search_datas()
        {
            // init nest type seach map
            auto & nest_type_mgr = __mt_manager<__tidx_t::nest_type>();

            __nest_type_map.init(nest_type_mgr.count());

            for (int index = 0, end = index + nest_type_mgr.count(); index < end; index++)
            {
                rt_mt_t<__tidx_t::nest_type> & mt = nest_type_mgr[index];
                __nest_type_map.append(mt.type, index);
            }

            __nest_type_map.commit();
        }

        // Returns entity at ref.
        template<__tidx_t tidx, typename ... _args_t>
        __entity_t<tidx> * __entity_at(ref_t ref, _args_t && ... args)
        {
            return __loader.template entity_at<tidx>(ref, std::forward<_args_t>(args) ...);
        }

        // Gets metadata at ref.
        template<__tidx_t tidx> rt_mt_t<tidx> & __metadata_at(ref_t ref)
        {
            return __loader.template metadata_at<tidx>(ref);
        }

        // Gets metadata manager.
        template<__tidx_t tidx> mt_manager_t<tidx, __mt_t<tidx>> & __mt_manager()
        {
            return __loader.template mt_manager<tidx>();
        }

        // Gets sid of specified string resource.
        rt_sid_t __to_sid(res_t s)
        {
            return __loader.to_sid(s);
        }

        // Gets sid of specified string.
        rt_sid_t __to_sid(const string_t & s)
        {
            return __loader.get_spool().to_sid(s);
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Loads assembly from stream.
    template<__lv_t lv>
    __rt_assembly_t<lv> * __load_rt_assembly(rt_context_t & ctx, xistream_t & stream)
    {
        __rt_assembly_t<lv> * assembly = memory_t::new_obj<__rt_assembly_t<lv>>(
            ctx.memory, ctx, stream
        );

        assembly->load();
        return assembly;
    }

    // Loads assembly from stream.
    rt_assembly_t * load_assembly(rt_context_t & ctx, xistream_t & stream)
    {
        mt_guide_t guide;
        assembly_read_guide(stream, &guide);

        rt_assembly_t * assembly;

        switch (guide.layout)
        {
            case 1:
                assembly = __load_rt_assembly<1>(ctx, stream);
                break;

            default:
                throw _ED(assembly_error_code_t::unsupported_layout, guide.layout);
        }

        return assembly;
    }

    // Loads assembly from assembly file.
    rt_assembly_t * load_assembly(rt_context_t & ctx, const string_t & path)
    {
        file_xistream_t fs;
        fs.open(path);

        return load_assembly(ctx, fs/*, ref_loader*/);
    }

    // Loads assembly by path and assembly name.
    rt_assembly_t * load_assembly(rt_context_t & ctx, const string_t & path,
            const string_t & package, const string_t & name)
    {
        if (package.empty())
        {
            return load_assembly(ctx,
                _F(_T("%1%/%2%.%3%"), path, name, JC_ASSEMBLY_EXT)
            );
        }

        X_UNEXPECTED();
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rt


