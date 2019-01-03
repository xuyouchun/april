
#include "utils.h"
#include <algorithm.h>

namespace X_ROOT_NS { namespace modules { namespace compile {

    typedef compile_error_code_t         __e_t;
    typedef expression_compile_context_t __cctx_t;

    ////////// ////////// ////////// ////////// //////////

    code_section_builder_t::code_section_builder_t(global_context_t & global_context,
            const char_t * code, lang_id_t default_lang,
            std::vector<code_section_t *> & sections, pool_t & pool)
        : __global_context(global_context), __code(code), __p(code), __sections(sections)
        , __default_lang(default_lang), __pool(pool)
    {
        _A(code != nullptr);
    }

   void code_section_builder_t::build()
    {
        __build([this](const char_t * start, const char_t * end, lang_id_t lang, int depth) {
            this->__sections.push_back(
                __pool.new_obj<code_section_t>(__global_context, start, end - start, lang, depth)
            );
        });
    }

    template<typename callback_t>
    void code_section_builder_t::__build(callback_t callback)
    {
        const char_t * start = __p;

        lang_id_t lang = __default_lang;

        while(true)
        {
            char_t c = *__p++;
            switch(c)
            {
                case _T('<'):
                    if(*__p == _T('`'))
                    {
                        callback(start, __p - 1, lang, __lang_stack.size());
                        __lang_stack.push(lang);

                        __p++;
                        lang = __global_context.lang_id_of(__read_lang());
                        start = __p;
                    }
                    break;

                case _T('`'):
                    if(*__p == _T('>'))
                    {
                        if(__lang_stack.size() == 0)
                            throw _E(__e_t::format_error, _T("unexpected langage end flag \"?>\""));

                        callback(start, __p - 1, lang, __lang_stack.size());

                        lang = __lang_stack.top();
                        __lang_stack.pop();
                        start = ++__p;
                    }
                    break;

                case _T('"'): case _T('\''):
                    if(!__skip_char(c))
                        throw _E(__e_t::format_error);
                    break;

                case _T('\0'):
                    if(__lang_stack.size() > 0)
                        throw _E(__e_t::format_error, _T("langage end flag \"`>\" missing"));
                    callback(start, __p, lang, 0);
                    return;
            }
        }
    }

    bool code_section_builder_t::__skip_char(char_t c0)
    {
        char_t c;
        while((c = *__p++))
        {
            if(c == _T('\\'))
            {
                if(*__p++ == _T('\0'))
                    return false;
                continue;
            }

            if(c == c0)
                return true;
        }

        return false;
    }

    string_t code_section_builder_t::__read_lang()
    {
        if(al::is_whitespace(*__p))
            throw _E(__e_t::format_error, _T("language name error"));

        const char_t * start = __p++;
        while(true)
        {
            char_t c = *__p;
            if((c == _T('?') && *(__p + 1) == _T('>')) || al::is_whitespace(c))
                return string_t(start, __p++);
            __p++;
        }
    }

    ////////// ////////// ////////// ////////// //////////

    __char_properties_t __char_properties;

    void char_property_t::init(enumerator_t & e)
    {
        set_flag(e, __char_flag_t::analyze_tree_token_end,
            '(', ')', '+', '*', '?', '|', ';', ' ', '\t', '\r', '\n', '\0'
        );
    }

    ////////// ////////// ////////// ////////// //////////

    ref_t __search_method_ref(__sctx_t & ctx, method_t * method)
    {
        ref_t ref = __is_extern(ctx, method)?
            __ref_of<mt_table_index_t::method_ref>(ctx, method) :
            __ref_of<mt_table_index_t::method>(ctx, method);

        if(ref == ref_t::null)
            throw _ECF(not_found, _T("method ref of '%1%' not found"), method);

        return ref;
    }

    ref_t __search_field_ref(__sctx_t & ctx, field_t * field)
    {
        ref_t ref = __is_extern(ctx, field)?
            __ref_of<mt_table_index_t::field_ref>(ctx, field) :
            __ref_of<mt_table_index_t::field>(ctx, field);

        if(ref == ref_t::null)
            throw _ECF(not_found, _T("field ref of '%1%' not found"), field);

        return ref;
    }

    ////////// ////////// ////////// ////////// //////////

} } }
