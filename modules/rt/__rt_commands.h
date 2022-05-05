#ifndef __RT_COMMANDS_H__
#define __RT_COMMANDS_H__


namespace X_ROOT_NS::modules::rt {

    namespace
    {
        using namespace core;

        typedef core::xil_command_t         __cmd_t;
        typedef core::xil_storage_type_t    __stype_t;
        typedef core::xil_smp_t             __smp_t;
    }

    ////////// ////////// ////////// ////////// //////////

    // Runtime command.
    struct rt_command_t
    {
        rt_command_t(__cmd_t cmd) _NE : cmd(cmd) { }

        __cmd_t     cmd;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Runtime command base type.
    template<__cmd_t _cmd>
    struct rt_command_base_t : rt_command_t
    {
        typedef rt_command_base_t   __self_t;
        typedef rt_command_t        __super_t;

        rt_command_base_t() _NE : __super_t(_cmd) { }

        static const __cmd_t this_cmd = _cmd;
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime push command base type.
    struct rt_push_command_base_t : rt_command_base_t<__cmd_t::push>
    {
        typedef rt_push_command_base_t           __self_t;
        typedef rt_command_base_t<__cmd_t::push> __super_t;

        rt_push_command_base_t(__stype_t stype) _NE
        {
            this->stype = stype;
        }

        __stype_t  stype;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Runtime push command base type.
    template<__stype_t _stype>
    struct rt_push_command_t : rt_push_command_base_t
    {
        typedef rt_push_command_t       __self_t;
        typedef rt_push_command_base_t  __super_t;

        rt_push_command_t() : __super_t(_stype) { }

        static const __stype_t this_stype = _stype;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Runtime push field command.
    struct rt_push_field_command_t : rt_push_command_t<__stype_t::field>
    {
        typedef rt_push_field_command_t             __self_t;
        typedef rt_push_command_t<__stype_t::field> __super_t;

        rt_push_field_command_t(rt_field_base_t * field) _NE : field(field)
        {
            _A(field != nullptr);
        }

        rt_field_base_t * field;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Runtime push argument command.
    struct rt_push_argument_command_t : rt_push_command_t<__stype_t::argument>
    {
        typedef rt_push_argument_command_t              __self_t;
        typedef rt_push_command_t<__stype_t::argument>  __super_t;

        rt_push_argument_command_t(msize_t index) _NE : index(index) { }

        msize_t index;
    };

    //-------- ---------- ---------- ---------- ----------

    // Runtime smp command base type.
    template<__smp_t _smp>
    struct rt_smp_command_base_t : rt_command_base_t<__cmd_t::smp>
    {
        typedef rt_smp_command_base_t           __self_t;
        typedef rt_command_base_t<__cmd_t::smp> __super_t;

        rt_smp_command_base_t() _NE : smp(_smp) { }

        __smp_t     smp;

        static const __smp_t this_smp = _smp;
    };

    // - - - - - - - - - - - - - - - - - - - - - - - - - -

    // Runtime ret command.
    struct rt_ret_command_t : rt_smp_command_base_t<__smp_t::ret>
    {
        typedef rt_ret_command_t                    __self_t;
        typedef rt_smp_command_base_t<__smp_t::ret> __super_t;
    };

    ////////// ////////// ////////// ////////// //////////

    // Dynamic method body.
    class rt_dynamic_method_body_t
    {
    public:
        rt_bytes_t commands;
    };

    //-------- ---------- ---------- ---------- ----------

    // Dynamic method body builder.
    class rt_dynamic_method_body_builder_t
    {
    public:

        // Constructor.
        rt_dynamic_method_body_builder_t();

        // Appends a command.
        template<typename _dcmd_t, typename ... _args_t>
        _dcmd_t * append(_args_t && ... args)
        {
            _dcmd_t * cmd = __new_obj<_dcmd_t>(std::forward<_args_t>(args) ...);
            __commands.push_back(std::make_tuple((rt_command_t *)cmd, sizeof(_dcmd_t)));

            // Accumulate total size, the byte_t is to record size of a command.
            __total_size += sizeof(_dcmd_t) + sizeof(byte_t);

            return cmd;
        }

        // Builds method body and returns it.
        rt_dynamic_method_body_t * to_method_body(memory_t * memory);

    private:

        al::svector_t<std::tuple<rt_command_t *, size_t>> __commands;
        size_t __total_size = 0;
        xheap_t __heap;

        template<typename _t, typename ... _args_t> _t * __new_obj(_args_t && ... args)
        {
            return memory_t::new_obj<_t>(&__heap, std::forward<_args_t>(args) ...);
        }
    };

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rt

#endif // __RT_COMMANDS_H__


