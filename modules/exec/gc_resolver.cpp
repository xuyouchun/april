
#include <exec.h>
#include "__gc_stack.h"

namespace X_ROOT_NS::modules::exec {

    using namespace core;
    using namespace rt;

    ////////// ////////// ////////// ////////// //////////
    // Resolves which parts of an object may hold references, from runtime metadata.
    //
    // The rule: answer precisely only when sure; every doubt (unknown storage,
    // unknown offsets, generic types, unexpected kinds) returns false, which
    // makes the collector scan the object conservatively. A wrong "precise"
    // answer could lose a live object, so the code below mirrors how the
    // layout code (type_layout_t) classifies and places fields.

    namespace
    {
        typedef gc_scan_info_t::kind_t __kind_t;

        // Adds a byte range, merging it with the previous one when adjacent.
        void __add_range(gc_scan_info_t & info, msize_t offset, msize_t size)
        {
            if (size == 0)
                return;

            if (!info.ranges.empty())
            {
                auto & last = info.ranges.back();
                if (last.first + last.second == offset)
                {
                    last.second += size;
                    return;
                }
            }

            info.ranges.push_back(std::make_pair((uint32_t)offset, (uint32_t)size));
        }

        // Returns whether it's a plain (non generic) general type.
        bool __is_plain_general(rt_type_t * type)
        {
            if (type->get_kind() != rt_type_kind_t::general)
                return false;

            rt_general_type_t * general = static_cast<rt_general_type_t *>(type);
            return !general->is_generic_template();
        }

        // Resolves scan info of an array: decided by the element storage.
        bool __resolve_array(analyzer_env_t & env, rt_array_type_t * type,
                             gc_scan_info_t & info)
        {
            if (type->element_type == nullptr)
                return false;

            storage_type_t storage = storage_type_t::__unknown__;
            type->element_type->get_variable_size(env, &storage);

            switch (storage)
            {
                case storage_type_t::value:
                    info.kind = __kind_t::none;         // e.g. int[], double[], enum[].
                    return true;

                case storage_type_t::ref:
                    info.kind = __kind_t::conservative; // All elements are references.
                    return true;

                default:                                // Mixture structs, unknown ...
                    return false;
            }
        }

        // Resolves scan info of a class instance by walking the fields of
        // the type and all of its base types.
        bool __resolve_class(analyzer_env_t & env, rt_type_t * type, gc_scan_info_t & info)
        {
            std::vector<std::pair<msize_t, msize_t>> refs;      // (offset, size)

            for (rt_type_t * t = type; t != nullptr; t = t->get_base_type(env))
            {
                if (!__is_plain_general(t))
                    return false;

                if (t->get_ttype(env) != ttype_t::class_)
                    return false;

                assembly_analyzer_t analyzer(env, t->get_assembly());
                bool ok = true;

                t->each_field(env, [&](ref_t, rt_field_base_t * field) {

                    if (field == nullptr || field->offset == unknown_msize)
                        return ok = false;

                    rt_type_t * field_type = field->get_field_type(analyzer);
                    if (field_type == nullptr)
                        return ok = false;

                    storage_type_t storage = storage_type_t::__unknown__;
                    msize_t size = field_type->get_variable_size(env, &storage);

                    switch (storage)
                    {
                        case storage_type_t::value:
                            break;                              // Never holds references.

                        case storage_type_t::ref:
                        case storage_type_t::mixture:           // Struct with refs: whole region.
                            refs.push_back(std::make_pair(field->offset, size));
                            break;

                        default:
                            return ok = false;
                    }

                    return true;
                });

                if (!ok)
                    return false;
            }

            std::sort(refs.begin(), refs.end());

            info.kind = __kind_t::ranges;
            info.ranges.clear();

            for (auto && r : refs)
            {
                __add_range(info, r.first, r.second);
            }

            if (info.ranges.empty())
                info.kind = __kind_t::none;

            return true;
        }
    }

    // Resolves gc scan info of a runtime type, see gc_type_resolver_t.
    bool resolve_gc_scan_info(analyzer_env_t & env, rt_type_t * type, gc_scan_info_t & info)
    {
        if (type == nullptr)
            return false;

        switch (type->get_kind())
        {
            case rt_type_kind_t::array:
                return __resolve_array(env, static_cast<rt_array_type_t *>(type), info);

            case rt_type_kind_t::general: {

                if (!__is_plain_general(type))
                    return false;

                switch (type->get_ttype(env))
                {
                    case ttype_t::class_:
                        return __resolve_class(env, type, info);

                    case ttype_t::struct_:
                    case ttype_t::enum_: {
                        // Boxed values: only the ones without references are precise.
                        storage_type_t storage = storage_type_t::__unknown__;
                        type->get_variable_size(env, &storage);

                        if (storage != storage_type_t::value)
                            return false;

                        info.kind = __kind_t::none;
                        return true;
                    }

                    default:
                        return false;
                }
            }

            default:                                // Generic types, placeholders ...
                return false;
        }
    }

    ////////// ////////// ////////// ////////// //////////
    // command_execute_context_t: gc roots of the execution stack.

    // Enumerates conservative gc root ranges: the execution stack, minus the value-only
    // regions of local variables of every calling frame (see each_gc_stack_range).
    void command_execute_context_t::each_gc_root_ranges(
                const std::function<void (void **, void **)> & f)
    {
        each_gc_stack_range<__calling_stub_t>(
            stack.bottom(), stack.top(), stack.lp(),
            [&f](rt_stack_unit_t * begin, rt_stack_unit_t * end) {
                f((void **)begin, (void **)end);
            }
        );
    }

    ////////// ////////// ////////// ////////// //////////
    // gc_type_resolver_scope_t

    // Installs the resolver into the heap.
    gc_type_resolver_scope_t::gc_type_resolver_scope_t(rt_heap_t * heap, analyzer_env_t & env)
        : __heap(heap)
    {
        _A(heap != nullptr);

        heap->set_gc_type_resolver([&env](rt_type_t * type, gc_scan_info_t & info) {
            return resolve_gc_scan_info(env, type, info);
        });
    }

    // Removes the resolver: it captures an environment that is about to go away.
    gc_type_resolver_scope_t::~gc_type_resolver_scope_t()
    {
        __heap->set_gc_type_resolver(gc_type_resolver_t());
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::exec
