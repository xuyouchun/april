
#include <native.h>
#include <exec.h>
#include <rtlib.h>

#include "../exec/parser.h"
#include "ll_builder.h"
#include "native_abi.h"

#include <deque>
#include <map>
#include <memory>
#include <set>

namespace X_ROOT_NS::modules::core {

    // Defined in core/xil.cpp: the text of a xil.
    string_t __to_string(const xil_t * xil) _NE;

}   // namespace X_ROOT_NS::modules::core

namespace X_ROOT_NS::modules::native {

    using namespace core;
    using namespace rt;
    using namespace exec;

    typedef native_error_code_t __e_t;

    ////////// ////////// ////////// ////////// //////////

    X_ENUM_INFO(native_error_code_t)

        // The xil is not supported yet.
        X_D(unsupported_xil,        _T("native: unsupported xil: %1%"))

        // The type is not supported yet.
        X_D(unsupported_type,       _T("native: unsupported type: %1%"))

        // The method is not supported yet.
        X_D(unsupported_method,     _T("native: unsupported method '%1%': %2%"))

        // The program has no entry point.
        X_D(no_entry_point,         _T("native: assembly '%1%' has no entry point"))

        // The runtime library function is not available to native programs.
        X_D(unsupported_libfunc,    _T("native: runtime library function '%1%' %2%"))

        // Internal error: the evaluation stack is inconsistent.
        X_D(bad_stack,              _T("native: inconsistent evaluation stack in '%1%': %2%"))

    X_ENUM_INFO_END

    ////////// ////////// ////////// ////////// //////////

    namespace
    {
        namespace llb = ll;

        // Runtime library functions that need the metadata of the program (an assembly
        // analyzer), which native programs do not have yet.
        const char * const __metadata_libfuncs[] = {
        };

        // Converts to a narrow string.
        std::string __narrow(const char_t * s)
        {
            return string_convert<char_t, char>(s);
        }

        // Converts a string of the virtual machine (UTF-16) to a narrow string.
        std::string __narrow(const rt_char_t * s)
        {
            return string_convert<char_t, char>(to_host_string(s));
        }

        // The code units of a string of the virtual machine: a string constant of the program has them
        // as they are (16 bits each).
        std::u32string __units_of(const rt_char_t * s)
        {
            std::u32string units;
            if (s != nullptr)
            {
                for (; *s != 0; s++)
                    units += (char32_t)(uint32_t)*s;
            }

            return units;
        }

        // The code units of a string of the program (a String value, it has its length: the character 0 can be in it).
        std::u32string __units_of_string(const rt_char_t * s)
        {
            std::u32string units;
            for (int32_t k = 0, length = rt_string_length(s); k < length; k++)
                units += (char32_t)(uint32_t)s[k];

            return units;
        }

        // Converts a narrow string (llvm names) to string_t, for messages.
        string_t __wide(const std::string & s)
        {
            return string_convert<char, char_t>(s);
        }

        //-------- ---------- ---------- ---------- ----------
        // Arithmetic helpers: the operand types of the xil commands (xil_type_t) and the
        // result types follow the tables of core/__vtype_traits.h.

        // Properties of a numeric xil type.
        struct __num_t
        {
            bool        valid = false;
            bool        is_float = false;
            bool        is_signed = false;
            int         bits = 0;
            const char *ll = "";
        };

        __num_t __num_of(xil_type_t xt)
        {
            __num_t n;
            n.valid = true;

            switch (xt)
            {
                case xil_type_t::int8:    n.is_signed = true;  n.bits = 8;  n.ll = "i8";  break;
                case xil_type_t::uint8:                        n.bits = 8;  n.ll = "i8";  break;
                case xil_type_t::int16:   n.is_signed = true;  n.bits = 16; n.ll = "i16"; break;
                case xil_type_t::uint16:                       n.bits = 16; n.ll = "i16"; break;
                case xil_type_t::int32:   n.is_signed = true;  n.bits = 32; n.ll = "i32"; break;
                case xil_type_t::uint32:                       n.bits = 32; n.ll = "i32"; break;
                case xil_type_t::int64:   n.is_signed = true;  n.bits = 64; n.ll = "i64"; break;
                case xil_type_t::uint64:                       n.bits = 64; n.ll = "i64"; break;

                case xil_type_t::float_:
                    n.is_float = true; n.is_signed = true; n.bits = 32; n.ll = "float";
                    break;

                case xil_type_t::double_:
                    n.is_float = true; n.is_signed = true; n.bits = 64; n.ll = "double";
                    break;

                default:
                    n.valid = false;
                    break;
            }

            return n;
        }

        // The type of a binary arithmetic / bit operation of two numeric types.
        xil_type_t __common_type(xil_type_t t1, xil_type_t t2)
        {
            __num_t n1 = __num_of(t1), n2 = __num_of(t2);
            if (!n1.valid || !n2.valid)
                return xil_type_t::empty;

            if (t1 == xil_type_t::double_ || t2 == xil_type_t::double_)
                return xil_type_t::double_;

            if (t1 == xil_type_t::float_ || t2 == xil_type_t::float_)
                return xil_type_t::float_;

            if (t1 == xil_type_t::uint64 || t2 == xil_type_t::uint64)
                return xil_type_t::uint64;

            if (t1 == xil_type_t::int64 || t2 == xil_type_t::int64)
                return xil_type_t::int64;

            if (t1 == xil_type_t::uint32 || t2 == xil_type_t::uint32)
            {
                xil_type_t other = t1 == xil_type_t::uint32? t2 : t1;
                return (other == xil_type_t::int8 || other == xil_type_t::int16
                        || other == xil_type_t::int32)? xil_type_t::int64 : xil_type_t::uint32;
            }

            return xil_type_t::int32;
        }

        // The type of a unitary operation.
        xil_type_t __unitary_type(xil_al_command_t cmd, xil_type_t t)
        {
            if (!__num_of(t).valid)
                return xil_type_t::empty;

            switch (t)
            {
                case xil_type_t::uint32:
                    return cmd == xil_al_command_t::minus? xil_type_t::int64 : xil_type_t::uint32;

                case xil_type_t::int64:
                case xil_type_t::uint64:
                case xil_type_t::float_:
                case xil_type_t::double_:
                    return t;

                default:
                    return xil_type_t::int32;
            }
        }

        // Converts a numeric value from a xil type to another one.
        llb::value_t __convert(llb::function_t & f, const llb::value_t & v,
                                                    xil_type_t from, xil_type_t to)
        {
            __num_t nf = __num_of(from), nt = __num_of(to);

            if (nf.is_float)
            {
                if (nt.bits == nf.bits)
                    return v;

                return f.cast(nt.bits > nf.bits? "fpext" : "fptrunc", v, nt.ll);
            }

            if (nt.is_float)
                return f.cast(nf.is_signed? "sitofp" : "uitofp", v, nt.ll);

            if (nt.bits == nf.bits)
                return v;

            return f.cast(nt.bits > nf.bits? (nf.is_signed? "sext" : "zext") : "trunc",
                          v, nt.ll);
        }

        // Numeric properties for conversions: like __num_of, with bool and char.
        __num_t __conv_num_of(xil_type_t xt)
        {
            __num_t n = __num_of(xt);
            if (n.valid)
                return n;

            n.valid = true;
            n.bits = xt == xil_type_t::bool_? 8 : (int)sizeof(rt_char_t) * 8;
            n.is_signed = xt == xil_type_t::char_ && std::is_signed<rt_char_t>::value;
            n.ll = xt == xil_type_t::bool_? "i8" : (n.bits == 32? "i32" : "i16");

            if (xt != xil_type_t::bool_ && xt != xil_type_t::char_)
                n.valid = false;

            return n;
        }

        // Converts between numeric types (like static_cast), also bool and char.
        // Returns an invalid value if the types are not supported.
        llb::value_t __convert_any(llb::function_t & f, const llb::value_t & v,
                                                        xil_type_t from, xil_type_t to)
        {
            __num_t nf = __conv_num_of(from), nt = __conv_num_of(to);
            if (!nf.valid || !nt.valid)
                return llb::value_t { };

            if (v.type != nf.ll)
                return llb::value_t { };

            if (from == to)
                return v;

            // To bool: not zero.
            if (to == xil_type_t::bool_)
            {
                llb::value_t c = nf.is_float?
                    f.fcmp("une", v, nf.bits == 32? llb::const_float(0.0f) : llb::const_double(0.0))
                  : f.icmp("ne", v, llb::const_int(nf.ll, 0));

                return f.cast("zext", c, "i8");
            }

            if (nf.is_float)
            {
                if (nt.is_float)
                    return nt.bits == nf.bits? v
                        : f.cast(nt.bits > nf.bits? "fpext" : "fptrunc", v, nt.ll);

                return f.cast(nt.is_signed? "fptosi" : "fptoui", v, nt.ll);
            }

            if (nt.is_float)
                return f.cast(nf.is_signed? "sitofp" : "uitofp", v, nt.ll);

            if (nt.bits == nf.bits)
                return v;

            return f.cast(nt.bits > nf.bits? (nf.is_signed? "sext" : "zext") : "trunc",
                          v, nt.ll);
        }

        // The LLVM type of a xil type that is a value of an unmanaged type (the content of a pointer), empty if it is not.
        std::string __ll_of_content(xil_type_t xt)
        {
            if (xt == xil_type_t::ptr)
                return "ptr";

            __num_t n = __conv_num_of(xt);
            return n.valid? n.ll : std::string();
        }

        // Makes an identifier part: [A-Za-z0-9_] only.
        std::string __ident(const std::string & s)
        {
            std::string r;
            for (char c : s)
                r += (std::isalnum((unsigned char)c) || c == '_')? c : '_';

            return r;
        }

        //-------- ---------- ---------- ---------- ----------

        // The generator.
        // The type that is the template of the current generic type (Box<T>), or a class that is nested
        // in it (the state of an iterator), is the one of the instance that runs.
        static rt_type_t * __current_instance_of(rt_type_t * type, rt_type_t * host_type,
                                                 assembly_analyzer_t & analyzer)
        {
            if (type == nullptr || host_type->get_kind() != rt_type_kind_t::generic
                    || !is_general(type) || !((rt_general_type_t *)type)->is_generic_template())
                return type;

            rt_generic_type_t * host = (rt_generic_type_t *)host_type;
            if (!is_host_type_template(type, host->template_))
                return type;

            if (type == (rt_type_t *)host->template_)
                return host_type;

            return analyzer.to_generic_type((rt_general_type_t *)type);
        }

        class __generator_t
        {
        public:

            // Constructor.
            // `denied` has the methods that only Method.Invoke needs and that could not be generated
            // by a former try (see generate_ir): they have a body that traps, and no invoker.
            __generator_t(rt_context_t & ctx, const std::set<string_t> & denied)
                : __env(ctx.memory, ctx.rpool, ctx.assemblies), __module("april"), __denied(denied)
            { }

            // Thrown when a method that only Method.Invoke needs could not be generated: the module is
            // made again without it.
            struct retry_t { string_t method; };

            // Generates the module text.
            std::string generate();

        private:

            // A method to generate.
            struct __fn_t
            {
                // The method: a general method, or a generic method (a method of a generic
                // type, or a generic method), which is generated for its type arguments.
                rt_method_base_t *          base;
                rt_method_t *               method;         // The method (the template).
                std::unique_ptr<generic_param_manager_t> gp_mgr;    // Of a generic method.

                std::string                 name;           // Function name in IR.
                string_t                    display;        // For messages.

                std::vector<std::string>    param_types;    // LLVM types.
                std::vector<rt_type_t *>    param_rt_types;
                std::string                 ret_type;       // LLVM type, "void" for none.

                // A struct result is written to an address that the caller passes as the
                // first (hidden) parameter: ret_type is void then.
                bool                        returns_struct = false;

                // The arguments of a pack (T ... items) are the params from pack_start.
                int                         pack_start = -1;
                int                         pack_count = 0;

                bool                        queued = false;

                // Only Method.Invoke needs the function: the method that made it needed (itself for
                // the first of them), null for the functions that the program calls.
                __fn_t *                    origin = nullptr;

                // The function is not generated: it traps (a former try failed).
                bool                        denied = false;
            };

            executor_env_t          __env;
            llb::module_t           __module;
            std::set<string_t>      __denied;
            bool                    __invoke_phase = false;     // The functions of Invoke are made.
            bool                    __uses_trap = false;        // llvm.trap

            std::map<rt_method_base_t *, std::unique_ptr<__fn_t>>   __fns;
            std::deque<__fn_t *>    __queue;
            std::set<std::string>   __used_libfuncs;
            size_t                  __fn_index = 0;
            bool                    __uses_memcpy = false;
            bool                    __uses_memset = false;

            // The intrinsics of the arithmetic that raises on overflow, with their integer types:
            // llvm.sadd.with.overflow.i32 -> i32 .
            std::map<std::string, std::string> __overflow_intrinsics;
            bool                    __uses_trunc = false;       // llvm.trunc.f64
            bool                    __uses_new_object = false;

            // The objects that hold the static fields of types: a global of each, which is
            // a root and is allocated before the entry point.
            struct __static_data_t { std::string global, desc; int64_t size; };
            std::map<rt_type_t *, __static_data_t> __static_data_map;

            // The delegates of virtual methods are bound to the implementation of the
            // instance: the virtual offset of the method that a method info stands for.
            std::map<std::string, int>  __virtual_delegate_offsets;

            // Virtual calls. The classes that are instantiated get a type descriptor
            // (see native_abi.h); the functions it holds are found once all call sites
            // are known: the offsets of the virtual methods and the interface methods
            // that are called.
            struct __class_t
            {
                rt_type_t *             type;
                std::string             name;           // The descriptor in IR.
                std::map<int, __fn_t *> vfunctions;     // virtual offset -> function
                std::map<std::pair<size_t, int>, __fn_t *> ifunctions;  // (interface, offset)
                std::map<size_t, __fn_t *> gfunctions;  // generic site -> function

                // The class is only the Type that the reflection of a type refers to: it has no
                // objects, its descriptor has the names and the tables of reflection, but no
                // functions.
                bool                    meta_only = false;

                // The class is the type of an array (Int32[], Int32[,]): it has the functions of the
                // class of Array<T> (they are the same), the name, and the key of its own.
                __class_t *             alias = nullptr;
            };

            // A virtual call of a generic method (or a call by an interface): the function that
            // runs is the method of the object with the generic arguments of the call, it is
            // generated for them. The functions follow the virtual functions in the descriptor.
            struct __generic_site_t
            {
                rt_type_t *                 declared;       // The type of the method called.
                int                         offset;         // The virtual offset, or of the interface.
                bool                        is_interface;
                size_t                      interface_index;
                std::vector<rt_type_t *>    atypes;
            };

            std::vector<__generic_site_t>              __generic_sites;

            std::vector<std::unique_ptr<__class_t>>    __classes;
            rt_reflection_t                            __reflection { __env };
            bool                                       __uses_reflection = false;
            bool                                       __uses_field_access = false;
            bool                                       __uses_enum_number = false;

            // Method.Invoke: the methods of the types of the program are generated, with the
            // functions that call them (one for each shape of signature).
            bool                                       __uses_invoke = false;

            // The attributes: the tables of the attributes of the members, and the constructors
            // that make them.
            bool                                       __uses_attributes = false;
            std::set<void *>                           __attributes_done;
            std::map<void *, std::string>              __attribute_tables;
            std::string __attributes_of(void * key);
            rt_reflect_member_t * __named_member_of(rt_type_t * type, const rt_char_t * name);
            void __encode_attribute_value(const cvalue_t & value, rt_type_t * type,
                        int64_t value_kind, int64_t size, uint64_t * bits, std::string * text);

            // What an argument is, when the parameter is not a number or a string (Object, Type).
            struct __attribute_extra_t
            {
                int64_t     kind = 0;
                int64_t     size = 0;
                std::string type = "null";
            };

            bool __encode_attribute_argument(rt_assembly_t * assembly, assembly_analyzer_t & analyzer,
                    rt_attribute_argument_t * argument, rt_type_t * param_type, int64_t value_kind,
                    int64_t size, uint64_t * bits, std::string * text, __attribute_extra_t * extra);
            rt_reflect_member_t * __constructor_member_of(rt_method_base_t * constructor);
            std::string __text_global_of(const rt_char_t * s);
            std::set<rt_method_base_t *>               __invoke_tried;
            std::map<std::string, std::string>         __shape_thunks;
            __fn_t * __invoker_fn_of(rt_reflect_member_t * member);
            std::string __shape_thunk_of(__fn_t & fn);
            std::string __interface_key_of(rt_type_t * type);
            bool __is_interface_member(rt_reflect_member_t * m);

            // The class of a number that a method returns is boxed by the runtime: it is a class of
            // the program, with its functions.
            void __promote_result_class(rt_reflect_member_t * m)
            {
                rt_type_t * type = (rt_type_t *)m->type;
                if (m->value_kind != (int64_t)rt_reflect_value_kind_t::value || type == nullptr
                        || (type->get_kind() != rt_type_kind_t::general
                            && type->get_kind() != rt_type_kind_t::generic))
                    return;

                try
                {
                    type->pre_new(__env);
                    __descriptor_of(type);
                }
                catch (const error_t &)
                {
                }
            }
            bool __queue_invokable();
            void __give_up(__fn_t * failed);
            bool __is_delegate_invoke(__fn_t & fn);
            void __emit_delegate_invoke(__fn_t & fn);
            std::map<rt_reflect_member_t *, std::string> __reflect_members;
            int                                        __array_blocks = 0;
            std::string __meta_descriptor_of(rt_type_t * type);
            std::string __static_data_global_of(rt_type_t * type);
            std::string __reflect_member_of(rt_reflect_member_t * member);
            std::string __reflect_meta_of(__class_t & c);
            std::map<rt_type_t *, __class_t *>         __class_of;
            std::set<std::pair<rt_type_t *, int>>      __virtual_sites;     // (declaring type, offset)
            std::vector<rt_type_t *>                   __interfaces;
            std::set<std::pair<size_t, int>>           __interface_sites;   // (interface, offset)
            bool                                       __uses_find_interface = false;
            bool                                       __uses_cast = false;
            bool                                       __uses_arrays = false;

            // The functions that delegates call, with the signature (instance, arguments...):
            // a static method gets a thunk that ignores the instance.
            std::map<__fn_t *, std::string>            __thunks;

            // The functions of the virtual tables of the primitive types (Int32.ToString): the
            // object that is boxed is the reference, the function takes the value as `this`.
            std::map<__fn_t *, std::string>            __unbox_thunks;
            int                                        __max_virtual_offset = -1;

            // Functions ----------

            // Returns the function of a method, queues it for generation.
            __fn_t * __function_of(rt_method_base_t * method);

            // The analyzer of the body of a function: it resolves the generic parameters.
            assembly_analyzer_t __analyzer_of(__fn_t & fn);

            // Computes the signature of a method.
            void __read_signature(__fn_t & fn);

            // Maps a runtime type to an LLVM type, or throws.
            std::string __ll_type_of(rt_type_t * type);
            static std::string __block_type_of(size_t size);

            // Maps a runtime type to a xil type.
            xil_type_t __xil_type_of(rt_type_t * type);

            // Generates the body of a function.
            void __emit(__fn_t & fn);

            // Emits the call of a runtime library function.
            llb::value_t __emit_libfunc_call(llb::function_t & f, __fn_t & caller,
                                    __fn_t & callee, const std::vector<llb::value_t> & args);

            // Returns the type descriptor of a class (generated at the end).
            const std::string & __descriptor_of(rt_type_t * type);

            // Returns the name of the function a delegate to the method calls.
            std::string __delegate_target_of(rt_method_base_t * method);

            // Returns the index of an interface in __interfaces.
            size_t __interface_index(rt_type_t * interface_type);

            // Finds the functions of the descriptors; returns whether something was added.
            bool __resolve_virtuals();

            // Emits the type descriptors.
            void __emit_descriptors();

            // Returns the symbol of a function in the virtual table of a class: the function, or the
            // function that takes the value from the boxed object when `this` is a value.
            std::string __virtual_symbol_of(__fn_t * fn);

            // Emits `main` and the initializer of the library functions.
            void __emit_main(__fn_t & entry);

            // The name of the global that holds a library function pointer.
            static std::string __libfunc_global(const std::string & name)
            {
                return "lf." + __ident(name);
            }
        };

        //-------- ---------- ---------- ---------- ----------

        // Maps a runtime type to an LLVM type, or throws.
        std::string __generator_t::__ll_type_of(rt_type_t * type)
        {
            if (type == nullptr)
                return "void";

            switch (type->get_vtype(__env))
            {
                case vtype_t::int8_:
                case vtype_t::uint8_:   return "i8";
                case vtype_t::int16_:
                case vtype_t::uint16_:  return "i16";
                case vtype_t::int32_:
                case vtype_t::uint32_:  return "i32";
                case vtype_t::int64_:
                case vtype_t::uint64_:  return "i64";
                case vtype_t::float_:   return "float";
                case vtype_t::double_:  return "double";
                case vtype_t::bool_:    return "i8";
                case vtype_t::char_:    return "i16";
                case vtype_t::ptr_:
                case vtype_t::string_:  return "ptr";       // A string is a const char_t *.
                case vtype_t::void_:    return "void";

                case vtype_t::mobject_:
                    // Class instances are references; a struct is a block of bytes with
                    // the layout of the interpreter.
                    if (is_ref_type(type->get_ttype(__env)))
                        return "ptr";

                    if (type->get_ttype(__env) == ttype_t::struct_)
                        return __block_type_of(type->get_size(__env));

                    break;

                default:
                    break;
            }

            throw _ED(__e_t::unsupported_type, type->get_name(__env));
        }

        // The type of a block of bytes that holds a struct. A pointer in it must stay a word when the
        // block is passed by value: LLVM splits an array to its elements, the bytes of a pointer
        // would be apart, and the collector (that looks for words) would not find the object.
        std::string __generator_t::__block_type_of(size_t size)
        {
            if (size > 0 && size % sizeof(void *) == 0)
                return "[" + std::to_string(size / sizeof(void *)) + " x i64]";

            return "[" + std::to_string(size) + " x i8]";
        }

        // Maps a runtime type to a xil type.
        xil_type_t __generator_t::__xil_type_of(rt_type_t * type)
        {
            // Instances of classes (also of generic ones) are references.
            if (is_ref_type(type->get_ttype(__env)))
                return xil_type_t::object;

            switch (type->get_vtype(__env))
            {
                case vtype_t::int8_:    return xil_type_t::int8;
                case vtype_t::uint8_:   return xil_type_t::uint8;
                case vtype_t::int16_:   return xil_type_t::int16;
                case vtype_t::uint16_:  return xil_type_t::uint16;
                case vtype_t::int32_:   return xil_type_t::int32;
                case vtype_t::uint32_:  return xil_type_t::uint32;
                case vtype_t::int64_:   return xil_type_t::int64;
                case vtype_t::uint64_:  return xil_type_t::uint64;
                case vtype_t::float_:   return xil_type_t::float_;
                case vtype_t::double_:  return xil_type_t::double_;
                case vtype_t::bool_:    return xil_type_t::bool_;
                case vtype_t::char_:    return xil_type_t::char_;
                case vtype_t::ptr_:     return xil_type_t::ptr;
                case vtype_t::string_:  return xil_type_t::string;
                case vtype_t::mobject_: return xil_type_t::object;
                default:                return xil_type_t::empty;
            }
        }

        // Computes the signature of a method.
        void __generator_t::__read_signature(__fn_t & fn)
        {
            rt_method_t * method = fn.method;
            rt_type_t * host_type = fn.base->get_host_type();

            assembly_analyzer_t analyzer = __analyzer_of(fn);

            // `this` is the first argument of an instance method.
            if (!((decorate_value_t)(*method)->decorate).is_static)
            {
                // The address of a struct, the value of a primitive type (e.g. Boolean).
                std::string this_type = __ll_type_of(host_type);

                fn.param_rt_types.push_back(host_type);
                fn.param_types.push_back(this_type[0] == '['? "ptr" : this_type);
            }

            method->get_assembly()->each_params((*method)->params,
                [&](int index, mt_param_t & mt_param) {

                    // A ref / out argument is the address of a variable.
                    bool is_addr = is_addr_param(mt_param.param_type);

                    // A pack (T ... items) is the arguments of its types, one by one.
                    if (mt_param.param_type == param_type_t::extends)
                    {
                        fn.pack_start = (int)fn.param_types.size();

                        analyzer.each_extend_params((*method)->params.at(index), [&](rt_type_t * type) {
                            fn.param_rt_types.push_back(type);
                            fn.param_types.push_back(__ll_type_of(type));
                            fn.pack_count++;
                            return true;
                        });

                        return true;
                    }

                    if (mt_param.param_type != param_type_t::default_ && !is_addr
                            && mt_param.param_type != param_type_t::params)
                    {
                        throw _ED(__e_t::unsupported_method, fn.display,
                                  _T("ref / out / params / extends arguments"));
                    }

                    rt_type_t * type = analyzer.get_type(mt_param.type);
                    if (type == nullptr)
                        throw _ED(__e_t::unsupported_type, _T("(unresolved argument type)"));

                    fn.param_rt_types.push_back(type);
                    fn.param_types.push_back(is_addr? "ptr" : __ll_type_of(type));

                    return true;
                }
            );

            fn.ret_type = __ll_type_of(fn.base->get_return_type(__env));
            if (fn.ret_type[0] == '[')
            {
                fn.returns_struct = true;
                fn.ret_type = "void";
            }
        }

        // Returns the function of a method, queues it for generation.
        __generator_t::__fn_t * __generator_t::__function_of(rt_method_base_t * base)
        {
            auto it = __fns.find(base);
            if (it != __fns.end())
                return it->second.get();

            std::unique_ptr<__fn_t> fn(new __fn_t());
            fn->base = base;

            switch (base->this_type())
            {
                case rt_member_type_t::general:
                    fn->method = static_cast<rt_method_t *>(base);
                    break;

                case rt_member_type_t::generic: {
                    rt_generic_method_t * generic = static_cast<rt_generic_method_t *>(base);
                    fn->method = generic->template_;

                    assembly_analyzer_t analyzer = to_analyzer(__env, fn->method->get_host_type());
                    fn->gp_mgr.reset(new generic_param_manager_t(analyzer, generic));
                }   break;

                default:
                    throw _ED(__e_t::unsupported_method, base->to_string(__env, true),
                              _T("a dynamic method"));
            }

            fn->display = base->to_string(__env, true);
            fn->denied  = __denied.count(fn->display) > 0;
            fn->name    ="april.m" + std::to_string(__fn_index++) + "."
                        + __ident(__narrow(base->get_name().c_str()));

            __read_signature(*fn);

            __fn_t * p = fn.get();
            __fns[base] = std::move(fn);

            return p;
        }

        // The analyzer of the body of a function: it resolves the generic parameters.
        assembly_analyzer_t __generator_t::__analyzer_of(__fn_t & fn)
        {
            rt_type_t * host_type = fn.base->get_host_type();
            return to_analyzer(__env, host_type, fn.gp_mgr.get());
        }

        //-------- ---------- ---------- ---------- ----------

        // Emits the call of a runtime library function: the arguments go to a block of
        // 8 byte units, with the layout of the interpreter's stack (see native_abi.h).
        llb::value_t __generator_t::__emit_libfunc_call(llb::function_t & f, __fn_t & caller,
                            __fn_t & callee, const std::vector<llb::value_t> & args)
        {
            std::string name = __narrow(callee.method->get_name().c_str());

            // The type of an object is its descriptor, that holds the name of the type after
            // the virtual functions: the number of them is the same for all the classes.
            if (name == "Reflection_GetType")
                return f.load("ptr", f.gep_bytes(args.at(0), -8));

            // The same object: the same reference; the hash code is made by the address (the
            // objects do not move).
            if (name == "Object_ReferenceEquals")
            {
                llb::value_t same = f.icmp("eq", args.at(0), args.at(1));
                return f.cast("zext", same, "i8");
            }

            if (name == "Object_GetHashCode")
            {
                llb::value_t address = f.cast("ptrtoint", args.at(0), "i64");
                llb::value_t shifted = f.binop("lshr", address, llb::const_int("i64", 4));
                llb::value_t hash = f.cast("trunc", shifted, "i32");

                return f.binop("and", hash, llb::const_int("i32", 0x7fffffff));
            }

            // The names follow the virtual functions: the name, then the full name, then the
            // tables of reflection (see native_abi.h).
            if (name == "Reflection_GetTypeName" || name == "Reflection_GetTypeFullName")
            {
                llb::value_t count = f.load("i64", llb::value_t { llb::types::ptr, "@april.vcount" });
                llb::value_t offset = f.binop("add", f.binop("mul", count, llb::const_int("i64", 8)),
                                                     llb::const_int("i64", name == "Reflection_GetTypeName"? 32 : 40));

                return f.load("ptr", f.gep(llb::types::i8, args.at(0), offset));
            }

            // The number of a box of an enum: the table of reflection of its type holds the type of the number (the
            // table is after the names, see native_abi.h).
            if (name == "Enum_GetNumber" || name == "Enum_ToString" || name == "Enum_Parse")
            {
                __uses_reflection = true;
                __uses_enum_number = true;

                // The type of the enum: the descriptor of the box, or the Type that is the argument.
                llb::value_t desc = name == "Enum_Parse"? args.at(0) : f.load("ptr", f.gep_bytes(args.at(0), -8));

                llb::value_t count = f.load("i64", llb::value_t { llb::types::ptr, "@april.vcount" });
                llb::value_t offset = f.binop("add", f.binop("mul", count, llb::const_int("i64", 8)),
                                                     llb::const_int("i64", 48));
                llb::value_t table = f.load("ptr", f.gep(llb::types::i8, desc, offset));

                if (name == "Enum_ToString")
                    return f.call("ptr", APRIL_NATIVE_SYM_ENUM_TO_STRING, { table, args.at(0) });

                if (name == "Enum_Parse")
                    return f.call("ptr", APRIL_NATIVE_SYM_ENUM_PARSE, { desc, table, args.at(1), args.at(2) });

                return f.call("i64", APRIL_NATIVE_SYM_ENUM_NUMBER, { table, args.at(0) });
            }

            // The access to the fields is made by the runtime: it reads the table of the field.
            if (name == "Reflection_CheckFieldAccess")
            {
                __uses_reflection = true;
                __uses_field_access = true;
                return f.call("i32", APRIL_NATIVE_SYM_REFLECT_CHECK,
                              { args.at(0), args.at(1), args.at(2), args.at(3) });
            }

            if (name == "Reflection_CheckInvoke")
            {
                __uses_reflection = true;
                __uses_invoke = true;
                return f.call("i32", APRIL_NATIVE_SYM_REFLECT_CHECK_INVOKE,
                              { args.at(0), args.at(1), args.at(2) });
            }

            if (name == "Reflection_Invoke")
            {
                __uses_reflection = true;
                __uses_invoke = true;
                return f.call("ptr", APRIL_NATIVE_SYM_REFLECT_INVOKE,
                              { args.at(0), args.at(1), args.at(2) });
            }

            if (name == "Reflection_GetFieldValue")
            {
                __uses_reflection = true;
                __uses_field_access = true;
                return f.call("ptr", APRIL_NATIVE_SYM_REFLECT_GET, { args.at(0), args.at(1) });
            }

            if (name == "Reflection_SetFieldValue")
            {
                __uses_reflection = true;
                __uses_field_access = true;
                f.call("void", APRIL_NATIVE_SYM_REFLECT_SET, { args.at(0), args.at(1), args.at(2) });
                return llb::value_t { };
            }

            // A table of a member has the bit reflect_flag_member in its flags (+24): a type does not.
            if (name == "Reflection_IsMember")
            {
                __uses_reflection = true;
                llb::value_t flags = f.load("i64", f.gep_bytes(args.at(0), 24));
                llb::value_t bit = f.binop("and", f.binop("lshr", flags, llb::const_int("i64", 62)),
                                           llb::const_int("i64", 1));

                return f.cast("trunc", bit, "i32");
            }

            // The attributes: the tables of a member have them (+136), those of a type too (+112); the
            // runtime makes the boxes of the arguments. The objects are made with the size of the
            // class (+104 of the table of the type).
            bool is_attribute = name.compare(0, 23, "Reflection_GetAttribute") == 0;
            bool is_type_attribute = name.compare(0, 27, "Reflection_GetTypeAttribute") == 0;

            if (is_attribute || is_type_attribute || name == "Reflection_NewObject")
            {
                __uses_reflection = true;
                __uses_attributes = true;
                __uses_invoke = true;

                auto meta_of = [&](llb::value_t type) {
                    llb::value_t count = f.load("i64", llb::value_t { llb::types::ptr, "@april.vcount" });
                    llb::value_t offset = f.binop("add", f.binop("mul", count, llb::const_int("i64", 8)),
                                                         llb::const_int("i64", 48));

                    return f.load("ptr", f.gep(llb::types::i8, type, offset));
                };

                if (name == "Reflection_NewObject")
                {
                    __uses_new_object = true;
                    llb::value_t size = f.load("i64", f.gep_bytes(meta_of(args.at(0)), 104));

                    return f.call("ptr", APRIL_NATIVE_SYM_NEW_OBJECT, { size, args.at(0) });
                }

                llb::value_t attributes = is_type_attribute?
                    f.load("ptr", f.gep_bytes(meta_of(args.at(0)), 112))
                  : f.load("ptr", f.gep_bytes(args.at(0), 136));

                std::string what = name.substr(is_type_attribute? 27 : 23);

                if (what == "Count")
                    return f.call("i32", APRIL_NATIVE_SYM_ATTR_COUNT, { attributes });

                if (what == "Constructor")
                    return f.call("ptr", APRIL_NATIVE_SYM_ATTR_CTOR, { attributes, args.at(1) });

                if (what == "ArgumentCount")
                    return f.call("i32", APRIL_NATIVE_SYM_ATTR_ARG_COUNT, { attributes, args.at(1) });

                if (what == "Argument")
                    return f.call("ptr", APRIL_NATIVE_SYM_ATTR_ARG, { attributes, args.at(1), args.at(2) });

                if (what == "NamedCount")
                    return f.call("i32", APRIL_NATIVE_SYM_ATTR_NAMED_COUNT, { attributes, args.at(1) });

                if (what == "NamedName")
                    return f.call("ptr", APRIL_NATIVE_SYM_ATTR_NAMED_NAME, { attributes, args.at(1), args.at(2) });

                if (what == "NamedValue")
                    return f.call("ptr", APRIL_NATIVE_SYM_ATTR_NAMED_VALUE, { attributes, args.at(1), args.at(2) });
            }

            // The reflection reads the tables (rt_reflect_type_t, rt_reflect_member_t): the
            // functions are loads of their fields.
            if (name.compare(0, 14, "Reflection_Get") == 0)
            {
                auto load_at = [&](const char * ll, llb::value_t base, int64_t offset) {
                    return f.load(ll, f.gep_bytes(base, offset));
                };

                // The table of a type is after the descriptor of it.
                auto meta_of = [&](llb::value_t type) {
                    llb::value_t count = f.load("i64", llb::value_t { llb::types::ptr, "@april.vcount" });
                    llb::value_t offset = f.binop("add", f.binop("mul", count, llb::const_int("i64", 8)),
                                                         llb::const_int("i64", 48));

                    return f.load("ptr", f.gep(llb::types::i8, type, offset));
                };

                // The item of an index in a list of members.
                auto item_at = [&](llb::value_t items, llb::value_t index) {
                    llb::value_t at = f.binop("mul", f.cast("sext", index, "i64"), llb::const_int("i64", 8));
                    return f.load("ptr", f.gep(llb::types::i8, items, at));
                };

                auto list_of = [&](llb::value_t type, int kind) {
                    return load_at("ptr", meta_of(type), 32 + 16 * kind);
                };

                std::string what = name.substr(14);
                __uses_reflection = true;

                if (what == "TypeBase")
                    return load_at("ptr", meta_of(args.at(0)), 0);

                if (what == "TypeFlags")
                    return load_at("i64", meta_of(args.at(0)), 8);

                if (what == "TypeAssembly")
                    return load_at("ptr", meta_of(args.at(0)), 16);

                // The nest types: after the others in the table of a type.
                if (what == "NestedTypeCount")
                    return f.cast("trunc", load_at("i64", meta_of(args.at(0)), 120), "i32");

                if (what == "NestedType")
                    return item_at(load_at("ptr", meta_of(args.at(0)), 128), args.at(1));

                if (what == "MemberCount")
                {
                    llb::value_t at = f.binop("add", f.binop("mul", f.cast("sext", args.at(1), "i64"),
                                                             llb::const_int("i64", 16)),
                                                     llb::const_int("i64", 24));
                    llb::value_t count = f.load("i64", f.gep(llb::types::i8, meta_of(args.at(0)), at));
                    return f.cast("trunc", count, "i32");
                }

                if (what == "Field")                return item_at(list_of(args.at(0), 0), args.at(1));
                if (what == "Method")               return item_at(list_of(args.at(0), 1), args.at(1));
                if (what == "Property")             return item_at(list_of(args.at(0), 2), args.at(1));
                if (what == "Event")                return item_at(list_of(args.at(0), 3), args.at(1));
                if (what == "TypeGenericParameter") return item_at(list_of(args.at(0), 4), args.at(1));

                if (what == "InfoName")             return load_at("ptr", args.at(0), 0);
                if (what == "InfoDeclaringType")    return load_at("ptr", args.at(0), 8);
                if (what == "InfoType")             return load_at("ptr", args.at(0), 16);
                if (what == "InfoFlags")            return load_at("i64", args.at(0), 24);

                if (what == "PropertyGetter" || what == "EventAdder")
                    return load_at("ptr", args.at(0), 32);

                if (what == "PropertySetter" || what == "EventRemover")
                    return load_at("ptr", args.at(0), 40);

                if (what == "EventRaiser")
                    return load_at("ptr", args.at(0), 80);

                if (what == "ParameterCount")
                    return f.cast("trunc", load_at("i64", args.at(0), 48), "i32");

                if (what == "Parameter")
                    return item_at(load_at("ptr", args.at(0), 56), args.at(1));

                if (what == "GenericParameterCount")
                    return f.cast("trunc", load_at("i64", args.at(0), 64), "i32");

                if (what == "GenericParameter")
                    return item_at(load_at("ptr", args.at(0), 72), args.at(1));
            }

            // The arrays of native programs have their own layout (see native_abi.h):
            // the library functions that read it are replaced.
            if (name.compare(0, 6, "Array_") == 0)
            {
                __uses_arrays = true;

                if (name == "Array_GetLength")
                    return f.call("i32", APRIL_NATIVE_SYM_ARRAY_LENGTH, { args.at(0) });

                if (name == "Array_GetRank")
                    return f.call("i32", APRIL_NATIVE_SYM_ARRAY_RANK, { args.at(0) });

                if (name == "Array_GetLengthOfDimension")
                {
                    return f.call("i32", APRIL_NATIVE_SYM_ARRAY_LENGTH_OF,
                                  { args.at(0), args.at(1) });
                }

                if (name == "Array_GetValue" || name == "Array_SetValue")
                {
                    // Array_GetValue<T>(index, array), Array_SetValue<T>(value, index, array).
                    if (callee.base->this_type() != rt_member_type_t::generic)
                        throw _ED(__e_t::unsupported_libfunc, __wide(name), _T("is not generic"));

                    rt_type_t * element_type = static_cast<rt_generic_method_t *>(callee.base)->atypes[0];
                    std::string ll = __ll_type_of(element_type);
                    if (ll[0] == '[')
                        throw _ED(__e_t::unsupported_type, element_type->get_name(__env));

                    bool get = name == "Array_GetValue";
                    const llb::value_t & index = args.at(get? 0 : 1);
                    const llb::value_t & array = args.at(get? 1 : 2);

                    // The index is among all the elements, whatever the dimension.
                    llb::value_t addr = f.call("ptr", APRIL_NATIVE_SYM_ARRAY_LINEAR, {
                        array, index,
                        llb::const_int("i64", (int64_t)element_type->get_variable_size(__env))
                    });

                    if (get)
                        return f.load(ll, addr);

                    f.store(args.at(0), addr);
                    return llb::value_t { };
                }
            }

            for (const char * unsupported : __metadata_libfuncs)
            {
                if (name == unsupported)
                {
                    throw _ED(__e_t::unsupported_libfunc, string_convert<char, char_t>(name),
                              _T("needs program metadata, not available to native programs yet"));
                }
            }

            if (rtlib::get_libfunc(string_convert<char, char_t>(name)) == nullptr)
                throw _ED(__e_t::unsupported_libfunc, string_convert<char, char_t>(name), _T("does not exist"));

            __used_libfuncs.insert(name);

            size_t units = std::max<size_t>(1, args.size());
            llb::value_t block = f.alloca_entry(llb::types::array(units, llb::types::i64), "args");

            for (size_t k = 0; k < args.size(); k++)
                f.store(args[k], f.gep_bytes(block, (int64_t)(k * sizeof(uint64_t))));

            llb::value_t fp = f.load(llb::types::ptr,
                llb::value_t { llb::types::ptr, "@" + __libfunc_global(name) });

            f.call("void", APRIL_NATIVE_SYM_INVOKE, { fp, block });

            if (callee.ret_type == "void")
                return llb::value_t { };

            return f.load(callee.ret_type, block);      // The result is in the first unit.
        }

        //-------- ---------- ---------- ---------- ----------

        // Generates the body of a function.
        void __generator_t::__emit(__fn_t & fn)
        {
            // A function that a former try could not make: only the signature, the body traps.
            if (fn.denied)
            {
                std::vector<std::pair<std::string, std::string>> stub_params;
                if (fn.returns_struct)
                    stub_params.push_back({ "ptr", "retaddr" });

                for (size_t k = 0; k < fn.param_types.size(); k++)
                    stub_params.push_back({ fn.param_types[k], "p" + std::to_string(k) });

                llb::function_t & stub = __module.define(fn.name, fn.ret_type, stub_params);

                __uses_trap = true;
                stub.call("void", "llvm.trap", { });
                stub.unreachable();
                return;
            }

            // Invoke of a delegate has no body that can be made: its calls are expanded inline. The
            // function (Method.Invoke calls it) is made here: it calls the method of the delegate.
            if (__is_delegate_invoke(fn))
            {
                __emit_delegate_invoke(fn);
                return;
            }

            rt_method_t * method = fn.method;
            rt_assembly_t * assembly = method->get_assembly();
            rt_type_t * host_type = fn.base->get_host_type();

            assembly_analyzer_t analyzer = __analyzer_of(fn);

            rt_bytes_t body = assembly->get_method_body(method);
            if (body.bytes == nullptr)
                throw _ED(__e_t::unsupported_method, fn.display, _T("no body (extern / abstract)"));

            method_reader_t reader(body.bytes, body.length, __env.memory);

            // The declarations of local variables, in the order of their identities.
            std::vector<rt_type_t *> local_rt_types;
            reader.each_variable([&](local_variable_defination_t & def) {
                rt_type_t * type = analyzer.get_type(def.type);
                if (type == nullptr)
                    throw _ED(__e_t::unsupported_type, _F(
                        _T("(unresolved local variable type %1% in %2%)"), (int)def.type.extra, fn.display
                    ));

                local_rt_types.push_back(type);
            });

            // Switch tables; protected blocks (try / catch) are not supported yet.
            struct __switch_table_t
            {
                int32_t default_step;
                std::vector<std::pair<int32_t, int32_t>> rows;     // (value, step)
            };

            std::vector<__switch_table_t> switch_tables;
            reader.each_switch_table([&](exec_switch_table_t * table) {
                __switch_table_t t;
                t.default_step = table->default_step;

                for (int k = 0; k < table->row_count; k++)
                    t.rows.push_back({ table->rows[k].value, table->rows[k].step });

                switch_tables.push_back(std::move(t));
            });

            std::vector<method_xil_block_t> xil_blocks;
            reader.each_xil_block([&](method_xil_block_t & block) {
                xil_blocks.push_back(block);
            });

            // The function.
            std::vector<std::pair<std::string, std::string>> params;
            if (fn.returns_struct)
                params.push_back({ "ptr", "retaddr" });

            const size_t hidden = fn.returns_struct? 1 : 0;
            for (size_t k = 0; k < fn.param_types.size(); k++)
                params.push_back({ fn.param_types[k], "p" + std::to_string(k) });

            llb::function_t & f = __module.define(fn.name, fn.ret_type, params);

            // Arguments and locals live in allocas (mem2reg promotes them later).
            struct __slot_t { llb::value_t ptr; std::string type; };
            std::vector<__slot_t> args, locals;
            std::vector<rt_type_t *> marker_types;      // The types of the type infos.

            for (size_t k = 0; k < fn.param_types.size(); k++)
            {
                llb::value_t slot = f.alloca_entry(fn.param_types[k], "arg");
                f.store(f.param(k + hidden), slot);
                args.push_back({ slot, fn.param_types[k] });
            }

            for (rt_type_t * type : local_rt_types)
            {
                // A local of a generic param that is Void (the place of the result of a call
                // that returns it) has no value: it takes a byte.
                std::string ll_type = __ll_type_of(type);
                if (ll_type == "void")
                    ll_type = "i8";

                locals.push_back({ f.alloca_entry(ll_type, "local"), ll_type });
            }

            // The evaluation stack, symbolically.
            std::vector<llb::value_t> stack;
            bool in_dead_code = false;

            // Checks that a stack value has the LLVM type of a numeric xil type.
            auto expect = [&](llb::value_t v, xil_type_t xt) {
                // A pointer is a 64 bits integer for the arithmetic and the compares of the pointers.
                if (v.type == "ptr" && (xt == xil_type_t::int64 || xt == xil_type_t::uint64))
                    v = f.cast("ptrtoint", v, "i64");

                if (__num_of(xt).valid && v.type != __num_of(xt).ll)
                {
                    // A constant of a number type that is narrower is given to a wider operation
                    // (an Int32 literal with an Int64: n % 10000): the interpreter reads it as a
                    // unit of the stack, it is widened here.
                    auto bits = [](const std::string & ll) -> int {
                        return ll == "i8"? 8 : ll == "i16"? 16 : ll == "i32"? 32 : ll == "i64"? 64 : 0;
                    };

                    int from = bits(v.type), to = bits(__num_of(xt).ll);
                    if (from > 0 && to > from && !__num_of(xt).is_float)
                    {
                        return f.cast(__num_of(xt).is_signed? "sext" : "zext", v, __num_of(xt).ll);
                    }
                }

                if (!__num_of(xt).valid || v.type != __num_of(xt).ll)
                {
                    throw _ED(__e_t::bad_stack, fn.display, _F(
                        _T("operand is %1%, not %2%"), __wide(v.type), xt
                    ));
                }

                return v;
            };

            // The offset and the type of a field.
            struct __field_info_t { msize_t offset; rt_type_t * type; xil_type_t xtype; };
            auto field_of = [&](ref_t ref) {
                __field_info_t info;

                // Like the interpreter: the offset and the type depend on the way the
                // field is referenced.
                switch ((mt_member_extra_t)ref.extra)
                {
                    case mt_member_extra_t::internal:
                    case mt_member_extra_t::import: {
                        rt_type_t * field_host = nullptr;
                        rt_field_base_t * field = analyzer.get_field(ref, &field_host);
                        if (field == nullptr || field_host == nullptr || !is_general(field_host))
                            throw _ED(__e_t::unsupported_xil, _T("unresolved field"));

                        rt_general_type_t * general = (rt_general_type_t *)field_host;
                        rt_type_t * owner = general->is_generic_template()?
                                                analyzer.to_generic_type(general) : field_host;

                        info.offset = owner->get_field_offset(__env, ref);

                        rt_type_t * type = field->get_field_type(analyzer,
                                                dynamic_cast<rt_generic_type_t *>(host_type));
                        if (type != nullptr && is_general(type)
                                && ((rt_general_type_t *)type)->is_generic_template())
                        {
                            type = analyzer.to_generic_type((rt_general_type_t *)type);
                        }

                        info.type = type;
                    }   break;

                    case mt_member_extra_t::position: {
                        rt_type_t * type = analyzer.get_host_type_by_field_ref(ref);
                        mt_position_field_t * mt = analyzer.current->mt_of_position_field(ref);
                        if (type == nullptr || mt == nullptr
                                || type->get_kind() != rt_type_kind_t::generic)
                        {
                            throw _ED(__e_t::unsupported_xil, _T("unresolved field"));
                        }

                        info.offset = type->get_field_offset(__env, mt->position);
                        info.type = ((rt_generic_type_t *)type)->atypes[mt->position];
                    }   break;

                    case mt_member_extra_t::generic: {
                        rt_type_t * type = analyzer.get_host_type_by_field_ref(ref);
                        mt_generic_field_t * mt = analyzer.current->mt_of_generic_field(ref);
                        if (type == nullptr || mt == nullptr)
                            throw _ED(__e_t::unsupported_xil, _T("unresolved field"));

                        info.offset = type->get_field_offset(__env, mt->template_);

                        rt_type_t * field_host = nullptr;
                        rt_field_base_t * field = analyzer.get_field(ref, &field_host);
                        if (field == nullptr || field_host == nullptr
                                || field_host->get_kind() != rt_type_kind_t::generic)
                        {
                            throw _ED(__e_t::unsupported_xil, _T("unresolved field"));
                        }

                        info.type = field->get_field_type(analyzer, (rt_generic_type_t *)field_host);
                    }   break;

                    default:
                        throw _ED(__e_t::unsupported_xil, _T("field reference"));
                }

                if (info.type == nullptr)
                    throw _ED(__e_t::unsupported_xil, _T("unresolved field type"));

                info.xtype = __xil_type_of(info.type);
                return info;
            };

            // All xils: jumps need to know the position of their targets.
            std::vector<const xil_base_t *> xils;
            reader.each_xil([&](const xil_base_t * xil) { xils.push_back(xil); });

            // APRIL_NATIVE_TRACE=1: prints the xils of every method (for debugging).
            if (std::getenv("APRIL_NATIVE_TRACE") != nullptr)
            {
                std::wcerr << _T("== ") << fn.display << std::endl;
                for (method_xil_block_t & b : xil_blocks)
                    std::wcerr << _T("  block ") << (string_t)b << std::endl;

                for (size_t k = 0; k < xils.size(); k++)
                {
                    std::wcerr << _T("  ") << k << _T(": ")
                               << core::__to_string((const xil_t *)xils[k]) << std::endl;
                }
            }

            // The basic blocks start at jump targets and after jumps. A jump moves
            // `step` xils (labels count) from itself.
            std::map<size_t, std::string> block_labels;
            auto jump_target = [&](size_t index, const jmp_xil_t * jmp) -> size_t {
                int64_t target = (int64_t)index + (int64_t)jmp->step();
                if (target < 0 || target >= (int64_t)xils.size())
                {
                    throw _ED(__e_t::bad_stack, fn.display, _T("jump out of the method"));
                }

                return (size_t)target;
            };

            auto switch_target = [&](size_t index, int32_t step) -> size_t {
                int64_t target = (int64_t)index + (int64_t)step;
                if (target < 0 || target >= (int64_t)xils.size())
                    throw _ED(__e_t::bad_stack, fn.display, _T("switch target out of the method"));

                return (size_t)target;
            };

            for (size_t k = 0; k < xils.size(); k++)
            {
                if (xils[k]->command() != xil_command_t::jmp)
                    continue;

                const jmp_xil_t * jmp = static_cast<const jmp_xil_t *>(xils[k]);
                switch (jmp->model())
                {
                    case xil_jmp_model_t::none:
                    case xil_jmp_model_t::true_:
                    case xil_jmp_model_t::false_:
                        break;

                    case xil_jmp_model_t::switch_:
                        break;

                    case xil_jmp_model_t::leave:
                        // Leaves a protected block: step 0 is a return.
                        if (jmp->step() != 0)
                            block_labels[jump_target(k, jmp)];

                        if (k + 1 < xils.size())
                            block_labels[k + 1];

                        continue;

                    default:
                        throw _ED(__e_t::unsupported_xil, _T("jmp model"));
                }

                if (jmp->model() == xil_jmp_model_t::switch_)
                {
                    if (jmp->tbl() < 0 || jmp->tbl() >= (int32_t)switch_tables.size())
                        throw _ED(__e_t::bad_stack, fn.display, _T("switch table out of range"));

                    const __switch_table_t & t = switch_tables[jmp->tbl()];
                    block_labels[switch_target(k, t.default_step)];
                    for (auto & row : t.rows)
                        block_labels[switch_target(k, row.second)];

                    continue;
                }

                block_labels[jump_target(k, jmp)];
                if (k + 1 < xils.size())
                    block_labels[k + 1];
                else if (jmp->model() != xil_jmp_model_t::none)
                    throw _ED(__e_t::bad_stack, fn.display, _T("a conditional jump ends the method"));
            }

            // The handlers of the blocks are entered from the dispatch of exceptions.
            for (const method_xil_block_t & b : xil_blocks)
            {
                if (b.entry_point >= xils.size())
                    throw _ED(__e_t::bad_stack, fn.display, _T("block entry out of the method"));

                block_labels[b.entry_point];
            }

            for (auto & it : block_labels)
                it.second = f.new_block("bb");

            size_t xil_index = 0;

            auto pop_value = [&](const char * what) {
                if (stack.empty())
                    throw _ED(__e_t::bad_stack, fn.display, what);

                llb::value_t v = stack.back();
                stack.pop_back();
                return v;
            };

            // ---- Exceptions. A thrown exception is stored in a global, and every call
            // checks it on return. The blocks (try / catch / finally) are compiled to
            // ordinary control flow: `dispatch` blocks look for the handler of the pending
            // exception, a finally block returns to the place it was entered from through
            // a selector (see `cont`).
            const size_t block_count = xil_blocks.size();

            // The enclosing block of each block: the first later block that includes it.
            std::vector<int> parent_of(block_count, -1);
            for (size_t p = 0; p < block_count; p++)
            {
                for (size_t q = p + 1; q < block_count; q++)
                {
                    if (xil_blocks[q].xil_start <= xil_blocks[p].xil_start
                            && xil_blocks[q].xil_end >= xil_blocks[p].xil_end)
                    {
                        parent_of[p] = (int)q;
                        break;
                    }
                }
            }

            // The innermost block that contains a xil, -1 for none.
            auto inner_block = [&](size_t index) -> int {
                for (size_t b = 0; b < block_count; b++)
                {
                    if (index >= xil_blocks[b].xil_start && index < xil_blocks[b].xil_end)
                        return (int)b;
                }

                return -1;
            };

            auto entry_label_of = [&](int b) -> std::string {
                return block_labels.at(xil_blocks[b].entry_point);
            };

            llb::value_t exception_global { llb::types::ptr, "@april.exception" };

            // Finally blocks: the selector of where to go at their end, and the blocks of
            // the `end finally`.
            std::vector<llb::value_t> cont_slots(block_count);
            std::vector<std::vector<std::string>> cont_targets(block_count);     // id - 1
            std::map<size_t, int> end_finally_of;           // xil index -> block

            {
                std::vector<int> open;
                for (size_t k = 0; k < xils.size(); k++)
                {
                    for (size_t b = 0; b < block_count; b++)
                    {
                        if (xil_blocks[b].type == method_xil_block_type_t::finally_
                                && xil_blocks[b].entry_point == k)
                        {
                            open.push_back((int)b);
                        }
                    }

                    if (xils[k]->command() == xil_command_t::smp
                            && static_cast<const smp_xil_t *>(xils[k])->smp() == xil_smp_t::end_finally)
                    {
                        if (open.empty())
                            throw _ED(__e_t::bad_stack, fn.display, _T("end finally outside a finally block"));

                        end_finally_of[k] = open.back();
                        open.pop_back();
                    }
                }
            }

            // The exception that a finally block was entered with: it is not pending while the
            // block runs (the calls in it would check it), and is raised again at the end.
            std::vector<llb::value_t> saved_exception_slots(block_count);

            for (size_t b = 0; b < block_count; b++)
            {
                if (xil_blocks[b].type == method_xil_block_type_t::finally_)
                {
                    cont_slots[b] = f.alloca_entry("i32", "selector");
                    saved_exception_slots[b] = f.alloca_entry("ptr", "saved_exc");
                }
            }

            auto cont_id = [&](int b, const std::string & label) -> int {
                std::vector<std::string> & targets = cont_targets[b];
                for (size_t k = 0; k < targets.size(); k++)
                {
                    if (targets[k] == label)
                        return (int)k + 1;
                }

                targets.push_back(label);
                return (int)targets.size();
            };

            // The dispatch block of a block (-1: the method).
            std::map<int, std::string> dispatch_labels;
            auto dispatch_label = [&](int b) -> std::string {
                auto it = dispatch_labels.find(b);
                if (it == dispatch_labels.end())
                    it = dispatch_labels.emplace(b, f.new_block("dispatch")).first;

                return it->second;
            };

            llb::value_t current_exception;         // The exception of the catch handlers.
            auto current_exception_slot = [&]() {
                if (!current_exception.valid())
                    current_exception = f.alloca_entry("ptr", "exc");

                return current_exception;
            };

            // Where `ret` goes when it passes finally blocks.
            llb::value_t return_slot;
            std::string return_label;

            // Branches to a label running the finally blocks that the jump leaves.
            // `target` is the xil index jumped to, or -1 for a return.
            auto jump_through_finally = [&](size_t index, int64_t target, const std::string & dest) {
                std::vector<int> chain;
                for (int b = inner_block(index); b != -1; b = parent_of[b])
                {
                    const method_xil_block_t & blk = xil_blocks[b];
                    if (blk.type != method_xil_block_type_t::finally_)
                        continue;

                    if (target >= 0 && (uint32_t)target >= blk.xil_start && (uint32_t)target < blk.xil_end)
                        continue;       // The jump stays inside.

                    chain.push_back(b);
                }

                if (chain.empty())
                {
                    f.br(dest);
                    return;
                }

                for (size_t k = 0; k < chain.size(); k++)
                {
                    const std::string & next = k + 1 < chain.size()? entry_label_of(chain[k + 1]) : dest;
                    f.store(llb::const_int("i32", cont_id(chain[k], next)), cont_slots[chain[k]]);
                }

                f.br(entry_label_of(chain[0]));
            };

            // Raises the exception: goes to the handlers of the current position.
            auto raise = [&](const llb::value_t & exception) {
                f.store(exception, exception_global);
                f.br(dispatch_label(inner_block(xil_index)));
            };

            // The values left on the stack by a branch live in slots, one per depth and type:
            // they are stored before the branch and loaded at the start of the target.
            std::map<std::string, std::vector<std::string>> entry_types;
            std::map<std::pair<size_t, std::string>, llb::value_t> stack_slots;

            auto stack_slot = [&](size_t depth, const std::string & type) {
                auto key = std::make_pair(depth, type);
                auto it = stack_slots.find(key);
                if (it == stack_slots.end())
                    it = stack_slots.emplace(key, f.alloca_entry(type, "stk")).first;

                return it->second;
            };

            auto spill = [&](const std::string & label) {
                std::vector<std::string> types;
                for (const llb::value_t & v : stack)
                    types.push_back(v.type);

                auto it = entry_types.find(label);
                if (it == entry_types.end())
                    entry_types[label] = types;
                else if (it->second != types)
                    throw _ED(__e_t::bad_stack, fn.display, _T("stacks differ at a branch"));

                for (size_t k = 0; k < stack.size(); k++)
                {
                    if (stack[k].text[0] == '&')
                    {
                        throw _ED(__e_t::unsupported_method, fn.display,
                                  _T("a struct argument across a branch"));
                    }

                    f.store(stack[k], stack_slot(k, stack[k].type));
                }
            };

            // The end of the finally blocks are made at the end.
            std::vector<std::pair<int, std::string>> finally_ends;

            // Returns, running the finally blocks that contain the return first.
            auto emit_return = [&]() {
                llb::value_t v;
                if (fn.ret_type != "void")
                {
                    v = pop_value("return value");
                    if (v.type != fn.ret_type)
                    {
                        throw _ED(__e_t::bad_stack, fn.display, _F(
                            _T("return type mismatch: %1% / %2%"), __wide(v.type), __wide(fn.ret_type)
                        ));
                    }
                }

                bool through_finally = false;
                for (int b = inner_block(xil_index); b != -1; b = parent_of[b])
                    through_finally = through_finally
                                    || xil_blocks[b].type == method_xil_block_type_t::finally_;

                if (!through_finally)
                {
                    if (fn.ret_type == "void")
                        f.ret();
                    else
                        f.ret(v);

                    return;
                }

                if (fn.ret_type != "void")
                {
                    if (!return_slot.valid())
                        return_slot = f.alloca_entry(fn.ret_type, "retval");

                    f.store(v, return_slot);
                }

                if (return_label.empty())
                    return_label = f.new_block("return");

                jump_through_finally(xil_index, -1, return_label);
            };

            // A catch handler starts with the exception on the stack.
            for (size_t b = 0; b < block_count; b++)
            {
                if (xil_blocks[b].type == method_xil_block_type_t::catch_)
                    entry_types[entry_label_of((int)b)] = { "ptr" };
            }

            // After a call: goes to the handlers when an exception is pending.
            auto check_exception = [&]() {
                llb::value_t pending = f.load("ptr", exception_global);
                llb::value_t c = f.icmp("ne", pending, llb::const_null());
                std::string go_on = f.new_block("aftercall");

                f.cond_br(c, dispatch_label(inner_block(xil_index)), go_on);
                f.set_block(go_on);
            };

            auto zero_of = [&](const std::string & type) -> llb::value_t {
                if (type == "float")
                    return llb::const_float(0.0f);

                if (type == "double")
                    return llb::const_double(0.0);

                if (type == "ptr")
                    return llb::const_null();

                return llb::const_int(type, 0);
            };

            // A cast that fails raises InvalidCastException, like the interpreter does.
            // (raise_runtime is defined below: the cast is made by the lambda after it.)
            // The exceptions that the runtime raises, like the interpreter does: an object of
            // a type of the core library, made without constructor. Leaves the current block.
            auto raise_runtime = [&](const char_t * type_name) {
                rt_type_t * exception_type = __env.get_core_type(_T(""), type_name, 0);
                if (exception_type == nullptr)
                    throw _ED(__e_t::unsupported_type, string_t(type_name));

                exception_type->pre_new(__env);
                __uses_new_object = true;

                llb::value_t exception = f.call("ptr", APRIL_NATIVE_SYM_NEW_OBJECT, {
                    llb::const_int("i64", (int64_t)exception_type->get_size(__env)),
                    llb::value_t { llb::types::ptr, "@" + __descriptor_of(exception_type) }
                });

                f.store(exception, exception_global);
                f.br(dispatch_label(inner_block(xil_index)));
            };

            // Continues after a check: `fail` is the condition of the exception.
            auto raise_if = [&](const llb::value_t & fail, const char_t * type_name) {
                std::string raise_block = f.new_block("raise"), go_on = f.new_block("checked");
                f.cond_br(fail, raise_block, go_on);

                f.set_block(raise_block);
                raise_runtime(type_name);

                f.set_block(go_on);
            };

            auto raise_if_null = [&](const llb::value_t & obj) {
                raise_if(f.icmp("eq", obj, llb::const_null()), CoreType_NullReferenceException);
            };

            // The address of an element of an array: the array is on the top of the stack,
            // the indices below it (the last dimension first).
            struct __element_t { llb::value_t address; rt_type_t * type; };
            auto element_of = [&](ref_t ref) {
                rt_type_t * type = analyzer.get_type(ref);
                if (type == nullptr || type->get_kind() != rt_type_kind_t::array)
                    throw _ED(__e_t::unsupported_type, _T("(unresolved array type)"));

                rt_array_type_t * array_type = (rt_array_type_t *)type;
                int dimension = array_type->dimension;

                llb::value_t array = pop_value("array");
                if (array.type != "ptr")
                    throw _ED(__e_t::bad_stack, fn.display, _T("array is not a reference"));

                raise_if_null(array);

                llb::value_t indices = f.alloca_entry("[" + std::to_string(dimension) + " x i32]", "idx");
                for (int k = dimension - 1; k >= 0; k--)
                {
                    llb::value_t index = pop_value("array index");
                    if (index.type != "i32")
                        throw _ED(__e_t::bad_stack, fn.display, _T("array index is not int32"));

                    f.store(index, f.gep_bytes(indices, 4 * k));
                }

                __uses_arrays = true;

                __element_t e;
                e.type = array_type->element_type;
                e.address = f.call("ptr", APRIL_NATIVE_SYM_ARRAY_TRY, {
                    array, indices, llb::const_int("i32", dimension),
                    llb::const_int("i64", (int64_t)e.type->get_variable_size(__env))
                });

                // Out of range: the exception of the interpreter.
                raise_if(f.icmp("eq", e.address, llb::const_null()), CoreType_OutOfRangeException);

                return e;
            };


            auto checked_cast = [&](const llb::value_t & obj, const llb::value_t & key) {
                llb::value_t is = f.call("i8", APRIL_NATIVE_SYM_IS_INSTANCE, { obj, key });
                llb::value_t is_instance = f.icmp("ne", is, llb::const_int("i8", 0));
                llb::value_t is_null = f.icmp("eq", obj, llb::const_null());
                llb::value_t ok = f.binop("or", is_instance, is_null);

                raise_if(f.binop("xor", ok, llb::const_bool(true)), CoreType_InvalidCastException);
            };

            auto emit_xil = [&](const xil_base_t * xil) {

                // A basic block starts here.
                auto block_it = block_labels.find(xil_index);
                if (block_it != block_labels.end())
                {
                    if (!f.terminated())
                    {
                        spill(block_it->second);
                        f.br(block_it->second);
                    }

                    f.set_block(block_it->second);
                    stack.clear();
                    in_dead_code = false;

                    auto types_it = entry_types.find(block_it->second);
                    if (types_it != entry_types.end())
                    {
                        for (size_t k = 0; k < types_it->second.size(); k++)
                        {
                            const std::string & type = types_it->second[k];
                            stack.push_back(f.load(type, stack_slot(k, type)));
                        }
                    }
                }

                // Code after a terminator (e.g. a second `ret`) goes to a dead block.
                if (f.terminated())
                {
                    f.set_block(f.new_block("dead"));
                    in_dead_code = true;
                }

                switch (xil->command())
                {
                    case xil_command_t::push: {
                        const push_xil_t * push = static_cast<const push_xil_t *>(xil);

                        switch (push->stype())
                        {
                            case xil_storage_type_t::constant: {

                                switch (push->dtype())
                                {
                                    #define __Const(_xt, _t, _make)                             \
                                        case xil_type_t::_xt: {                                 \
                                            _t v = push->get_extra<_t>();                       \
                                            stack.push_back(_make);                             \
                                        }   break;

                                    __Const(int8,   int8_t,   llb::const_int("i8",  v))
                                    __Const(uint8,  uint8_t,  llb::const_int("i8",  (int8_t)v))
                                    __Const(int16,  int16_t,  llb::const_int("i16", v))
                                    __Const(uint16, uint16_t, llb::const_int("i16", (int16_t)v))
                                    __Const(int32,  int32_t,  llb::const_int("i32", v))
                                    __Const(uint32, uint32_t, llb::const_int("i32", (int32_t)v))
                                    __Const(int64,  int64_t,  llb::const_int("i64", v))
                                    __Const(uint64, uint64_t, llb::const_int("i64", (int64_t)v))
                                    __Const(bool_,  bool,     llb::const_int("i8",  v? 1 : 0))
                                    __Const(char_,  rt_char_t, llb::const_int("i16", (int64_t)v))
                                    __Const(float_, float,    llb::const_float(v))
                                    __Const(double_,double,   llb::const_double(v))

                                    #undef __Const

                                    case xil_type_t::string: {
                                        std::u32string chars = __units_of_string(analyzer.to_cstr(
                                            push->get_extra<res_t>()
                                        ));

                                        stack.push_back(__module.add_wstring(chars, 16));
                                    }   break;

                                    case xil_type_t::object:        // The null constant.
                                        stack.push_back(llb::const_null());
                                        break;

                                    default:
                                        throw _ED(__e_t::unsupported_xil, _F(
                                            _T("push constant %1%"), push->dtype()
                                        ));
                                }
                            }   break;

                            case xil_storage_type_t::argument:
                            case xil_storage_type_t::local: {
                                bool is_arg = push->stype() == xil_storage_type_t::argument;
                                std::vector<__slot_t> & slots = is_arg? args : locals;

                                size_t id = push->get_identity();

                                // The address of the result of a function that returns a struct.
                                if (is_arg && id == XIL_CALLING_BOTTOM_IDENTITY)
                                {
                                    if (!fn.returns_struct)
                                        throw _ED(__e_t::bad_stack, fn.display, _T("no return address"));

                                    stack.push_back(f.param(0));
                                    break;
                                }

                                if (id >= slots.size())
                                {
                                    throw _ED(__e_t::bad_stack, fn.display,
                                              _T("variable identity out of range"));
                                }

                                stack.push_back(f.load(slots[id].type, slots[id].ptr));
                            }   break;

                            case xil_storage_type_t::argument_content: {
                                // The content of a ref / out argument: loads through the address.
                                size_t id = push->get_identity();
                                if (id >= args.size() || args[id].type != "ptr")
                                    throw _ED(__e_t::bad_stack, fn.display, _T("not an address argument"));

                                rt_type_t * content_type = fn.param_rt_types[id];
                                std::string ll = __ll_type_of(content_type);
                                xil_type_t xt = __xil_type_of(content_type);
                                if (ll[0] == '[' || xt == xil_type_t::empty)
                                    throw _ED(__e_t::unsupported_type, content_type->get_name(__env));

                                llb::value_t addr = f.load("ptr", args[id].ptr);
                                llb::value_t v = f.load(ll, addr);

                                if (push->dtype() != xt && __conv_num_of(xt).valid
                                                        && __conv_num_of(push->dtype()).valid)
                                {
                                    v = __convert_any(f, v, xt, push->dtype());
                                }

                                stack.push_back(v);
                            }   break;

                            case xil_storage_type_t::type_size: {
                                // sizeof(T): the size of the type, of the instance when it is a generic param.
                                rt_type_t * sized = analyzer.get_type(push->get_ref());
                                if (sized == nullptr)
                                    throw _ED(__e_t::unsupported_type, _T("(unresolved type of sizeof)"));

                                sized = __current_instance_of(sized, host_type, analyzer);
                                stack.push_back(llb::const_int("i64", (int64_t)sized->get_size(__env)));
                            }   break;

                            case xil_storage_type_t::ptr_content: {
                                // *p: the address is at the top; it is not checked (it is what the program says).
                                llb::value_t addr = pop_value("content of an empty stack");
                                if (addr.type != "ptr")
                                    addr = f.cast("inttoptr", addr, "ptr");

                                std::string ll = __ll_of_content(push->dtype());
                                if (ll.empty())
                                    throw _ED(__e_t::unsupported_xil, _F(_T("push ptr_content %1%"), push->dtype()));

                                stack.push_back(f.load_unaligned(ll, addr));
                            }   break;

                            case xil_storage_type_t::field:
                            case xil_storage_type_t::field_addr: {
                                __field_info_t info = field_of(push->get_ref());

                                llb::value_t obj = pop_value("field of an empty stack");
                                if (obj.type != "ptr")
                                    throw _ED(__e_t::bad_stack, fn.display, _T("field of a non reference"));

                                raise_if_null(obj);

                                llb::value_t addr = f.gep_bytes(obj, (int64_t)info.offset);

                                if (push->stype() == xil_storage_type_t::field_addr)
                                {
                                    stack.push_back(addr);
                                    break;
                                }

                                std::string ll = __ll_type_of(info.type);
                                if (info.xtype == xil_type_t::empty || ll[0] == '[')
                                    throw _ED(__e_t::unsupported_type, info.type->get_name(__env));

                                llb::value_t v = f.load(ll, addr);

                                // Converts to the type the xil asks for, like the interpreter.
                                if (push->dtype() != info.xtype && __conv_num_of(info.xtype).valid
                                                    && __conv_num_of(push->dtype()).valid)
                                {
                                    v = __convert_any(f, v, info.xtype, push->dtype());
                                }

                                stack.push_back(v);
                            }   break;

                            case xil_storage_type_t::argument_addr:
                            case xil_storage_type_t::local_addr: {
                                bool is_arg = push->stype() == xil_storage_type_t::argument_addr;
                                std::vector<__slot_t> & slots = is_arg? args : locals;

                                size_t id = push->get_identity();
                                if (id >= slots.size())
                                {
                                    throw _ED(__e_t::bad_stack, fn.display,
                                              _T("variable identity out of range"));
                                }

                                stack.push_back(slots[id].ptr);     // The address of the slot.
                            }   break;

                            case xil_storage_type_t::object: {
                                if (push->get_object_type() == xil_storage_object_type_t::static_data)
                                {
                                    rt_type_t * data_type = analyzer.get_type(push->get_ref());
                                    if (data_type == nullptr)
                                        throw _ED(__e_t::unsupported_type, _T("(static data type)"));

                                    // The static fields of a generic type are those of its instance.
                                    data_type = __current_instance_of(data_type, host_type, analyzer);

                                    auto sit = __static_data_map.find(data_type);
                                    if (sit == __static_data_map.end())
                                    {
                                        data_type->pre_new(__env);

                                        __static_data_t sd {
                                            "april.static." + std::to_string(__static_data_map.size()),
                                            __descriptor_of(data_type),
                                            (int64_t)data_type->get_size(__env)
                                        };

                                        __module.add_global_line("@" + sd.global + " = internal global ptr null");
                                        sit = __static_data_map.emplace(data_type, sd).first;
                                    }

                                    stack.push_back(f.load("ptr",
                                        llb::value_t { llb::types::ptr, "@" + sit->second.global }));
                                    break;
                                }

                                // A type info is only used by the functions of the tuples: it is
                                // known when the program is compiled.
                                if (push->get_object_type() == xil_storage_object_type_t::type_info)
                                {
                                    rt_type_t * info_type = analyzer.get_type(push->get_ref());
                                    if (info_type == nullptr)
                                        throw _ED(__e_t::unsupported_xil, _T("push object (type info)"));

                                    // The type of an object is its descriptor: typeof is the
                                    // descriptor of the type.
                                    if (!is_tuple_type(info_type))
                                    {
                                        // The type of an array: it is only its type that is wanted.
                                        if (info_type->get_kind() == rt_type_kind_t::array)
                                        {
                                            stack.push_back(llb::value_t { llb::types::ptr,
                                                "@" + __meta_descriptor_of(info_type) });
                                            break;
                                        }

                                        if (info_type->get_kind() != rt_type_kind_t::general
                                                && info_type->get_kind() != rt_type_kind_t::generic)
                                            throw _ED(__e_t::unsupported_xil, _T("push object (type info)"));

                                        stack.push_back(llb::value_t { llb::types::ptr,
                                            "@" + __descriptor_of(info_type) });
                                        break;
                                    }

                                    marker_types.push_back(info_type);
                                    stack.push_back(llb::value_t { llb::types::ptr,
                                        "typeinfo:" + std::to_string(marker_types.size() - 1) });
                                    break;
                                }

                                if (push->get_object_type() != xil_storage_object_type_t::method_info)
                                    throw _ED(__e_t::unsupported_xil, _T("push object (type info)"));

                                // A method info is the function a delegate calls.
                                rt_method_base_t * method = analyzer.get_method(push->get_ref());
                                if (method != nullptr && method->this_type() == rt_member_type_t::general)
                                {
                                    rt_type_t * method_host = method->get_host_type();
                                    rt_type_t * instance = __current_instance_of(method_host, host_type, analyzer);

                                    if (instance != method_host)
                                    {
                                        method = analyzer.to_generic_method(
                                            static_cast<rt_method_t *>(method), instance
                                        );
                                    }
                                }

                                if (method == nullptr || method->this_type() == rt_member_type_t::dynamic)
                                    throw _ED(__e_t::unsupported_xil, _T("method info of a dynamic method"));

                                llb::value_t method_info {
                                    llb::types::ptr, "@" + __delegate_target_of(method)
                                };

                                if (method->this_type() == rt_member_type_t::general
                                        && method->get_host_type()->get_kind() != rt_type_kind_t::generic
                                        && method->get_host_type()->get_ttype(__env) != ttype_t::interface_)
                                {
                                    decorate_t decorate = (decorate_t)(*(rt_method_t *)method)->decorate;
                                    if (decorate.is_virtual || decorate.is_override || decorate.is_abstract)
                                    {
                                        int voffset = analyzer.get_virtual_method_offset(push->get_ref());

                                        __virtual_sites.insert({ method->get_host_type(), voffset });
                                        __max_virtual_offset = std::max(__max_virtual_offset, voffset);
                                        __virtual_delegate_offsets[method_info.text] = voffset;
                                    }
                                }

                                stack.push_back(method_info);
                            }   break;

                            case xil_storage_type_t::array_element:
                            case xil_storage_type_t::array_element_addr: {
                                __element_t e = element_of(push->get_ref());

                                if (push->stype() == xil_storage_type_t::array_element_addr)
                                {
                                    stack.push_back(e.address);
                                    break;
                                }

                                std::string ll = __ll_type_of(e.type);
                                xil_type_t xt = __xil_type_of(e.type);
                                if (ll[0] == '[' || xt == xil_type_t::empty)
                                    throw _ED(__e_t::unsupported_type, e.type->get_name(__env));

                                llb::value_t v = f.load(ll, e.address);
                                if (push->dtype() != xt && __conv_num_of(xt).valid
                                                        && __conv_num_of(push->dtype()).valid)
                                {
                                    v = __convert_any(f, v, xt, push->dtype());
                                }

                                stack.push_back(v);
                            }   break;

                            case xil_storage_type_t::params:
                                // The items of the pack: only the functions of the tuples use them.
                                stack.push_back(llb::value_t { llb::types::ptr, "params:" });
                                break;

                            case xil_storage_type_t::params_values: {
                                // The items of the pack are the arguments of a call: the values
                                // of the arguments of the function from the pack, one by one. A
                                // struct is the region of its argument (see the call).
                                if (fn.pack_start < 0)
                                    throw _ED(__e_t::bad_stack, fn.display, _T("no pack argument"));

                                for (int k = 0; k < fn.pack_count; k++)
                                {
                                    __slot_t & slot = args.at((size_t)(fn.pack_start + k));

                                    if (slot.type[0] == '[')
                                        stack.push_back(llb::value_t { slot.type, "&" + slot.ptr.text });
                                    else
                                        stack.push_back(f.load(slot.type, slot.ptr));
                                }
                            }   break;

                            case xil_storage_type_t::tuple_values: {
                                // The items of a tuple are the arguments of a call: the tuple is
                                // popped, the values of its items are pushed, one by one.
                                rt_type_t * type = analyzer.get_type(push->get_ref());
                                if (type == nullptr || !is_tuple_type(type))
                                    throw _ED(__e_t::unsupported_type, _T("(the items of a type that is not a tuple)"));

                                if (stack.empty())
                                    throw _ED(__e_t::bad_stack, fn.display, _T("the items of an empty stack"));

                                llb::value_t object = stack.back();
                                stack.pop_back();

                                rt_generic_type_t * tuple = (rt_generic_type_t *)type;
                                tuple->pre_new(__env);

                                for (int k = 0, count = tuple->atype_count(); k < count; k++)
                                {
                                    std::string ll = __ll_type_of(tuple->atypes[k]);
                                    msize_t offset = tuple->get_field_offset(__env, k);
                                    llb::value_t address = f.gep_bytes(object, (int64_t)offset);

                                    // A struct is the region of its item (see the arguments of a call).
                                    if (ll[0] == '[')
                                        stack.push_back(llb::value_t { ll, "&" + address.text });
                                    else
                                        stack.push_back(f.load(ll, address));
                                }
                            }   break;

                            case xil_storage_type_t::box: {
                                // Boxes the value (or the struct at the address) on the top.
                                rt_type_t * type = analyzer.get_type(push->get_ref());
                                if (type == nullptr)
                                    throw _ED(__e_t::unsupported_type, _T("(unresolved boxed type)"));

                                std::string ll = __ll_type_of(type);
                                bool is_block = ll[0] == '[';

                                bool pick = push->get_box_type() == xil_box_type_t::pick;
                                if (stack.empty())
                                    throw _ED(__e_t::bad_stack, fn.display, _T("box of an empty stack"));

                                llb::value_t value = stack.back();
                                if (!pick)
                                    stack.pop_back();

                                if (value.type != (is_block? "ptr" : ll))
                                {
                                    throw _ED(__e_t::bad_stack, fn.display, _F(
                                        _T("box of %1%, not %2%"), __wide(value.type), __wide(ll)
                                    ));
                                }

                                type->pre_new(__env);
                                __uses_new_object = true;

                                llb::value_t obj = f.call("ptr", APRIL_NATIVE_SYM_NEW_OBJECT, {
                                    llb::const_int("i64", (int64_t)type->get_size(__env)),
                                    llb::value_t { llb::types::ptr, "@" + __descriptor_of(type) }
                                });

                                if (is_block)
                                {
                                    __uses_memcpy = true;
                                    f.call("void", "llvm.memcpy.p0.p0.i64", { obj, value,
                                        llb::const_int("i64", (int64_t)type->get_size(__env)),
                                        llb::const_bool(false) });
                                }
                                else
                                {
                                    f.store(value, obj);
                                }

                                stack.push_back(obj);
                            }   break;

                            case xil_storage_type_t::unbox: {
                                // Unboxes: the value, or a copy into the struct at the address
                                // below the object.
                                rt_type_t * type = analyzer.get_type(push->get_ref());
                                if (type == nullptr)
                                    throw _ED(__e_t::unsupported_type, _T("(unresolved unboxed type)"));

                                std::string ll = __ll_type_of(type);
                                bool is_block = ll[0] == '[';

                                bool pick = push->get_box_type() == xil_box_type_t::pick;
                                if (stack.empty())
                                    throw _ED(__e_t::bad_stack, fn.display, _T("unbox of an empty stack"));

                                llb::value_t obj = stack.back();
                                if (!pick)
                                    stack.pop_back();

                                if (obj.type != "ptr")
                                    throw _ED(__e_t::bad_stack, fn.display, _T("unbox of a non reference"));

                                // Fails unless the object was boxed from this type.
                                llb::value_t key { llb::types::ptr,
                                    "@april.iface." + std::to_string(__interface_index(type)) };
                                __uses_cast = true;
                                raise_if_null(obj);
                                checked_cast(obj, key);

                                if (is_block)
                                {
                                    llb::value_t dst = pop_value("unbox destination");
                                    __uses_memcpy = true;
                                    f.call("void", "llvm.memcpy.p0.p0.i64", { dst, obj,
                                        llb::const_int("i64", (int64_t)type->get_size(__env)),
                                        llb::const_bool(false) });
                                }
                                else
                                {
                                    stack.push_back(f.load(ll, obj));
                                }
                            }   break;

                            case xil_storage_type_t::duplicate:
                                if (stack.empty())
                                    throw _ED(__e_t::bad_stack, fn.display, _T("dup of an empty stack"));

                                stack.push_back(stack.back());
                                break;

                            case xil_storage_type_t::convert:
                            case xil_storage_type_t::checked_convert: {
                                llb::value_t v = pop_value("convert");

                                // A pointer and an integer of 64 bits: the same bits.
                                if (push->dtype() == xil_type_t::ptr || push->dtype2() == xil_type_t::ptr)
                                {
                                    if (push->dtype2() == xil_type_t::ptr)
                                    {
                                        if (v.type != "ptr")
                                            v = f.cast("inttoptr", v, "ptr");
                                    }
                                    else if (v.type == "ptr")
                                    {
                                        v = f.cast("ptrtoint", v, "i64");
                                    }

                                    stack.push_back(v);
                                    break;
                                }

                                // The interpreter reads the top of the stack as the source
                                // type, whatever was pushed (e.g. an int64 constant followed
                                // by a convert from int32): do the same for integers.
                                __num_t from_num = __conv_num_of(push->dtype());
                                if (from_num.valid && !from_num.is_float && v.type != from_num.ll
                                    && (v.type == "i8" || v.type == "i16" || v.type == "i32"
                                                                         || v.type == "i64"))
                                {
                                    int bits = std::stoi(v.type.substr(1));
                                    v = f.cast(bits > from_num.bits? "trunc" : "zext", v, from_num.ll);
                                }

                                // checked: the value must fit the type.
                                __num_t cf = __conv_num_of(push->dtype());
                                __num_t ct = __conv_num_of(push->dtype2());

                                if (push->stype() == xil_storage_type_t::checked_convert
                                    && cf.valid && ct.valid && v.type == cf.ll
                                    && push->dtype() != xil_type_t::bool_
                                    && push->dtype2() != xil_type_t::bool_
                                    && push->dtype() != push->dtype2() && !ct.is_float)
                                {
                                    llb::value_t fail;

                                    if (cf.is_float)
                                    {
                                        // The value is truncated, and it must be in the range.
                                        llb::value_t d = cf.bits == 32? f.cast("fpext", v, "double") : v;
                                        __uses_trunc = true;
                                        d = f.call("double", "llvm.trunc.f64", { d });

                                        double lo = ct.is_signed? -std::ldexp(1.0, ct.bits - 1) : 0.0;
                                        double hi = std::ldexp(1.0, ct.is_signed? ct.bits - 1 : ct.bits);

                                        // The unordered conditions are also true for NaN.
                                        fail = f.binop("or",
                                            f.fcmp("ult", d, llb::const_double(lo)),
                                            f.fcmp("uge", d, llb::const_double(hi))
                                        );
                                    }
                                    else
                                    {
                                        llb::value_t n = v;
                                        if (ct.bits > cf.bits)
                                            n = f.cast(cf.is_signed? "sext" : "zext", v, ct.ll);
                                        else if (ct.bits < cf.bits)
                                            n = f.cast("trunc", v, ct.ll);

                                        fail = llb::const_int("i1", 0);

                                        // Narrower: the value changes when it is converted back.
                                        if (ct.bits < cf.bits)
                                        {
                                            llb::value_t back = f.cast(ct.is_signed? "sext" : "zext", n, cf.ll);
                                            fail = f.icmp("ne", back, v);
                                        }

                                        // A negative number is not an unsigned number, and an
                                        // unsigned number that is too large is negative.
                                        if (cf.is_signed && !ct.is_signed)
                                        {
                                            fail = f.binop("or", fail,
                                                f.icmp("slt", v, llb::const_int(cf.ll, 0)));
                                        }

                                        if (!cf.is_signed && ct.is_signed)
                                        {
                                            fail = f.binop("or", fail,
                                                f.icmp("slt", n, llb::const_int(ct.ll, 0)));
                                        }
                                    }

                                    raise_if(fail, CoreType_OverflowException);
                                }

                                llb::value_t r = __convert_any(f, v, push->dtype(), push->dtype2());
                                if (!r.valid())
                                {
                                    throw _ED(__e_t::unsupported_xil, _F(
                                        _T("push convert %1% => %2% (the value is %3%)"), push->dtype(), push->dtype2(),
                                        __wide(v.type)
                                    ));
                                }

                                stack.push_back(r);
                            }   break;

                            default:
                                throw _ED(__e_t::unsupported_xil, _F(
                                    _T("push %1%"), push->stype()
                                ));
                        }
                    }   break;

                    case xil_command_t::pick:
                    case xil_command_t::pop: {
                        // A pick has the layout of a pop: the value stays in the stack, it is
                        // copied below the operands of the destination.
                        const pop_xil_t * pop = reinterpret_cast<const pop_xil_t *>(xil);

                        if (xil->command() == xil_command_t::pick)
                        {
                            size_t operands = 0;
                            if (pop->stype() == xil_storage_type_t::field)
                            {
                                operands = 1;
                            }
                            else if (pop->stype() == xil_storage_type_t::array_element)
                            {
                                rt_type_t * array_type = analyzer.get_type(pop->get_ref());
                                if (array_type == nullptr || array_type->get_kind() != rt_type_kind_t::array)
                                    throw _ED(__e_t::unsupported_type, _T("(unresolved array type)"));

                                operands = 1 + ((rt_array_type_t *)array_type)->dimension;
                            }
                            else if (pop->stype() != xil_storage_type_t::local
                                        && pop->stype() != xil_storage_type_t::argument
                                        && pop->stype() != xil_storage_type_t::argument_addr)
                            {
                                throw _ED(__e_t::unsupported_xil, _F(_T("pick %1%"), pop->stype()));
                            }

                            std::vector<llb::value_t> held;
                            for (size_t k = 0; k < operands; k++)
                                held.push_back(pop_value("operand of a pick"));

                            llb::value_t kept = pop_value("value of a pick");
                            stack.push_back(kept);
                            stack.push_back(kept);

                            for (size_t k = operands; k > 0; k--)
                                stack.push_back(held[k - 1]);
                        }

                        switch (pop->stype())
                        {
                            case xil_storage_type_t::empty:         // Discards the top.
                                // The result of a call whose type is a generic param, that is
                                // not used: nothing is left when the type is a custom struct
                                // (the callee writes it at the address that was pushed), else the
                                // value (nothing for Void) and the address that was pushed.
                                if (pop->is_call_result())
                                {
                                    rt_type_t * result_type = analyzer.get_type(pop->get_ref());
                                    if (result_type == nullptr)
                                        throw _ED(__e_t::unsupported_type, _T("(unresolved result type)"));

                                    if (is_custom_struct(__env, result_type))
                                        break;

                                    if (result_type->get_vtype(__env) != vtype_t::void_)
                                        pop_value("result of a call");

                                    pop_value("place of the result of a call");
                                    break;
                                }

                                pop_value("pop of an empty stack");
                                break;

                            case xil_storage_type_t::argument:
                            case xil_storage_type_t::local: {
                                bool is_arg = pop->stype() == xil_storage_type_t::argument;
                                std::vector<__slot_t> & slots = is_arg? args : locals;

                                size_t id = pop->get_identity();
                                if (id >= slots.size())
                                {
                                    throw _ED(__e_t::bad_stack, fn.display,
                                              _T("variable identity out of range"));
                                }

                                llb::value_t v = pop_value("pop to a variable");
                                if (v.type != slots[id].type)
                                {
                                    throw _ED(__e_t::bad_stack, fn.display, _F(
                                        _T("type mismatch storing to a variable: %1% / %2%"),
                                        __wide(v.type), __wide(slots[id].type)
                                    ));
                                }

                                f.store(v, slots[id].ptr);
                            }   break;

                            case xil_storage_type_t::array_element: {
                                __element_t e = element_of(pop->get_ref());

                                std::string ll = __ll_type_of(e.type);
                                xil_type_t xt = __xil_type_of(e.type);
                                if (ll[0] == '[' || xt == xil_type_t::empty)
                                    throw _ED(__e_t::unsupported_type, e.type->get_name(__env));

                                llb::value_t v = pop_value("pop to an array element");
                                if (pop->dtype() != xt && __conv_num_of(xt).valid
                                                        && __conv_num_of(pop->dtype()).valid)
                                {
                                    v = __convert_any(f, v, pop->dtype(), xt);
                                }

                                if (v.type != ll)
                                {
                                    throw _ED(__e_t::bad_stack, fn.display, _F(
                                        _T("type mismatch storing to an element: %1% / %2%"),
                                        __wide(v.type), __wide(ll)
                                    ));
                                }

                                f.store(v, e.address);
                            }   break;

                            case xil_storage_type_t::argument_addr: {
                                // A ref / out argument: stores through the address.
                                size_t id = pop->get_identity();
                                if (id >= args.size() || args[id].type != "ptr")
                                    throw _ED(__e_t::bad_stack, fn.display, _T("not an address argument"));

                                rt_type_t * content_type = fn.param_rt_types[id];
                                std::string ll = __ll_type_of(content_type);
                                xil_type_t xt = __xil_type_of(content_type);
                                if (ll[0] == '[' || xt == xil_type_t::empty)
                                    throw _ED(__e_t::unsupported_type, content_type->get_name(__env));

                                llb::value_t v = pop_value("pop to an address argument");
                                if (pop->dtype() != xt && __conv_num_of(xt).valid
                                                        && __conv_num_of(pop->dtype()).valid)
                                {
                                    v = __convert_any(f, v, pop->dtype(), xt);
                                }

                                if (v.type != ll)
                                {
                                    throw _ED(__e_t::bad_stack, fn.display, _F(
                                        _T("type mismatch storing to an argument: %1% / %2%"),
                                        __wide(v.type), __wide(ll)
                                    ));
                                }

                                f.store(v, f.load("ptr", args[id].ptr));
                            }   break;

                            case xil_storage_type_t::ptr_content: {
                                // *p = value: the value is at the top, the address is below it.
                                llb::value_t v = pop_value("pop to a pointer");
                                llb::value_t addr = pop_value("address of a pointer");
                                if (addr.type != "ptr")
                                    addr = f.cast("inttoptr", addr, "ptr");

                                std::string ll = __ll_of_content(pop->dtype());
                                if (ll.empty())
                                    throw _ED(__e_t::unsupported_xil, _F(_T("pop ptr_content %1%"), pop->dtype()));

                                if (v.type != ll && ll == "ptr")
                                    v = f.cast("inttoptr", v, "ptr");

                                if (v.type != ll)
                                {
                                    throw _ED(__e_t::bad_stack, fn.display, _F(
                                        _T("type mismatch storing to a pointer: %1% / %2%"),
                                        __wide(v.type), __wide(ll)
                                    ));
                                }

                                f.store_unaligned(v, addr);
                            }   break;

                            case xil_storage_type_t::field: {
                                __field_info_t info = field_of(pop->get_ref());

                                llb::value_t obj = pop_value("field of an empty stack");
                                llb::value_t v = pop_value("pop to a field");
                                if (obj.type != "ptr")
                                    throw _ED(__e_t::bad_stack, fn.display, _T("field of a non reference"));

                                raise_if_null(obj);

                                // The value is of the xil type, the field of its own type.
                                if (info.xtype != pop->dtype() && __conv_num_of(info.xtype).valid
                                                    && __conv_num_of(pop->dtype()).valid)
                                {
                                    v = __convert_any(f, v, pop->dtype(), info.xtype);
                                }

                                if (v.type != __ll_type_of(info.type))
                                {
                                    throw _ED(__e_t::bad_stack, fn.display, _F(
                                        _T("type mismatch storing to a field: %1% / %2%"),
                                        __wide(v.type), __wide(__ll_type_of(info.type))
                                    ));
                                }

                                f.store(v, f.gep_bytes(obj, (int64_t)info.offset));
                            }   break;

                            default:
                                throw _ED(__e_t::unsupported_xil, _F(
                                    _T("pop %1%"), pop->stype()
                                ));
                        }
                    }   break;

                    case xil_command_t::call: {
                        const call_xil_t * call = static_cast<const call_xil_t *>(xil);
                        xil_call_type_t call_type = call->call_type();

                        if (call_type != xil_call_type_t::static_
                                && call_type != xil_call_type_t::instance
                                && call_type != xil_call_type_t::virtual_
                                && call_type != xil_call_type_t::internal
                                && call_type != xil_call_type_t::constrained)
                        {
                            throw _ED(__e_t::unsupported_xil, _F(_T("call %1%"), call_type));
                        }

                        rt_method_base_t * base = analyzer.get_method(call->method_ref());
                        if (base != nullptr && base->this_type() == rt_member_type_t::dynamic)
                        {
                            // The getter of an item of a tuple: reads the field of its struct.
                            rt_type_t * tuple = base->get_host_type();
                            std::string name = __narrow(base->get_name().c_str());
                            const char * prefix = "get_Item";

                            if (tuple->get_kind() != rt_type_kind_t::generic
                                    || name.compare(0, 8, prefix) != 0)
                            {
                                throw _ED(__e_t::unsupported_xil, _T("call of a dynamic method"));
                            }

                            int position = std::stoi(name.substr(8)) - 1;
                            rt_generic_type_t * tuple_type = (rt_generic_type_t *)tuple;
                            rt_type_t * item_type = tuple_type->atypes[position];

                            std::string item_ll = __ll_type_of(item_type);

                            llb::value_t object = pop_value("tuple");
                            raise_if_null(object);

                            msize_t offset = tuple_type->get_field_offset(__env, position);
                            if (item_ll[0] == '[')
                            {
                                // A struct is written at the address that is below the tuple.
                                llb::value_t dst = pop_value("address of a struct result");
                                __uses_memcpy = true;
                                f.call("void", "llvm.memcpy.p0.p0.i64", { dst,
                                    f.gep_bytes(object, (int64_t)offset),
                                    llb::const_int("i64", (int64_t)item_type->get_size(__env)),
                                    llb::const_bool(false) });
                                break;
                            }

                            stack.push_back(f.load(item_ll, f.gep_bytes(object, (int64_t)offset)));
                            break;
                        }

                        if (base == nullptr)
                            throw _ED(__e_t::unsupported_xil, _T("call of a dynamic method"));

                        // A member of a generic param: the method of the type that the param is
                        // (the argument is a concrete type here).
                        int constrained_offset = -1;
                        bool generic_virtual_site = false;
                        if (base->this_type() == rt_member_type_t::generic)
                        {
                            rt_generic_method_t * generic_method = static_cast<rt_generic_method_t *>(base);

                            // The method that implements it is found by the object, with the
                            // generic arguments of the call (see __generic_site_t).
                            if (call_type == xil_call_type_t::virtual_
                                    && generic_method->template_->generic_param_count() > 0)
                            {
                                generic_virtual_site = true;
                            }

                            if (call_type == xil_call_type_t::constrained)
                            {
                                rt::constrained_target_t t =
                                    rt::resolve_constrained_instance(__env, generic_method);

                                base = t.target;
                                call_type = xil_call_type_t::instance;

                                if (t.is_virtual)
                                {
                                    constrained_offset = t.virtual_offset;
                                    call_type = xil_call_type_t::virtual_;
                                }
                            }
                            else if (call_type == xil_call_type_t::static_)
                            {
                                if (rt_method_base_t * target =
                                        rt::resolve_constrained_static(__env, generic_method))
                                {
                                    base = target;
                                }
                            }
                        }

                        // A call to a method of the template of the current generic type:
                        // it is the method of the current instance.
                        if (base->this_type() == rt_member_type_t::general)
                        {
                            rt_type_t * base_host = base->get_host_type();
                            rt_type_t * instance = __current_instance_of(base_host, host_type, analyzer);

                            if (instance != base_host)
                            {
                                base = analyzer.to_generic_method(
                                    static_cast<rt_method_t *>(base), instance
                                );
                            }
                        }

                        // Delegates: the constructor and Invoke are expanded inline.
                        if (base->this_type() == rt_member_type_t::generic
                                && call_type == xil_call_type_t::instance
                                && base->get_host_type()->get_kind() == rt_type_kind_t::generic)
                        {
                            rt_generic_type_t * dtype = (rt_generic_type_t *)base->get_host_type();
                            std::string tname = __narrow(dtype->template_->get_name(__env).c_str());

                            // Only the constructors and Invoke are expanded; the other methods of a
                            // delegate (Equals) are called.
                            bool is_delegate_ctor =
                                static_cast<rt_generic_method_t *>(base)->template_ != nullptr
                                && (*static_cast<rt_generic_method_t *>(base)->template_)->trait
                                        == method_trait_t::constructor;
                            bool is_invoke = __narrow(base->get_name().c_str()) == "Invoke";

                            if ((tname.compare(0, 9, "Delegate<") == 0 || tname.find(".Delegate<") != std::string::npos)
                                    && (is_delegate_ctor || is_invoke))
                            {
                                bool invoke = is_invoke;

                                // The fields are __instance and __method, in this order.
                                std::vector<msize_t> field_offsets;
                                dtype->each_field(__env, [&](ref_t ref, rt_field_base_t *) {
                                    field_offsets.push_back(dtype->get_field_offset(__env, ref));
                                    return true;
                                });

                                if (field_offsets.size() != 2)
                                    throw _ED(__e_t::unsupported_type, dtype->get_name(__env));

                                msize_t instance_offset = field_offsets[0];
                                msize_t method_offset   = field_offsets[1];

                                if (!invoke)
                                {
                                    // Delegate(instance, method): stores the two.
                                    int param_count = 0;
                                    static_cast<rt_generic_method_t *>(base)->template_->get_assembly()
                                        ->each_params((*static_cast<rt_generic_method_t *>(base)
                                            ->template_)->params,
                                            [&](int, mt_param_t &) { param_count++; return true; });

                                    if (param_count != 2)
                                        throw _ED(__e_t::unsupported_xil, _T("delegate with a call type"));

                                    llb::value_t method_ptr = pop_value("delegate method");
                                    llb::value_t instance = pop_value("delegate instance");
                                    llb::value_t self = pop_value("delegate");

                                    // A virtual method: the implementation of the instance.
                                    auto vit = __virtual_delegate_offsets.find(method_ptr.text);
                                    if (vit != __virtual_delegate_offsets.end())
                                    {
                                        raise_if_null(instance);

                                        llb::value_t desc = f.load("ptr", f.gep_bytes(instance, -8));
                                        method_ptr = f.load("ptr", f.gep_bytes(
                                            desc, 32 + 8 * (int64_t)vit->second));
                                    }

                                    f.store(instance, f.gep_bytes(self, (int64_t)instance_offset));
                                    f.store(method_ptr, f.gep_bytes(self, (int64_t)method_offset));
                                    break;
                                }

                                // Invoke(args...): TReturn is the first type argument.
                                int count = dtype->atype_count() - 1;
                                rt_type_t * ret_rt = dtype->atypes[0];
                                std::string ret_ll = __ll_type_of(ret_rt);
                                bool returns_struct = ret_ll[0] == '[';
                                if (returns_struct)
                                    ret_ll = "void";

                                if ((int)stack.size() < count + 1)
                                    throw _ED(__e_t::bad_stack, fn.display, _T("not enough arguments"));

                                std::vector<llb::value_t> args(stack.end() - count, stack.end());
                                stack.resize(stack.size() - count);

                                for (int k = 0; k < count; k++)
                                {
                                    if (args[k].type[0] == '[' && args[k].text[0] == '&')
                                    {
                                        args[k] = f.load(__ll_type_of(dtype->atypes[1 + k]),
                                            llb::value_t { llb::types::ptr, args[k].text.substr(1) });
                                    }
                                }

                                llb::value_t self = pop_value("delegate");

                                llb::value_t instance = f.load("ptr", f.gep_bytes(self, (int64_t)instance_offset));
                                llb::value_t target = f.load("ptr", f.gep_bytes(self, (int64_t)method_offset));

                                std::vector<llb::value_t> call_args;
                                if (returns_struct)
                                    call_args.push_back(pop_value("address of a struct result"));

                                call_args.push_back(instance);
                                call_args.insert(call_args.end(), args.begin(), args.end());

                                llb::value_t r = f.call_indirect(ret_ll, target, call_args);
                                check_exception();
                                if (ret_ll != "void")
                                    stack.push_back(r);

                                break;
                            }
                        }

                        // The functions of the tuples are made inline: their type is known here.
                        std::string internal_name = call_type == xil_call_type_t::internal?
                                                __narrow(base->get_name().c_str()) : std::string();

                        if (internal_name == "Tuple_Create" || internal_name == "Tuple_SetRange"
                                || internal_name == "Tuple_Get" || internal_name == "Tuple_GetCount")
                        {
                            auto marker_type = [&](const llb::value_t & v) -> rt_generic_type_t * {
                                if (v.text.compare(0, 9, "typeinfo:") != 0)
                                    throw _ED(__e_t::unsupported_xil, _T("tuple type is not known"));

                                return (rt_generic_type_t *)marker_types.at(std::stoul(v.text.substr(9)));
                            };

                            // Reads the argument of the position of the pack.
                            auto pack_arg = [&](int k) -> __slot_t & {
                                if (fn.pack_start < 0 || k >= fn.pack_count)
                                    throw _ED(__e_t::bad_stack, fn.display, _T("no pack argument"));

                                return args.at((size_t)(fn.pack_start + k));
                            };

                            // Writes the items of the pack to the tuple.
                            auto store_items = [&](rt_generic_type_t * tuple, llb::value_t object) {
                                int count = tuple->atype_count();
                                for (int k = 0; k < count; k++)
                                {
                                    std::string ll = __ll_type_of(tuple->atypes[k]);

                                    __slot_t & slot = pack_arg(k);
                                    msize_t offset = tuple->get_field_offset(__env, k);
                                    if (ll[0] == '[')
                                    {
                                        __uses_memcpy = true;
                                        f.call("void", "llvm.memcpy.p0.p0.i64", {
                                            f.gep_bytes(object, (int64_t)offset), slot.ptr,
                                            llb::const_int("i64", (int64_t)tuple->atypes[k]->get_size(__env)),
                                            llb::const_bool(false) });
                                        continue;
                                    }

                                    f.store(f.load(slot.type, slot.ptr), f.gep_bytes(object, (int64_t)offset));
                                }
                            };

                            if (internal_name == "Tuple_Create" || internal_name == "Tuple_SetRange")
                            {
                                llb::value_t data = pop_value("tuple items");
                                llb::value_t type_value = pop_value("tuple type");
                                rt_generic_type_t * tuple = marker_type(type_value);

                                if (data.text != "params:")
                                    throw _ED(__e_t::unsupported_xil, _T("tuple items"));

                                if (internal_name == "Tuple_Create")
                                {
                                    tuple->pre_new(__env);
                                    __uses_new_object = true;

                                    llb::value_t obj = f.call("ptr", APRIL_NATIVE_SYM_NEW_OBJECT, {
                                        llb::const_int("i64", (int64_t)tuple->get_size(__env)),
                                        llb::value_t { llb::types::ptr, "@" + __descriptor_of(tuple) }
                                    });

                                    store_items(tuple, obj);
                                    stack.push_back(obj);
                                }
                                else
                                {
                                    // The tuple is the this of the constructor that calls it.
                                    store_items(tuple, f.load("ptr", args.at(0).ptr));
                                }

                                break;
                            }

                            if (internal_name == "Tuple_GetCount")
                            {
                                rt_generic_type_t * tuple = marker_type(pop_value("tuple type"));
                                stack.push_back(llb::const_int("i32", (int64_t)tuple->atype_count()));
                                break;
                            }

                            // Tuple_Get: the item is a reference, or an object that is made of it.
                            llb::value_t index = pop_value("tuple index");
                            rt_generic_type_t * tuple = marker_type(pop_value("tuple type"));
                            llb::value_t object = f.load("ptr", args.at(0).ptr);

                            llb::value_t result = llb::const_null();
                            int count = tuple->atype_count();

                            for (int k = 0; k < count; k++)
                            {
                                rt_type_t * item_type = tuple->atypes[k];
                                std::string ll = __ll_type_of(item_type);
                                bool is_block = ll[0] == '[';

                                msize_t offset = tuple->get_field_offset(__env, k);
                                llb::value_t address = f.gep_bytes(object, (int64_t)offset);
                                llb::value_t value = is_block? address : f.load(ll, address);

                                llb::value_t item;
                                bool is_string = item_type->get_vtype(__env) == vtype_t::string_;

                                if (is_ref_type(item_type->get_ttype(__env)) && !is_string)
                                {
                                    item = value;
                                }
                                else
                                {
                                    item_type->pre_new(__env);
                                    __uses_new_object = true;

                                    item = f.call("ptr", APRIL_NATIVE_SYM_NEW_OBJECT, {
                                        llb::const_int("i64", (int64_t)item_type->get_size(__env)),
                                        llb::value_t { llb::types::ptr, "@" + __descriptor_of(item_type) }
                                    });

                                    if (is_block)
                                    {
                                        __uses_memcpy = true;
                                        f.call("void", "llvm.memcpy.p0.p0.i64", { item, value,
                                            llb::const_int("i64", (int64_t)item_type->get_size(__env)),
                                            llb::const_bool(false) });
                                    }
                                    else
                                    {
                                        f.store(value, item);
                                    }

                                    // A string that is null is a null object.
                                    if (is_string)
                                    {
                                        item = f.select(f.icmp("eq", value, llb::const_null()),
                                                        llb::const_null(), item);
                                    }
                                }

                                result = f.select(
                                    f.icmp("eq", index, llb::const_int("i32", (int64_t)k)), item, result
                                );
                            }

                            stack.push_back(result);
                            break;
                        }

                        __fn_t * callee = __function_of(base);

                        // Takes the arguments (the last one is on the top).
                        size_t count = callee->param_types.size();
                        if (stack.size() < count)
                            throw _ED(__e_t::bad_stack, fn.display, _T("not enough arguments"));

                        std::vector<llb::value_t> call_args(stack.end() - count, stack.end());
                        stack.resize(stack.size() - count);

                        // A struct argument: loads the value of the region, as the type the
                        // function takes (a struct, or e.g. a pointer for a generic param).
                        for (size_t k = 0; k < call_args.size(); k++)
                        {
                            llb::value_t & arg = call_args[k];
                            if (arg.type[0] == '[' && arg.text[0] == '&')
                            {
                                arg = f.load(callee->param_types[k], llb::value_t {
                                    llb::types::ptr, arg.text.substr(1)
                                });
                            }
                        }

                        for (size_t k = 0; k < count; k++)
                        {
                            // A Char (16 bits, unsigned) is given where a number of 32 or 64 bits is taken.
                            if (call_args[k].type == "i16" && (callee->param_types[k] == "i32"
                                                              || callee->param_types[k] == "i64"))
                            {
                                call_args[k] = f.cast("zext", call_args[k], callee->param_types[k]);
                            }

                            if (call_args[k].type != callee->param_types[k])
                            {
                                throw _ED(__e_t::bad_stack, fn.display, _F(
                                    _T("argument %1% of %2%: %3% instead of %4%"), k,
                                    callee->display, __wide(call_args[k].type), __wide(callee->param_types[k])
                                ));
                            }
                        }

                        llb::value_t result;
                        bool hidden_dst = callee->returns_struct;
                        if (hidden_dst)
                        {
                            // The address of the result is below the arguments.
                            llb::value_t dst = pop_value("address of a struct result");
                            call_args.insert(call_args.begin(), dst);
                        }

                        if (call_type == xil_call_type_t::virtual_)
                        {
                            // Through the type descriptor of the object (see native_abi.h).
                            rt_method_base_t * declared = base;
                            ref_t method_ref = call->method_ref();
                            int offset = constrained_offset >= 0? constrained_offset
                                            : analyzer.get_virtual_method_offset(method_ref,
                                                                    base->get_host_type());

                            llb::value_t object = call_args[hidden_dst? 1 : 0];
                            raise_if_null(object);

                            llb::value_t desc = f.load("ptr", f.gep_bytes(object, -8));
                            llb::value_t target;

                            if (generic_virtual_site)
                            {
                                rt_generic_method_t * gm = static_cast<rt_generic_method_t *>(declared);

                                __generic_site_t site;
                                site.declared = declared->get_host_type();
                                site.offset = offset;
                                site.is_interface = site.declared->get_ttype(__env) == ttype_t::interface_;
                                site.interface_index = site.is_interface?
                                                __interface_index(site.declared) : 0;

                                for (int k = 0; k < gm->atype_count(); k++)
                                    site.atypes.push_back(gm->atypes[k]);

                                size_t g = 0;
                                for (; g < __generic_sites.size(); g++)
                                {
                                    const __generic_site_t & s = __generic_sites[g];
                                    if (s.declared == site.declared && s.offset == site.offset
                                            && s.atypes == site.atypes)
                                        break;
                                }

                                if (g == __generic_sites.size())
                                    __generic_sites.push_back(site);

                                // The functions of the generic sites follow the virtual ones.
                                llb::value_t real = f.load("i64",
                                    llb::value_t { llb::types::ptr, "@april.vreal" });
                                llb::value_t slot = f.binop("add", real,
                                                            llb::const_int("i64", (int64_t)g));
                                llb::value_t at = f.binop("add", f.binop("mul", slot,
                                                            llb::const_int("i64", 8)),
                                                            llb::const_int("i64", 32));

                                target = f.load("ptr", f.gep(llb::types::i8, desc, at));
                            }
                            else if (declared->get_host_type()->get_ttype(__env) == ttype_t::interface_)
                            {
                                size_t index = __interface_index(declared->get_host_type());
                                __interface_sites.insert({ index, offset });
                                __uses_find_interface = true;

                                target = f.call("ptr", APRIL_NATIVE_SYM_FIND_IFACE, {
                                    desc,
                                    llb::value_t { llb::types::ptr, "@april.iface." + std::to_string(index) },
                                    llb::const_int("i64", offset)
                                });
                            }
                            else
                            {
                                // The type that declares the method is the instance when the class is
                                // nested in a generic type: the tables have the instances.
                                rt_type_t * site_host = declared->get_host_type();
                                if (is_general(site_host)
                                        && ((rt_general_type_t *)site_host)->is_generic_template())
                                {
                                    site_host = analyzer.to_generic_type((rt_general_type_t *)site_host);
                                }

                                __virtual_sites.insert({ site_host, offset });
                                __max_virtual_offset = std::max(__max_virtual_offset, offset);

                                target = f.load("ptr", f.gep_bytes(desc, 32 + 8 * (int64_t)offset));
                            }

                            result = f.call_indirect(callee->ret_type, target, call_args);
                            check_exception();
                        }
                        else if (call_type == xil_call_type_t::internal)
                        {
                            // The array functions that read an array: it is not null.
                            if (callee->name.find("Array_Get") != std::string::npos
                                    && !call_args.empty() && call_args[0].type == "ptr")
                            {
                                raise_if_null(call_args[0]);
                            }

                            result = __emit_libfunc_call(f, fn, *callee, call_args);

                            // The method that Reflection_Invoke calls can raise.
                            if (__narrow(callee->method->get_name().c_str()) == "Reflection_Invoke")
                                check_exception();
                        }
                        else
                        {
                            // The object of an instance method is not null. (A string that is null is the empty string,
                            // as it is for the interpreter: its methods read it.)
                            size_t this_index = hidden_dst? 1 : 0;
                            rt_type_t * host_type = callee->base != nullptr? callee->base->get_host_type() : nullptr;
                            bool string_host = host_type != nullptr && host_type->get_vtype(__env) == vtype_t::string_;

                            // The this of a pointer (Ptr, Ptr<T>) is the address, that can be 0.
                            bool pointer_host = host_type != nullptr && host_type->get_vtype(__env) == vtype_t::ptr_;

                            if (call_type == xil_call_type_t::instance && this_index < call_args.size()
                                    && call_args[this_index].type == "ptr" && !string_host && !pointer_host)
                            {
                                raise_if_null(call_args[this_index]);
                            }

                            result = f.call(callee->ret_type, callee->name, call_args);
                            check_exception();

                            if (!callee->queued)
                            {
                                callee->queued = true;
                                __queue.push_back(callee);
                            }
                        }

                        if (callee->ret_type != "void")
                            stack.push_back(result);
                    }   break;

                    case xil_command_t::smp: {
                        const smp_xil_t * smp = static_cast<const smp_xil_t *>(xil);

                        if (smp->smp() == xil_smp_t::label)
                            break;      // Only a position: the blocks were made above.

                        if (smp->smp() == xil_smp_t::throw_)
                        {
                            llb::value_t exception = pop_value("exception");
                            if (exception.type != "ptr")
                                throw _ED(__e_t::bad_stack, fn.display, _T("throw of a non reference"));

                            // Throwing null raises a NullReferenceException.
                            raise_if_null(exception);

                            f.store(exception, exception_global);
                            f.br(dispatch_label(inner_block(xil_index)));
                            break;
                        }

                        if (smp->smp() == xil_smp_t::rethrow)
                        {
                            // The exception of the catch handler.
                            f.store(f.load("ptr", current_exception_slot()), exception_global);
                            f.br(dispatch_label(inner_block(xil_index)));
                            break;
                        }

                        if (smp->smp() == xil_smp_t::end_finally)
                        {
                            // Continues where the finally block was entered from: see the end.
                            std::string end_label = f.new_block("finallyend");
                            f.br(end_label);
                            finally_ends.push_back({ end_finally_of.at(xil_index), end_label });
                            break;
                        }

                        if (smp->smp() != xil_smp_t::ret)
                            throw _ED(__e_t::unsupported_xil, _F(_T("smp %1%"), smp->smp()));

                        emit_return();
                    }   break;

                    case xil_command_t::new_: {
                        const new_xil_t * nw = static_cast<const new_xil_t *>(xil);

                        if (nw->new_type() == xil_new_type_t::default_)
                        {
                            // An instance of a class, on the heap.
                            rt_type_t * type = analyzer.get_type(nw->type_ref());
                            if (type == nullptr)
                                throw _ED(__e_t::unsupported_type, _T("(unresolved allocated type)"));

                            // The template of the current generic type is the current instance, and
                            // a class that is nested in it is one of the instance too.
                            type = __current_instance_of(type, host_type, analyzer);

                            // An instance of a class, or a boxed struct.
                            std::string new_ll = __ll_type_of(type);
                            if (new_ll != "ptr" && new_ll[0] != '[')
                                throw _ED(__e_t::unsupported_type, type->get_name(__env));

                            // Computes the size and builds the virtual table, like the
                            // interpreter does before the first object of a type.
                            type->pre_new(__env);

                            __uses_new_object = true;
                            llb::value_t desc { llb::types::ptr, "@" + __descriptor_of(type) };
                            stack.push_back(f.call("ptr", APRIL_NATIVE_SYM_NEW_OBJECT, {
                                llb::const_int("i64", (int64_t)type->get_size(__env)), desc
                            }));
                            break;
                        }

                        if (nw->new_type() == xil_new_type_t::array)
                        {
                            rt_type_t * type = analyzer.get_type(nw->type_ref());
                            if (type == nullptr || type->get_kind() != rt_type_kind_t::array)
                                throw _ED(__e_t::unsupported_type, _T("(unresolved array type)"));

                            rt_array_type_t * array_type = (rt_array_type_t *)type;
                            int dimension = array_type->dimension;

                            // The length of the last dimension is on the top.
                            llb::value_t lengths = f.alloca_entry(
                                "[" + std::to_string(dimension) + " x i32]", "lens");

                            for (int k = dimension - 1; k >= 0; k--)
                            {
                                llb::value_t len = pop_value("array length");
                                if (len.type != "i32")
                                    throw _ED(__e_t::bad_stack, fn.display, _T("array length is not int32"));

                                f.store(len, f.gep_bytes(lengths, 4 * k));
                            }

                            __uses_arrays = true;
                            // Arrays of numbers hold no addresses: the collector skips them.
                            std::string element_ll = __ll_type_of(array_type->element_type);
                            bool has_refs = element_ll == "ptr" || element_ll[0] == '[';

                            // The class of an array has the methods of the interfaces that an array
                            // implements (IReadOnlyList<T>), they are those of Array<T>.
                            llb::value_t desc { llb::types::ptr, "@" + __descriptor_of(array_type) };

                            llb::value_t new_array = f.call("ptr", APRIL_NATIVE_SYM_NEW_ARRAY, {
                                llb::const_int("i64", (int64_t)array_type->element_type->get_variable_size(__env)),
                                llb::const_int("i32", dimension), lengths, desc,
                                llb::const_int("i32", has_refs? 1 : 0)
                            });

                            // A negative or too large length.
                            raise_if(f.icmp("eq", new_array, llb::const_null()),
                                     CoreType_OutOfRangeException);

                            stack.push_back(new_array);
                            break;
                        }

                        if (nw->new_type() != xil_new_type_t::stack_alloc
                                && nw->new_type() != xil_new_type_t::generic_stack_alloc)
                        {
                            throw _ED(__e_t::unsupported_xil, _F(_T("new %1%"), nw->new_type()));
                        }

                        // A temporary struct: a slot of the function, its address is pushed.
                        rt_type_t * type = analyzer.get_type(nw->type_ref());
                        if (type == nullptr)
                            throw _ED(__e_t::unsupported_type, _T("(unresolved allocated type)"));

                        // The destination of a value that a call returns: the argument of the
                        // generic param is a custom struct, or there is nothing to allocate.
                        if (nw->new_type() == xil_new_type_t::generic_stack_alloc
                                && __ll_type_of(type)[0] != '[')
                        {
                            break;
                        }

                        // A struct, or a value of the (concrete) type of a generic param.
                        msize_t size = type->get_ttype(__env) == ttype_t::struct_?
                                            type->get_size(__env) : type->get_variable_size(__env);
                        std::string ll = __block_type_of(size);

                        // The interpreter reserves the units on its stack and pushes their
                        // address: the region stays below, as the value of a struct argument.
                        // Here the region is a slot, and a marker of its value goes below
                        // the address; the value is loaded when it is used (see below).
                        llb::value_t slot = f.alloca_entry(ll, "tmp");
                        stack.push_back(llb::value_t { ll, "&" + slot.text });
                        stack.push_back(slot);
                    }   break;

                    case xil_command_t::cast: {
                        const cast_xil_t * ct = static_cast<const cast_xil_t *>(xil);

                        rt_type_t * target = analyzer.get_type(ct->type_ref());
                        if (target == nullptr)
                            throw _ED(__e_t::unsupported_type, _T("(unresolved cast type)"));

                        // A class that is nested in a generic type is the one of the instance.
                        if (is_general(target)
                                && ((rt_general_type_t *)target)->is_generic_template())
                        {
                            target = analyzer.to_generic_type((rt_general_type_t *)target);
                        }

                        // The classes, the interfaces, and the values that are boxed (o is int, o is
                        // String).
                        ttype_t target_ttype = target->get_ttype(__env);
                        bool target_is_value = target_ttype == ttype_t::struct_ || target_ttype == ttype_t::enum_;

                        if ((target->get_kind() != rt_type_kind_t::general && target->get_kind() != rt_type_kind_t::generic
                                    && target->get_kind() != rt_type_kind_t::array)
                                || (target_ttype != ttype_t::interface_ && target_ttype != ttype_t::class_
                                    && !target_is_value))
                        {
                            throw _ED(__e_t::unsupported_type, target->get_name(__env));
                        }

                        llb::value_t obj = pop_value("cast of an empty stack");
                        if (obj.type != "ptr")
                            throw _ED(__e_t::bad_stack, fn.display, _T("cast of a non reference"));

                        llb::value_t key { llb::types::ptr,
                            "@april.iface." + std::to_string(__interface_index(target)) };
                        __uses_cast = true;

                        switch (ct->cmd())
                        {
                            case xil_cast_command_t::default_:
                                checked_cast(obj, key);
                                stack.push_back(obj);
                                break;

                            case xil_cast_command_t::is:
                                stack.push_back(f.call("i8", APRIL_NATIVE_SYM_IS_INSTANCE, { obj, key }));
                                break;

                            case xil_cast_command_t::as: {
                                llb::value_t is = f.call("i8", APRIL_NATIVE_SYM_IS_INSTANCE, { obj, key });
                                llb::value_t c = f.icmp("ne", is, llb::const_int("i8", 0));
                                stack.push_back(f.select(c, obj, llb::const_null()));
                            }   break;

                            default:
                                throw _ED(__e_t::unsupported_xil, _F(_T("cast %1%"), ct->cmd()));
                        }
                    }   break;

                    case xil_command_t::init: {
                        const init_xil_t * init = static_cast<const init_xil_t *>(xil);

                        switch (init->init_type())
                        {
                            case xil_init_type_t::array_begin: {
                                // The array stays below: a cursor to its elements is pushed.
                                if (stack.empty() || stack.back().type != "ptr")
                                    throw _ED(__e_t::bad_stack, fn.display, _T("init of a non array"));

                                stack.push_back(stack.back());
                            }   break;

                            case xil_init_type_t::array_element: {
                                if (init->dtype() == xil_type_t::empty)
                                    throw _ED(__e_t::unsupported_xil, _T("init of a struct element"));

                                llb::value_t v = pop_value("array element");
                                if (stack.empty() || stack.back().type != "ptr")
                                    throw _ED(__e_t::bad_stack, fn.display, _T("init without a cursor"));

                                llb::value_t cursor = stack.back();
                                f.store(v, cursor);
                                // The element is as large as the value stored: a reference is an address.
                                int64_t element_size = v.type == "ptr"? 8 : (int64_t)size_of(init->dtype());
                                stack.back() = f.gep_bytes(cursor, element_size);
                            }   break;

                            case xil_init_type_t::array_end:
                                pop_value("array cursor");
                                break;

                            default:
                                throw _ED(__e_t::unsupported_xil, _F(_T("init %1%"), init->init_type()));
                        }
                    }   break;

                    case xil_command_t::copy: {
                        const copy_xil_t * copy = static_cast<const copy_xil_t *>(xil);

                        rt_type_t * type = analyzer.get_type(copy->type_ref());
                        if (type == nullptr)
                            throw _ED(__e_t::unsupported_type, _T("(unresolved copied type)"));

                        // The template of the current generic type is the current instance.
                        if (type->get_kind() == rt_type_kind_t::general
                                && host_type->get_kind() == rt_type_kind_t::generic
                                && is_host_type_template(type,
                                        ((rt_generic_type_t *)host_type)->template_))
                        {
                            type = host_type;
                        }



                        // A generic copy is of a generic param, already of a concrete type.
                        msize_t type_size = copy->copy_type() == xil_copy_type_t::generic_copy?
                                        type->get_variable_size(__env) : type->get_size(__env);

                        // The object of a call that is a generic param: a struct is its address,
                        // else the reference is loaded.
                        if (copy->copy_kind() == xil_copy_kind_t::receiver)
                        {
                            std::string ll = __ll_type_of(type);
                            if (ll[0] == '[')
                                break;

                            llb::value_t address = pop_value("load receiver");
                            if (address.type != "ptr")
                                throw _ED(__e_t::unsupported_type, type->get_name(__env));

                            stack.push_back(f.load(ll, address));
                            break;
                        }

                        // The value that a call returned is stored at the destination below it: a
                        // struct was written there by the call.
                        if (copy->copy_kind() == xil_copy_kind_t::store)
                        {
                            std::string ll = __ll_type_of(type);
                            if (ll[0] == '[')
                                break;

                            llb::value_t value = pop_value("stored value");
                            llb::value_t dst = pop_value("store destination");
                            if (dst.type != "ptr" || value.type != ll)
                                throw _ED(__e_t::bad_stack, fn.display, _T("store of a value"));

                            f.store(value, dst);
                            break;
                        }

                        // The value that a call returned: a struct was written at the
                        // destination that the call had (the region on the top), it is copied to
                        // the destination of this function; else it is the value on the top.
                        if (copy->copy_kind() == xil_copy_kind_t::call_ret)
                        {
                            std::string ll = __ll_type_of(type);
                            if (ll[0] != '[')
                                break;

                            if (!fn.returns_struct)
                                throw _ED(__e_t::bad_stack, fn.display, _T("no return address"));

                            llb::value_t region = pop_value("call result");
                            if (region.type[0] != '[' || region.text[0] != '&')
                                throw _ED(__e_t::bad_stack, fn.display, _T("call result is not a region"));

                            __uses_memcpy = true;
                            f.call("void", "llvm.memcpy.p0.p0.i64", { f.param(0),
                                llb::value_t { llb::types::ptr, region.text.substr(1) },
                                llb::const_int("i64", (int64_t)type_size), llb::const_bool(false) });
                            break;
                        }

                        // (T)obj: a reference is the object (it was checked by the cast), a value is loaded
                        // from the box (a string that is null is null), a struct is copied to the
                        // destination below.
                        if (copy->copy_kind() == xil_copy_kind_t::unbox)
                        {
                            bool is_string = type->get_vtype(__env) == vtype_t::string_;
                            if (is_ref_type(type->get_ttype(__env)) && !is_string)
                                break;

                            std::string ll = __ll_type_of(type);
                            llb::value_t obj = pop_value("unbox source");
                            if (obj.type != "ptr")
                                throw _ED(__e_t::bad_stack, fn.display, _T("unbox of a non reference"));

                            if (is_string)
                            {
                                llb::value_t is_null = f.icmp("eq", obj, llb::const_null());
                                llb::value_t value = f.load("ptr", obj);
                                stack.push_back(f.select(is_null, llb::const_null(), value));
                                break;
                            }

                            llb::value_t key { llb::types::ptr,
                                "@april.iface." + std::to_string(__interface_index(type)) };
                            __uses_cast = true;
                            raise_if_null(obj);
                            checked_cast(obj, key);

                            if (ll[0] == '[')
                            {
                                llb::value_t dst = pop_value("unbox destination");
                                __uses_memcpy = true;
                                f.call("void", "llvm.memcpy.p0.p0.i64", { dst, obj,
                                    llb::const_int("i64", (int64_t)type_size), llb::const_bool(false) });
                            }
                            else
                            {
                                stack.push_back(f.load(ll, obj));
                            }

                            break;
                        }

                        // The object at the address is made an object: a reference is loaded, a
                        // value is boxed.
                        if (copy->copy_kind() == xil_copy_kind_t::box)
                        {
                            bool is_string = type->get_vtype(__env) == vtype_t::string_;

                            std::string ll = __ll_type_of(type);
                            llb::value_t address = pop_value("box source");

                            // A number is the value itself, not its address.
                            bool is_value = address.type == ll && ll != "ptr";
                            if (address.type != "ptr" && !is_value)
                                throw _ED(__e_t::unsupported_type, type->get_name(__env));

                            if (is_ref_type(type->get_ttype(__env)) && !is_string)
                            {
                                stack.push_back(f.load("ptr", address));
                                break;
                            }

                            type->pre_new(__env);
                            __uses_new_object = true;

                            llb::value_t obj = f.call("ptr", APRIL_NATIVE_SYM_NEW_OBJECT, {
                                llb::const_int("i64", (int64_t)type_size),
                                llb::value_t { llb::types::ptr, "@" + __descriptor_of(type) }
                            });

                            if (ll[0] == '[')
                            {
                                __uses_memcpy = true;
                                f.call("void", "llvm.memcpy.p0.p0.i64", { obj, address,
                                    llb::const_int("i64", (int64_t)type_size), llb::const_bool(false) });
                            }
                            else
                            {
                                llb::value_t value = is_value? address : f.load(ll, address);
                                f.store(value, obj);

                                // A string that is null is a null object.
                                if (is_string)
                                {
                                    obj = f.select(f.icmp("eq", value, llb::const_null()),
                                                   llb::const_null(), obj);
                                }
                            }

                            stack.push_back(obj);
                            break;
                        }

                        // The object at the address is pushed as a value.
                        if (copy->copy_kind() == xil_copy_kind_t::ret)
                        {
                            llb::value_t address = pop_value("load source");
                            std::string ll = __ll_type_of(type);
                            if (address.type != "ptr")
                                throw _ED(__e_t::unsupported_type, type->get_name(__env));

                            // A custom struct: written at the address of the caller.
                            if (ll[0] == '[')
                            {
                                if (!fn.returns_struct)
                                    throw _ED(__e_t::bad_stack, fn.display, _T("no return address"));

                                __uses_memcpy = true;
                                f.call("void", "llvm.memcpy.p0.p0.i64", { f.param(0), address,
                                    llb::const_int("i64", (int64_t)type_size), llb::const_bool(false) });
                                break;
                            }

                            stack.push_back(f.load(ll, address));
                            break;
                        }

                        llb::value_t size = llb::const_int("i64", (int64_t)type_size);
                        llb::value_t dst, src;

                        switch (copy->copy_kind())
                        {
                            case xil_copy_kind_t::default_:      // dst is on the top.
                                dst = pop_value("copy destination");
                                src = pop_value("copy source");
                                break;

                            case xil_copy_kind_t::reverse:       // src is on the top.
                                src = pop_value("copy source");
                                dst = pop_value("copy destination");
                                break;

                            case xil_copy_kind_t::zero:
                                dst = pop_value("zero destination");
                                __uses_memset = true;
                                f.call("void", "llvm.memset.p0.i64",
                                       { dst, llb::const_int("i8", 0), size, llb::const_bool(false) });
                                break;

                            default:
                                throw _ED(__e_t::unsupported_xil, _F(
                                    _T("copy %1%"), copy->copy_kind()
                                ));
                        }

                        if (copy->copy_kind() != xil_copy_kind_t::zero)
                        {
                            __uses_memcpy = true;
                            f.call("void", "llvm.memcpy.p0.p0.i64",
                                   { dst, src, size, llb::const_bool(false) });
                        }
                    }   break;

                    case xil_command_t::jmp: {
                        const jmp_xil_t * jmp = static_cast<const jmp_xil_t *>(xil);

                        if (jmp->model() == xil_jmp_model_t::leave)
                        {
                            if (jmp->step() == 0)
                            {
                                // `leave.ret`: the compiler puts it before the xils that
                                // compute the result, which the interpreter returns without.
                                // The `ret` that follows runs the finally blocks, after the
                                // result is computed.
                                break;
                            }

                            size_t target_index = jump_target(xil_index, jmp);
                            jump_through_finally(xil_index, (int64_t)target_index,
                                                 block_labels.at(target_index));
                            break;
                        }

                        if (jmp->model() == xil_jmp_model_t::switch_)
                        {
                            const __switch_table_t & t = switch_tables[jmp->tbl()];

                            llb::value_t v = pop_value("switch value");
                            if (v.type != "i32")
                            {
                                throw _ED(__e_t::bad_stack, fn.display,
                                          _F(_T("switch value is %1%"), __wide(v.type)));
                            }

                            std::string def = block_labels.at(switch_target(xil_index, t.default_step));
                            spill(def);

                            std::vector<std::pair<int64_t, std::string>> cases;
                            for (auto & row : t.rows)
                            {
                                std::string label = block_labels.at(switch_target(xil_index, row.second));
                                spill(label);
                                cases.push_back({ row.first, label });
                            }

                            f.switch_(v, def, cases);
                            break;
                        }

                        std::string target = block_labels.at(jump_target(xil_index, jmp));

                        if (jmp->model() == xil_jmp_model_t::none)
                        {
                            spill(target);
                            f.br(target);
                            break;
                        }

                        llb::value_t v = pop_value("jump condition");
                        if (v.type != "i8")
                        {
                            throw _ED(__e_t::bad_stack, fn.display,
                                      _F(_T("jump condition is %1%"), __wide(v.type)));
                        }

                        std::string next = block_labels.at(xil_index + 1);
                        spill(target);
                        spill(next);

                        llb::value_t c = f.icmp("ne", v, llb::const_int("i8", 0));

                        if (jmp->model() == xil_jmp_model_t::true_)
                            f.cond_br(c, target, next);
                        else
                            f.cond_br(c, next, target);
                    }   break;

                    case xil_command_t::al: {
                        const al_xil_t * al = static_cast<const al_xil_t *>(xil);
                        xil_al_command_t cmd = al->cmd();

                        // The arithmetic that raises OverflowException: the operation, and
                        // the check.
                        bool checked = cmd == xil_al_command_t::checked_add
                                    || cmd == xil_al_command_t::checked_sub
                                    || cmd == xil_al_command_t::checked_mul
                                    || cmd == xil_al_command_t::checked_minus;

                        if (checked)
                        {
                            switch (cmd)
                            {
                                case xil_al_command_t::checked_add:   cmd = xil_al_command_t::add;   break;
                                case xil_al_command_t::checked_sub:   cmd = xil_al_command_t::sub;   break;
                                case xil_al_command_t::checked_mul:   cmd = xil_al_command_t::mul;   break;
                                default:                              cmd = xil_al_command_t::minus; break;
                            }
                        }

                        // Unitary.
                        if (cmd == xil_al_command_t::minus || cmd == xil_al_command_t::positive)
                        {
                            xil_type_t rt = __unitary_type(cmd, al->dtype1());
                            if (rt == xil_type_t::empty)
                                throw _ED(__e_t::unsupported_xil, _F(_T("al %1%"), al->dtype1()));

                            llb::value_t v = expect(pop_value("operand"), al->dtype1());
                            v = __convert(f, v, al->dtype1(), rt);

                            if (cmd == xil_al_command_t::minus)
                            {
                                // - MIN does not fit.
                                if (checked && !__num_of(rt).is_float && __num_of(rt).is_signed)
                                {
                                    int bits = __num_of(rt).bits;
                                    int64_t min_value = bits >= 64? INT64_MIN
                                                                  : -((int64_t)1 << (bits - 1));

                                    raise_if(f.icmp("eq", v, llb::const_int(v.type, min_value)),
                                             CoreType_OverflowException);
                                }

                                if (__num_of(rt).is_float)
                                    v = f.binop("fsub", rt == xil_type_t::float_? llb::const_float(-0.0f) : llb::const_double(-0.0), v);
                                else
                                    v = f.binop("sub", llb::const_int(v.type, 0), v);
                            }

                            stack.push_back(v);
                            break;
                        }

                        // Binary.
                        xil_type_t rt = __common_type(al->dtype1(), al->dtype2());
                        if (rt == xil_type_t::empty)
                        {
                            throw _ED(__e_t::unsupported_xil,
                                      _F(_T("al %1% %2%"), al->dtype1(), al->dtype2()));
                        }

                        llb::value_t b = expect(pop_value("operand"), al->dtype2());
                        llb::value_t a = expect(pop_value("operand"), al->dtype1());
                        a = __convert(f, a, al->dtype1(), rt);
                        b = __convert(f, b, al->dtype2(), rt);

                        __num_t n = __num_of(rt);
                        const char * op = nullptr;

                        switch (cmd)
                        {
                            case xil_al_command_t::add:
                                op = n.is_float? "fadd" : "add";
                                break;

                            case xil_al_command_t::sub:
                                op = n.is_float? "fsub" : "sub";
                                break;

                            case xil_al_command_t::mul:
                                op = n.is_float? "fmul" : "mul";
                                break;

                            case xil_al_command_t::div:
                                op = n.is_float? "fdiv" : (n.is_signed? "sdiv" : "udiv");
                                break;

                            case xil_al_command_t::mod:
                                op = n.is_float? "frem" : (n.is_signed? "srem" : "urem");
                                break;

                            default:
                                throw _ED(__e_t::unsupported_xil, _F(_T("al %1%"), cmd));
                        }

                        // Integer division: the divisor is not zero, and the result fits.
                        if (!n.is_float && (cmd == xil_al_command_t::div || cmd == xil_al_command_t::mod))
                        {
                            raise_if(f.icmp("eq", b, llb::const_int(b.type, 0)),
                                     CoreType_DivideByZeroException);

                            if (n.is_signed)
                            {
                                int64_t min_value = n.bits >= 64? INT64_MIN
                                                                : -((int64_t)1 << (n.bits - 1));

                                raise_if(f.binop("and",
                                    f.icmp("eq", a, llb::const_int(a.type, min_value)),
                                    f.icmp("eq", b, llb::const_int(b.type, -1))
                                ), CoreType_OverflowException);
                            }
                        }

                        // Add, sub, mul of integers: the intrinsics tell whether it overflows.
                        if (checked && !n.is_float)
                        {
                            const char * name = cmd == xil_al_command_t::add? "add"
                                              : cmd == xil_al_command_t::sub? "sub" : "mul";

                            std::string intrinsic = std::string("llvm.") + (n.is_signed? "s" : "u")
                                + name + ".with.overflow." + a.type;

                            __overflow_intrinsics[intrinsic] = a.type;

                            std::string pair_type = "{ " + a.type + ", i1 }";
                            llb::value_t pair = f.call(pair_type, intrinsic, { a, b });

                            llb::value_t value = f.extract_value(pair, 0, a.type);
                            llb::value_t overflow = f.extract_value(pair, 1, "i1");

                            raise_if(overflow, CoreType_OverflowException);
                            stack.push_back(value);
                            break;
                        }

                        stack.push_back(f.binop(op, a, b));
                    }   break;

                    case xil_command_t::bit: {
                        const bit_xil_t * bit = static_cast<const bit_xil_t *>(xil);
                        xil_bit_command_t cmd = bit->cmd();

                        if (cmd == xil_bit_command_t::bit_not)
                        {
                            xil_type_t rt = __unitary_type(xil_al_command_t::positive, bit->dtype1());
                            if (rt == xil_type_t::empty || __num_of(rt).is_float)
                                throw _ED(__e_t::unsupported_xil, _F(_T("bit %1%"), bit->dtype1()));

                            llb::value_t v = expect(pop_value("operand"), bit->dtype1());
                            v = __convert(f, v, bit->dtype1(), rt);
                            stack.push_back(f.binop("xor", v, llb::const_int(v.type, -1)));
                            break;
                        }

                        xil_type_t rt = __common_type(bit->dtype1(), bit->dtype2());
                        if (rt == xil_type_t::empty || __num_of(rt).is_float)
                        {
                            throw _ED(__e_t::unsupported_xil,
                                      _F(_T("bit %1% %2%"), bit->dtype1(), bit->dtype2()));
                        }

                        llb::value_t b = expect(pop_value("operand"), bit->dtype2());
                        llb::value_t a = expect(pop_value("operand"), bit->dtype1());
                        a = __convert(f, a, bit->dtype1(), rt);
                        b = __convert(f, b, bit->dtype2(), rt);

                        __num_t n = __num_of(rt);
                        const char * op = nullptr;

                        switch (cmd)
                        {
                            case xil_bit_command_t::bit_and: op = "and"; break;
                            case xil_bit_command_t::bit_or:  op = "or";  break;
                            case xil_bit_command_t::bit_xor: op = "xor"; break;

                            case xil_bit_command_t::left_shift:
                            case xil_bit_command_t::right_shift:
                                // The count is masked, like the hardware (and C#) does.
                                b = f.binop("and", b, llb::const_int(b.type, n.bits - 1));
                                op = cmd == xil_bit_command_t::left_shift? "shl"
                                        : (n.is_signed? "ashr" : "lshr");
                                break;

                            default:
                                throw _ED(__e_t::unsupported_xil, _F(_T("bit %1%"), cmd));
                        }

                        stack.push_back(f.binop(op, a, b));
                    }   break;

                    case xil_command_t::logic: {
                        const logic_xil_t * logic = static_cast<const logic_xil_t *>(xil);

                        if (logic->cmd() == xil_logic_command_t::not_)
                        {
                            llb::value_t v = pop_value("operand");
                            stack.push_back(f.binop("xor", v, llb::const_int("i8", 1)));
                            break;
                        }

                        llb::value_t b = pop_value("operand");
                        llb::value_t a = pop_value("operand");

                        // Both operands are evaluated already: no short circuit here.
                        stack.push_back(f.binop(
                            logic->cmd() == xil_logic_command_t::and_? "and" : "or", a, b
                        ));
                    }   break;

                    case xil_command_t::cmp: {
                        const cmp_xil_t * cmp = static_cast<const cmp_xil_t *>(xil);
                        xil_cmp_command_t cmd = cmp->cmd();

                        xil_type_t t1 = cmp->dtype1(), t2 = cmp->dtype2();

                        llb::value_t b = pop_value("operand");
                        llb::value_t a = pop_value("operand");

                        const char * icond = nullptr, * fcond = nullptr;
                        bool is_unsigned = false;

                        switch (cmd)
                        {
                            case xil_cmp_command_t::greater:
                                icond = "gt"; fcond = "ogt"; break;
                            case xil_cmp_command_t::greater_equal:
                                icond = "ge"; fcond = "oge"; break;
                            case xil_cmp_command_t::less:
                                icond = "lt"; fcond = "olt"; break;
                            case xil_cmp_command_t::less_equal:
                                icond = "le"; fcond = "ole"; break;
                            case xil_cmp_command_t::equal:
                                icond = "eq"; fcond = "oeq"; break;
                            case xil_cmp_command_t::not_equal:
                                icond = "ne"; fcond = "une"; break;
                            default:
                                throw _ED(__e_t::unsupported_xil, _F(_T("cmp %1%"), cmd));
                        }

                        llb::value_t r;
                        if (__num_of(t1).valid && __num_of(t2).valid)
                        {
                            xil_type_t ct = __common_type(t1, t2);
                            a = expect(a, t1);
                            b = expect(b, t2);
                            a = __convert(f, a, t1, ct);
                            b = __convert(f, b, t2, ct);

                            __num_t n = __num_of(ct);
                            if (n.is_float)
                            {
                                r = f.fcmp(fcond, a, b);
                            }
                            else
                            {
                                std::string cond = icond;
                                if (cond != "eq" && cond != "ne")
                                    cond = (n.is_signed? "s" : "u") + cond;

                                r = f.icmp(cond, a, b);
                            }
                        }
                        else if (t1 == t2 && (t1 == xil_type_t::bool_ || t1 == xil_type_t::char_
                                           || t1 == xil_type_t::object || t1 == xil_type_t::ptr))
                        {
                            // Chars are unsigned.
                            std::string cond = icond;
                            if (cond != "eq" && cond != "ne")
                                cond = "u" + cond;

                            if (a.type != b.type)
                            {
                                throw _ED(__e_t::bad_stack, fn.display,
                                          _T("operands of a comparison differ in type"));
                            }

                            r = f.icmp(cond, a, b);
                        }
                        else
                        {
                            throw _ED(__e_t::unsupported_xil, _F(_T("cmp %1% %2%"), t1, t2));
                        }

                        (void)is_unsigned;
                        stack.push_back(f.cast("zext", r, "i8"));       // bool: i8
                    }   break;

                    default:
                        throw _ED(__e_t::unsupported_xil, _F(_T("%1%"), xil->command()));
                }
            };

            for (xil_index = 0; xil_index < xils.size(); xil_index++)
                emit_xil(xils[xil_index]);

            // A body that ends without `ret` (or only with dead code) stays well formed.
            if (!f.terminated())
            {
                if (in_dead_code)
                    f.unreachable();
                else if (fn.ret_type == "void")
                    f.ret();
                else
                    f.unreachable();
            }

            // The shared return of the returns that pass finally blocks.
            if (!return_label.empty())
            {
                f.set_block(return_label);
                if (fn.ret_type == "void")
                    f.ret();
                else
                    f.ret(f.load(fn.ret_type, return_slot));
            }

            // The end of the finally blocks: goes back to where the block was entered from.
            for (auto & end : finally_ends)
            {
                int b = end.first;
                f.set_block(end.second);

                // 0: the exception goes on to the enclosing handlers.
                std::vector<std::pair<int64_t, std::string>> cases;
                for (size_t k = 0; k < cont_targets[b].size(); k++)
                    cases.push_back({ (int64_t)k + 1, cont_targets[b][k] });

                std::string reraise = f.new_block("reraise");

                llb::value_t selector = f.load("i32", cont_slots[b]);
                f.switch_(selector, reraise, cases);

                f.set_block(reraise);
                f.store(f.load("ptr", saved_exception_slots[b]), exception_global);
                f.br(dispatch_label(parent_of[b]));
            }

            // The dispatch blocks: the handler of a pending exception. They may need other
            // dispatch blocks, the map grows while they are made.
            std::set<int> made;
            for (bool more = true; more; )
            {
                more = false;

                std::vector<int> todo;
                for (auto & it : dispatch_labels)
                {
                    if (!made.count(it.first))
                        todo.push_back(it.first);
                }

                for (int b : todo)
                {
                    more = true;
                    made.insert(b);
                    f.set_block(dispatch_labels.at(b));

                    if (b == -1)
                    {
                        // No handler in this method: the exception stays pending.
                        if (fn.ret_type == "void")
                            f.ret();
                        else
                            f.ret(zero_of(fn.ret_type));

                        continue;
                    }

                    const method_xil_block_t & blk = xil_blocks[b];
                    if (blk.type == method_xil_block_type_t::finally_)
                    {
                        // Runs the finally block, then goes on looking for the handler.
                        f.store(f.load("ptr", exception_global), saved_exception_slots[b]);
                        f.store(llb::const_null(), exception_global);
                        f.store(llb::const_int("i32", 0), cont_slots[b]);
                        f.br(entry_label_of(b));
                        continue;
                    }

                    // A catch block: for the exceptions of the type.
                    rt_type_t * catch_type = analyzer.get_type(blk.relation_type);
                    if (catch_type == nullptr)
                        throw _ED(__e_t::unsupported_type, _T("(unresolved catch type)"));

                    llb::value_t exception = f.load("ptr", exception_global);
                    llb::value_t key { llb::types::ptr,
                        "@april.iface." + std::to_string(__interface_index(catch_type)) };
                    __uses_cast = true;

                    llb::value_t is = f.call("i8", APRIL_NATIVE_SYM_IS_INSTANCE, { exception, key });
                    llb::value_t match = f.icmp("ne", is, llb::const_int("i8", 0));

                    std::string take = f.new_block("catch");
                    f.cond_br(match, take, dispatch_label(parent_of[b]));

                    f.set_block(take);
                    f.store(llb::const_null(), exception_global);
                    f.store(exception, current_exception_slot());
                    f.store(exception, stack_slot(0, "ptr"));
                    f.br(entry_label_of(b));
                }
            }
        }

        //-------- ---------- ---------- ---------- ----------

        // Returns the type descriptor of a class (generated at the end).
        const std::string & __generator_t::__descriptor_of(rt_type_t * type)
        {
            auto it = __class_of.find(type);
            if (it != __class_of.end())
            {
                it->second->meta_only = false;      // The program uses it: it has its functions.

                // The class of an array has the functions of Array<T>.
                if (it->second->alias != nullptr)
                    it->second->alias->meta_only = false;

                return it->second->name;
            }

            if (type->get_kind() != rt_type_kind_t::general
                    && type->get_kind() != rt_type_kind_t::generic
                    && type->get_kind() != rt_type_kind_t::array)
            {
                throw _ED(__e_t::unsupported_type, type->get_name(__env));
            }

            // An array: its class has the functions of Array<T> (the one of its objects).
            __class_t * alias = nullptr;
            if (type->get_kind() == rt_type_kind_t::array)
            {
                type->pre_new(__env);

                rt_type_t * generic = ((rt_array_type_t *)type)->as_generic(__env);
                __descriptor_of(generic);
                alias = __class_of[generic];
            }

            std::unique_ptr<__class_t> c(new __class_t());
            c->type = type;
            c->alias = alias;
            c->name = "april.type." + std::to_string(__classes.size());

            __class_t * p = c.get();
            __classes.push_back(std::move(c));
            __class_of[type] = p;

            return p->name;
        }

        // Returns the descriptor of a type that the tables of reflection refer to (a class that has
        // no objects: it is only its names and its tables), or an empty string if the type has
        // none (the others).
        std::string __generator_t::__meta_descriptor_of(rt_type_t * type)
        {
            auto it = __class_of.find(type);
            if (it != __class_of.end())
                return it->second->name;

            if (type->get_kind() != rt_type_kind_t::general
                    && type->get_kind() != rt_type_kind_t::generic
                    && type->get_kind() != rt_type_kind_t::array)
            {
                return std::string();
            }

            // An array: only its type is wanted, the class of Array<T> has no objects either.
            __class_t * alias = nullptr;
            if (type->get_kind() == rt_type_kind_t::array)
            {
                type->pre_new(__env);

                rt_type_t * generic = ((rt_array_type_t *)type)->as_generic(__env);
                __meta_descriptor_of(generic);
                alias = __class_of[generic];
            }

            std::unique_ptr<__class_t> c(new __class_t());
            c->type = type;
            c->alias = alias;
            c->name = "april.type." + std::to_string(__classes.size());
            c->meta_only = true;

            __class_t * p = c.get();
            __classes.push_back(std::move(c));
            __class_of[type] = p;

            return p->name;
        }

        // Returns the global that holds the object of the static data of a type.
        std::string __generator_t::__static_data_global_of(rt_type_t * type)
        {
            auto sit = __static_data_map.find(type);
            if (sit == __static_data_map.end())
            {
                type->pre_new(__env);

                __static_data_t sd {
                    "april.static." + std::to_string(__static_data_map.size()),
                    __descriptor_of(type),
                    (int64_t)type->get_size(__env)
                };

                __module.add_global_line("@" + sd.global + " = internal global ptr null");
                sit = __static_data_map.emplace(type, sd).first;
            }

            return sit->second.global;
        }

        // Returns the name of the function a delegate to the method calls: it takes the
        // instance first (after the address of a struct result), then the arguments.
        std::string __generator_t::__delegate_target_of(rt_method_base_t * method)
        {
            __fn_t * target = __function_of(method);
            if (!target->queued)
            {
                target->queued = true;
                __queue.push_back(target);
            }

            // An instance method takes the instance as `this`.
            if (!((decorate_value_t)(*target->method)->decorate).is_static)
                return target->name;

            auto it = __thunks.find(target);
            if (it != __thunks.end())
                return it->second;

            std::string name = target->name + ".thunk";

            std::vector<std::pair<std::string, std::string>> params;
            std::vector<llb::value_t> call_args;

            if (target->returns_struct)
            {
                params.push_back({ "ptr", "retaddr" });
                call_args.push_back(llb::value_t { llb::types::ptr, "%retaddr" });
            }

            params.push_back({ "ptr", "instance" });

            for (size_t k = 0; k < target->param_types.size(); k++)
            {
                params.push_back({ target->param_types[k], "p" + std::to_string(k) });
                call_args.push_back(llb::value_t { target->param_types[k], "%p" + std::to_string(k) });
            }

            llb::function_t & thunk = __module.define(name, target->ret_type, params);
            llb::value_t r = thunk.call(target->ret_type, target->name, call_args);

            if (target->ret_type == "void")
                thunk.ret();
            else
                thunk.ret(r);

            __thunks[target] = name;
            return name;
        }

        // Returns the symbol of a function in the virtual table of a class.
        std::string __generator_t::__virtual_symbol_of(__fn_t * fn)
        {
            bool is_static = ((decorate_value_t)(*fn->method)->decorate).is_static;

            // `this` is the reference of the object, a struct is its address: only the value of a
            // primitive type is not, and a string (the object that boxes a string has its pointer).
            bool this_is_string = !fn->param_rt_types.empty() && fn->param_rt_types[0] != nullptr
                    && fn->param_rt_types[0]->get_vtype(__env) == vtype_t::string_;

            if (is_static || fn->param_types.empty() || (fn->param_types[0] == "ptr" && !this_is_string)
                    || fn->param_types[0][0] == '[')
                return fn->name;

            auto it = __unbox_thunks.find(fn);
            if (it != __unbox_thunks.end())
                return it->second;

            std::string name = fn->name + ".unbox";

            std::vector<std::pair<std::string, std::string>> params;
            std::vector<llb::value_t> call_args;

            if (fn->returns_struct)
            {
                params.push_back({ "ptr", "retaddr" });
                call_args.push_back(llb::value_t { llb::types::ptr, "%retaddr" });
            }

            params.push_back({ "ptr", "instance" });

            for (size_t k = 1; k < fn->param_types.size(); k++)
                params.push_back({ fn->param_types[k], "p" + std::to_string(k) });

            llb::function_t & thunk = __module.define(name, fn->ret_type, params);

            call_args.push_back(thunk.load(fn->param_types[0],
                                           llb::value_t { llb::types::ptr, "%instance" }));

            for (size_t k = 1; k < fn->param_types.size(); k++)
                call_args.push_back(llb::value_t { fn->param_types[k], "%p" + std::to_string(k) });

            llb::value_t r = thunk.call(fn->ret_type, fn->name, call_args);

            if (fn->ret_type == "void")
                thunk.ret();
            else
                thunk.ret(r);

            __unbox_thunks[fn] = name;
            return name;
        }

        // Returns the index of an interface in __interfaces.
        size_t __generator_t::__interface_index(rt_type_t * interface_type)
        {
            for (size_t k = 0; k < __interfaces.size(); k++)
            {
                if (__interfaces[k] == interface_type)
                    return k;
            }

            __interfaces.push_back(interface_type);
            return __interfaces.size() - 1;
        }

        // Finds the functions of the descriptors; returns whether something was added.
        bool __generator_t::__resolve_virtuals()
        {
            bool added = false;

            // Finds the method of a function of a virtual table.
            auto method_of = [&](const rt_vfunction_t & vf, rt_type_t * type) -> rt_method_base_t * {
                if (vf.initialized())
                    throw _ED(__e_t::unsupported_method, _T("(vtable)"), _T("a resolved function"));

                rt_assembly_t * assembly = __env.assemblies.at(vf.assembly_idx);
                assembly_analyzer_t analyzer(__env, assembly);

                rt_method_base_t * base = analyzer.get_method(ref_t(vf.method_idx));
                if (base == nullptr || base->this_type() != rt_member_type_t::general)
                    throw _ED(__e_t::unsupported_method, _T("(vtable)"), _T("a generic method"));

                // The method of a generic type is generated for its type arguments.
                // A method that a generic type has from a base type that is not generic (Object.Equals)
                // is the method of that type.
                if (type->get_kind() == rt_type_kind_t::generic)
                {
                    rt_type_t * declaring = base->get_host_type();
                    bool host_is_generic = declaring != nullptr
                        && (declaring->get_kind() == rt_type_kind_t::generic
                            || (is_general(declaring)
                                && ((rt_general_type_t *)declaring)->is_generic_template()));

                    if (host_is_generic)
                        return analyzer.to_generic_method(static_cast<rt_method_t *>(base), type);
                }

                return base;
            };

            auto queue = [&](rt_method_base_t * method) {
                __fn_t * fn = __function_of(method);
                if (!fn->queued)
                {
                    fn->queued = true;
                    __queue.push_back(fn);
                }

                return fn;
            };

            for (size_t k = 0; k < __classes.size(); k++)       // The list may grow.
            {
                __class_t & c = *__classes[k];
                if (c.meta_only || c.alias != nullptr)
                    continue;

                c.type->pre_new(__env);                         // Builds the virtual table.

                rt_vtable_t * vtbl = get_vtable(c.type);
                if (vtbl == nullptr)
                    throw _ED(__e_t::unsupported_type, c.type->get_name(__env));

                // The number of virtual functions: the base types follow them.
                size_t vcount = ((byte_t *)vtbl->base_types - (byte_t *)vtbl->functions)
                                                                / sizeof(rt_vfunction_t);

                for (auto & site : __virtual_sites)
                {
                    int offset = site.second;
                    if (c.vfunctions.count(offset) || (size_t)offset >= vcount)
                        continue;

                    // Only the classes derived from the declaring type of the call.
                    bool derived = c.type == site.first;
                    for (msize_t b = 0; !derived && b < vtbl->base_type_count; b++)
                        derived = vtbl->base_types[b] == site.first;

                    if (!derived)
                        continue;

                    // An abstract method has no body: the slot of the class that declares it stays empty (a class
                    // that has static fields has a descriptor, though it has no object).
                    rt_method_base_t * target = method_of(vtbl->functions[offset], c.type);
                    rt_method_t * target_method = target->this_type() == rt_member_type_t::general?
                        (rt_method_t *)target : ((rt_generic_method_t *)target)->template_;

                    if (((decorate_t)(*target_method)->decorate).is_abstract)
                        continue;

                    c.vfunctions[offset] = queue(target);
                    added = true;
                }

                for (size_t g = 0; g < __generic_sites.size(); g++)
                {
                    if (c.gfunctions.count(g))
                        continue;

                    const __generic_site_t & site = __generic_sites[g];
                    rt_vfunction_t vf;

                    if (site.is_interface)
                    {
                        rt_vtable_interface_t * itf = vtbl->search_vtable_interface_variant(__env,
                                                            __interfaces[site.interface_index]);
                        if (itf == nullptr)
                            continue;

                        vf = itf->functions[site.offset];
                    }
                    else
                    {
                        bool derived = c.type == site.declared;
                        for (msize_t b = 0; !derived && b < vtbl->base_type_count; b++)
                            derived = vtbl->base_types[b] == site.declared;

                        if (!derived || (size_t)site.offset >= vcount)
                            continue;

                        vf = vtbl->functions[site.offset];
                    }

                    if (vf.initialized())
                        throw _ED(__e_t::unsupported_method, _T("(vtable)"), _T("a resolved function"));

                    rt_assembly_t * assembly = __env.assemblies.at(vf.assembly_idx);
                    assembly_analyzer_t analyzer(__env, assembly);

                    rt_method_base_t * base = analyzer.get_method(ref_t(vf.method_idx));
                    if (base == nullptr || base->this_type() != rt_member_type_t::general)
                        throw _ED(__e_t::unsupported_method, _T("(vtable)"), _T("a generic method"));

                    // The generic arguments are those of the call, the type is the one of the object.
                    rt_method_t * method = static_cast<rt_method_t *>(base);
                    bool is_generic = method->generic_param_count() > 0;

                    rt_method_base_t * target = base;
                    if (is_generic || c.type->get_kind() == rt_type_kind_t::generic)
                    {
                        target = analyzer.to_generic_method(
                            method, c.type,
                            is_generic? const_cast<rt_type_t **>(site.atypes.data()) : nullptr,
                            is_generic? (int)site.atypes.size() : 0
                        );
                    }

                    c.gfunctions[g] = queue(target);
                    added = true;
                }

                for (auto & site : __interface_sites)
                {
                    if (c.ifunctions.count(site))
                        continue;

                    rt_vtable_interface_t * itf = vtbl->search_vtable_interface_variant(__env,
                                                        __interfaces[site.first]);
                    if (itf == nullptr)
                        continue;

                    c.ifunctions[site] = queue(method_of(itf->functions[site.second], c.type));
                    added = true;
                }
            }

            return added;
        }

        // Returns the function that a method of the tables has, null when the method cannot be
        // called by Invoke: it has no function, or returns a struct.
        __generator_t::__fn_t * __generator_t::__invoker_fn_of(rt_reflect_member_t * m)
        {
            if (!__uses_invoke || m->static_data == nullptr || (m->flags & reflect_flag_field) != 0)
                return nullptr;

            // A method of an interface has no function: the one of the object is found when it is
            // called (see __interface_method_of), the signature is the one of the method.
            __fn_t * interface_fn = nullptr;
            if (__is_interface_member(m))
            {
                try
                {
                    interface_fn = __function_of((rt_method_base_t *)m->static_data);
                }
                catch (const error_t &)
                {
                    return nullptr;
                }
            }

            auto it = __fns.find((rt_method_base_t *)m->static_data);
            if (interface_fn == nullptr && (it == __fns.end() || !it->second->queued))
                return nullptr;

            __fn_t & fn = interface_fn != nullptr? *interface_fn : *it->second;
            if (fn.ret_type[0] == '[' || fn.denied)
                return nullptr;

            bool is_static = (m->flags & 16) != 0;
            if (fn.param_types.size() != (size_t)m->param_count + (is_static? 0 : 1))
                return nullptr;

            return &fn;
        }

        // Returns the function that calls the functions of a shape of signature: it takes the
        // function, the units of the values (8 bytes each), and the address of the result.
        std::string __generator_t::__shape_thunk_of(__fn_t & fn)
        {
            std::string key = fn.ret_type + (fn.returns_struct? "!" : "");
            for (const std::string & type : fn.param_types)
                key += "," + type;

            auto it = __shape_thunks.find(key);
            if (it != __shape_thunks.end())
                return it->second;

            std::string name = "april.shape." + std::to_string(__shape_thunks.size());
            __shape_thunks[key] = name;

            llb::function_t & thunk = __module.define(name, "void", {
                { "ptr", "function" }, { "ptr", "values" }, { "ptr", "result" }
            });

            llb::value_t function { llb::types::ptr, "%function" };
            llb::value_t values   { llb::types::ptr, "%values" };
            llb::value_t result   { llb::types::ptr, "%result" };

            // A struct is a block of bytes: it takes the units that its size needs. A struct that
            // the function returns is written to the result, that is passed first.
            std::vector<llb::value_t> args;
            if (fn.returns_struct)
                args.push_back(result);

            int64_t unit = 0;
            for (size_t k = 0; k < fn.param_types.size(); k++)
            {
                const std::string & type = fn.param_types[k];
                args.push_back(thunk.load(type, thunk.gep_bytes(values, 8 * unit)));

                if (type[0] != '[')
                {
                    unit++;
                    continue;
                }

                // [N x i64], or [N x i8].
                int64_t count = std::stoll(type.substr(1));
                unit += type.find("x i64") != std::string::npos? count : (count + 7) / 8;
            }

            llb::value_t r = thunk.call_indirect(fn.ret_type, function, args);
            if (fn.ret_type != "void")
                thunk.store(r, result);

            thunk.ret();
            return name;
        }

        // Returns whether the member is a method of an interface that is called on the objects.
        bool __generator_t::__is_interface_member(rt_reflect_member_t * m)
        {
            return m->offset >= 0 && m->declaring != nullptr
                    && ((rt_type_t *)m->declaring)->get_ttype(__env) == ttype_t::interface_
                    && (m->flags & reflect_flag_field) == 0 && (m->flags & 16) == 0;
        }

        // Returns the key of a type, "null" if it has none.
        std::string __generator_t::__interface_key_of(rt_type_t * type)
        {
            // Object: every object is one (the key is 1, see april_native_is_instance).
            if (type != nullptr && type->get_kind() == rt_type_kind_t::general
                    && type->get_ttype(__env) == ttype_t::class_
                    && type->get_base_type(__env) == nullptr)
            {
                return "inttoptr (i64 1 to ptr)";
            }

            // The objects of reflection (Type, Method, ...) are handles: they have no descriptor
            // before them (the key is 2: they are not checked).
            if (type != nullptr && (type->get_kind() == rt_type_kind_t::general))
            {
                rt_sid_t sid = type->get_full_name(__env);
                string_t full = sid.value != nullptr? (string_t)*sid.value : string_t();

                if (full.find(_T("Reflection.")) == 0 || full.find(_T("System.Reflection.")) == 0)
                    return "inttoptr (i64 2 to ptr)";
            }

            for (size_t k = 0; k < __interfaces.size(); k++)
            {
                if (__interfaces[k] == type)
                    return "@april.iface." + std::to_string(k);
            }

            return "null";
        }

        // Generates the methods that Method.Invoke can call: those of the classes of the program
        // that have objects. Returns whether it found something to generate.
        bool __generator_t::__queue_invokable()
        {
            if (!__uses_invoke)
                return false;

            bool added = false;

            // The constructors of the attributes of the members that the tables have: their
            // classes have objects, and the constructors are called by Invoke.
            if (__uses_attributes)
            {
                auto visit = [&](void * key) {
                    if (!__attributes_done.insert(key).second)
                        return;

                    rt_reflect_attributes_t attributes;
                    if (!__reflection.get_attributes(key, &attributes))
                        return;

                    assembly_analyzer_t analyzer(__env, attributes.assembly);
                    for (ref_t ref : attributes.refs)
                    {
                        rt_attribute_t * attribute = attributes.assembly->get_attribute(ref);
                        rt_method_base_t * constructor = analyzer.get_method((*attribute)->constructor);

                        if (constructor == nullptr || constructor->this_type() != rt_member_type_t::general)
                            continue;

                        try
                        {
                            __descriptor_of(constructor->get_host_type());
                            __invoke_tried.insert(constructor);

                            __fn_t * fn = __function_of(constructor);
                            if (!fn->queued)
                            {
                                fn->queued = true;
                                __queue.push_back(fn);
                            }

                            added = true;
                        }
                        catch (const error_t &)
                        {
                            continue;
                        }

                        // The types of the values that are the arguments of an Object (the box of
                        // a number or of a string) and of a Type: their descriptors are needed.
                        for (ref_t arg_ref : (*attribute)->arguments)
                        {
                            rt_attribute_argument_t * argument =
                                                attributes.assembly->get_attribute_argument(arg_ref);
                            ref_t value_ref = (*argument)->value;
                            if (value_ref == ref_t::null)
                                continue;

                            try
                            {
                                ref_t type_ref;
                                if (attributes.assembly->get_constant_type(value_ref, &type_ref))
                                {
                                    rt_type_t * type = analyzer.get_type(type_ref);
                                    if (type != nullptr)
                                        __meta_descriptor_of(type);
                                }
                                else if ((*argument)->value_type != ref_t::null)
                                {
                                    rt_type_t * type = analyzer.get_type((*argument)->value_type);
                                    if (type != nullptr
                                            && type->get_vtype(__env) != vtype_t::mobject_)
                                    {
                                        type->pre_new(__env);
                                        __descriptor_of(type);
                                        added = true;
                                    }
                                }
                            }
                            catch (const error_t &)
                            {
                                continue;
                            }
                        }
                    }
                };

                for (size_t k = 0; k < __classes.size(); k++)
                {
                    __class_t & c = *__classes[k];
                    if (c.meta_only || (c.type->get_kind() != rt_type_kind_t::general
                                        && c.type->get_kind() != rt_type_kind_t::generic))
                        continue;

                    // The table makes the attributes of the type known.
                    rt_reflect_type_t * info = __reflection.get_type(c.type);
                    visit(c.type);
                    visit(info->assembly);

                    for (int kind = 0; kind < 5; kind++)
                    {
                        rt_reflect_list_t & list = info->lists[kind];
                        for (int64_t i = 0; i < list.count; i++)
                        {
                            visit(list.items[i]);

                            for (int64_t p = 0; p < list.items[i]->param_count; p++)
                                visit(list.items[i]->params[p]);

                            for (int64_t p = 0; p < list.items[i]->gparam_count; p++)
                                visit(list.items[i]->gparams[p]);
                        }
                    }
                }
            }

            // The methods of the interfaces that the tables have: the classes have their functions in
            // the tables of their interfaces (the offset is the one of the method in it).
            for (size_t k = 0; k < __classes.size(); k++)
            {
                __class_t & c = *__classes[k];
                if (c.type->get_ttype(__env) != ttype_t::interface_
                        || (c.type->get_kind() != rt_type_kind_t::general
                            && c.type->get_kind() != rt_type_kind_t::generic))
                    continue;

                rt_reflect_type_t * info = __reflection.get_type(c.type);
                rt_reflect_list_t & list = info->lists[(int)rt_reflect_kind_t::method];

                for (int64_t i = 0; i < list.count; i++)
                {
                    rt_reflect_member_t * m = list.items[i];
                    if (!__is_interface_member(m) || __invoker_fn_of(m) == nullptr)
                        continue;

                    size_t index = __interface_index(c.type);
                    if (__interface_sites.insert({ index, (int)m->offset }).second)
                    {
                        __uses_find_interface = true;
                        added = true;

                        // The box of a number that it returns has its functions (ToString).
                        __promote_result_class(m);
                    }
                }
            }

            for (size_t k = 0; k < __classes.size(); k++)
            {
                __class_t & c = *__classes[k];
                vtype_t class_vtype = c.type->get_vtype(__env);
                bool is_number = (class_vtype >= vtype_t::int8_ && class_vtype < vtype_t::ptr_)
                                    || class_vtype == vtype_t::string_;

                // The classes, and the structs of the program, and the numbers (Int32.ToString).
                if (c.meta_only || (c.type->get_ttype(__env) != ttype_t::class_
                                    && !is_custom_struct(__env, c.type) && !is_number)
                        || (c.type->get_kind() != rt_type_kind_t::general
                            && c.type->get_kind() != rt_type_kind_t::generic))
                    continue;

                // The arrays have their own layout in native programs: their methods are the
                // functions of the runtime. The delegates are made and called by the code (the
                // constructor and Invoke are expanded inline): they have no functions.
                bool is_delegate = false;
                if (c.type->get_kind() == rt_type_kind_t::generic)
                {
                    string_t name = c.type->get_name(__env);
                    if (name.compare(0, 6, _T("Array<")) == 0)
                        continue;

                    is_delegate = name.compare(0, 9, _T("Delegate<")) == 0;
                }

                rt_reflect_type_t * info = __reflection.get_type(c.type);
                rt_reflect_list_t & list = info->lists[(int)rt_reflect_kind_t::method];

                for (int64_t i = 0; i < list.count; i++)
                {
                    rt_reflect_member_t * m = list.items[i];
                    rt_method_base_t * base = (rt_method_base_t *)m->static_data;

                    if (base == nullptr || (base->this_type() != rt_member_type_t::general
                                            && base->this_type() != rt_member_type_t::generic)
                            || !__invoke_tried.insert(base).second)
                        continue;

                    // The method of an instance of a generic type is generated for its arguments.
                    rt_method_t * method = base->this_type() == rt_member_type_t::general?
                        (rt_method_t *)base : ((rt_generic_method_t *)base)->template_;
                    decorate_t decorate = (decorate_t)(*method)->decorate;

                    // The constructors of a delegate that have params are expanded inline (they have
                    // no body that can be made).
                    bool inline_delegate_ctor = is_delegate && (*method)->trait == method_trait_t::constructor
                                                    && m->param_count > 0;

                    if (method->generic_param_count() > 0 || decorate.is_abstract
                            || decorate.is_extern || inline_delegate_ctor
                            || (*method)->trait == method_trait_t::static_constructor
                            || (is_number && (*method)->trait == method_trait_t::constructor))
                        continue;

                    try
                    {
                        __fn_t * fn = __function_of(base);
                        if (!fn->queued)
                        {
                            fn->queued = true;
                            __queue.push_back(fn);
                        }
                    }
                    catch (const error_t &)
                    {
                        continue;
                    }

                    added = true;
                    __promote_result_class(m);

                    // A virtual method is called through the descriptor of the object.
                    if (m->offset >= 0)
                    {
                        __virtual_sites.insert({ base->get_host_type(), (int)m->offset });
                        __max_virtual_offset = std::max(__max_virtual_offset, (int)m->offset);
                    }
                }
            }

            return added;
        }

        // Returns whether the function is Invoke of a delegate (Delegate<R, A ...>).
        bool __generator_t::__is_delegate_invoke(__fn_t & fn)
        {
            rt_type_t * host = fn.base->get_host_type();
            if (host == nullptr || host->get_kind() != rt_type_kind_t::generic
                    || fn.base->this_type() != rt_member_type_t::generic)
                return false;

            std::string tname = __narrow(((rt_generic_type_t *)host)->template_->get_name(__env).c_str());
            return tname.compare(0, 9, "Delegate<") == 0 && __narrow(fn.base->get_name().c_str()) == "Invoke";
        }

        // Makes the function of Invoke of a delegate: the instance and the method are the fields of the
        // delegate, the method is called with the instance and the arguments.
        void __generator_t::__emit_delegate_invoke(__fn_t & fn)
        {
            rt_generic_type_t * dtype = (rt_generic_type_t *)fn.base->get_host_type();

            std::vector<msize_t> field_offsets;
            dtype->each_field(__env, [&](ref_t ref, rt_field_base_t *) {
                field_offsets.push_back(dtype->get_field_offset(__env, ref));
                return true;
            });

            if (field_offsets.size() != 2)
                throw _ED(__e_t::unsupported_type, dtype->get_name(__env));

            std::vector<std::pair<std::string, std::string>> params;
            if (fn.returns_struct)
                params.push_back({ "ptr", "retaddr" });

            for (size_t k = 0; k < fn.param_types.size(); k++)
            {
                params.push_back({ fn.param_types[k], "p" + std::to_string(k) });
            }

            const size_t hidden = fn.returns_struct? 1 : 0;
            llb::function_t & f = __module.define(fn.name, fn.ret_type, params);

            llb::value_t self = f.param(hidden);
            llb::value_t instance = f.load("ptr", f.gep_bytes(self, (int64_t)field_offsets[0]));
            llb::value_t target = f.load("ptr", f.gep_bytes(self, (int64_t)field_offsets[1]));

            std::vector<llb::value_t> call_args;
            if (fn.returns_struct)
                call_args.push_back(f.param(0));

            call_args.push_back(instance);
            for (size_t k = 1; k < fn.param_types.size(); k++)
                call_args.push_back(f.param(hidden + k));

            // A pending exception stays pending: the caller of this function looks at it.
            llb::value_t r = f.call_indirect(fn.ret_type, target, call_args);
            if (fn.ret_type == "void")
                f.ret();
            else
                f.ret(r);
        }

        // A function failed. When only Method.Invoke needs it, the method that made it needed is denied
        // and the module is made again (the exception retry_t); the others stop the generation.
        void __generator_t::__give_up(__fn_t * failed)
        {
            if (!__invoke_phase || failed->origin == nullptr || __denied.count(failed->origin->display))
                return;

            throw retry_t { failed->origin->display };
        }

        // Returns the symbol of a string of the program.
        std::string __generator_t::__text_global_of(const rt_char_t * s)
        {
            return __module.add_wstring(__units_of_string(s), 16).text;
        }

        // Returns the table of the constructor of an attribute, null if the type has none for it.
        rt_reflect_member_t * __generator_t::__constructor_member_of(rt_method_base_t * constructor)
        {
            rt_reflect_type_t * info = __reflection.get_type(constructor->get_host_type());
            rt_reflect_list_t & list = info->lists[(int)rt_reflect_kind_t::method];

            for (int64_t k = 0; k < list.count; k++)
            {
                if (list.items[k]->static_data == (void *)constructor)
                    return list.items[k];
            }

            return nullptr;
        }

        // Returns the field or the property of a type (or of its base types) that has a name.
        rt_reflect_member_t * __generator_t::__named_member_of(rt_type_t * type, const rt_char_t * name)
        {
            for (rt_type_t * t = type; t != nullptr; t = t->get_base_type(__env))
            {
                rt_reflect_type_t * info = __reflection.get_type(t);

                for (int kind : { (int)rt_reflect_kind_t::field, (int)rt_reflect_kind_t::property })
                {
                    rt_reflect_list_t & list = info->lists[kind];
                    for (int64_t k = 0; k < list.count; k++)
                    {
                        if (ustring_t(list.items[k]->name) == ustring_t(name))
                            return list.items[k];
                    }
                }
            }

            return nullptr;
        }

        // Encodes the value of a constant for the table of the attributes: the bits of a number, in
        // the type of what it is the value of, or a string.
        void __generator_t::__encode_attribute_value(const cvalue_t & value, rt_type_t * type,
                                    int64_t value_kind, int64_t size, uint64_t * bits, std::string * text)
        {
            *bits = 0;
            *text = "null";

            if (type == nullptr || value.is_null())
                return;

            vtype_t vtype = type->get_vtype(__env);

            if (vtype == vtype_t::string_)
            {
                if (value.value_type == cvalue_type_t::string)
                    *text = __text_global_of(value.rt_string());

                return;
            }

            if (value_kind != (int64_t)rt_reflect_value_kind_t::value
                    || value.value_type != cvalue_type_t::number)
                return;

            cvalue_t number = value.change_type(vtype);

            switch (vtype)
            {
                case vtype_t::float_: {
                    float v = number.number.get_value<float>();
                    uint32_t u = 0;
                    memcpy(&u, &v, sizeof(u));
                    *bits = u;
                }   break;

                case vtype_t::double_: {
                    double v = number.number.get_value<double>();
                    memcpy(bits, &v, sizeof(*bits));
                }   break;

                case vtype_t::bool_:
                    *bits = number.number.get_value<bool>()? 1 : 0;
                    break;

                case vtype_t::uint8_:
                case vtype_t::uint16_:
                case vtype_t::uint32_:
                case vtype_t::uint64_:
                case vtype_t::char_:
                    *bits = (uint64_t)number.number.get_value<uint64_t>();
                    break;

                default:
                    *bits = (uint64_t)number.number.get_value<int64_t>();
                    break;
            }

            if (size > 0 && size < 8)
                *bits &= ((uint64_t)1 << (8 * size)) - 1;
        }

        // Encodes the value of an argument.
        bool __generator_t::__encode_attribute_argument(rt_assembly_t * assembly,
                assembly_analyzer_t & analyzer, rt_attribute_argument_t * argument,
                rt_type_t * param_type, int64_t value_kind, int64_t size, uint64_t * bits,
                std::string * text, __attribute_extra_t * extra)
        {
            *bits = 0;
            *text = "null";

            ref_t value_ref = (*argument)->value;
            if (value_ref == ref_t::null)
                return true;

            // An array of numbers or of strings: the constants that follow one another are the rank,
            // the length of each dimension, and the elements.
            if (value_ref.extra == 1 && (*argument)->value_type != ref_t::null)
            {
                rt_type_t * element = analyzer.get_type((*argument)->value_type);
                if (element == nullptr)
                    return true;

                bool is_string = element->get_vtype(__env) == vtype_t::string_;
                int64_t element_size = is_string? 8 : (int64_t)element->get_size(__env);

                // The units of the block are those of the rank and the lengths, then the elements.
                auto int_at = [&](uint32_t k) {
                    cvalue_t value = assembly->get_constant(ref_t(value_ref.index + k, 1, 0));
                    return value.change_type(vtype_t::int32_).number.get_value<int32_t>();
                };

                int32_t rank = value_ref.count > 0? int_at(0) : 0;
                if (rank < 1 || (uint32_t)rank + 1 > value_ref.count)
                    throw _ED(__e_t::unsupported_type, _T("(an array of an attribute that has no header)"));

                uint32_t first = 1 + (uint32_t)rank;
                std::string units;

                for (uint32_t k = 0; k < value_ref.count; k++)
                {
                    uint64_t item_bits = 0;
                    std::string item_text = "null";

                    if (k < first)
                    {
                        item_bits = (uint64_t)(int64_t)int_at(k);
                    }
                    else
                    {
                        cvalue_t item = assembly->get_constant(ref_t(value_ref.index + k, 1, 0));
                        __encode_attribute_value(item, element,
                                (int64_t)rt_reflect_value_kind_t::value, element_size, &item_bits, &item_text);
                    }

                    units += std::string(k > 0? ", " : "") + "i64 "
                           + (k >= first && is_string? "ptrtoint (ptr " + item_text + " to i64)"
                                                     : std::to_string((int64_t)item_bits));
                }

                std::string block = "@april.array." + std::to_string(__array_blocks++);
                __module.add_global_line(block + " = private constant [" + std::to_string(value_ref.count)
                                         + " x i64] [" + units + "]");

                extra->kind = 5;
                extra->size = element_size;
                *bits = value_ref.count;
                *text = block;
                return true;
            }

            // typeof(A): the Type is the class.
            ref_t type_ref;
            if (assembly->get_constant_type(value_ref, &type_ref))
            {
                rt_type_t * type = analyzer.get_type(type_ref);
                std::string name = type != nullptr? __meta_descriptor_of(type) : std::string();

                if (!name.empty())
                {
                    extra->kind = 4;
                    extra->type = "@" + name;
                }

                return true;
            }

            cvalue_t value = assembly->get_constant(value_ref);

            // An Object takes the type of the value: the number or the string is boxed in it.
            if (param_type != nullptr && param_type->get_vtype(__env) == vtype_t::mobject_
                    && (value.value_type == cvalue_type_t::number
                        || value.value_type == cvalue_type_t::string)
                    && (*argument)->value_type != ref_t::null)
            {
                rt_type_t * value_type = analyzer.get_type((*argument)->value_type);
                if (value_type == nullptr)
                    return true;

                value_type->pre_new(__env);

                extra->kind = value.value_type == cvalue_type_t::string? 3 : 2;
                extra->size = (int64_t)value_type->get_size(__env);
                extra->type = "@" + __descriptor_of(value_type);

                __encode_attribute_value(value, value_type,
                        (int64_t)rt_reflect_value_kind_t::value, extra->size, bits, text);
                return true;
            }

            __encode_attribute_value(value, param_type, value_kind, size, bits, text);
            return true;
        }

        // Returns the symbol of the attributes of a member or of a type (see
        // april_native_attributes_t), "null" if it has none.
        std::string __generator_t::__attributes_of(void * key)
        {
            if (!__uses_attributes)
                return "null";

            auto found = __attribute_tables.find(key);
            if (found != __attribute_tables.end())
                return found->second;

            rt_reflect_attributes_t attributes;
            if (!__reflection.get_attributes(key, &attributes))
                return "null";

            std::string symbol = "@april.attrs." + std::to_string(__attribute_tables.size());
            __attribute_tables[key] = "null";

            assembly_analyzer_t analyzer(__env, attributes.assembly);
            std::string items;
            size_t count = 0;

            for (ref_t ref : attributes.refs)
            {
                rt_attribute_t * attribute = attributes.assembly->get_attribute(ref);
                rt_method_base_t * constructor = analyzer.get_method((*attribute)->constructor);
                if (constructor == nullptr)
                    continue;

                rt_reflect_member_t * member = __constructor_member_of(constructor);
                if (member == nullptr || __invoker_fn_of(member) == nullptr)
                    continue;

                // The arguments of the constructor: the bits of a number, or a string; then the
                // named ones: the field or the property, and the value.
                std::string values, named;
                int64_t position = 0, named_count = 0;

                std::string prefix = symbol + "." + std::to_string(count);

                for (ref_t arg_ref : (*attribute)->arguments)
                {
                    rt_attribute_argument_t * argument = attributes.assembly->get_attribute_argument(arg_ref);

                    uint64_t bits = 0;
                    std::string text = "null";
                    __attribute_extra_t extra;

                    if ((*argument)->name_type == argument_name_type_t::field)
                    {
                        const rt_char_t * name = analyzer.to_cstr((*argument)->name);
                        rt_reflect_member_t * target = __named_member_of(constructor->get_host_type(), name);
                        if (target == nullptr)
                            continue;

                        __encode_attribute_argument(attributes.assembly, analyzer, argument,
                                (rt_type_t *)target->type, target->value_kind, target->size,
                                &bits, &text, &extra);

                        named += std::string(named_count > 0? ", " : "")
                               + "{ ptr, ptr, i64, ptr, i64, i64, ptr } { ptr "
                               + __text_global_of(name) + ", ptr " + __reflect_member_of(target) + ", i64 "
                               + std::to_string((int64_t)bits) + ", ptr " + text + ", i64 "
                               + std::to_string(extra.kind) + ", i64 " + std::to_string(extra.size)
                               + ", ptr " + extra.type + " }";
                        named_count++;
                        continue;
                    }

                    rt_reflect_member_t * p = position < member->param_count? member->params[position] : nullptr;
                    if (p != nullptr)
                    {
                        __encode_attribute_argument(attributes.assembly, analyzer, argument,
                                (rt_type_t *)p->type, p->value_kind, p->size, &bits, &text, &extra);
                    }

                    values += std::string(position > 0? ", " : "")
                            + "{ i64, ptr, i64, i64, ptr } { i64 "
                            + std::to_string((int64_t)bits) + ", ptr " + text + ", i64 "
                            + std::to_string(extra.kind) + ", i64 " + std::to_string(extra.size)
                            + ", ptr " + extra.type + " }";
                    position++;
                }

                std::string args = "null", named_table = "null";
                if (position > 0)
                {
                    args = prefix + ".a";
                    __module.add_global_line(args + " = private constant [" + std::to_string(position)
                                             + " x { i64, ptr, i64, i64, ptr }] [" + values + "]");
                }

                if (named_count > 0)
                {
                    named_table = prefix + ".n";
                    __module.add_global_line(named_table + " = private constant [" + std::to_string(named_count)
                                             + " x { ptr, ptr, i64, ptr, i64, i64, ptr }] [" + named + "]");
                }

                items += std::string(count > 0? ", " : "") + "{ ptr, ptr, i64, ptr } { ptr "
                       + __reflect_member_of(member) + ", ptr " + args + ", i64 "
                       + std::to_string(named_count) + ", ptr " + named_table + " }";
                count++;
            }

            if (count == 0)
                return "null";

            __module.add_global_line(symbol + ".items = private constant [" + std::to_string(count)
                                     + " x { ptr, ptr, i64, ptr }] [" + items + "]");
            __module.add_global_line(symbol + " = private constant { i64, ptr } { i64 "
                                     + std::to_string(count) + ", ptr " + symbol + ".items }");

            __attribute_tables[key] = symbol;
            return symbol;
        }

        // Returns the symbol of the table of a member (a parameter, an assembly): emitted once.
        std::string __generator_t::__reflect_member_of(rt_reflect_member_t * m)
        {
            auto found = __reflect_members.find(m);
            if (found != __reflect_members.end())
                return found->second;

            std::string symbol = "@april.rm." + std::to_string(__reflect_members.size());
            __reflect_members[m] = symbol;

            auto text_of = [&](const rt_char_t * s) {
                return __module.add_wstring(__units_of(s), 16).text;
            };

            // The Type of a member: the descriptor of its class, null if the type has none.
            auto type_of = [&](void * t) -> std::string {
                if (t == nullptr)
                    return "null";

                auto it = __class_of.find((rt_type_t *)t);
                return it == __class_of.end()? "null" : "@" + it->second->name;
            };

            auto list_of = [&](rt_reflect_member_t ** items, int64_t count, const char * suffix) {
                if (count == 0)
                    return std::string("null");

                std::string text;
                for (int64_t k = 0; k < count; k++)
                    text += std::string(k > 0? ", " : "") + "ptr " + __reflect_member_of(items[k]);

                std::string list = symbol + suffix;
                __module.add_global_line(list + " = private constant [" + std::to_string(count)
                                         + " x ptr] [" + text + "]");
                return list;
            };

            std::string params = list_of(m->params, m->param_count, ".params");
            std::string gparams = list_of(m->gparams, m->gparam_count, ".gparams");
            std::string a = m->accessor_a != nullptr? __reflect_member_of(m->accessor_a) : "null";
            std::string b = m->accessor_b != nullptr? __reflect_member_of(m->accessor_b) : "null";
            std::string c = m->accessor_c != nullptr? __reflect_member_of(m->accessor_c) : "null";

            // A field: the key of the type that has it, or the global of its static data, and the
            // key of its type.
            std::string key_or_static = "null", type_key = "null", invoker = "null";
            __fn_t * invoker_fn = __invoker_fn_of(m);

            if (invoker_fn != nullptr)
            {
                // A method: its function, the key of its type, the function that calls it.
                if (__is_interface_member(m))
                {
                    // The function is the one of the object: found by the key of the interface and
                    // the offset.
                    key_or_static = "null";
                    type_key      = "@april.iface."
                                  + std::to_string(__interface_index((rt_type_t *)m->declaring));
                }
                else
                {
                    key_or_static = "@" + invoker_fn->name;
                    type_key      = __interface_key_of((rt_type_t *)m->declaring);
                }

                invoker       = "@" + __shape_thunk_of(*invoker_fn);
            }
            else if ((m->flags & reflect_flag_field) == 0)
            {
                // A parameter: the key of its type.
                if (m->value_kind == (int64_t)rt_reflect_value_kind_t::reference && m->type != nullptr)
                    type_key = __interface_key_of((rt_type_t *)m->type);
            }
            else if (m->value_kind != 0 && m->declaring != nullptr && (m->flags & (1 << 10)) != 0)
            {
                // A constant: the value is in the table.
                key_or_static = "null";
            }
            else if (m->value_kind != 0 && m->declaring != nullptr)
            {
                if ((m->flags & 16) != 0)
                    key_or_static = "@" + __static_data_global_of((rt_type_t *)m->declaring);
                else
                    key_or_static = "@april.iface." + std::to_string(__interface_index((rt_type_t *)m->declaring));

                rt_type_t * field_type = (rt_type_t *)m->type;
                if (field_type != nullptr && (field_type->get_kind() == rt_type_kind_t::general
                                              || field_type->get_kind() == rt_type_kind_t::generic
                                              || field_type->get_kind() == rt_type_kind_t::array))
                {
                    // Object and the objects of reflection have keys of their own.
                    std::string special = __interface_key_of(field_type);
                    type_key = special.compare(0, 8, "inttoptr") == 0? special
                        : "@april.iface." + std::to_string(__interface_index(field_type));
                }
            }

            __module.add_global_line(symbol + " = private constant { ptr, ptr, ptr, i64, ptr, ptr, i64, ptr, i64, ptr, ptr, i64, ptr, i64, i64, ptr, ptr, ptr } { ptr "
                + text_of(m->name) + ", ptr " + type_of(m->declaring) + ", ptr " + type_of(m->type)
                + ", i64 " + std::to_string(m->flags) + ", ptr " + a + ", ptr " + b
                + ", i64 " + std::to_string(m->param_count) + ", ptr " + params
                + ", i64 " + std::to_string(m->gparam_count) + ", ptr " + gparams + ", ptr " + c + ", i64 " + std::to_string(m->offset)
                + ", ptr " + key_or_static + ", i64 " + std::to_string(m->size)
                + ", i64 " + std::to_string(m->value_kind) + ", ptr " + type_key
                + ", ptr " + invoker + ", ptr " + __attributes_of(m) + " }");

            return symbol;
        }

        // Emits the table of reflection of a class (see rt_reflect_type_t), returns its symbol.
        std::string __generator_t::__reflect_meta_of(__class_t & c)
        {
            rt_reflect_type_t * info = __reflection.get_type(c.type);
            std::string meta = "@" + c.name + ".meta";

            auto type_of = [&](void * t) -> std::string {
                if (t == nullptr)
                    return "null";

                auto it = __class_of.find((rt_type_t *)t);
                return it == __class_of.end()? "null" : "@" + it->second->name;
            };

            std::string lists;
            for (int k = 0; k < 5; k++)
            {
                rt_reflect_list_t & list = info->lists[k];
                std::string items = "null";

                if (list.count > 0)
                {
                    std::string text;
                    for (int64_t i = 0; i < list.count; i++)
                        text += std::string(i > 0? ", " : "") + "ptr " + __reflect_member_of(list.items[i]);

                    items = meta + ".l" + std::to_string(k);
                    __module.add_global_line(items + " = private constant [" + std::to_string(list.count)
                                             + " x ptr] [" + text + "]");
                }

                lists += std::string(k > 0? ", " : "") + "{ i64, ptr } { i64 " + std::to_string(list.count)
                       + ", ptr " + items + " }";
            }

            std::string assembly = info->assembly != nullptr? __reflect_member_of(info->assembly) : "null";

            // The nest types: the descriptors of their classes.
            std::string nested_items = "null";
            if (info->nested.count > 0)
            {
                std::string text;
                for (int64_t i = 0; i < info->nested.count; i++)
                    text += std::string(i > 0? ", " : "") + "ptr " + type_of((void *)info->nested.items[i]);

                nested_items = meta + ".nested";
                __module.add_global_line(nested_items + " = private constant [" + std::to_string(info->nested.count)
                                         + " x ptr] [" + text + "]");
            }

            // The size of the objects, and the attributes.
            int64_t size = 0;
            if (!c.meta_only && (c.type->get_ttype(__env) == ttype_t::class_ || c.type->get_ttype(__env) == ttype_t::enum_))
            {
                c.type->pre_new(__env);
                size = (int64_t)c.type->get_size(__env);
            }

            __module.add_global_line(meta + " = private constant { ptr, i64, ptr, [5 x { i64, ptr }], i64, ptr, { i64, ptr } } { ptr "
                + type_of(info->base) + ", i64 " + std::to_string(info->flags) + ", ptr " + assembly
                + ", [5 x { i64, ptr }] [" + lists + "], i64 " + std::to_string(size) + ", ptr "
                + __attributes_of(c.type) + ", { i64, ptr } { i64 " + std::to_string(info->nested.count)
                + ", ptr " + nested_items + " } }");

            return meta;
        }

        // Emits the type descriptors.
        void __generator_t::__emit_descriptors()
        {
            // The tables of reflection refer to other types: they are classes of their own (the
            // list grows).
            if (__uses_reflection)
            {
                for (size_t k = 0; k < __classes.size(); k++)
                {
                    rt_reflect_type_t * info = __reflection.get_type(__classes[k]->type);
                    __reflection.each_referenced_type(info, [&](rt_type_t * type) {
                        __meta_descriptor_of(type);
                    });
                }

                // The fields are read and written by the runtime: the keys of the types of the
                // checks, and the static data, are made before the tables of the casts are.
                for (size_t k = 0; k < __classes.size(); k++)
                {
                    rt_reflect_type_t * info = __reflection.get_type(__classes[k]->type);
                    rt_reflect_list_t & fields = info->lists[(int)rt_reflect_kind_t::field];

                    for (int64_t i = 0; i < fields.count; i++)
                    {
                        rt_reflect_member_t * m = fields.items[i];
                        if (m->value_kind == 0 || m->declaring == nullptr)
                            continue;

                        if ((m->flags & 16) != 0)
                            __static_data_global_of((rt_type_t *)m->declaring);
                        else
                            __interface_index((rt_type_t *)m->declaring);

                        rt_type_t * field_type = (rt_type_t *)m->type;
                        if (field_type != nullptr && (field_type->get_kind() == rt_type_kind_t::general
                                                      || field_type->get_kind() == rt_type_kind_t::generic
                                                      || field_type->get_kind() == rt_type_kind_t::array))
                            __interface_index(field_type);
                    }
                }

                // The methods are called by the runtime: the keys of the types that have them, and of
                // the types of their parameters that are references.
                for (size_t k = 0; k < __classes.size(); k++)
                {
                    rt_reflect_type_t * info = __reflection.get_type(__classes[k]->type);
                    rt_reflect_list_t & methods = info->lists[(int)rt_reflect_kind_t::method];

                    for (int64_t i = 0; i < methods.count; i++)
                    {
                        rt_reflect_member_t * m = methods.items[i];
                        if (__invoker_fn_of(m) == nullptr)
                            continue;

                        __interface_index((rt_type_t *)m->declaring);

                        for (int64_t p = 0; p < m->param_count; p++)
                        {
                            rt_type_t * type = (rt_type_t *)m->params[p]->type;
                            if (m->params[p]->value_kind == (int64_t)rt_reflect_value_kind_t::reference
                                    && type != nullptr && (type->get_kind() == rt_type_kind_t::general
                                                           || type->get_kind() == rt_type_kind_t::generic
                                                           || type->get_kind() == rt_type_kind_t::array))
                                __interface_index(type);
                        }
                    }
                }
            }

            for (auto & cp : __classes)
            {
                __class_t & c = *cp;

                // The functions of the class of an array are those of Array<T>.
                __class_t & functions = c.alias != nullptr? *c.alias : c;

                // The interface table.
                std::string table = "null";
                if (!functions.ifunctions.empty())
                {
                    std::string text = "@" + c.name + ".itab = private constant ["
                                     + std::to_string(functions.ifunctions.size()) + " x { ptr, i64, ptr }] [";

                    bool first = true;
                    for (auto & it : functions.ifunctions)
                    {
                        text += std::string(first? "" : ", ") + "{ ptr, i64, ptr } { ptr @april.iface."
                              + std::to_string(it.first.first) + ", i64 " + std::to_string(it.first.second)
                              + ", ptr @" + __virtual_symbol_of(it.second) + " }";
                        first = false;
                    }

                    __module.add_global_line(text + "]");
                    table = "@" + c.name + ".itab";
                }

                // The types the class is derived from or implements, among the keys.
                std::string casts = "null";
                size_t cast_count = 0;
                if (!c.meta_only)
                {
                    c.type->pre_new(__env);
                    rt_vtable_t * vtbl = get_vtable(c.type);

                    std::string text;
                    for (size_t k = 0; k < __interfaces.size(); k++)
                    {
                        rt_type_t * key = __interfaces[k];
                        bool is_a = c.type == key || vtbl->search_vtable_interface_variant(__env, key) != nullptr;
                        for (msize_t b = 0; !is_a && b < vtbl->base_type_count; b++)
                            is_a = vtbl->base_types[b] == key;

                        if (!is_a)
                            continue;

                        text += std::string(cast_count > 0? ", " : "") + "ptr @april.iface." + std::to_string(k);
                        cast_count++;
                    }

                    if (cast_count > 0)
                    {
                        __module.add_global_line("@" + c.name + ".casts = private constant ["
                                                 + std::to_string(cast_count) + " x ptr] [" + text + "]");
                        casts = "@" + c.name + ".casts";
                    }
                }

                // The virtual functions.
                size_t real_count = (size_t)(__max_virtual_offset + 1);
                size_t vcount = real_count + __generic_sites.size();
                std::string vtext;
                for (size_t k = 0; k < vcount; k++)
                {
                    if (k < real_count)
                    {
                        auto it = functions.vfunctions.find((int)k);
                        vtext += std::string(k > 0? ", " : "") + "ptr "
                               + (it == functions.vfunctions.end()? "null" : "@" + __virtual_symbol_of(it->second));
                    }
                    else
                    {
                        auto it = functions.gfunctions.find(k - real_count);
                        vtext += std::string(k > 0? ", " : "") + "ptr "
                               + (it == functions.gfunctions.end()? "null" : "@" + __virtual_symbol_of(it->second));
                    }
                }

                // The name of the type.
                llb::value_t name_value = __module.add_wstring(__units_of(c.type->get_name(__env).ce_str()), 16);

                // The full name of the type.
                llb::value_t full_name_value = __module.add_wstring(__units_of(c.type->get_full_name(__env).ce_str()), 16);

                std::string meta = __uses_reflection? __reflect_meta_of(c) : "null";

                std::string vtype = "[" + std::to_string(vcount) + " x ptr]";
                __module.add_global_line(
                    "@" + c.name + " = private constant { ptr, i64, ptr, i64, " + vtype + ", ptr, ptr, ptr } { ptr "
                    + table + ", i64 " + std::to_string(functions.ifunctions.size()) + ", ptr " + casts
                    + ", i64 " + std::to_string(cast_count) + ", " + vtype + " [" + vtext + "], ptr "
                    + name_value.text + ", ptr " + full_name_value.text + ", ptr " + meta + " }"
                );
            }

            __module.add_global_line("@april.vcount = private constant i64 "
                                     + std::to_string(__max_virtual_offset + 1 + __generic_sites.size()));
            __module.add_global_line("@april.vreal = private constant i64 "
                                     + std::to_string(__max_virtual_offset + 1));

            for (size_t k = 0; k < __interfaces.size(); k++)
                __module.add_global_line("@april.iface." + std::to_string(k) + " = private constant i8 0");
        }

        // Emits `main` and the initializer of the library functions.
        void __generator_t::__emit_main(__fn_t & entry)
        {
            if (!__static_data_map.empty())
                __uses_new_object = true;

            // The runtime support, declared.
            __module.declare(APRIL_NATIVE_SYM_INIT,         "void", { "ptr" });
            __module.declare(APRIL_NATIVE_SYM_ADD_ROOT,     "void", { "ptr" });
            __module.declare(APRIL_NATIVE_SYM_SHUTDOWN,     "void", { });
            __module.declare(APRIL_NATIVE_SYM_FIND_LIBFUNC, "ptr",  { "ptr" });
            __module.declare(APRIL_NATIVE_SYM_INVOKE,       "void", { "ptr", "ptr" });

            if (__uses_new_object)
                __module.declare(APRIL_NATIVE_SYM_NEW_OBJECT, "ptr", { "i64", "ptr" });

            __module.add_global_line("@april.exception = global ptr null");
            __module.declare(APRIL_NATIVE_SYM_UNHANDLED, "void", { "ptr" });

            if (__uses_find_interface)
                __module.declare(APRIL_NATIVE_SYM_FIND_IFACE, "ptr", { "ptr", "ptr", "i64" });

            if (__uses_field_access)
            {
                __module.declare(APRIL_NATIVE_SYM_REFLECT_CHECK, "i32", { "ptr", "ptr", "ptr", "i32" });
                __module.declare(APRIL_NATIVE_SYM_REFLECT_GET,   "ptr", { "ptr", "ptr" });
                __module.declare(APRIL_NATIVE_SYM_REFLECT_SET,   "void", { "ptr", "ptr", "ptr" });
            }

            if (__uses_enum_number)
            {
                __module.declare(APRIL_NATIVE_SYM_ENUM_NUMBER, "i64", { "ptr", "ptr" });
                __module.declare(APRIL_NATIVE_SYM_ENUM_TO_STRING, "ptr", { "ptr", "ptr" });
                __module.declare(APRIL_NATIVE_SYM_ENUM_PARSE, "ptr", { "ptr", "ptr", "ptr", "i8" });
            }

            if (__uses_attributes)
            {
                __module.declare(APRIL_NATIVE_SYM_ATTR_COUNT, "i32", { "ptr" });
                __module.declare(APRIL_NATIVE_SYM_ATTR_CTOR, "ptr", { "ptr", "i32" });
                __module.declare(APRIL_NATIVE_SYM_ATTR_ARG_COUNT, "i32", { "ptr", "i32" });
                __module.declare(APRIL_NATIVE_SYM_ATTR_ARG, "ptr", { "ptr", "i32", "i32" });
                __module.declare(APRIL_NATIVE_SYM_ATTR_NAMED_COUNT, "i32", { "ptr", "i32" });
                __module.declare(APRIL_NATIVE_SYM_ATTR_NAMED_NAME, "ptr", { "ptr", "i32", "i32" });
                __module.declare(APRIL_NATIVE_SYM_ATTR_NAMED_VALUE, "ptr", { "ptr", "i32", "i32" });
            }

            if (__uses_invoke)
            {
                __module.declare(APRIL_NATIVE_SYM_REFLECT_CHECK_INVOKE, "i32", { "ptr", "ptr", "ptr" });
                __module.declare(APRIL_NATIVE_SYM_REFLECT_INVOKE, "ptr", { "ptr", "ptr", "ptr" });
            }

            if (__uses_arrays)
            {
                __module.declare(APRIL_NATIVE_SYM_NEW_ARRAY, "ptr", { "i64", "i32", "ptr", "ptr", "i32" });
                __module.declare(APRIL_NATIVE_SYM_ARRAY_ELEMENT, "ptr", { "ptr", "ptr", "i32", "i64" });
                __module.declare(APRIL_NATIVE_SYM_ARRAY_TRY, "ptr", { "ptr", "ptr", "i32", "i64" });
                __module.declare(APRIL_NATIVE_SYM_ARRAY_LINEAR, "ptr", { "ptr", "i32", "i64" });
                __module.declare(APRIL_NATIVE_SYM_ARRAY_LENGTH, "i32", { "ptr" });
                __module.declare(APRIL_NATIVE_SYM_ARRAY_RANK, "i32", { "ptr" });
                __module.declare(APRIL_NATIVE_SYM_ARRAY_LENGTH_OF, "i32", { "ptr", "i32" });
            }

            if (__uses_cast)
            {
                __module.declare(APRIL_NATIVE_SYM_IS_INSTANCE, "i8", { "ptr", "ptr" });
                __module.declare(APRIL_NATIVE_SYM_CAST, "ptr", { "ptr", "ptr" });
            }

            if (__uses_memcpy)
                __module.declare("llvm.memcpy.p0.p0.i64", "void", { "ptr", "ptr", "i64", "i1" });

            if (__uses_memset)
                __module.declare("llvm.memset.p0.i64", "void", { "ptr", "i8", "i64", "i1" });

            if (__uses_trunc)
                __module.declare("llvm.trunc.f64", "double", { "double" });

            if (__uses_trap)
                __module.declare("llvm.trap", "void", { });

            for (auto && it : __overflow_intrinsics)
                __module.declare(it.first, "{ " + it.second + ", i1 }", { it.second, it.second });

            // Globals of the library function pointers, resolved at startup by name.
            for (const std::string & name : __used_libfuncs)
            {
                __module.add_global_line(
                    "@" + __libfunc_global(name) + " = internal global ptr null"
                );
            }

            llb::function_t & init = __module.define("april.init_libfuncs", "void", { });
            for (const std::string & name : __used_libfuncs)
            {
                llb::value_t name_ptr = __module.add_cstring(name);
                llb::value_t fp = init.call("ptr", APRIL_NATIVE_SYM_FIND_LIBFUNC, { name_ptr });

                init.store(fp, llb::value_t { llb::types::ptr, "@" + __libfunc_global(name) });
            }
            init.ret();

            // main.
            llb::function_t & main_f = __module.define("main", "i32",
                { { "i32", "argc" }, { "ptr", "argv" } }
            );

            // The collector scans the stack up to the frame of `main`.
            llb::value_t stack_base = main_f.alloca_("i8", "stackbase");
            main_f.call("void", APRIL_NATIVE_SYM_INIT, { stack_base });
            main_f.call("void", APRIL_NATIVE_SYM_ADD_ROOT,
                        { llb::value_t { llb::types::ptr, "@april.exception" } });
            main_f.call("void", "april.init_libfuncs", { });

            // The resources of the assemblies (Resource.GetBytes): they are constants of the program, that the
            // runtime finds by the name. Only a program that reads resources has them.
            if (__used_libfuncs.count("Resource_GetSize") != 0 || __used_libfuncs.count("Resource_Read") != 0)
            {
                __module.declare(APRIL_NATIVE_SYM_ADD_RESOURCE, "void", { "ptr", "ptr", "i64" });

                for (int k = 0; k < __env.assemblies.count(); k++)
                {
                    rt_assembly_t * assembly = __env.assemblies.at(k);

                    for (int index = 0, count = assembly->get_resource_count(); index < count; index++)
                    {
                        string_t name = (string_t)assembly->get_resource_name(index);
                        rt_bytes_t bytes = assembly->get_resource_bytes(index);

                        llb::value_t name_ptr = __module.add_cstring(string_convert<char_t, char>(name));
                        llb::value_t data_ptr = __module.add_cstring(
                            std::string((const char *)bytes.bytes, bytes.bytes == nullptr? 0 : bytes.length)
                        );

                        main_f.call("void", APRIL_NATIVE_SYM_ADD_RESOURCE, {
                            name_ptr, data_ptr, llb::const_int("i64", (int64_t)bytes.length)
                        });
                    }
                }
            }

            for (auto & it : __static_data_map)
            {
                llb::value_t slot { llb::types::ptr, "@" + it.second.global };
                llb::value_t obj = main_f.call("ptr", APRIL_NATIVE_SYM_NEW_OBJECT, {
                    llb::const_int("i64", it.second.size),
                    llb::value_t { llb::types::ptr, "@" + it.second.desc }
                });

                main_f.store(obj, slot);
                main_f.call("void", APRIL_NATIVE_SYM_ADD_ROOT, { slot });
            }

            main_f.call("void", entry.name, { });

            // An exception that nothing caught.
            llb::value_t pending = main_f.load("ptr", llb::value_t { llb::types::ptr, "@april.exception" });
            std::string unhandled = main_f.new_block("unhandled"), finish = main_f.new_block("finish");
            main_f.cond_br(main_f.icmp("ne", pending, llb::const_null()), unhandled, finish);

            main_f.set_block(unhandled);
            main_f.call("void", APRIL_NATIVE_SYM_UNHANDLED, { pending });
            main_f.unreachable();

            main_f.set_block(finish);
            main_f.call("void", APRIL_NATIVE_SYM_SHUTDOWN, { });
            main_f.ret(llb::const_int("i32", 0));
        }

        //-------- ---------- ---------- ---------- ----------

        // Generates the module text.
        std::string __generator_t::generate()
        {
            rt_assembly_t * main_assembly = __env.assemblies.load_main_assembly();
            rt_method_t * entry_point = main_assembly->get_entry_point();

            if (entry_point == nullptr)
                throw _ED(__e_t::no_entry_point, main_assembly->get_name());

            __fn_t * entry = __function_of(entry_point);

            if (!entry->param_types.empty() || entry->ret_type != "void")
            {
                throw _ED(__e_t::unsupported_method, entry->display,
                          _T("entry points with arguments or a return value"));
            }

            entry->queued = true;
            __queue.push_back(entry);

            // The functions that the queue got since `from` have the origin: only Method.Invoke
            // needs them.
            auto set_origins = [&](size_t from, __fn_t * origin) {
                for (size_t k = from; k < __queue.size(); k++)
                {
                    if (__queue[k]->origin == nullptr)
                        __queue[k]->origin = origin != nullptr? origin : __queue[k];
                }
            };

            // Emitting functions finds classes and call sites, which find functions...
            do
            {
                do
                {
                    while (!__queue.empty())
                    {
                        __fn_t * fn = __queue.front();
                        __queue.pop_front();

                        size_t queued_before = __queue.size();

                        try
                        {
                            __emit(*fn);
                        }
                        catch (const error_t &)
                        {
                            __give_up(fn);
                            throw;
                        }
                        catch (const llb::ll_error_t &)
                        {
                            __give_up(fn);
                            throw;
                        }

                        if (__invoke_phase)
                            set_origins(queued_before, fn->origin);
                    }

                    // The functions that the program needs are all made before the first of those that
                    // only Invoke needs: a failure of the latter does not stop the program.
                    size_t queued_before = __queue.size();
                    bool more = __resolve_virtuals();

                    if (__invoke_phase)
                        set_origins(queued_before, nullptr);

                    if (!more && __queue.empty())
                        break;
                }
                while (true);

                size_t queued_before = __queue.size();
                bool more = __queue_invokable();

                if (more)
                    __invoke_phase = true;

                set_origins(queued_before, nullptr);

                if (!more)
                    break;
            }
            while (true);

            __emit_descriptors();
            __emit_main(*entry);

            return __module.str();
        }

    }   // anonymous namespace

    ////////// ////////// ////////// ////////// //////////

    // Generates the LLVM IR of the program.
    std::string generate_ir(rt_context_t & ctx)
    {
        // A method that only Method.Invoke needs may be one that native programs cannot have: it is
        // denied (its Invoke fails) and the module is made again, the program stays native.
        std::set<string_t> denied;

        while (true)
        {
            try
            {
                return __generator_t(ctx, denied).generate();
            }
            catch (const __generator_t::retry_t & retry)
            {
                denied.insert(retry.method);
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::native
