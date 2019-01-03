
#include <compile.h>

namespace X_ROOT_NS { namespace modules { namespace compile {

    using namespace core;

    ////////// ////////// ////////// ////////// //////////

    codepos_t codepos_helper_t::pos_of(const char_t * p)
    {
        _A(p != nullptr);

        __ensure_initialized();

        if(p < __pos_arr[0] || p >= __pos_arr[__pos_arr.size() -1])
            return codepos_t { 0, 0 };

        for(size_t start = 0, end = __pos_arr.size() - 2; start <= end; )
        {
            size_t index = (start + end) / 2;
            if(p < __pos_arr[index])
            {
                end = index - 1;
            }
            else if(p >= __pos_arr[index + 1])
            {
                start = index + 1;
            }
            else  // found
            {
                return codepos_t { index + 1, (size_t)(p - __pos_arr[index]) + 1 };
            }
        }

        return codepos_t { 0, 0 };
    }

    void codepos_helper_t::__ensure_initialized()
    {
        if(__initialized)
            return;

        __pos_arr.push_back(__code);

        const char_t * p = __code;
        const char_t * p_end = p + __length < p?
                (const char_t *)max_value<size_t>() : p + __length;

        for(; *p && p < p_end; p++)
        {
            if(*p == _T('\n'))
            {
                __pos_arr.push_back(p + 1);
            }
        }

        __pos_arr.push_back(p);

        __initialized = true;
    }

    ////////// ////////// ////////// ////////// //////////

    token_pool_t::token_pool_t()
    {

    }

    token_pool_t::~token_pool_t()
    {

    }

    size_t token_pool_t::push(token_t * token)
    {
        _A(token != nullptr);

        __tokens.push_back(token);
        return __tokens.size() - 1;
    }

    token_t * token_pool_t::operator[](size_t index) const
    {
        _A(index < __tokens.size());
        return __tokens[index];
    }

    ////////// ////////// ////////// ////////// //////////

} } }  // X_ROOT_NS::modules::compile
