#ifndef __COMPILE_ANALYZE_UTILS_H__
#define __COMPILE_ANALYZE_UTILS_H__

namespace X_ROOT_NS { namespace modules { namespace compile {

    namespace
    {
        using namespace core;
    }

    ////////// ////////// ////////// ////////// //////////

    // Code position.
    struct codepos_t
    {
        size_t line, col;

        bool is_empty() const
        {
            return line == 0 && col == 0;
        }
    };

    //-------- ---------- ---------- ---------- ----------

    // Code position helper.
    // Helps to find code position.
    class codepos_helper_t : public object_t
    {
    public:

        // Constructor.
        codepos_helper_t(const char_t * code, size_t length = max_value<size_t>())
            : __code(code), __length(length)
        {
            _A(code != nullptr);
        }

        codepos_t pos_of(const char_t * p);

    private:
        bool __initialized = false;
        const char_t * __code;
        const size_t __length;
        std::vector<const char_t *> __pos_arr;

        void __ensure_initialized();
    };

    ////////// ////////// ////////// ////////// //////////

    // Token pool.
    class token_pool_t : public object_t
    {
    public:

        // Constructor.
        token_pool_t();
        virtual ~token_pool_t() override;

        // Push a token.
        size_t push(token_t * token);

        // Returns the token at specified index.
        token_t * operator[](size_t index) const;

        // Returns pool size.
        size_t size() const { return __tokens.size(); }

        // Writes to a stream.
        template<typename stream_t>
        void write_to(stream_t & stream, size_t begin_index, size_t end_index = max_value<size_t>())
        {
            end_index = al::min(size(), end_index);

            for (size_t index = begin_index; index < end_index; index++)
            {
                if (index != begin_index)
                    stream << _T(" ");

                token_t * token = __tokens[index];
                stream << string_t(*token);
            }
        }

    private:
        std::vector<token_t *> __tokens;
    };

    ////////// ////////// ////////// ////////// //////////

    // Finds the element which match the predicate.
    template<typename itor_t, typename pred_t>
    bool walk(itor_t & p, pred_t pred)
    {
        char_t c;
        while ((c = *p) != _T('\0'))
        {
            if (!pred(p))
                return true;
            p++;
        }

        return false;
    }

    // Skips specified character.
    // Returns false if not found.
    template<typename itor_t>
    bool skip_char(itor_t & p, char_t c)
    {
        if (walk(p, [c](itor_t & p) { return *p != c; }))
        {
            p++;
            return true;
        }

        return false;
    }

    // Skips next whitespace.
    // Returns false if whitespace is not found.
    template<typename itor_t>
    bool skip_whitespace(itor_t & p)
    {
        return walk(p, [](itor_t & p) { return al::is_whitespace(*p); });
    }

    // Read next token.
    // Returns false if at the end of the sequence.
    template<typename itor_t>
    bool read_token(itor_t & p)
    {
        return walk(p, [](itor_t & p) { return al::is_word(*p); });
    }

} } }  // X_ROOT_NS::modules::compile


#endif  // __COMPILE_ANALYZE_UTILS_H__
