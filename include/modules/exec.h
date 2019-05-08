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

    #define EXEC_QUICK_EXECUTE          0
    #define EXEC_TRACE                  0

    const size_t __default_stack_size = 1024 * 1024;

    ////////// ////////// ////////// ////////// //////////

    class exec_method_t;
    class exec_method_block_manager_t;

    ////////// ////////// ////////// ////////// //////////

    // Execute error codes.
    X_ENUM(exec_error_code_t)

        // Assembly not found.
        assembly_not_found,

        // Assembly no entry point.
        assembly_no_entry_point,

        // Method no body.
        method_no_body,

        // Internal function not found.
        internal_function_not_found,

        // Method parse error: unexpected index.
        method_parse_error__unexpected_index,

        // Switch table index overflow.
        switch_table_index_overflow,

        // Type not found.
        type_not_found,

        // Block index out of range.
        xil_block_index_out_of_range,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    // Execute env codes.
    X_ENUM(exec_env_code_t)

        // At the end of commands.
        end,

        // Raised by uncaughted exception.
        terminal,

    X_ENUM_END

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // Aligns forward.
        template<typename t> constexpr size_t __alignf()
        {
            return _alignf(sizeof(t), sizeof(rt_stack_unit_t));
        }

        // Stack operations.
        template<typename t, bool tiny> struct __stack_operation_t
        {
            // Pushes a unit.
            __AlwaysInline static void push(rt_stack_unit_t * top, t value)
            {
                *(t *)top = value;
            }

            // Pops a unit.
            __AlwaysInline static t pop(rt_stack_unit_t * top)
            {
                return *(t *)(top - __alignf<t>() / sizeof(rt_stack_unit_t));
            }

            // Pops a unit reference.
            __AlwaysInline static t & pop_reference(rt_stack_unit_t * top)
            {
                return *(t *)(top - __alignf<t>() / sizeof(rt_stack_unit_t));
            }
        };

        // Stack operations. when value size is less or equals than unit.
        template<typename t> struct __stack_operation_t<t, true>
        {
            union __tmp_t { t value; rt_stack_unit_t unit; };

            // Pushes a unit.
            __AlwaysInline static void push(rt_stack_unit_t * top, t value)
            {
                __tmp_t tmp = { value };
                *top = reinterpret_cast<rt_stack_unit_t>(tmp.unit);
            }

            // Pops a unit.
            __AlwaysInline static t pop(rt_stack_unit_t * top)
            {
                return ((__tmp_t *)(top - 1))->value;
            }

            // Pops a unit reference.
            __AlwaysInline static t & pop_reference(rt_stack_unit_t * top)
            {
                return ((__tmp_t *)(top - 1))->value;
            }

        };
    }

    //-------- ---------- ---------- ---------- ----------

    // Executor stack.
    class executor_stack_t : public object_t, public no_copy_ctor_t
    {
    public:

        // Constructor.
        executor_stack_t(rt_stack_unit_t * buffer)
            : __buffer(buffer), __top(buffer), __lp(buffer)
        {
            _A(buffer != nullptr);
        }

        // Pushes a value.
        template<typename t> __AlwaysInline void push(const t & value)
        {
            __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)>::push(__top, value);
            __top += __alignf<t>() / sizeof(rt_stack_unit_t);
        }

        // Pops a value.
        template<typename t> __AlwaysInline t pop()
        {
            t value = __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)>::pop(__top);
            __top -= __alignf<t>() / sizeof(rt_stack_unit_t);

            return value;
        }

        // Pops a value reference.
        template<typename t> __AlwaysInline t & pop_reference()
        {
            typedef __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)> stack_operation_t;

            t & value = stack_operation_t::pop_reference(__top);
            __top -= __alignf<t>() / sizeof(rt_stack_unit_t);

            return value;
        }

        // Pops units of specified count.
        template<typename t> __AlwaysInline rt_stack_unit_t * pop(int count)
        {
            return __top -= __alignf<t>() / sizeof(rt_stack_unit_t) * count;
        }

        // Pops units of specified count.
        template<int count, typename t = rt_stack_unit_t>
        __AlwaysInline rt_stack_unit_t * pop()
        {
            return __top -= __alignf<t>() / sizeof(rt_stack_unit_t) * count;
        }

        // Picks top unit.
        template<typename t> __AlwaysInline t pick()
        {
            return __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)>::pop(__top);
        }

        // Picks top unit reference.
        template<typename t> __AlwaysInline t & pick_reference()
        {
            return __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)>::pop_reference(__top);
        }

        // Picks top unit.
        template<typename t> __AlwaysInline t pick(int offset)
        {
            return __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)>::pop(__top + offset);
        }

        // The top unit.
        __AlwaysInline rt_stack_unit_t * top() { return __top; }

        // Sets the top unit.
        __AlwaysInline void set_top(rt_stack_unit_t * top) { __top = top; }

        // Pops units of specified count.
        __AlwaysInline rt_stack_unit_t * pop(size_t unit_size) { return __top -= unit_size; }

        // Increases top of unit size.
        __AlwaysInline void increase_top(size_t unit_size) { __top += unit_size; }

        // Increases top of unit size.
        template<size_t unit_size> __AlwaysInline void increase_top() { __top += unit_size; }

        // Local variables top.
        __AlwaysInline rt_stack_unit_t * lp() { return __lp; }

        // Sets local variables top.
        __AlwaysInline void set_lp(rt_stack_unit_t * lp) { __lp = lp; }

        // Returns stack bottom.
        __AlwaysInline rt_stack_unit_t * bottom() { return __buffer; }

    private:
        rt_stack_unit_t * __top, * __lp;
        rt_stack_unit_t * __buffer;
    };

    ////////// ////////// ////////// ////////// //////////

    // Returns a string hashcode.
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

    // Returns a string hashcode.
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

    // Runtime string pool.
    class rt_string_pool_t : public object_t
    {
    public:
        rt_string_pool_t() { }

        // Gets the specified string.
        rt_string_t * get(const char_t * s, rt_length_t length);

    private:
        std::unordered_map<
            __string_key_t, rt_string_t *,
            __string_hash_t, __string_equals_t
        > __map;

        al::heap_t<rt_string_t> __heap;
    };

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        // A resource manager base.
        template<typename _node_t>
        class __resource_manager_base_t : public object_t, private memory_base_t
        {
            typedef __resource_manager_base_t<_node_t> __self_t;
            typedef _node_t __node_t;

        public:

            // Constructor.
            __resource_manager_base_t(memory_t * memory = nullptr) : memory_base_t(memory) { }

            // The head node.
            __node_t * head = nullptr;

            // Returns whether the queue/stack is empty.
            bool empty() const { return head == nullptr; }

            // Destructor.
            virtual ~__resource_manager_base_t()
            {
                __node_t * node = head;
                while(node != nullptr)
                {
                    this->__free((void *)node);
                    node = node->next;
                }

                while(!__node_queue.empty())
                {
                    this->__free((void *)__node_queue.front());
                    __node_queue.pop();
                }
            }

        protected:

            // Acquires a new node.
            template<typename ... _args_t>
            __node_t * __acquire_node(_args_t && ... args)
            {
                __node_t * node;

                if(!__node_queue.empty())
                {
                    node = __node_queue.front();
                    __node_queue.pop();
                }
                else
                {
                    node = (__node_t *)this->__alloc(sizeof(__node_t));
                }

                return new ((void *)node) __node_t(std::forward<_args_t>(args) ...);
            }

            // Releases a node.
            void __release_node(__node_t * node)
            {
                __node_queue.push(node);
            }

        private:
            std::queue<__node_t *> __node_queue;
        };
    };

    ////////// ////////// ////////// ////////// //////////

    class command_t;

    // The exception stack node.
    struct exception_node_t
    {
        typedef exception_node_t __self_t;

        // Constructor.
        exception_node_t() = default;

        // Constructor.
        exception_node_t(command_t ** throw_point, rt_ref_t exception)
            : throw_point(throw_point), exception(exception)
        { }

        command_t **    throw_point;    // The command where the exception throwed.
        rt_ref_t        exception;      // Exception.
        __self_t *      next;           // Next node of the chain.
    };

    // Exception node stack.
    class exception_stack_t : public __resource_manager_base_t<exception_node_t>
    {
        typedef exception_stack_t   __self_t;
        typedef exception_node_t    __node_t;

        typedef __resource_manager_base_t<exception_node_t> __super_t;

    public:

        using __super_t::__super_t;

        // Pushes an exception.
        __AlwaysInline void push(command_t ** throw_point, rt_ref_t exception)
        {
            __node_t * node = this->__acquire_node(throw_point, exception);
            node->next = this->head;
            this->head = node;
        }

        // Pops an exception.
        __AlwaysInline void pop()
        {
            _A(this->head != nullptr);

            __release_node(this->head);
            this->head = this->head->next;
        }
    };

    ////////// ////////// ////////// ////////// //////////
    // exec_method_block_t

    // Method block.
    struct exec_method_block_t
    {
        // Start of protected block.
        command_t ** start;

        // End of protected block.
        command_t ** end;

        // Entry point.
        command_t ** entry_point;

        // Relation type.
        rt_type_t * relation_type;

        // Block type.
        method_xil_block_type_t type;

        // Returns whether the block contains specified command.
        __AlwaysInline bool include(command_t ** command)
        {
            return command >= start && command < end;
        }

        // Returns whether the block not contains specified command.
        __AlwaysInline bool exclude(command_t ** command)
        {
            return command < start || command >= end;
        }
    };

    // Method block manager.
    class exec_method_block_manager_t
    {
    public:

        // Block array.
        exec_method_block_t * blocks = nullptr;

        // Block count.
        int count = 0;

        // Finds block contains the specified point.
        template<typename _f_t> void find_block(command_t ** point, _f_t f)
        {
            exec_method_block_t * block = blocks, * block_end = block + count;

            for(; block < block_end; block++)
            {
                if(point >= block->start && point < block->end)
                {
                    if(!f(block))
                        return;

                    break;
                }
            }

            for(block++; block < block_end; block++)
            {
                if(point < block->start || point >= block->end || !f(block))
                    return;
            }
        }
    };

    ////////// ////////// ////////// ////////// //////////

    // Finally block node.
    class finally_block_node_t : public object_t
    {
        typedef finally_block_node_t __self_t;

    public:

        // Constructor.
        finally_block_node_t(exec_method_block_t * block) : block(block) { }

        // Block.
        exec_method_block_t * block;

        // Next node in chain.
        __self_t * next;
    };

    ////////// ////////// ////////// ////////// //////////
    // Finally block node manager.

    class finally_queue_t : public __resource_manager_base_t<finally_block_node_t>
    {
        typedef finally_queue_t       __self_t;
        typedef finally_block_node_t  __node_t;
        typedef exec_method_block_t   __block_t;

        typedef __resource_manager_base_t<finally_block_node_t> __super_t;

    public:

        // Constructor.
        using __super_t::__super_t;

        // Enqueue a finally block.
        __AlwaysInline void enque(__block_t * block)
        {
            _A(block != nullptr);

            __node_t * node = this->__acquire_node(block);
            node->next = nullptr;

            if(__current == nullptr)
            {
                __current = this->head = node;
            }
            else
            {
                __current->next = node;
                __current = node;
            }
        }

        // Dequeue a finally block.
        __AlwaysInline __block_t * deque()
        {
            __node_t * node = this->head;
            if(node == nullptr)
                return nullptr;

            this->head = node->next;
            if(this->head == nullptr)
                __current = nullptr;

            __block_t * block = node->block;
            this->__release_node(node);

            return block;
        }

    private:
        __node_t * __current = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////
    // exec_method_t

    // Executing method.
    class exec_method_t : public object_t
    {
    public:

        // Constructor.
        exec_method_t(command_t ** commands, uint16_t ref_objects, msize_t stack_unit_size)
            : commands(commands), ref_objects(ref_objects), stack_unit_size(stack_unit_size)
        {
            _A(commands != nullptr);
        }

        uint16_t ref_objects;
        msize_t  stack_unit_size;

        // Commands.
        command_t ** commands;

        // Block manager.
        exec_method_block_manager_t * block_manager = nullptr;

        // Returns method name.
        string_t get_name() const;

        // Runtime method.
        rt_method_base_t * rt_method = nullptr;

        // Finds block contains the specified point.
        template<typename _f_t>
        __AlwaysInline void find_block(command_t ** point, _f_t f)
        {
            if(block_manager == nullptr)
                return;

            block_manager->find_block(point, f);
        }

        // Converts to string.
        X_TO_STRING
    };

    ////////// ////////// ////////// ////////// //////////

    // Calling stub.
    struct __calling_stub_t
    {
        rt_stack_unit_t *   lp;         // Local variables top.
        command_t **        current;    // Current command.
        exec_method_t *     method;     // Method.
    };

    class executor_env_t;

    // Context of command executing.
    class command_execute_context_t : public object_t, public no_copy_ctor_t
    {
    public:

        // Constructor.
        command_execute_context_t(rt_heap_t * heap, executor_env_t & env,
                    size_t stack_size = __default_stack_size)
            : stack(__stack_buffer = memory_t::alloc_objs<rt_stack_unit_t>(nullptr, stack_size))
            , heap(heap), env(env)
        {
            _A(heap != nullptr);
        }

        executor_stack_t    stack;              // Stack.
        rt_string_pool_t    string_pool;        // String pool.
        rt_heap_t *         heap;               // Runtime heap.
        executor_env_t &    env;                // Executing environment.

        command_t ** current = nullptr;         // Current command.

        exception_stack_t exception_stack;      // Exception stack.
        finally_queue_t   finally_queue;        // Finally queue.

        // Pushes calling context.
        __AlwaysInline void push_calling(exec_method_t * method)
        {
            stack.push(__calling_stub_t { stack.lp(), this->current, method });
            this->current = method->commands;
            stack.set_lp(stack.top());
        }

        // Pushes calling context by commands.
        __AlwaysInline void push_calling(command_t ** commands)
        {
            stack.push(__calling_stub_t { stack.lp(), this->current, nullptr });
            this->current = commands;
            stack.set_lp(stack.top());
        }

        // Pops calling context.
        __AlwaysInline void pop_calling()
        {
            __calling_stub_t * stub = ((__calling_stub_t *)stack.lp() - 1);

            stack.set_top(stack.lp() - sizeof(__calling_stub_t) / sizeof(rt_stack_unit_t));

            this->current = stub->current;
            stack.set_lp(stub->lp);
        }

        // Returns whether stack is on head.
        __AlwaysInline bool stack_empty()
        {
            __calling_stub_t * stub = ((__calling_stub_t *)stack.lp() - 1);
            return stub->current == nullptr;
        }

        // Pops calling context.
        __AlwaysInline void pop_calling(const __calling_stub_t * p)
        {
            current = p->current;
            stack.set_lp(p->lp);
        }

        // Creates a new runtime string.
        rt_string_t * new_rt_string(const char_t * s);

        // Pushes an exception.
        __AlwaysInline void push_exception(rt_ref_t exception)
        {
            exception_stack.push(current, exception);
        }

        // Pushes an exception.
        __AlwaysInline void pop_exception()
        {
            exception_stack.pop();
        }

        // Returns current method.
        __AlwaysInline exec_method_t * current_method()
        {
            __calling_stub_t * stub = ((__calling_stub_t *)stack.lp() - 1);
            return stub->method;
        }

        // Destructor.
        virtual ~command_execute_context_t() override
        {
            memory_t::free(nullptr, __stack_buffer);
        }

    private:
        rt_stack_unit_t * __stack_buffer;
        pool_t            __memory_pool;
    };

    ////////// ////////// ////////// ////////// //////////

    typedef uint32_t cmd_value_t;

    // Command.
    class command_t
    {
        typedef command_t __self_t;
        typedef object_t  __super_t;

    public:

        #if EXEC_QUICK_EXECUTE

        cmd_value_t cmd_value = 0;

        #else   // EXEC_QUICK_EXECUTE

        // Executes this command.
        virtual void execute(command_execute_context_t & ctx) = 0;

        // Returns this string.
        virtual const string_t to_string(command_execute_context_t & ctx) const = 0;

        #endif  // EXEC_TRACE
    };

    ////////// ////////// ////////// ////////// //////////
    // exec_assemblies_t

    // Executing assemblies.
    class exec_assemblies_t : public object_t
    {
    public:

        // Appends a assembly.
        void append(rt_assembly_t * assembly, const string_t & name = _T(""),
                                              const string_t & package = _T(""));

        // Gets a assembly by package and name.
        rt_assembly_t * get(const string_t & package, const string_t & name);

        // Returns size.
        size_t size() const { return __map.size(); }

        // Last assembly.
        rt_assembly_t * last() const { return __last; }

    private:
        typedef std::tuple<string_t, string_t> __key_t;
        std::map<__key_t, rt_assembly_t *> __map;

        rt_assembly_t * __last = nullptr;
    };

    ////////// ////////// ////////// ////////// //////////
    // executor_env_t

    // Executor environment.
    class executor_env_t : public analyzer_env_t, public no_copy_ctor_t
    {
        typedef analyzer_env_t __super_t;
        typedef executor_env_t __self_t;

        typedef al::heap_t<command_t *[]> __command_heap_t;
        typedef al::heap_t<exec_method_block_t []> __method_block_heap_t;

    public:
        executor_env_t(memory_t * memory, rt_pool_t & rpool, rt_assemblies_t & assemblies)
            : __super_t(memory, rpool, assemblies), __command_heap(memory)
        { }

        // Execute method of runtime method.
        exec_method_t * exec_method_of(rt_method_t * rt_method);

        // Execute method of runtime generic method.
        exec_method_t * exec_method_of(rt_generic_method_t * rt_generic_method);

        // Creates an array of command_t *.
        command_t ** new_commands(size_t count);

        // Creates an array of exec_method_block_t.
        exec_method_block_t * new_blocks(size_t count);

    private:
        std::map<void *, exec_method_t *> __method_map;
        __command_heap_t        __command_heap;
        __method_block_heap_t   __method_block_heap;

        // Gets exec_method_t from cache, returns nullptr if not found.
        exec_method_t * __from_cache(void * rt_method);

        // Execute method of runtime method with template arguments.
        exec_method_t * __parse_commands(rt_method_t * rt_method, rt_type_t * host_type = nullptr,
                                         const generic_param_manager_t * gp_manager = nullptr);
    };

    ////////// ////////// ////////// ////////// //////////
    // executor_t

    // Executor.
    class executor_t : public object_t
    {
    public:

        // Constructor.
        executor_t(rt_context_t & ctx)
            : env(ctx.memory, ctx.rpool, ctx.assemblies), __ctx(ctx)
        { }

        // Executes commands.
        void execute();

        executor_env_t env;

    private:
        rt_context_t & __ctx;
    };

    ////////// ////////// ////////// ////////// //////////

    // Executes with specified context.
    void execute(rt_context_t & ctx);

    ////////// ////////// ////////// ////////// //////////

    #undef __AlwaysInline

} } }

#endif  // __EXEC_H__
