#ifndef __EXEC_H__
#define __EXEC_H__

#include <common.h>
#include <algorithm.h>
#include <jc.h>
#include <core.h>
#include <rt.h>

namespace X_ROOT_NS { namespace modules { namespace exec {

    namespace
    {
        using namespace core;
        using namespace rt;
    }

    #define __AlwaysInline X_ALWAYS_INLINE

    const size_t __default_stack_size = 1024 * 1024;

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(exec_error_code_t)

        assembly_not_found,

        assembly_no_entry_point,

        method_no_body,

        internal_function_not_found,

        method_parse_error__unexpected_index,

        switch_table_index_overflow,

        type_not_found,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    X_ENUM(exec_env_code_t)

        end,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        template<typename t> constexpr size_t __alignf()
        {
            return _alignf(sizeof(t), sizeof(rt_stack_unit_t));
        }

        template<typename t, bool tiny> struct __stack_operation_t
        {
            __AlwaysInline static void push(rt_stack_unit_t * top, t value)
            {
                *(t *)top = value;
            }

            __AlwaysInline static t pop(rt_stack_unit_t * top)
            {
                return *(t *)(top - __alignf<t>() / sizeof(rt_stack_unit_t));
            }
        };

        template<typename t> struct __stack_operation_t<t, true>
        {
            union __tmp_t { t value; rt_stack_unit_t unit; };

            __AlwaysInline static void push(rt_stack_unit_t * top, t value)
            {
                __tmp_t tmp = { value };
                *top = reinterpret_cast<rt_stack_unit_t>(tmp.unit);
            }

            __AlwaysInline static t pop(rt_stack_unit_t * top)
            {
                return ((__tmp_t *)(top - 1))->value;
            }
        };
    }

    //-------- ---------- ---------- ---------- ----------

    class executor_stack_t : public object_t, public no_copy_ctor_t
    {
    public:
        executor_stack_t(rt_stack_unit_t * buffer)
            : __buffer(buffer), __top(buffer), __lp(buffer)
        {
            _A(buffer != nullptr);
        }

        template<typename t> __AlwaysInline void push(const t & value)
        {
            __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)>::push(__top, value);
            __top += __alignf<t>() / sizeof(rt_stack_unit_t);
        }

        template<typename t> __AlwaysInline t pop()
        {
            t value = __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)>::pop(__top);
            __top -= __alignf<t>() / sizeof(rt_stack_unit_t);

            return value;
        }

        template<typename t> __AlwaysInline rt_stack_unit_t * pop(int count)
        {
            return __top -= __alignf<t>() / sizeof(rt_stack_unit_t) * count;
        }

        template<int count, typename t = rt_stack_unit_t>
        __AlwaysInline rt_stack_unit_t * pop()
        {
            return __top -= __alignf<t>() / sizeof(rt_stack_unit_t) * count;
        }

        template<typename t> __AlwaysInline t pick()
        {
            return __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)>::pop(__top);
        }

        template<typename t> __AlwaysInline t pick(int offset)
        {
            return __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)>::pop(__top + offset);
        }

        __AlwaysInline rt_stack_unit_t * top() { return __top; }
        __AlwaysInline void set_top(rt_stack_unit_t * top) { __top = top; }
        __AlwaysInline rt_stack_unit_t * pop(size_t unit_size) { return __top -= unit_size; }

        __AlwaysInline void increase_top(size_t unit_size) { __top += unit_size; }
        template<size_t unit_size> __AlwaysInline void increase_top() { __top += unit_size; }

        __AlwaysInline rt_stack_unit_t * lp() { return __lp; }
        __AlwaysInline void set_lp(rt_stack_unit_t * lp) { __lp = lp; }

    private:
        rt_stack_unit_t * __top, * __lp;
        rt_stack_unit_t * __buffer;
    };

    ////////// ////////// ////////// ////////// //////////

    template<typename _size_t>
    __AlwaysInline _size_t __hash(_size_t hash, _size_t v)
    {
        const _size_t mask = (_size_t)0x0F << (sizeof(_size_t) - 4);

        hash = (hash << 4) + v;
        _size_t x;

        if((x = hash & mask) != 0) 
        {
            hash ^= (x >> (sizeof(_size_t) - (sizeof(_size_t) >> 2))); 
            hash &= ~x;
        }

        return hash;
    }

    template<typename _size_t, typename _char_t>
    _size_t elf_hash(const _char_t * s, size_t length)
    {
        const _size_t * t = (const _size_t *)s;
        size_t s_len = length * sizeof(_char_t);
        size_t t_len = _align(s_len, sizeof(_size_t));
        const _size_t * t_end = t + (t_len / sizeof(_size_t));

        _size_t hash = 0;

        for(; t < t_end; t++)
        {
            hash = __hash(hash, *t);
        }

        if(s_len > t_len)
        {
            _size_t v = 0;
            std::memcpy((void *)&v, (byte_t *)s + t_len, s_len - t_len);
            hash = __hash(hash, v);
        }

        return hash & ~((_size_t)0x01 << (sizeof(_size_t) - 1));
    }

    namespace
    {
        struct __string_key_t
        {
            __string_key_t(const char_t * s, rt_length_t length)
                : s(s), length(length) { }

            rt_length_t length;
            const char_t * s;

            bool operator == (const __string_key_t & other) const
            {
                if(s == other.s)
                    return true;

                if(length != other.length)
                    return false;

                return al::strcmp(s, other.s) == cmp_t::equals;
            }
        };

        struct __string_hash_t
        {
            size_t operator()(const __string_key_t & key) const
            {
                return elf_hash<size_t>(key.s, key.length);
            }
        };

        struct __string_equals_t
        {
            bool operator()(const __string_key_t & key1, const __string_key_t & key2) const
            {
                return key1 == key2;
            }
        };
    }

    class rt_string_pool_t : public object_t
    {
    public:
        rt_string_pool_t() { }

        rt_string_t * get(const char_t * s, rt_length_t length);

    private:
        std::unordered_map<
            __string_key_t, rt_string_t *,
            __string_hash_t, __string_equals_t
        > __map;

        al::heap_t<rt_string_t> __heap;
    };

    ////////// ////////// ////////// ////////// //////////

    class command_t;

    struct __calling_stub_t
    {
        rt_stack_unit_t *   lp;
        command_t **        current;
    };

    class executor_env_t;
    class command_execute_context_t : public object_t, public no_copy_ctor_t
    {
    public:
        command_execute_context_t(rt_heap_t * heap, executor_env_t & env,
                    size_t stack_size = __default_stack_size)
            : stack(__stack_buffer = memory_t::alloc_objs<rt_stack_unit_t>(nullptr, stack_size))
            , heap(heap), env(env)
        {
            _A(heap != nullptr);
        }

        executor_stack_t stack;
        rt_string_pool_t string_pool;
        rt_heap_t *      heap;
        executor_env_t & env;

        command_t ** current = nullptr;

        void push_calling(command_t ** command);
        void pop_calling();
        void pop_calling(const __calling_stub_t * p);

        rt_string_t * new_rt_string(const char_t * s);

        virtual ~command_execute_context_t() override
        {
            memory_t::free(nullptr, __stack_buffer);
        }

    private:
        rt_stack_unit_t * __stack_buffer;
        pool_t            __memory_pool;
    };

    ////////// ////////// ////////// ////////// //////////

    class command_t : public object_t
    {
        typedef command_t __self_t;
        typedef object_t  __super_t;

    public:
        virtual void execute(command_execute_context_t & ctx) = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    class commands_t : public object_t
    {
    public:
        commands_t(command_t * commands) : __commands(commands) { }

    private:
        command_t * __commands;
    };

    ////////// ////////// ////////// ////////// //////////

    class exec_method_t : public object_t
    {
    public:
        exec_method_t(command_t ** commands, uint16_t ref_objects, msize_t stack_unit_size)
            : commands(commands), ref_objects(ref_objects), stack_unit_size(stack_unit_size)
        {
            _A(commands != nullptr);
        }

        uint16_t ref_objects;
        msize_t  stack_unit_size;

        command_t ** commands;
    };

    ////////// ////////// ////////// ////////// //////////

    class rt_assemblies_t : public object_t
    {
    public:
        void append(rt_assembly_t * assembly, const string_t & name = _T(""),
                                              const string_t & package = _T(""));
        rt_assembly_t * get(const string_t & package, const string_t & name);

        size_t size() const { return __map.size(); }
        rt_assembly_t * last() const { return __last; }

    private:
        typedef std::tuple<string_t, string_t> __key_t;
        std::map<__key_t, rt_assembly_t *> __map;

        rt_assembly_t * __last = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////

    class executor_env_t : public object_t
    {
    public:
        executor_env_t(memory_t * memory, rt_assembly_provider_t * assembly_provider)
            : assembly_provider(assembly_provider)
            , __memory(memory)
        {
            _A(assembly_provider != nullptr);
        }

        rt_assembly_provider_t * const assembly_provider;
        rt_pool_t pool;

        memory_t * get_memory() { return __memory; }

        exec_method_t * exec_method_of(rt_method_t * rt_method, rt_type_t * type,
                                                     rt_assembly_t * rt_assembly);

    private:
        memory_t * __memory;
        std::map<rt_method_t *, exec_method_t * > __method_map;
    };

    ////////// ////////// ////////// ////////// //////////

    class executor_t : public object_t
    {
    public:
        executor_t(rt_context_t & ctx, rt_assembly_provider_t * assembly_provider)
            : env(ctx.memory, assembly_provider), __ctx(ctx)
        { }

        void execute();

        executor_env_t env;

    private:
        rt_context_t & __ctx;

        void __execute_method(command_execute_context_t & ctx, command_t ** commands);
    };

    ////////// ////////// ////////// ////////// //////////

    void execute(rt_context_t & ctx, rt_assemblies_t & assemblies, rt_assembly_t & entrypoint);
    void execute(rt_context_t & ctx, rt_assembly_t & assembly);
    void execute(rt_context_t & ctx, rt_assemblies_t & assemblies);

    ////////// ////////// ////////// ////////// //////////

    #undef __AlwaysInline

} } }

#endif  // __EXEC_H__
