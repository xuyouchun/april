// Tests of modules/native/ll_builder.h (standalone: standard library only).
#include "../../modules/native/ll_builder.h"

#include <cstdio>

using namespace april::modules::native::ll;

static int failures = 0, checks = 0;

#define CHECK(cond) do { checks++; \
    if (!(cond)) { std::printf("FAIL %d: %s\n", __LINE__, #cond); failures++; } \
} while (0)

#define CHECK_THROWS(expr) do { checks++; bool thrown = false; \
    try { expr; } catch (const ll_error_t &) { thrown = true; } \
    if (!thrown) { std::printf("FAIL %d: no ll_error_t from %s\n", __LINE__, #expr); failures++; } \
} while (0)

static bool contains(const std::string & s, const std::string & sub)
{
    return s.find(sub) != std::string::npos;
}

int main()
{
    // 1. A small module compared with the exact expected text.
    {
        module_t m("hello");
        m.declare("april_call_libfunc", "void", { "ptr", "ptr" });
        value_t s = m.add_wstring(U"Hi");

        function_t & f = m.define("april_main", "i32", { });
        value_t block = f.alloca_(types::array(1, types::i64), "args");
        value_t as_int = f.cast("ptrtoint", s, types::i64);
        f.store(as_int, f.gep_bytes(block, 0));
        f.ret(const_int(types::i32, 0));

        const char * expected =
            "; ModuleID = 'hello'\n"
            "source_filename = \"hello\"\n"
            "\n"
            "@.wstr.0 = private unnamed_addr constant [3 x i32] [i32 72, i32 105, i32 0]\n"
            "\n"
            "declare void @april_call_libfunc(ptr, ptr)\n"
            "\n"
            "define i32 @april_main() {\n"
            "entry:\n"
            "  %args0 = alloca [1 x i64]\n"
            "  %t1 = ptrtoint ptr @.wstr.0 to i64\n"
            "  %t2 = getelementptr inbounds i8, ptr %args0, i64 0\n"
            "  store i64 %t1, ptr %t2\n"
            "  ret i32 0\n"
            "}\n"
            "\n";

        std::string actual = m.str();
        if (actual != expected)
            std::printf("--- actual ---\n%s--------------\n", actual.c_str());

        CHECK(actual == expected);
    }

    // 2. Control flow: blocks, branches, comparison, params.
    {
        module_t m;
        function_t & f = m.define("max", "i32", { { "i32", "a" }, { "i32", "b" } });

        value_t greater = f.icmp("sgt", f.param(0), f.param(1));
        std::string yes = f.new_block("yes"), no = f.new_block("no");
        f.cond_br(greater, yes, no);

        f.set_block(yes);
        f.ret(f.param(0));

        f.set_block(no);
        f.ret(f.param(1));

        std::string text = m.str();
        CHECK(contains(text, "define i32 @max(i32 %a, i32 %b) {"));
        CHECK(contains(text, "%t0 = icmp sgt i32 %a, %b"));
        CHECK(contains(text, "br i1 %t0, label %yes, label %no"));
        CHECK(contains(text, "yes:\n  ret i32 %a\n"));
        CHECK(contains(text, "no:\n  ret i32 %b\n"));
    }

    // 3. Calls: void and non void results.
    {
        module_t m;
        m.declare("puts_like", "void", { "ptr" });
        m.declare("sqrt", "double", { "double" });

        function_t & f = m.define("f", "double", { });
        value_t str = m.add_cstring("a\"b\n");

        value_t none = f.call("void", "puts_like", { str });
        CHECK(!none.valid());

        value_t r = f.call("double", "sqrt", { const_double(4.0) });
        CHECK(r.valid() && r.type == "double");
        f.ret(r);

        std::string text = m.str();
        CHECK(contains(text, "call void @puts_like(ptr @.str.0)"));
        CHECK(contains(text, "= call double @sqrt(double 0x4010000000000000)"));
        CHECK(contains(text, "c\"a\\22b\\0A\\00\""));
    }

    // 4. Constants.
    {
        CHECK(const_int(types::i32, -5).text == "-5");
        CHECK(const_bool(true).text == "true");
        CHECK(const_null().text == "null");
        CHECK(const_double(1.0).text == "0x3FF0000000000000");
        CHECK(const_float(0.5f).type == "float" && const_float(0.5f).text == "0x3FE0000000000000");
    }

    // 5. Structural checks: every misuse is rejected.
    {
        module_t m;

        {   // A block without a terminator.
            function_t & f = m.define("open", "void", { });
            (void)f;
            CHECK_THROWS(m.str());
            f.ret();
            CHECK(contains(m.str(), "define void @open()"));
        }

        {   // An instruction after the terminator.
            function_t & f = m.define("closed", "void", { });
            f.ret();
            CHECK_THROWS(f.alloca_("i32"));
            CHECK_THROWS(f.ret());
        }

        {   // Unknown label, closed block as target of set_block.
            function_t & f = m.define("labels", "void", { });
            CHECK_THROWS(f.br("nowhere"));

            std::string next = f.new_block("next");
            f.br(next);
            CHECK_THROWS(f.set_block("entry"));
            f.set_block(next);
            f.ret();
        }

        {   // Operand checks.
            function_t & f = m.define("operands", "void", { { "i32", "x" } });
            CHECK_THROWS(f.load("i32", f.param(0)));                       // not a pointer
            CHECK_THROWS(f.binop("add", f.param(0), const_int("i64", 1))); // types differ
            CHECK_THROWS(f.icmp("eq", f.param(0), const_int("i64", 1)));
            CHECK_THROWS(f.ret(value_t { }));                              // invalid operand
            CHECK_THROWS(f.param(5));
            f.ret();
        }

        // Duplicated names.
        CHECK_THROWS(m.define("open", "void", { }));
        CHECK_THROWS(m.declare("closed", "void", { }));
        CHECK_THROWS(m.define("", "void", { }));
        CHECK_THROWS(m.define("dup_params", "void", { { "i32", "a" }, { "i32", "a" } }));
    }

    // 6. Unique labels for the same hint.
    {
        module_t m;
        function_t & f = m.define("labels", "void", { });
        std::string a = f.new_block("loop"), b = f.new_block("loop"), c = f.new_block("loop");

        CHECK(a != b && b != c && a != c);

        f.br(a);
        f.set_block(a);  f.br(b);
        f.set_block(b);  f.br(c);
        f.set_block(c);  f.ret();
        CHECK(contains(m.str(), "br label %" + b));
    }

    // 7. alloca_entry: lands in the entry block, in order, wherever the cursor is.
    {
        module_t m;
        function_t & f = m.define("g", "void", { });
        value_t a = f.alloca_entry("i32", "a");
        std::string next = f.new_block("next");
        f.store(const_int("i32", 1), a);
        f.br(next);
        f.set_block(next);
        value_t b = f.alloca_entry("i64", "b");       // from another block
        f.store(const_int("i64", 2), b);
        f.ret();

        std::string text = m.str();
        size_t pos_a = text.find("%a0 = alloca i32");
        size_t pos_b = text.find("%b1 = alloca i64");
        size_t pos_store = text.find("store i32 1");
        size_t pos_next = text.find("next:");

        CHECK(pos_a != std::string::npos && pos_b != std::string::npos);
        CHECK(pos_a < pos_b && pos_b < pos_store && pos_store < pos_next);
    }

    // 8. switch: cases and a default, labels are checked.
    {
        module_t m;
        function_t & f = m.define("sw", "i32", { { "i32", "v" } });
        std::string a = f.new_block("a"), b = f.new_block("b"), def = f.new_block("def");
        f.switch_(f.param(0), def, { { 1, a }, { -5, b } });

        f.set_block(a);
        f.ret(const_int("i32", 10));
        f.set_block(b);
        f.ret(const_int("i32", 20));
        f.set_block(def);
        f.ret(const_int("i32", 0));

        CHECK(contains(m.str(), "switch i32 %v, label %def [ i32 1, label %a i32 -5, label %b ]"));

        module_t m2;
        function_t & g = m2.define("g", "void", { { "i32", "v" } });
        CHECK_THROWS(g.switch_(g.param(0), "missing", { }));
    }

    std::printf("%d checks, %d failures\n", checks, failures);
    return failures == 0? 0 : 1;
}
