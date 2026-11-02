
#ifndef __NATIVE_LL_BUILDER_H__
#define __NATIVE_LL_BUILDER_H__

// A small builder of textual LLVM IR (.ll), header only, depends on the standard
// library only: the native back end of April generates IR as text and lets
// clang / llc do the rest, so that april itself needs no LLVM to be built, and
// is not affected by changes of the LLVM C++ API.
//
// Requires LLVM 15+ to consume the output: it uses opaque pointers (`ptr`).
//
// The builder checks the structure while it builds (every block ends with exactly
// one terminator, names are unique, operands are well formed); type correctness of
// the operands is up to the caller, and finally verified by clang / llvm-as.

#include <cstdint>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#ifndef X_ROOT_NS
#define X_ROOT_NS april
#endif

namespace X_ROOT_NS::modules::native::ll {

    ////////// ////////// ////////// ////////// //////////

    // Thrown when the builder is used in a wrong way.
    class ll_error_t : public std::logic_error
    {
    public:
        using std::logic_error::logic_error;
    };

    ////////// ////////// ////////// ////////// //////////

    // An operand: its type and the way to write it (e.g. "%t3", "42", "@str0").
    struct value_t
    {
        std::string type;
        std::string text;

        bool valid() const { return !type.empty() && !text.empty(); }
    };

    // Common types.
    namespace types
    {
        inline const char * const void_ = "void";
        inline const char * const i1    = "i1";
        inline const char * const i8    = "i8";
        inline const char * const i16   = "i16";
        inline const char * const i32   = "i32";
        inline const char * const i64   = "i64";
        inline const char * const f32   = "float";
        inline const char * const f64   = "double";
        inline const char * const ptr   = "ptr";

        // An array type: [count x element].
        inline std::string array(size_t count, const std::string & element)
        {
            return "[" + std::to_string(count) + " x " + element + "]";
        }
    }

    // Constants.
    inline value_t const_int(const std::string & type, int64_t value)
    {
        return value_t { type, std::to_string(value) };
    }

    inline value_t const_bool(bool value)
    {
        return value_t { types::i1, value? "true" : "false" };
    }

    inline value_t const_null()
    {
        return value_t { types::ptr, "null" };
    }

    // A floating point constant, written as the exact bit pattern (LLVM accepts
    // the hexadecimal form of doubles: it is lossless, also for float).
    inline value_t const_double(double value)
    {
        union { double d; uint64_t u; } bits;
        bits.d = value;

        char buffer[32];
        std::snprintf(buffer, sizeof(buffer), "0x%016llX", (unsigned long long)bits.u);

        return value_t { types::f64, buffer };
    }

    inline value_t const_float(float value)
    {
        // A float constant is written as the double it converts to exactly.
        value_t v = const_double((double)value);
        v.type = types::f32;
        return v;
    }

    ////////// ////////// ////////// ////////// //////////

    class module_t;

    // A function being defined.
    class function_t
    {
        friend class module_t;

    public:

        // Returns the parameter with the specified index as an operand.
        value_t param(size_t index) const
        {
            if (index >= __params.size())
                throw ll_error_t("parameter index out of range");

            return value_t { __params[index].first, "%" + __params[index].second };
        }

        size_t param_count() const { return __params.size(); }

        const std::string & name() const { return __name; }

        // Basic blocks ----------

        // Creates a new block (not yet current), returns its label.
        std::string new_block(const std::string & hint = "bb")
        {
            std::string label = __unique(hint);
            __blocks.push_back(__block_t { label, { }, false });

            return label;
        }

        // Makes the block current: instructions are appended to it.
        void set_block(const std::string & label)
        {
            __current = __find(label);

            if (__blocks[__current].terminated)
                throw ll_error_t("block '" + label + "' is already terminated");
        }

        // Returns the label of the current block.
        const std::string & current_block() const { return __blocks[__current].label; }

        // Returns whether the current block already ends with a terminator.
        bool terminated() const { return __blocks[__current].terminated; }

        // Instructions ----------

        // alloca <type>
        value_t alloca_(const std::string & type, const std::string & hint = "t")
        {
            std::string r = __temp(hint);
            __emit(r + " = alloca " + type);

            return value_t { types::ptr, r };
        }

        // alloca in the entry block (after the allocas already there), whatever the current
        // block is: allocas of other blocks would grow the stack in loops.
        value_t alloca_entry(const std::string & type, const std::string & hint = "t")
        {
            std::string r = __temp(hint);
            __block_t & entry = __blocks[0];

            entry.lines.insert(entry.lines.begin() + __entry_allocas,
                               r + " = alloca " + type + __alignment_of(type));
            __entry_allocas++;

            return value_t { types::ptr, r };
        }

        // load <type>, ptr <ptr>
        value_t load(const std::string & type, const value_t & ptr)
        {
            __expect_ptr(ptr);

            std::string r = __temp();
            __emit(r + " = load " + type + ", ptr " + ptr.text);

            return value_t { type, r };
        }

        // store <type> <value>, ptr <ptr>
        void store(const value_t & value, const value_t & ptr)
        {
            __expect_value(value);
            __expect_ptr(ptr);

            __emit("store " + value.type + " " + value.text + ", ptr " + ptr.text);
        }

        // <op> <type> a, b: add, sub, mul, sdiv, udiv, srem, urem, fadd, fsub, fmul,
        // fdiv, frem, and, or, xor, shl, lshr, ashr.
        value_t binop(const std::string & op, const value_t & a, const value_t & b)
        {
            __expect_value(a);
            __expect_value(b);

            if (a.type != b.type)
                throw ll_error_t("binop " + op + ": operand types differ: "
                                 + a.type + " and " + b.type);

            std::string r = __temp();
            __emit(r + " = " + op + " " + a.type + " " + a.text + ", " + b.text);

            return value_t { a.type, r };
        }

        // icmp <cond> (eq, ne, sgt, sge, slt, sle, ugt, uge, ult, ule) -> i1
        value_t icmp(const std::string & cond, const value_t & a, const value_t & b)
        {
            return __cmp("icmp", cond, a, b);
        }

        // fcmp <cond> (oeq, one, ogt, oge, olt, ole, ...) -> i1
        value_t fcmp(const std::string & cond, const value_t & a, const value_t & b)
        {
            return __cmp("fcmp", cond, a, b);
        }

        // <op> <from> v to <type>: trunc, zext, sext, fptrunc, fpext, fptoui, fptosi,
        // uitofp, sitofp, ptrtoint, inttoptr, bitcast.
        value_t cast(const std::string & op, const value_t & v, const std::string & to)
        {
            __expect_value(v);

            std::string r = __temp();
            __emit(r + " = " + op + " " + v.type + " " + v.text + " to " + to);

            return value_t { to, r };
        }

        // extractvalue <aggregate>, <index>: a field of the value that a call returns, such as
        // { i32, i1 } of llvm.sadd.with.overflow.i32 .
        value_t extract_value(const value_t & aggregate, int index, const std::string & type)
        {
            __expect_value(aggregate);

            std::string r = __temp();
            __emit(r + " = extractvalue " + aggregate.type + " " + aggregate.text + ", "
                   + std::to_string(index));

            return value_t { type, r };
        }

        // getelementptr inbounds <elem>, ptr <base>, i64 <index>
        value_t gep(const std::string & element_type, const value_t & base,
                    const value_t & index)
        {
            __expect_ptr(base);
            __expect_value(index);

            std::string r = __temp();
            __emit(r + " = getelementptr inbounds " + element_type + ", ptr " + base.text
                   + ", " + index.type + " " + index.text);

            return value_t { types::ptr, r };
        }

        // getelementptr inbounds i8, ptr <base>, i64 <offset>: byte offset.
        value_t gep_bytes(const value_t & base, int64_t offset)
        {
            return gep(types::i8, base, const_int(types::i64, offset));
        }

        // call <ret> @callee(args...). `callee` is a function name (without '@'),
        // returns an invalid value when the return type is void.
        value_t call(const std::string & ret_type, const std::string & callee,
                     const std::vector<value_t> & args)
        {
            std::string text = "call " + ret_type + " @" + callee + "(";

            for (size_t k = 0; k < args.size(); k++)
            {
                __expect_value(args[k]);
                text += (k > 0? ", " : "") + args[k].type + " " + args[k].text;
            }

            text += ")";

            if (ret_type == types::void_)
            {
                __emit(text);
                return value_t { };
            }

            std::string r = __temp();
            __emit(r + " = " + text);

            return value_t { ret_type, r };
        }

        // call <ret> <fn>(args...), where `fn` is a pointer operand (an indirect call).
        value_t call_indirect(const std::string & ret_type, const value_t & fn,
                              const std::vector<value_t> & args)
        {
            __expect_ptr(fn);

            std::string text = "call " + ret_type + " " + fn.text + "(";

            for (size_t k = 0; k < args.size(); k++)
            {
                __expect_value(args[k]);
                text += (k > 0? ", " : "") + args[k].type + " " + args[k].text;
            }

            text += ")";

            if (ret_type == types::void_)
            {
                __emit(text);
                return value_t { };
            }

            std::string r = __temp();
            __emit(r + " = " + text);

            return value_t { ret_type, r };
        }

        // select i1 c, <type> a, <type> b
        value_t select(const value_t & cond, const value_t & a, const value_t & b)
        {
            __expect_value(cond);
            __expect_value(a);
            __expect_value(b);

            std::string r = __temp();
            __emit(r + " = select i1 " + cond.text + ", " + a.type + " " + a.text + ", "
                   + b.type + " " + b.text);

            return value_t { a.type, r };
        }

        // Terminators ----------

        void ret()
        {
            __terminate("ret void");
        }

        void ret(const value_t & value)
        {
            __expect_value(value);
            __terminate("ret " + value.type + " " + value.text);
        }

        void br(const std::string & label)
        {
            __find(label);
            __terminate("br label %" + label);
        }

        void cond_br(const value_t & cond, const std::string & if_true,
                     const std::string & if_false)
        {
            __expect_value(cond);
            __find(if_true);
            __find(if_false);

            __terminate("br i1 " + cond.text + ", label %" + if_true
                        + ", label %" + if_false);
        }

        // switch <type> v, label %default [ <type> c, label %target ... ]
        void switch_(const value_t & value, const std::string & default_label,
                     const std::vector<std::pair<int64_t, std::string>> & cases)
        {
            __expect_value(value);
            __find(default_label);

            std::string text = "switch " + value.type + " " + value.text
                             + ", label %" + default_label + " [";

            for (auto && c : cases)
            {
                __find(c.second);
                text += " " + value.type + " " + std::to_string(c.first)
                      + ", label %" + c.second;
            }

            __terminate(text + " ]");
        }

        void unreachable()
        {
            __terminate("unreachable");
        }

    private:

        // Blocks of bytes ([N x i8]) hold structs of the interpreter layout, whose fields
        // may need 8 byte alignment.
        static std::string __alignment_of(const std::string & type)
        {
            return type.size() > 0 && type[0] == '['? ", align 8" : "";
        }

        struct __block_t
        {
            std::string                 label;
            std::vector<std::string>    lines;
            bool                        terminated;
        };

        function_t(std::string name, std::string ret_type, std::string linkage,
                   std::vector<std::pair<std::string, std::string>> params)
            : __name(std::move(name)), __ret_type(std::move(ret_type))
            , __linkage(std::move(linkage)), __params(std::move(params))
        {
            for (auto && p : __params)
            {
                if (!__names.insert(p.second).second)
                    throw ll_error_t("duplicated parameter name '" + p.second + "'");
            }

            __names.insert("entry");
            __blocks.push_back(__block_t { "entry", { }, false });
        }

        // Writes the function.
        void write(std::string & out) const
        {
            out += "define " + (__linkage.empty()? std::string() : __linkage + " ")
                 + __ret_type + " @" + __name + "(";

            for (size_t k = 0; k < __params.size(); k++)
            {
                out += (k > 0? ", " : "") + __params[k].first + " %" + __params[k].second;
            }

            out += ") {\n";

            for (const __block_t & block : __blocks)
            {
                if (!block.terminated)
                {
                    throw ll_error_t("block '" + block.label + "' of function '" + __name
                                     + "' has no terminator");
                }

                out += block.label + ":\n";
                for (const std::string & line : block.lines)
                    out += "  " + line + "\n";
            }

            out += "}\n";
        }

        std::string __unique(const std::string & hint)
        {
            std::string name = hint;
            for (int k = 0; !__names.insert(name).second; )
                name = hint + std::to_string(++k);

            return name;
        }

        std::string __temp(const std::string & hint = "t")
        {
            // Temporaries are numbered by the module wide counter of this function.
            return "%" + hint + std::to_string(__temp_index++);
        }

        size_t __find(const std::string & label) const
        {
            for (size_t k = 0; k < __blocks.size(); k++)
            {
                if (__blocks[k].label == label)
                    return k;
            }

            throw ll_error_t("unknown block '" + label + "'");
        }

        void __emit(const std::string & line)
        {
            __block_t & block = __blocks[__current];
            if (block.terminated)
                throw ll_error_t("instruction after the terminator of block '" + block.label + "'");

            block.lines.push_back(line);
        }

        void __terminate(const std::string & line)
        {
            __emit(line);
            __blocks[__current].terminated = true;
        }

        value_t __cmp(const char * op, const std::string & cond, const value_t & a,
                      const value_t & b)
        {
            __expect_value(a);
            __expect_value(b);

            if (a.type != b.type)
                throw ll_error_t(std::string(op) + ": operand types differ");

            std::string r = __temp();
            __emit(r + " = " + op + " " + cond + " " + a.type + " " + a.text + ", " + b.text);

            return value_t { types::i1, r };
        }

        static void __expect_value(const value_t & v)
        {
            if (!v.valid())
                throw ll_error_t("invalid operand (a void call result?)");
        }

        static void __expect_ptr(const value_t & v)
        {
            __expect_value(v);
            if (v.type != types::ptr)
                throw ll_error_t("operand of type " + v.type + " is not a pointer");
        }

        std::string     __name, __ret_type, __linkage;
        std::vector<std::pair<std::string, std::string>> __params;   // (type, name)

        std::vector<__block_t>          __blocks;
        size_t                          __current = 0;
        size_t                          __temp_index = 0;
        size_t                          __entry_allocas = 0;
        std::unordered_set<std::string> __names;
    };

    ////////// ////////// ////////// ////////// //////////

    // An IR module.
    class module_t
    {
    public:

        module_t(std::string source_name = "april")
            : __source_name(std::move(source_name)) { }

        // Declares an external function (e.g. of the native runtime).
        void declare(const std::string & name, const std::string & ret_type,
                     const std::vector<std::string> & param_types)
        {
            __check_function_name(name);

            std::string text = "declare " + ret_type + " @" + name + "(";
            for (size_t k = 0; k < param_types.size(); k++)
                text += (k > 0? ", " : "") + param_types[k];

            text += ")";
            __declarations.push_back(text);
        }

        // Defines a function: params are (type, name) pairs.
        // The function is owned by the module and stays valid.
        function_t & define(const std::string & name, const std::string & ret_type,
                const std::vector<std::pair<std::string, std::string>> & params,
                const std::string & linkage = "")
        {
            __check_function_name(name);

            __functions.push_back(std::unique_ptr<function_t>(
                new function_t(name, ret_type, linkage, params)
            ));

            return *__functions.back();
        }

        // Adds a wide string constant (a NUL-terminated array of 32 bit chars, the
        // layout of april's wchar_t strings on Linux; 16 bit chars on Windows), returns
        // a pointer operand.
        value_t add_wstring(const std::u32string & chars, unsigned char_bits = 32)
        {
            std::string name = "@.wstr." + std::to_string(__globals.size());
            std::string type = char_bits == 16? types::i16 : types::i32;

            std::string text = name + " = private unnamed_addr constant "
                             + types::array(chars.size() + 1, type) + " [";

            for (char32_t ch : chars)
                text += type + " " + std::to_string((uint32_t)ch) + ", ";

            text += type + " 0]";
            __globals.push_back(text);

            return value_t { types::ptr, name };
        }

        // Adds a byte string constant (NUL-terminated), returns a pointer operand.
        value_t add_cstring(const std::string & bytes)
        {
            std::string name = "@.str." + std::to_string(__globals.size());

            std::string text = name + " = private unnamed_addr constant "
                             + types::array(bytes.size() + 1, types::i8) + " c\"";

            for (unsigned char c : bytes)
            {
                if (c >= 0x20 && c < 0x7F && c != '"' && c != '\\')
                {
                    text += (char)c;
                }
                else
                {
                    char hex[8];
                    std::snprintf(hex, sizeof(hex), "\\%02X", (unsigned)c);
                    text += hex;
                }
            }

            text += "\\00\"";
            __globals.push_back(text);

            return value_t { types::ptr, name };
        }

        // Adds a raw global definition line (the caller formats it).
        void add_global_line(const std::string & line)
        {
            __globals.push_back(line);
        }

        // Generates the module text. Throws ll_error_t on a malformed function.
        std::string str() const
        {
            std::string out = "; ModuleID = '" + __source_name + "'\n";
            out += "source_filename = \"" + __source_name + "\"\n\n";

            for (const std::string & g : __globals)
                out += g + "\n";

            if (!__globals.empty())
                out += "\n";

            for (const std::string & d : __declarations)
                out += d + "\n";

            if (!__declarations.empty())
                out += "\n";

            for (auto && f : __functions)
            {
                f->write(out);
                out += "\n";
            }

            return out;
        }

    private:

        void __check_function_name(const std::string & name)
        {
            if (name.empty())
                throw ll_error_t("empty function name");

            if (!__function_names.insert(name).second)
                throw ll_error_t("duplicated function '" + name + "'");
        }

        std::string                 __source_name;
        std::vector<std::string>    __globals, __declarations;
        std::unordered_set<std::string> __function_names;
        std::vector<std::unique_ptr<function_t>> __functions;
    };

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::native::ll

#endif  // __NATIVE_LL_BUILDER_H__
