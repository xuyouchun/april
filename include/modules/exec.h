#ifndef __EXEC_H__
#define __EXEC_H__

#include <common.h>
#include <algorithm.h>
#include <jc.h>
#include <core.h>
#include <rt.h>

namespace X_ROOT_NS::modules::exec {

    namespace
    {
        using namespace core;
        using namespace rt;
    }

    #define __AlwaysInline X_ALWAYS_INLINE

    #define EXEC_EXECUTE_MODEL_VIRTUAL  1
    #define EXEC_EXECUTE_MODEL_MAUNAL   2
    #define EXEC_EXECUTE_MODEL_INLINE   3

    #define EXEC_TRACE          3  // 0:none, 1:trace, 2:trace details, 3:trace more details
    #define EXEC_EXECUTE_MODEL  EXEC_EXECUTE_MODEL_VIRTUAL

    const size_t __default_stack_size = 1024 * 1024;

    ////////// ////////// ////////// ////////// //////////

    class executor_env_t;
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
        template<typename t> constexpr size_t __alignf() _NE
        {
            return _alignf(sizeof(t), sizeof(rt_stack_unit_t));
        }

        // Stack operations.
        template<typename t, bool tiny> struct __stack_operation_t
        {
            // Pushes a unit.
            __AlwaysInline static void push(rt_stack_unit_t * top, t value) _NE
            {
                *(t *)top = value;
            }

            // Pops a unit.
            __AlwaysInline static t pop(rt_stack_unit_t * top) _NE
            {
                return *(t *)(top - __alignf<t>() / sizeof(rt_stack_unit_t));
            }

            // Pops a unit reference.
            __AlwaysInline static t & pop_reference(rt_stack_unit_t * top) _NE
            {
                return *(t *)(top - __alignf<t>() / sizeof(rt_stack_unit_t));
            }
        };

        // Stack operations. when value size is less or equals than unit.
        template<typename t> struct __stack_operation_t<t, true>
        {
            union __tmp_t { t value; rt_stack_unit_t unit; };

            // Pushes a unit.
            __AlwaysInline static void push(rt_stack_unit_t * top, t value) _NE
            {
                __tmp_t tmp = { value };
                *top = reinterpret_cast<rt_stack_unit_t>(tmp.unit);
            }

            // Pops a unit.
            __AlwaysInline static t pop(rt_stack_unit_t * top) _NE
            {
                return ((__tmp_t *)(top - 1))->value;
            }

            // Pops a unit reference.
            __AlwaysInline static t & pop_reference(rt_stack_unit_t * top) _NE
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
        executor_stack_t(rt_stack_unit_t * buffer) _NE
            : __buffer(buffer), __top(buffer), __lp(buffer)
        {
            _A(buffer != nullptr);
        }

        // Allocate from stack.
        __AlwaysInline rt_stack_unit_t * alloc(size_t size) _NE
        {
            return alloc_units(
                _alignf(size, sizeof(rt_stack_unit_t)) / sizeof(rt_stack_unit_t)
            );
        }

        // Allocate units from stack.
        __AlwaysInline rt_stack_unit_t * alloc_units(size_t units) _NE
        {
            rt_stack_unit_t * top = this->top();
            __top += units;
            return top;
        }

        // Pushes a value.
        template<typename t> __AlwaysInline void push(const t & value) _NE
        {
            __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)>::push(__top, value);
            __top += __alignf<t>() / sizeof(rt_stack_unit_t);
        }

        // Pops a value.
        template<typename t> __AlwaysInline t pop() _NE
        {
            t value = __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)>::pop(__top);
            __top -= __alignf<t>() / sizeof(rt_stack_unit_t);

            return value;
        }

        // Pops a value reference.
        template<typename t> __AlwaysInline t & pop_reference() _NE
        {
            typedef __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)> stack_operation_t;

            t & value = stack_operation_t::pop_reference(__top);
            __top -= __alignf<t>() / sizeof(rt_stack_unit_t);

            return value;
        }

        // Pops units of specified count.
        template<typename t> __AlwaysInline rt_stack_unit_t * pop(int count) _NE
        {
            return __top -= __alignf<t>() / sizeof(rt_stack_unit_t) * count;
        }

        // Pops units of specified count.
        template<int count, typename t = rt_stack_unit_t>
        __AlwaysInline rt_stack_unit_t * pop() _NE
        {
            return __top -= __alignf<t>() / sizeof(rt_stack_unit_t) * count;
        }

        // Picks top unit.
        template<typename t> __AlwaysInline t pick() _NE
        {
            return __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)>::pop(__top);
        }

        // Picks top unit reference.
        template<typename t> __AlwaysInline t & pick_reference() _NE
        {
            return __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)>::pop_reference(__top);
        }

        // Picks top unit.
        template<typename t> __AlwaysInline t pick(int offset) _NE
        {
            return __stack_operation_t<t, sizeof(t) < sizeof(rt_stack_unit_t)>::pop(__top + offset);
        }

        // The top unit.
        __AlwaysInline rt_stack_unit_t * top() _NE { return __top; }

        // Sets the top unit.
        __AlwaysInline void set_top(rt_stack_unit_t * top) _NE { __top = top; }

        // Pops units of specified count.
        __AlwaysInline rt_stack_unit_t * pop(size_t unit_size) _NE { return __top -= unit_size; }

        // Increases top of unit size.
        __AlwaysInline void increase_top(int unit_size) _NE { __top += unit_size; }

        // Increases top of unit size.
        template<int unit_size> __AlwaysInline void increase_top() _NE { __top += unit_size; }

        // Local variables top.
        __AlwaysInline rt_stack_unit_t * lp() _NE { return __lp; }

        // Sets local variables top.
        __AlwaysInline void set_lp(rt_stack_unit_t * lp) _NE { __lp = lp; }

        // Returns stack bottom.
        __AlwaysInline rt_stack_unit_t * bottom() _NE { return __buffer; }

        // Returns the position (a index) of current stack top.
        __AlwaysInline int pos() _NE { return __top - __buffer; }

        X_TO_STRING_IMPL(_T("executor_stack_t"))

    private:
        rt_stack_unit_t * __top, * __lp;
        rt_stack_unit_t * __buffer;
    };

    ////////// ////////// ////////// ////////// //////////

    // Returns a string hashcode.
    template<typename _size_t>
    __AlwaysInline _size_t __hash(_size_t hash, _size_t v) _NE
    {
        const _size_t mask = (_size_t)0x0F << (sizeof(_size_t) - 4);

        hash = (hash << 4) + v;
        _size_t x;

        if ((x = hash & mask) != 0) 
        {
            hash ^= (x >> (sizeof(_size_t) - (sizeof(_size_t) >> 2))); 
            hash &= ~x;
        }

        return hash;
    }

    // Returns a string hashcode.
    template<typename _size_t, typename _char_t>
    _size_t elf_hash(const _char_t * s, size_t length) _NE
    {
        const _size_t * t = (const _size_t *)s;
        size_t s_len = length * sizeof(_char_t);
        size_t t_len = _align(s_len, sizeof(_size_t));
        const _size_t * t_end = t + (t_len / sizeof(_size_t));

        _size_t hash = 0;

        for (; t < t_end; t++)
        {
            hash = __hash(hash, *t);
        }

        if (s_len > t_len)
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
            __string_key_t(const char_t * s, rt_length_t length) _NE
                : s(s), length(length) { }

            rt_length_t length;
            const char_t * s;

            bool operator == (const __string_key_t & other) const _NE
            {
                if (s == other.s)
                    return true;

                if (length != other.length)
                    return false;

                return al::strcmp(s, other.s) == cmp_t::equals;
            }
        };

        struct __string_hash_t
        {
            size_t operator()(const __string_key_t & key) const _NE
            {
                return elf_hash<size_t>(key.s, key.length);
            }
        };

        struct __string_equals_t
        {
            bool operator()(const __string_key_t & key1, const __string_key_t & key2) const _NE
            {
                return key1 == key2;
            }
        };
    }

    // Runtime string pool.
    class rt_string_pool_t : public object_t
    {
    public:
        rt_string_pool_t() _NE { }

        // Gets the specified string.
        rt_string_t * get(const char_t * s, rt_length_t length);

        X_TO_STRING_IMPL(_T("rt_string_pool_t"))

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
            __resource_manager_base_t(memory_t * memory = nullptr) _NE
                : memory_base_t(memory) { }

            // The head node.
            __node_t * head = nullptr;

            // Returns whether the queue/stack is empty.
            bool empty() const _NE { return head == nullptr; }

            // Clean all nodes.
            __AlwaysInline void clean() _NE
            {
                while (head != nullptr)
                {
                    __node_t * node = head;
                    head = head->next;

                    __release_node(node);
                }
            }

            // Remove next node of specified node.
            __AlwaysInline void remove_next(__node_t * node) _NE
            {
                __node_t * next = node->next;
                node->next = next->next;

                __release_node(next);
            }

            // Destructor.
            virtual ~__resource_manager_base_t()
            {
                __node_t * node = head;
                while (node != nullptr)
                {
                    this->__free((void *)node);
                    node = node->next;
                }

                while (!__node_queue.empty())
                {
                    this->__free((void *)__node_queue.front());
                    __node_queue.pop();
                }
            }

        X_TO_STRING_IMPL(_T("__resource_manager_base_t"))

        protected:

            // Acquires a new node.
            template<typename ... _args_t>
            __node_t * __acquire_node(_args_t && ... args)
            {
                __node_t * node;

                if (!__node_queue.empty())
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
        exception_node_t(command_t ** throw_point, rt_ref_t exception, exec_method_t * method)
            _NE
            : throw_point(throw_point), exception(exception), method(method)
        { }

        command_t **    throw_point;    // The command where the exception throwed.
        rt_ref_t        exception;      // Exception.
        exec_method_t * method;         // Method which raised the exception.
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
        __AlwaysInline void push(command_t ** throw_point, rt_ref_t exception, exec_method_t * method)
            _NE
        {
            __node_t * node = this->__acquire_node(throw_point, exception, method);
            node->next = this->head;
            this->head = node;
        }

        // Pops an exception.
        __AlwaysInline void pop() _NE
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

        // Parent block.
        exec_method_block_t * parent;

        // Returns whether the block contains specified command.
        __AlwaysInline bool include(command_t ** command) _NE
        {
            return command >= start && command < end;
        }

        // Returns whether the block not contains specified command.
        __AlwaysInline bool exclude(command_t ** command) _NE
        {
            return command < start || command >= end;
        }

        // Returns whether the block contains/equals specified block.
        __AlwaysInline bool include(exec_method_block_t * block) _NE
        {
            return start <= block->start && end >= block->end;
        }

        // Returns whether the block not contains/equals specified block.
        __AlwaysInline bool exclude(exec_method_block_t * block) _NE
        {
            return start > block->start || end < block->end;
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
        template<method_xil_block_type_t _type, typename _f_t>
        void find_block(command_t ** point, _f_t f) _NE
        {
            exec_method_block_t * block = blocks, * block_end = block + count;

            #define __MatchBlockType(_block)                    \
                (_type == method_xil_block_type_t::__unknown__ || _type == _block->type)

            for (; block < block_end; block++)
            {
                if (__MatchBlockType(block) && point >= block->start && point < block->end)
                {
                    if (!f(block))
                        return;

                    break;
                }
            }

            while ((block = block->parent) != nullptr)
            {
                if (__MatchBlockType(block) && !f(block))
                    return;
            }

            #undef __MatchBlockType
        }
    };

    ////////// ////////// ////////// ////////// //////////

    typedef void * block_node_identity_t;

    // The action after finally block executed.
    X_ENUM(block_node_finally_action_t)

        // No action.
        default_ = __default__,

        // Jump to next command.
        jmp_to_next_command,

        // Executes following command.
        execute_following_command,

    X_ENUM_END

    //-------- ---------- ---------- ---------- ----------

    // Block node.
    class block_node_t : public object_t
    {
        typedef block_node_t                __self_t;
        typedef block_node_identity_t       __identity_t;
        typedef block_node_finally_action_t __action_t;

    public:

        // Constructor.
        block_node_t(__identity_t identity, exec_method_block_t * block,
                exception_node_t * exception_node = nullptr) _NE
            : block(block), exception_node(exception_node), identity(identity)
        { }

        // Group identity.
        __identity_t identity;

        // Block.
        exec_method_block_t * block;

        // Exception node.
        exception_node_t * exception_node;

        // The action after finally block executed.
        __action_t action = __action_t::default_;

        union
        {
            // Next command.
            command_t ** next_command;

            // Following command.
            command_t * following_command;
        };

        // Sets next command.
        __AlwaysInline void set_next_command(command_t ** next_command) _NE
        {
            _A(next_command != nullptr);

            this->next_command = next_command;
            this->action = __action_t::jmp_to_next_command;
        }

        // Sets following command.
        __AlwaysInline void set_following_command(command_t * following_command) _NE
        {
            _A(following_command != nullptr);

            this->following_command = following_command;
            this->action = __action_t::execute_following_command;
        }

        // Next node in chain.
        __self_t * next;

        X_TO_STRING_IMPL(_T("block_node_t"))
    };

    ////////// ////////// ////////// ////////// //////////
    // Block node manager.

    class block_queue_t : public __resource_manager_base_t<block_node_t>
    {
        typedef block_queue_t           __self_t;
        typedef block_node_t            __node_t;
        typedef exec_method_block_t     __block_t;
        typedef block_node_identity_t   __identity_t;

        typedef __resource_manager_base_t<block_node_t> __super_t;

    public:

        // Constructor.
        using __super_t::__super_t;

        // Begin a block group.
        __AlwaysInline void begin_group(__identity_t identity) _NE
        {
            __old_head = this->head;
            __current  = nullptr;
            __identity = identity;
        }

        // The end of a group.
        // Returns true if blocks overwrited, happend when throws exceptions in finally block.
        __AlwaysInline bool end_group() _NE
        {
            if (__current != nullptr)
            {
                __node_t * h = __old_head;
                while (__old_head != nullptr && __old_head->identity == __current->identity
                        && __current->block->include(__old_head->block))
                {
                    __release_node(__old_head);
                    __old_head = __old_head->next;
                }

                __current->next = __old_head;
                return h != __old_head;
            }

            return false;
        }

        // Returns last node that enqueue.
        __AlwaysInline __node_t * last_current() _NE
        {
            return __current;
        }

        // Enqueue a finally block.
        __AlwaysInline void enque_finally(__block_t * block) _NE
        {
            __enque(block, nullptr);
        }

        // Enqueue a catch block.
        __AlwaysInline void enque_catch(__block_t * block, exception_node_t * exception_node) _NE
        {
            __enque(block, exception_node);
        }

        // Dequeue a block.
        __AlwaysInline __node_t * deque() _NE
        {
            __node_t * node = this->head;
            if (node == nullptr)
                return nullptr;

            this->head = node->next;
            this->__release_node(node);

            return node;
        }

        // Returns head block.
        __AlwaysInline __node_t * pick() _NE
        {
            if (this->head != nullptr)
                return this->head;

            return nullptr;
        }

        // Dequeue a block for executing.
        __AlwaysInline __node_t * deque_for_executing() _NE
        {
            __node_t * node = this->head;
            if (node == nullptr)
                return nullptr;

            this->head = node->next;

            if (__executing == nullptr)
            {
                __executing = node;
            }
            else
            {
                node->next = __executing;
                __executing = node;
            }

            return node;
        }

        // Pop an executing block.
        __AlwaysInline __node_t * pop_executing() _NE
        {
            __node_t * node = __executing;
            __executing = __executing->next;

            this->__release_node(node);

            return node;
        }

        // Returns current executing block.
        __AlwaysInline __node_t * current_executing() _NE
        {
            return __executing;
        }

        // Clean all nodes.
        __AlwaysInline void clean() _NE
        {
            __super_t::clean();
        }

    private:
        __node_t * __current   = nullptr;
        __node_t * __old_head  = nullptr;
        __node_t * __executing = nullptr;

        __identity_t __identity;

        // Enqueue a block.
        __AlwaysInline void __enque(__block_t * block, exception_node_t * exception_node) _NE
        {
            _A(block != nullptr);

            __node_t * node = this->__acquire_node(__identity, block, exception_node);
            node->next = nullptr;

            if (__current == nullptr)
            {
                __current  = node;
                this->head = node;
            }
            else
            {
                __current->next = node;
                __current = node;
            }
        }

    };

    ////////// ////////// ////////// ////////// //////////
    // exec_method_t

    // Executing method.
    class exec_method_t : public object_t
    {
        friend class __method_parser;

    public:

        // Constructor.
        template<typename _rt_method_t>
        exec_method_t(executor_env_t & env, _rt_method_t * method);

        // Initialize this executable method.
        void initialize();

        // Block manager.
        exec_method_block_manager_t * block_manager = nullptr;

        // Returns method name.
        string_t get_name() const;

        // Runtime method.
        rt_method_base_t * method = nullptr;

        // Returns commands.
        command_t ** get_commands();

        // Finds block contains the specified point.
        template<method_xil_block_type_t _type, typename _f_t>
        __AlwaysInline void find_block(command_t ** point, _f_t f) _NE
        {
            if (block_manager == nullptr)
                return;

            block_manager->find_block<_type>(point, f);
        }

        // Converts to string.
        X_TO_STRING

        // Deconstructor.
        virtual ~exec_method_t() override;

        uint16_t    ref_objects;
        msize_t     stack_unit_size;
        msize_t     this_offset;

    private:
        // Commands.
        union
        {
            struct
            {
                bool    __commands_uninitialized : 1;
            };

            command_t ** __commands;
            object_t *   __method_parser;
        };

        // Returns method parser.
        object_t * __get_method_parser();
    };

    ////////// ////////// ////////// ////////// //////////

    // Calling stub.
    struct __calling_stub_t
    {
        rt_stack_unit_t *   lp;             // Local variables top.
        command_t **        current;        // Current command.
        exec_method_t *     method;         // Method.
    };

    // Context of command executing.
    class command_execute_context_t : public object_t, public no_copy_ctor_t
    {
    public:

        // Constructor.
        command_execute_context_t(rt_heap_t * heap, executor_env_t & env,
                    size_t stack_size = __default_stack_size) _NE
            : stack(__stack_buffer = (rt_stack_unit_t *)heap->alloc(
                                            stack_size * sizeof(rt_stack_unit_t)))
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
        block_queue_t     block_queue;          // Block queue.

        // Pushes calling context.
        __AlwaysInline void push_calling(exec_method_t * method) _NE
        {
            stack.push(__calling_stub_t { stack.lp(), this->current, method });
            this->current = method->get_commands();
            stack.set_lp(stack.top());

            stack.increase_top(method->stack_unit_size);
        }

        // Pushes calling context by commands.
        __AlwaysInline void push_calling(command_t ** commands) _NE
        {
            stack.push(__calling_stub_t { stack.lp(), this->current, nullptr });
            this->current = commands;
            stack.set_lp(stack.top());
        }

        // Pops calling context.
        __AlwaysInline void pop_calling() _NE
        {
            __calling_stub_t * stub = ((__calling_stub_t *)stack.lp() - 1);

            stack.set_top(stack.lp() - sizeof(__calling_stub_t) / sizeof(rt_stack_unit_t));

            this->current = stub->current;
            stack.set_lp(stub->lp);
        }

        // Returns current calling stub.
        __AlwaysInline __calling_stub_t * stub() _NE
        {
            return (__calling_stub_t *)stack.lp() - 1;
        }

        // Switch to specified method.
        __AlwaysInline void switch_to(exec_method_t * method) _NE
        {
            __calling_stub_t * stub = this->stub();

            exec_method_t * old_method = stub->method;
            stub->method = method;

            stack.increase_top(method->stack_unit_size - old_method->stack_unit_size);

            this->current = method->get_commands();
        }

        // Returns whether stack is on head.
        __AlwaysInline bool stack_empty() _NE
        {
            __calling_stub_t * stub = ((__calling_stub_t *)stack.lp() - 1);
            return stub->current == nullptr;
        }

        // Pops calling context.
        __AlwaysInline void pop_calling(const __calling_stub_t * p) _NE
        {
            current = p->current;
            stack.set_lp(p->lp);
        }

        // Restore stack to it's initialize state.
        __AlwaysInline void restore_stack() _NE
        {
            stack.set_top(stack.lp() + current_method()->stack_unit_size);
        }

        // Creates a new runtime string.
        rt_string_t * new_rt_string(const char_t * s) _NE;

        // Pushes an exception.
        __AlwaysInline void push_exception(rt_ref_t exception) _NE
        {
            exception_stack.push(current - 1, exception, current_method());
        }

        // Pushes an exception.
        __AlwaysInline void pop_exception() _NE
        {
            exception_stack.pop();
        }

        // Returns current method.
        __AlwaysInline exec_method_t * current_method() _NE
        {
            __calling_stub_t * stub = ((__calling_stub_t *)stack.lp() - 1);
            return stub->method;
        }

        // Destructor.
        virtual ~command_execute_context_t() override
        {
            heap->free(__stack_buffer);
        }

        X_TO_STRING_IMPL(_T("command_execute_context_t"))

    private:
        rt_stack_unit_t * __stack_buffer;
        pool_t            __memory_pool;
    };

    ////////// ////////// ////////// ////////// //////////

    typedef uint16_t command_value_t;

    typedef void (*command_execute_method_t)
        (command_t * command, command_execute_context_t & ctx);

    typedef const string_t (*command_to_string_method_t)(const command_t * command);

    // Command.
    class command_t
    #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL && EXEC_TRACE
        : public object_t
    #endif
    {
        typedef command_t __self_t;
        typedef object_t  __super_t;

    public:

        #if EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_MANUAL

        // #pragma message("Execute model: MANUAL METHOD")
        command_execute_method_t   execute_method;

        #if EXEC_TRACE
        command_to_string_method_t to_string_method;
        #endif  // EXEC_TRACE

        #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_INLINE

        // #pragma message("Execute model: INLINE")
        command_t(command_value_t cmd_value) _NE : cmd_value(cmd_value) { }

        const command_value_t cmd_value;

        #elif EXEC_EXECUTE_MODEL == EXEC_EXECUTE_MODEL_VIRTUAL

        // #pragma message("Execute model: VIRTUAL METHOD")
        // Executes this command.
        virtual void execute(command_execute_context_t & ctx) = 0;

        #if EXEC_TRACE
        // Returns this string.
        virtual const string_t to_string() const override { return _T("[command]"); }
        #endif // EXEC_TRACE

        #else

        #error unknown EXEC_EXECUTE_MODEL

        #endif  // EXEC_EXECUTE_MODEL = xxx
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

        X_TO_STRING_IMPL(_T("exec_assemblies_t"))

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

        // Returns execute method of runtime method.
        exec_method_t * exec_method_of(rt_method_t * rt_method);

        // Returns execute method of runtime generic method.
        exec_method_t * exec_method_of(rt_generic_method_t * rt_generic_method);

        // Returns execute method of runtime method or generic method.
        exec_method_t * exec_method_of(rt_method_base_t * rt_method_base);

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

        // Returns execute method of runtime method / generic method.
        template<typename _rt_method_t>
        exec_method_t * __exec_method_of(_rt_method_t * method);
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

        X_TO_STRING_IMPL(_T("executor_t"))

    private:
        rt_context_t & __ctx;
    };

    ////////// ////////// ////////// ////////// //////////

    // Executes with specified context.
    void execute(rt_context_t & ctx);

    ////////// ////////// ////////// ////////// //////////

    #undef __AlwaysInline

}   // namespace X_ROOT_NS::modules::exec

#endif  // __EXEC_H__
