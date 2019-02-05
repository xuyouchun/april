#include <common.h>
#include <lib.h>
#include <algorithm.h>
#include <arch.h>
#include <modules/compile.h>
#include <modules/rt.h>
#include <modules/mm.h>
#include <modules/lang_cs.h>
#include <modules/lang_lq.h>
#include <modules/exec.h>

#include <iostream>
#include <type_traits>
#include <iomanip>
#include <locale>
#include <string>
#include <stdio.h>
#include <codecvt>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include <time.h>


using namespace jc;
using namespace jc::lib;
using namespace jc::algorithm;
using namespace jc::modules::compile;
using namespace jc::modules::lang_cs;
using namespace jc::modules::lang_lq;
using namespace jc::modules::mm;
using namespace jc::modules::exec;
using namespace jc::modules::rt;

using namespace std::placeholders;

void __analyze2()
{
    memory_t * memory = new al::xheap_t();
    simple_lang_factory_t * lang_factory = memory->new_obj<simple_lang_factory_t>();

    lang_factory->register_lang_t<cs_lang_t>();
    lang_factory->register_lang_t<lq_lang_t>();

    //xobject_heap_t * heap = memory->new_obj<xobject_heap_t>();
    
    compiler_t * compiler = memory->new_obj<compiler_t>(lang_factory);
    global_context_t & global_context = compiler->get_context();

    const char_t * solution_path = _T("/Users/xuyc/solutions");
    const char_t * bin_path = _T("/Users/xuyc/solutions/bin");
    solution_t * solution = memory->new_obj<solution_t>(global_context, _T("solution1"));

    std::vector<path_t> project_paths = lib::all_directories(solution_path);
    for(path_t & project_path : project_paths)
    {
        string_t project_name = filename(project_path);
        if(project_name[0] == _T('.') || project_name == _T("bin"))
            continue;

        project_t * project = memory->new_obj<project_t>(global_context, project_name);

        for(path_t & file_path : lib::all_files(project_path, _T("*.cs")))
        {
            file_t * file = file_t::load(global_context, memory, file_path);
            project->append_file(file);
        }

        if(project->file_count() > 0)
        {
            solution->append_project(project);
        }
    }

    for(path_t & jcd_path : lib::all_files(solution_path, _T("*.jcd")))
    {
        string_t assembly_name = filename(jcd_path);
        if(assembly_name[0] == _T('.'))
            continue;

        file_ref_assembly_t * ref_assembly = memory_t::new_obj<file_ref_assembly_t>(
            memory, global_context, lib::truncate_fileext(assembly_name), _T(""), jcd_path
        );

        solution->ref_assemblies.append_ref_assembly(ref_assembly);
    }

    assemblies_t * assemblies = compiler->compile(nullptr, solution, bin_path);
}

xheap_t __xheap;
rt_context_t rt_ctx(&__xheap, create_heap(&__xheap));

rt_assembly_t * __load_rt_assembly(const string_t & path)
{
    rt_assembly_t * assembly = load_assembly(rt_ctx, path, nullptr);

    return assembly;
}

void __analyze3()
{
    const char_t * assembly_path = _T("/Users/xuyc/solutions/Project1.jcd");
    rt_assembly_t * rt_assembly = __load_rt_assembly(assembly_path);
}

void __exec()
{
    const char_t * assembly_path[] = {
        _T("/Users/xuyc/solutions/System.jcd"),
        _T("/Users/xuyc/solutions/Project1.jcd")
    };

    rt_assemblies_t assemblies;
    for(const char_t * path : assembly_path)
    {
        rt_assembly_t * rt_assembly = __load_rt_assembly(path);
        assemblies.append(rt_assembly);
    }

    execute(rt_ctx, assemblies);
}

int main(int argc, char ** argv)
{
    try
    {
        if(argc > 1)
        {
            if(strcmp(argv[1], "build_lib") == 0)
                __analyze2();
            else if(strcmp(argv[1], "execute") == 0)
                __exec();
        }

        //__analyze2();
        //__exec();
    }
    catch(error_t & e)
    {
        _P(_T("\n~~ ERROR ~~"));
        println(e.to_string());
        return 1;
    }

    return 0;
}

