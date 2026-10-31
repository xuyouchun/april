// Standalone tests of modules/native/driver.{h,cpp}: no boost, no april, no LLVM.
// A fake "clang++" shell script stands in for the real one.

#include "../../modules/native/driver.h"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include <sys/stat.h>
#include <unistd.h>

namespace fs = std::filesystem;
using namespace april::modules::native;

static int __failed = 0, __total = 0;

#define CHECK(cond) do { __total++; if (!(cond)) { __failed++; \
    std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << "  " #cond << std::endl; } } while (0)

static void write_file(const fs::path & p, const std::string & text, bool exec = false)
{
    std::ofstream(p, std::ios::binary) << text;
    if (exec)
        chmod(p.c_str(), 0755);
}

static std::string read_file(const fs::path & p)
{
    std::ifstream in(p, std::ios::binary);
    std::stringstream ss;
    ss << in.rdbuf();
    return ss.str();
}

template<typename f_t> static bool throws_with(f_t f, const std::string & part)
{
    try { f(); } catch (const driver_error_t & e)
    {
        bool ok = std::string(e.what()).find(part) != std::string::npos;
        if (!ok) std::cerr << "  message was: " << e.what() << std::endl;
        return ok;
    }
    return false;
}

int main()
{
    fs::path root = fs::temp_directory_path() / ("april_driver_test_" + std::to_string(getpid()));
    fs::remove_all(root);       // a stale directory of a previous run with the same pid
    fs::create_directories(root / "libs");
    fs::create_directories(root / "bin");

    // ---- pure functions
    CHECK(shell_quote("abc") == "abc");
    CHECK(shell_quote("a b") == "'a b'");
    CHECK(shell_quote("it's") == "'it'\\''s'");
    CHECK(shell_quote("") == "''");
    CHECK(command_line_of({ "a", "b c" }) == "a 'b c'");

    CHECK(parse_clang_major_version("Ubuntu clang version 17.0.6 (x)\nTarget") == 17);
    CHECK(parse_clang_major_version("clang version 14.0.0") == 14);
    CHECK(parse_clang_major_version("gcc (GCC) 12") == -1);
    CHECK(parse_clang_major_version("clang version x") == -1);

    CHECK(default_output_path("a/b/prog.ll") == "a/b/prog.native");
    CHECK(default_output_path("prog.ll") == "prog.native");

    {
        auto c = compile_command("clang++", "p.ll", "p.o", 7);
        CHECK(c.front() == "clang++" && c.back() == "p.o");
        bool has_o3 = false;
        for (auto & a : c) has_o3 |= (a == "-O3");
        CHECK(has_o3);

        auto l = link_command("clang++", "p.o", "p", { "libA.a", "libB.a" }, true);
        std::string s = command_line_of(l);
        CHECK(s.find("-no-pie") != std::string::npos);
        CHECK(s.find("-Wl,--start-group libA.a libB.a -Wl,--end-group") != std::string::npos);
        CHECK(s.find("-lboost_log") != std::string::npos);
        CHECK(command_line_of(link_command("c", "p.o", "p", {}, false)).find("-no-pie")
                                                                    == std::string::npos);
    }

    // ---- libs
    {
        build_options_t o;
        o.libs_dir = (root / "libs").string();
        CHECK(throws_with([&] { find_libs_dir(o); }, "libnative.a not found"));

        write_file(root / "libs" / "libnative.a", "x");
        write_file(root / "libs" / "librtlib.a", "x");
        write_file(root / "libs" / "notalib.txt", "x");
        write_file(root / "libs" / "lib.a", "x");      // too short a name
        CHECK(find_libs_dir(o) == o.libs_dir);

        auto libs = list_static_libs(o.libs_dir);
        CHECK(libs.size() == 2);
        CHECK(fs::path(libs[0]).filename() == "libnative.a");
        CHECK(fs::path(libs[1]).filename() == "librtlib.a");

        // Found next to the april executable.
        build_options_t o2;
        o2.april_path = (root / "bin" / "april").string();
        CHECK(find_libs_dir(o2).find("libs") != std::string::npos);

        build_options_t o3;
        o3.april_path = "/nonexistent/bin/april";
        CHECK(throws_with([&] { find_libs_dir(o3); }, "Searched"));
    }

    // ---- processes
    {
        CHECK(run_process({ "true" }) == 0);
        CHECK(run_process({ "false" }) == 1);
        CHECK(run_process({ "sh", "-c", "exit 7" }) == 7);
        CHECK(run_process({ "sh", "-c", "kill -9 $$" }) == 128 + 9);
        CHECK(throws_with([] { run_process({ "no-such-program-xyz" }); }, "cannot run"));

        std::string out;
        CHECK(run_process_capture({ "echo", "hello" }, out) == 0);
        CHECK(out == "hello\n");
    }

    // ---- build_executable with a fake clang++
    fs::path ir = root / "prog.ll";
    write_file(ir, "; fake ir\n");

    fs::path fake = root / "fake-clang";
    fs::path argv_log = root / "argv.log";

    auto write_fake = [&](int major, bool fail_compile, bool fail_link) {
        std::ostringstream s;
        s << "#!/bin/sh\n"
             "if [ \"$1\" = \"--version\" ]; then echo 'clang version " << major << ".0.1'; exit 0; fi\n"
             "echo \"$@\" >> '" << argv_log.string() << "'\n"
             "case \"$1\" in\n"
             "  -c) " << (fail_compile? "echo 'prog.ll:1:1: error: bad' >&2; exit 1" : ":")
          << "; out=''; while [ $# -gt 0 ]; do [ \"$1\" = -o ] && out=$2; shift; done;"
             " echo obj > \"$out\"; exit 0;;\n"
             "  *) " << (fail_link? "echo 'ld: failed' >&2; exit 1" : ":")
          << "; out=''; while [ $# -gt 0 ]; do [ \"$1\" = -o ] && out=$2; shift; done;"
             " printf '#!/bin/sh\\necho native-ran \"$@\"; exit 3\\n' > \"$out\"; chmod +x \"$out\"; exit 0;;\n"
             "esac\n";
        write_file(fake, s.str(), true);
        fs::remove(argv_log);
    };

    build_options_t bo;
    bo.ir_path = ir.string();
    bo.libs_dir = (root / "libs").string();
    bo.clang = fake.string();
    bo.optimize = 1;

    std::vector<std::string> logged;
    bo.log = [&](const std::string & c) { logged.push_back(c); };

    // success
    write_fake(17, false, false);
    {
        std::string exe = build_executable(bo);
        CHECK(exe == (root / "prog.native").string());
        CHECK(fs::exists(exe));
        CHECK(!fs::exists(exe + ".o"));         // object removed
        CHECK(logged.size() == 2);
        std::string log = read_file(argv_log);
        CHECK(log.find("-O1") != std::string::npos);
        CHECK(log.find("libnative.a") != std::string::npos);
        CHECK(log.find("librtlib.a") != std::string::npos);

        // the produced executable runs and its exit code is forwarded
        CHECK(run_executable(exe, { "x" }) == 3);
    }

    // keep_object + explicit output
    {
        build_options_t b2 = bo;
        b2.keep_object = true;
        b2.output_path = (root / "custom.bin").string();
        std::string exe = build_executable(b2);
        CHECK(exe == b2.output_path);
        CHECK(fs::exists(exe + ".o"));
    }

    // old clang
    write_fake(14, false, false);
    CHECK(throws_with([&] { build_executable(bo); }, "too old"));

    // not clang
    write_file(fake, "#!/bin/sh\necho 'gcc 12'\n", true);
    CHECK(throws_with([&] { build_executable(bo); }, "does not look like clang"));

    // missing clang
    {
        build_options_t b3 = bo;
        b3.clang = "no-such-clang-xyz";
        CHECK(throws_with([&] { build_executable(b3); }, "cannot run"));
    }

    // compile failure
    write_fake(17, true, false);
    CHECK(throws_with([&] { build_executable(bo); }, "compiling"));

    // link failure
    write_fake(17, false, true);
    CHECK(throws_with([&] { build_executable(bo); }, "linking"));

    // missing ir
    {
        build_options_t b4 = bo;
        b4.ir_path = (root / "missing.ll").string();
        CHECK(throws_with([&] { build_executable(b4); }, "not found"));
    }

    // run_executable with a bare name must not use PATH
    {
        std::string old = fs::current_path().string();
        fs::current_path(root);
        CHECK(run_executable("prog.native") == 3);
        fs::current_path(old);
    }

    fs::remove_all(root);

    std::cout << (__total - __failed) << "/" << __total << " checks passed" << std::endl;
    return __failed == 0? 0 : 1;
}
