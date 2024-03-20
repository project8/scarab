/*
 * param_visitor.cc
 *
 *  Created on: Mar 19, 2024
 *      Author: N.S. Oblath
 */

#define SCARAB_API_EXPORTS

#include "param_visitor.hh"

#include "param.hh"

#include "logger.hh"

LOGGER( plog, "param_visitor" );

namespace scarab
{
        param_visitor::param_visitor() {}
        param_visitor::~param_visitor() {}


        param_visitor_callback::param_visitor_callback() :
                param_visitor(),
                f_param_callback( [](const param&) {
                    LDEBUG( plog, "param callback" );
                }),
                f_param_array_callback( [](const param_array& an_array) {
                    LDEBUG( plog, "Param callback: " << an_array );
                }),
                f_param_node_callback( [](const param_node& a_node) {
                    LDEBUG( plog, "param_node callback: " << a_node );
                }),
                f_param_value_callback( [](const param_value& a_value) {
                    LDEBUG( plog, "param_value callback: " << a_value );
                })
        {}

        param_visitor_callback::~param_visitor_callback() 
        {}

        void param_visitor_callback::operator()( const param& a_null )
        {
            f_param_callback( a_null );
            return;
        };

        void param_visitor_callback::operator()( const param_array& an_array )
        {
            f_param_array_callback( an_array );
            return;
        };

        void param_visitor_callback::operator()( const param_node& a_node )
        {
            f_param_node_callback( a_node );
            return;
        };

        void param_visitor_callback::operator()( const param_value& a_value )
        {
            f_param_value_callback( a_value );
            return;
        };

};
