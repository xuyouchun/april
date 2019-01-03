#ifndef __RT_ASSEMBLY_LOADER_H__
#define __RT_ASSEMBLY_LOADER_H__

namespace X_ROOT_NS { namespace modules { namespace rt {

    namespace
    {
        using namespace core;
    }

    ////////// ////////// ////////// ////////// //////////

    X_INTERFACE rt_assembly_ref_loader_t
    {
        virtual rt_assembly_t * load_asembly_ref() = 0;
    };

    ////////// ////////// ////////// ////////// //////////

    rt_assembly_t * load_assembly(rt_context_t & ctx, xistream_t & stream,
            rt_assembly_ref_loader_t * ref_loader = nullptr);

    rt_assembly_t * load_assembly(rt_context_t & ctx, const string_t & path,
            rt_assembly_ref_loader_t * ref_loader = nullptr);

    ////////// ////////// ////////// ////////// //////////

} } }

#endif // __RT_ASSEMBLY_LOADER_H__
