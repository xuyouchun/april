#ifndef __LANG_CS_EXPRESSIONS_H__
#define __LANG_CS_EXPRESSIONS_H__

#include "services.h"

namespace X_ROOT_NS { namespace modules { namespace lang_cs {

    namespace
    {
        using namespace core;
        using namespace compile;
    }

    ////////// ////////// ////////// ////////// //////////

    class cs_condition_expression_t : public texpression_t<3>
    {
        typedef texpression_t<3> __super_t;

    public:
        using __super_t::__super_t;

        virtual expression_family_t this_family() const override
        {
            return expression_family_t::__unknown__;
        }

        virtual vtype_t get_vtype() const override;
        virtual type_t * get_type() const override;

        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool,
                                        xil_type_t dtype) override;
        virtual cvalue_t execute(expression_execute_context_t & ctx) override;

        expression_t * condition() const { return exps[0]; }
        expression_t * value1()    const { return exps[1]; }
        expression_t * value2()    const { return exps[2]; }

        virtual const string_t to_string() const override
        {
            return _F(_T("(%1%? %2% : %3%)"), condition(), value1(), value2());
        }
    };

    ////////// ////////// ////////// ////////// //////////

    class cs_expressions_t : public expressions_t
    {
        typedef expressions_t __super_t;

    public:
        using __super_t::__super_t;

        virtual void compile(expression_compile_context_t & ctx, xil_pool_t & pool,
                                        xil_type_t dtype) override;
        virtual cvalue_t execute(expression_execute_context_t & ctx) override;
    };

    ////////// ////////// ////////// ////////// //////////

} } }

#endif // __LANG_CS_EXPRESSIONS_H__
