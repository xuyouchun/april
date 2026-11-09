
#include "utils.h"
#include <algorithm.h>

namespace X_ROOT_NS::modules::compile {

    typedef compile_error_code_t         __e_t;
    typedef expression_compile_context_t __cctx_t;
    typedef mt_table_index_t             __tidx_t;

    ////////// ////////// ////////// ////////// //////////

    // Constructor.
    code_section_builder_t::code_section_builder_t(global_context_t & global_context,
            code_t * code, lang_id_t default_lang,
            std::vector<code_section_t *> & sections, pool_t & pool,
            std::vector<code_section_error_t> & errors)
        : __global_context(global_context), __code(code)
        , __source_code(code->source_code()), __length(code->code.length())
        , __sections(sections)
        , __default_lang(default_lang), __pool(pool), __errors(errors)
    {
        _A(code != nullptr);
    }

    // Builds code sections.
    void code_section_builder_t::build()
    {
        __build([this](const char_t * start, const char_t * end, lang_id_t lang, int depth,
                                                                bool open_ended = false) {

            code_section_t * section = __pool.new_obj<code_section_t>(__global_context,
                start, end - start, lang, depth, __code
            );

            section->open_ended = open_ended;
            this->__sections.push_back(section);

        });
    }

    // The markers of the code of an embedded language: <%lq ... %> .
    static const char_t * const __markers[] = { _T("<%"), _T("%>") };

    enum { __begin_marker = 0, __end_marker = 1 };

    // Finds a marker in the code of the language, the language knows what is not code.
    bool code_section_builder_t::__find_marker(lang_id_t lang, size_t start, size_t count,
                        size_t & position, size_t & marker_index, size_t first_marker)
    {
        lang_t * lang0 = __global_context.lang_of(lang);
        if (lang0 != nullptr)
        {
            auto * service = (lang_marker_service_t *)lang0->get_service(
                lang_service_type_t::marker
            );

            if (service != nullptr)
            {
                return service->find_marker(
                    __source_code, __length, start, __markers + first_marker, count,
                    position, marker_index
                );
            }
        }

        // The language does not tell: nothing is skipped.
        for (size_t p = start; p + 1 < __length; p++)
        {
            for (size_t k = 0; k < count; k++)
            {
                const char_t * marker = __markers[first_marker + k];
                if (__source_code[p] == marker[0] && __source_code[p + 1] == marker[1])
                {
                    position = p, marker_index = k;
                    return true;
                }
            }
        }

        return false;
    }

    // Build code sections with callback.
    template<typename callback_t>
    void code_section_builder_t::__build(callback_t callback)
    {
        size_t start = 0, position = 0;
        lang_id_t lang = __default_lang;

        while (true)
        {
            // The end flag is searched only in the code of an embedded language.
            size_t count = __lang_stack.size() > 0? 2 : 1, marker_index;

            bool found = __find_marker(lang, position, count, position, marker_index);
            if (!found)
            {
                // The embedded code is not closed: the error is told. The code of the language that
                // has it is before the start flag, the embedded code is the section after the flag,
                // that has no end flag: the reader of the elements finds where its code ends (as it
                // does for ``lq), and the code of the language that has it goes on there.
                if (__lang_stack.size() > 0)
                {
                    const string_t & name = __lang_names.front();
                    size_t begin = __begins.front();

                    __errors.push_back(code_section_error_t {
                        __source_code + begin, (size_t)(2 + name.length()),
                        (int)compile_log_code_t::embedded_end_missing, name
                    });

                    __sections.resize(__section_counts.front());

                    callback(__source_code + __host_starts.front(), __source_code + begin,
                                                                        __lang_stack.front(), 0);

                    callback(__source_code + begin + 2 + name.length(), __source_code + __length,
                        __global_context.lang_factory->get_lang_id(name), 1, true);

                    return;
                }

                callback(__source_code + start, __source_code + __length, lang, 0);
                return;
            }

            if (marker_index == __begin_marker)
            {
                size_t name_begin = position + 2, name_end = name_begin;
                while (name_end < __length && !al::is_whitespace(__source_code[name_end]))
                    name_end++;

                string_t name(__source_code + name_begin, name_end - name_begin);
                lang_id_t embedded_lang = name.empty()? unknown_lang_id
                                            : __global_context.lang_factory->get_lang_id(name);

                // The language is not known: the flag is code of the language that has it.
                if (embedded_lang == unknown_lang_id)
                {
                    // Its code is skipped to its end flag, the language that has it goes on there.
                    // Without an end flag the reader of the elements tells the error.
                    size_t end_position, end_index;
                    if (__find_marker(lang, name_end, 1, end_position, end_index, __end_marker))
                    {
                        __errors.push_back(code_section_error_t {
                            __source_code + position, (size_t)(2 + name.length()),
                            (int)compile_log_code_t::unknown_embedded_language, name
                        });

                        callback(__source_code + start, __source_code + position, lang,
                                                                __lang_stack.size());
                        start = position = end_position + 2;
                    }
                    else
                    {
                        // No end flag: the error is told, and the code is skipped to the end of
                        // the statement (the first ;), the language that has it goes on there.
                        __errors.push_back(code_section_error_t {
                            __source_code + position, (size_t)(2 + name.length()),
                            (int)compile_log_code_t::unknown_embedded_language, name
                        });

                        size_t semicolon = name_end;
                        while (semicolon < __length && __source_code[semicolon] != _T(';'))
                            semicolon++;

                        callback(__source_code + start, __source_code + position, lang,
                                                                __lang_stack.size());
                        start = position = semicolon;
                    }

                    continue;
                }

                __host_starts.push_back(start);
                __section_counts.push_back(__sections.size());

                callback(__source_code + start, __source_code + position, lang,
                                                                __lang_stack.size());

                __lang_stack.push_back(lang);
                __lang_names.push_back(name);
                __begins.push_back(position);

                lang = embedded_lang;
                start = position = name_end;
            }
            else
            {
                callback(__source_code + start, __source_code + position, lang,
                                                                __lang_stack.size());

                lang = __lang_stack.back();
                __lang_stack.pop_back();
                __lang_names.pop_back();
                __begins.pop_back();
                __host_starts.pop_back();
                __section_counts.pop_back();

                start = position = position + 2;
            }
        }
    }

    ////////// ////////// ////////// ////////// //////////

    __char_properties_t __char_properties;

    // Initialize char properties.
    void char_property_t::init(enumerator_t & e)
    {
        set_flag(e, __char_flag_t::analyze_tree_token_end,
            '(', ')', '+', '*', '?', '|', ';', ' ', '\t', '\r', '\n', '\0'
        );
    }

    ////////// ////////// ////////// ////////// //////////

    // Searches method ref.
    ref_t __search_method_ref(__sctx_t & ctx, method_base_t * method)
    {
        ref_t ref = ref_t::null;

        // A static abstract (or virtual) member of an interface is used through a generic param
        // that has the interface as a constraint: its host is the param, not the interface.
        {
            decorate_t * decorate = method->get_decorate();
            type_t * host = method->get_host_type();

            if (decorate != nullptr && decorate->is_static
                    && (decorate->is_abstract || decorate->is_virtual)
                    && host != nullptr && host->this_ttype() == ttype_t::interface_)
            {
                throw _ECF(invalid_operation, _T("'%1%': a static abstract or virtual member of an "
                    "interface can be used only through a generic parameter that has the interface "
                    "as a constraint"), to_general(method)->to_prototype());
            }
        }

        switch (method->this_family())
        {
            case member_family_t::general: {
                method_t * m = (method_t *)method;
                ref = __is_extern(ctx, m)?
                    __ref_of<__tidx_t::method_ref>(ctx, m) :
                    __ref_of<__tidx_t::method>(ctx, m);
            }   break;

            case member_family_t::impl: {
                impl_method_t * m = (impl_method_t *)method;

                generic_method_t * generic_method;

                if (m->args.size() == 0 && m->host_args.size() > 0)
                {
                    // The method of a generic param takes the arguments of its interface, that
                    // are not those of the method.
                    generic_method = __XPool.new_obj<generic_method_t>(
                        m->raw, m->host_args, m->host_type
                    );
                }
                else
                {
                    generic_method = __XPool.new_generic_method(m->raw, m->args, m->host_type);
                }

                ref = __ref_of<__tidx_t::generic_method>(ctx, generic_method);
            }   break;

            case member_family_t::generic: {
                generic_method_t * m = (generic_method_t *)method;
                ref = __ref_of<__tidx_t::generic_method>(ctx, m);
            }   break;

            default:
                X_UNEXPECTED();
        }

        if (ref == ref_t::null)
            throw _ECF(not_found, _T("method ref of '%1%' not found"), method);

        return ref;
    }

    // Searches field ref.
    ref_t __search_field_ref(__sctx_t & ctx, field_t * field)
    {
        _A(field != nullptr);

        member_family_t family = field->this_family();

        if (family == member_family_t::impl)
            return __ref_of<__tidx_t::generic_field>(ctx, (impl_field_t *)field);

        if (family == member_family_t::position)
            return __ref_of<__tidx_t::position_field>(ctx, (position_field_t *)field);

        ref_t ref = __is_extern(ctx, field)?
            __ref_of<__tidx_t::field_ref>(ctx, field) :
            __ref_of<__tidx_t::field>(ctx, field);

        if (ref == ref_t::null)
            throw _ECF(not_found, _T("field ref of '%1%' not found"), field);

        return ref;
    }

    ////////// ////////// ////////// ////////// //////////

}   // namespace X_ROOT_NS::modules::compile

