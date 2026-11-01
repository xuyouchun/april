// The JIT back end of april: the IR of the native back end is compiled with LLVM, to an
// object file, and the object is run in the process with the JIT (ORC) of LLVM.
// See modules/native/jit_api.h.

#include "../native/jit_api.h"

#include <llvm/Config/llvm-config.h>

#include <llvm-c/Analysis.h>
#include <llvm-c/Core.h>
#include <llvm-c/Error.h>
#include <llvm-c/IRReader.h>
#include <llvm-c/LLJIT.h>
#include <llvm-c/Orc.h>
#include <llvm-c/Support.h>
#include <llvm-c/Target.h>
#include <llvm-c/TargetMachine.h>
#include <llvm-c/Transforms/PassBuilder.h>
#include <llvm-c/Types.h>

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>

namespace
{
    // Writes a message to the buffer of the caller.
    void __set_error(char * error, size_t error_size, const std::string & message)
    {
        if (error == nullptr || error_size == 0)
            return;

        size_t n = message.size() < error_size - 1? message.size() : error_size - 1;
        std::memcpy(error, message.data(), n);
        error[n] = 0;
    }

    // The message of an error of LLVM; the error is consumed.
    std::string __message_of(LLVMErrorRef error)
    {
        char * text = LLVMGetErrorMessage(error);
        std::string message = text? text : "unknown error";
        LLVMDisposeErrorMessage(text);

        return message;
    }

    // Initializes LLVM for the host.
    void __init_llvm()
    {
        static std::once_flag once;

        std::call_once(once, [] {
            LLVMInitializeNativeTarget();
            LLVMInitializeNativeAsmPrinter();
            LLVMInitializeNativeAsmParser();
        });
    }

    // Disposes the message of LLVM, and returns it.
    std::string __take(char * message)
    {
        std::string s = message? message : "";
        if (message != nullptr)
            LLVMDisposeMessage(message);

        return s;
    }

    // What is made by compiling, it is freed at the end.
    struct __compiling_t
    {
        LLVMContextRef              context = nullptr;
        LLVMModuleRef               module = nullptr;
        LLVMTargetMachineRef        machine = nullptr;
        LLVMTargetDataRef           layout = nullptr;
        LLVMPassBuilderOptionsRef   options = nullptr;
        char *                      triple = nullptr;
        char *                      cpu = nullptr;
        char *                      features = nullptr;

        ~__compiling_t()
        {
            if (options)    LLVMDisposePassBuilderOptions(options);
            if (layout)     LLVMDisposeTargetData(layout);
            if (machine)    LLVMDisposeTargetMachine(machine);
            if (module)     LLVMDisposeModule(module);
            if (context)    LLVMContextDispose(context);
            if (triple)     LLVMDisposeMessage(triple);
            if (cpu)        LLVMDisposeMessage(cpu);
            if (features)   LLVMDisposeMessage(features);
        }
    };
}

extern "C" {

    APRIL_JIT_EXPORT int april_jit_version(void)
    {
        return APRIL_JIT_API_VERSION;
    }

    APRIL_JIT_EXPORT const char * april_jit_llvm_version(void)
    {
        return LLVM_VERSION_STRING;
    }

    APRIL_JIT_EXPORT int april_jit_compile(const char * ir, size_t length, int optimize,
                    const char * object_path, char * error, size_t error_size)
    {
        __init_llvm();

        __compiling_t c;
        char * message = nullptr;

        c.context = LLVMContextCreate();

        // The buffer is consumed by the parser.
        LLVMMemoryBufferRef buffer = LLVMCreateMemoryBufferWithMemoryRangeCopy(ir, length, "april");
        if (LLVMParseIRInContext(c.context, buffer, &c.module, &message))
        {
            __set_error(error, error_size, "the IR is not valid: " + __take(message));
            return 1;
        }

        // The host.
        c.triple = LLVMGetDefaultTargetTriple();

        LLVMTargetRef target = nullptr;
        if (LLVMGetTargetFromTriple(c.triple, &target, &message))
        {
            __set_error(error, error_size, "no target for '" + std::string(c.triple) + "': "
                                            + __take(message));
            return 1;
        }

        c.cpu = LLVMGetHostCPUName();
        c.features = LLVMGetHostCPUFeatures();

        if (optimize < 0) optimize = 0;
        if (optimize > 3) optimize = 3;

        static const LLVMCodeGenOptLevel levels[] = {
            LLVMCodeGenLevelNone, LLVMCodeGenLevelLess,
            LLVMCodeGenLevelDefault, LLVMCodeGenLevelAggressive
        };

        c.machine = LLVMCreateTargetMachine(target, c.triple, c.cpu, c.features,
                                    levels[optimize], LLVMRelocPIC, LLVMCodeModelJITDefault);
        if (c.machine == nullptr)
        {
            __set_error(error, error_size, "cannot make the target machine");
            return 1;
        }

        LLVMSetTarget(c.module, c.triple);
        c.layout = LLVMCreateTargetDataLayout(c.machine);
        LLVMSetModuleDataLayout(c.module, c.layout);

        if (LLVMVerifyModule(c.module, LLVMReturnStatusAction, &message))
        {
            __set_error(error, error_size, "the IR is not valid: " + __take(message));
            return 1;
        }

        // Optimizes.
        char passes[32];
        std::snprintf(passes, sizeof(passes), "default<O%d>", optimize);

        c.options = LLVMCreatePassBuilderOptions();
        if (LLVMErrorRef e = LLVMRunPasses(c.module, passes, c.machine, c.options))
        {
            __set_error(error, error_size, "optimizing failed: " + __message_of(e));
            return 1;
        }

        // The object file.
        if (LLVMTargetMachineEmitToFile(c.machine, c.module, (char *)object_path,
                                        LLVMObjectFile, &message))
        {
            __set_error(error, error_size, "cannot make the object file: " + __take(message));
            return 1;
        }

        return 0;
    }

    APRIL_JIT_EXPORT int april_jit_run(const char * object_path,
                    const april_jit_symbol_t * symbols, size_t symbol_count,
                    int argc, char ** argv, int * exit_code, char * error, size_t error_size)
    {
        __init_llvm();

        LLVMOrcLLJITBuilderRef builder = LLVMOrcCreateLLJITBuilder();

        LLVMOrcLLJITRef jit = nullptr;
        if (LLVMErrorRef e = LLVMOrcCreateLLJIT(&jit, builder))
        {
            __set_error(error, error_size, "cannot make the JIT: " + __message_of(e));
            return 1;
        }

        LLVMOrcJITDylibRef dylib = LLVMOrcLLJITGetMainJITDylib(jit);

        // The symbols of april.
        std::vector<LLVMOrcCSymbolMapPair> pairs;
        for (size_t k = 0; k < symbol_count; k++)
        {
            LLVMOrcCSymbolMapPair pair;
            pair.Name = LLVMOrcLLJITMangleAndIntern(jit, symbols[k].name);
            pair.Sym.Address = (LLVMOrcExecutorAddress)(uintptr_t)symbols[k].address;
            pair.Sym.Flags.GenericFlags = LLVMJITSymbolGenericFlagsExported
                                        | LLVMJITSymbolGenericFlagsCallable;
            pair.Sym.Flags.TargetFlags = 0;

            pairs.push_back(pair);
        }

        if (LLVMErrorRef e = LLVMOrcJITDylibDefine(dylib,
                        LLVMOrcAbsoluteSymbols(pairs.data(), pairs.size())))
        {
            __set_error(error, error_size, "cannot define the symbols: " + __message_of(e));
            LLVMOrcDisposeLLJIT(jit);
            return 1;
        }

        // The C library, and what else the process has.
        LLVMOrcDefinitionGeneratorRef generator = nullptr;
        if (LLVMErrorRef e = LLVMOrcCreateDynamicLibrarySearchGeneratorForProcess(
                            &generator, LLVMOrcLLJITGetGlobalPrefix(jit), nullptr, nullptr))
        {
            __set_error(error, error_size, "cannot search the process: " + __message_of(e));
            LLVMOrcDisposeLLJIT(jit);
            return 1;
        }

        LLVMOrcJITDylibAddGenerator(dylib, generator);

        // The program.
        char * message = nullptr;
        LLVMMemoryBufferRef object = nullptr;
        if (LLVMCreateMemoryBufferWithContentsOfFile(object_path, &object, &message))
        {
            __set_error(error, error_size, "cannot read '" + std::string(object_path) + "': "
                                            + __take(message));
            LLVMOrcDisposeLLJIT(jit);
            return 1;
        }

        if (LLVMErrorRef e = LLVMOrcLLJITAddObjectFile(jit, dylib, object))
        {
            __set_error(error, error_size, "cannot load the program: " + __message_of(e));
            LLVMOrcDisposeLLJIT(jit);
            return 1;
        }

        LLVMOrcExecutorAddress address = 0;
        if (LLVMErrorRef e = LLVMOrcLLJITLookup(jit, &address, "main"))
        {
            __set_error(error, error_size, "cannot link the program: " + __message_of(e));
            LLVMOrcDisposeLLJIT(jit);
            return 1;
        }

        typedef int (*main_t)(int, char **);
        *exit_code = ((main_t)(uintptr_t)address)(argc, argv);

        // The code of the program is not needed any more.
        if (LLVMErrorRef e = LLVMOrcDisposeLLJIT(jit))
            LLVMConsumeError(e);

        return 0;
    }

}
