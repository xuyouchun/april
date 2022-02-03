#ifndef __RT_ASSEMBLY_LOADER_H__
#define __RT_ASSEMBLY_LOADER_H__

namespace X_ROOT_NS::modules::rt {

    namespace
    {
        using namespace core;
    }

    ////////// ////////// ////////// ////////// //////////

    // Loads assembly from stream.
    rt_assembly_t * load_assembly(rt_context_t & ctx, xistream_t & stream);

    // Loads assembly from assembly file.
    rt_assembly_t * load_assembly(rt_context_t & ctx, const string_t & path);

    // Loads assembly by path and assembly name.
    rt_assembly_t * load_assembly(rt_context_t & ctx, const string_t & path,
            const string_t & package, const string_t & name);

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::rt

#endif // __RT_ASSEMBLY_LOADER_H__
