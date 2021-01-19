/*
 * expression.hh
 *
 *  Created on: Jan 18, 2021
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_PARAM_EXPRESSION_HH_
#define SCARAB_PARAM_EXPERSSION_HH_

#include "base_exception.hh"

#include "tinyexpr.h"

#include <vector>

namespace scarab
{
    class param_node;
    class param_value;

    class SCARAB_API eval_error : public base_exception< error >
    {
        public:
            error( const std::string& an_expr, unsigned a_pos_plus_one ) : 
                    f_expr( an_expr ), f_pos(a_pos_plus_one - 1) 
            {}
            error( const error& ) = default;
            ~error() noexcept = default;

            const char* what() const noexcept;

            const std::string& expr() const { return f_expr; }
            unsigned pos() const { return f_pos; }

        protected:
            std::string f_expr;
            unsigned f_pos;
    };


    /// Directly evaluate an expression from a string
    /// Throws an eval_error on any error
    double SCARAB_API interpret( const std::string& an_expr );

    /// Directly evaluate an expression from a param_value::as_string()
    /// Throws an eval_error on any error
    double SCARAB_API interpret( const param_value& an_expr );

    /*!
     @class expression
     @author N.S. Oblath

     @brief Mathematical expression evaluation

     @details

     */
    class SCARAB_API expression()
    {
        public:
            expression( const std::string& an_expr, const param_node& a_vars );
            expression( const param_value& an_expr, const param_node& a_vars );
            virtual ~expression();

            double eval();

            const std::string& expr() const { return f_expr; }

        protected:
            std::string f_expr;
            std::vector< te_variable > f_vars;

            te_expr* f_comp_expr;
    }

    inline double expression::eval()
    {
        return te_eval( f_comp_expr );
    }

} /* namespace scarab */

#endif /* SCARAB_PARAM_EXPERSSION_HH_ */
