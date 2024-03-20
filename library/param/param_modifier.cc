/*
 * param_modifier.cc
 *
 *  Created on: Mar 20, 2024
 *      Author: N.S. Oblath
 */

#define SCARAB_API_EXPORTS

#include "param_modifier.hh"

#include "param.hh"

#include "logger.hh"

LOGGER( plog, "param_modifier" );

namespace scarab
{
        param_modifier::param_modifier() {}
        param_modifier::~param_modifier() {}


        param_modifier_callback::param_modifier_callback() :
                param_modifier(),
                f_param_callback( [](param&) {
                    LDEBUG( plog, "param callback" );
                }),
                f_param_array_callback( [this](param_array& an_array) {
                    LDEBUG( plog, "param_array callback: " << an_array );
                    for( auto& it: an_array )
                    {
                        it.accept( *this );
                    }
                }),
                f_param_node_callback( [this](param_node& a_node) {
                    LDEBUG( plog, "param_node callback: " << a_node );
                    for( auto& it: a_node )
                    {
                        it.accept( *this );
                    }
                }),
                f_param_value_callback( [](param_value& a_value) {
                    LDEBUG( plog, "param_value callback: " << a_value );
                })
        {}

        param_modifier_callback::~param_modifier_callback() 
        {}

        void param_modifier_callback::operator()( param& a_null )
        {
            f_param_callback( a_null );
            return;
        };

        void param_modifier_callback::operator()( param_array& an_array )
        {
            f_param_array_callback( an_array );
            return;
        };

        void param_modifier_callback::operator()( param_node& a_node )
        {
            f_param_node_callback( a_node );
            return;
        };

        void param_modifier_callback::operator()( param_value& a_value )
        {
            f_param_value_callback( a_value );
            return;
        };

};
