/*
 * expression.cc
 *
 *  Created on: Jan 18, 2021
 *      Author: N.S. Oblath
 */

#define SCARAB_API_EXPORTS

#include "expression.hh"

#include "param_node.hh"
#include "param_value.hh"

namespace scarab
{
    // eval_error

    const char* what() const
    {
        std::string t_message( "Expression parse error" )
        if( ! base_exception::f_error.empty() )
        {
            t_message += ": " + f_error;
        }
        if( ! f_expr.empty() )
        {
            t_message += "\n\t" + f_expr + "\n\t" + std::string( f_pos - 1, ' ' ) + '^';
        }
        return t_message.c_str();
    }

    // interpret()

    double interpret( const std::string& an_expr )
    {
        int t_error = 0;

        double t_answer = te_interp( an_expr.c_str(), &t_error );

        if( std::isnan(t_answer) || t_error != 0 )
        {
            throw eval_error( an_expr, t_error ) << "Error during expression interpretation";
        }
        return t_answer;
    }

    double interpret( const param_value& an_expr )
    {
        return interpret( an_expr.as_string() );
    }

    // expression

    expression::expression( const std::string& an_expr, const param_node& a_vars ) :
            f_expr( an_expr ),
            f_vars(),
            f_comp_expr( nullptr )
    {
        int t_error = 0;

        // deal with variables
        for( auto var_it = a_vars.begin(); var_it != a_vars.end(); ++var_it )
        {
            // create a te_variable with vector::emplace()
            // (*var_it)().value() gets the low-level access to the boost::variant behind the param_value; this will throw scarab::error if the param object is not a param_value
            // 
            f_vars.emplace_back( te_variable{ var_it->name(), boost::get<double>((*var_it)().value()) } );
        }

        f_comp_expr = te_compile( f_expr, f_vars.data(), f_vars.size(), &t_error );

        if( f_comp_expr == nullptr || t_error == 0 )
        {
            throw eval_error( f_expr, t_error ) << "Error compiling expression";
        }
    }

    expression::expression( const param_value& an_expr, const param_node& a_vars ) : 
            expression( an_expr.as_string(), a_vars )
    {}

    expression::~expression()
    {
        if( f_comp_expr != 0 ) te_free( f_comp_expr );
    }


}
