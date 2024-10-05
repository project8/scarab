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
        param_visitor::param_visitor() 
        {}

        param_visitor::~param_visitor() 
        {}

        void param_visitor::operator()( const param& ) const
        {
            return;
        }

        void param_visitor::operator()( const param_array& an_array ) const
        {
            for( const auto& it: an_array )
            {
                it.accept( *this );
            }
            return;
        }

        void param_visitor::operator()( const param_node& a_node ) const
        {
            for( const auto& it: a_node )
            {
                it.accept( *this );
            }
        }

        void param_visitor::operator()( const param_value& ) const
        {
            return;
        }


        param_visitor_callback::param_visitor_callback() :
                param_visitor(),
                f_param_callback( [](const param&) {
                    LDEBUG( plog, "param callback" );
                }),
                f_param_array_callback( [this](const param_array& an_array) {
                    LDEBUG( plog, "param_array callback: " << an_array );
                    for( const auto& it: an_array )
                    {
                        it.accept( *this );
                    }
                }),
                f_param_node_callback( [this](const param_node& a_node) {
                    LDEBUG( plog, "param_node callback: " << a_node );
                    for( const auto& it: a_node )
                    {
                        it.accept( *this );
                    }
                }),
                f_param_value_callback( [](const param_value& a_value) {
                    LDEBUG( plog, "param_value callback: " << a_value );
                })
        {}

        param_visitor_callback::~param_visitor_callback() 
        {}

        void param_visitor_callback::operator()( const param& a_null ) const
        {
            f_param_callback( a_null );
            return;
        };

        void param_visitor_callback::operator()( const param_array& an_array ) const
        {
            f_param_array_callback( an_array );
            return;
        };

        void param_visitor_callback::operator()( const param_node& a_node ) const
        {
            f_param_node_callback( a_node );
            return;
        };

        void param_visitor_callback::operator()( const param_value& a_value ) const
        {
            f_param_value_callback( a_value );
            return;
        };

};
