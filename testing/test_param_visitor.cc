/*
 * test_param_visitor.cc
 *
 *  Created on: Jul 13, 2018
 *      Author: N.S. Oblath
 */

#include "param_visitor.hh"

#include "logger.hh"
#include "param.hh"
#include "param_helpers_impl.hh"

#include "catch.hpp"

LOGGER( tlog, "test_param_visitor" )

using namespace scarab;

class param_visitor_callback_tester : public param_visitor_callback
{
    public:
        param_visitor_callback_tester() :
                param_visitor_callback(),
                f_param_count( 0 ),
                f_param_array_count( 0 ),
                f_param_node_count( 0 ),
                f_param_value_count( 0 )
        {
            f_param_callback = [this](const param&) {
                LDEBUG( tlog, "param callback" );
                this->set_param_count( this->get_param_count() + 1 );
            };
            f_param_array_callback = [this](const param_array& an_array) {
                LDEBUG( tlog, "param_array callback: " << an_array );
                this->set_param_array_count( this->get_param_array_count() + 1 );
                for( const auto& it: an_array )
                {
                    it.accept( *this );
                }
            };
            f_param_node_callback = [this](const param_node& a_node) {
                LDEBUG( tlog, "param_node callback: " << a_node );
                this->set_param_node_count( this->get_param_node_count() + 1 );
                for( const auto& it: a_node )
                {
                    it.accept( *this );
                }
            };
            f_param_value_callback = [this](const param_value& a_value) {
                LDEBUG( tlog, "param_value callback: " << a_value );
                this->set_param_value_count( this->get_param_value_count() + 1 );
            };
        }
        mv_accessible( int, param_count );
        mv_accessible( int, param_array_count );
        mv_accessible( int, param_node_count );
        mv_accessible( int, param_value_count );
};

TEST_CASE( "param_visitor", "[param]" )
{
    param_node test_nested(  
        "null"_a=param(),
        "one"_a=1,
        "array"_a=param_array( args(
            5, 
            5.0, 
            "five", 
            param_array( args(5) ), 
            param_node("five"_a=5)
        )),
        "node"_a=param_node(
            "one-thousand"_a=1000
        )
    );

    param_visitor_callback_tester t_visitor;

    test_nested.accept( t_visitor );

    REQUIRE( t_visitor.get_param_count() == 1 );
    REQUIRE( t_visitor.get_param_array_count() == 2 );
    REQUIRE( t_visitor.get_param_node_count() == 3 );
    REQUIRE( t_visitor.get_param_value_count() == 7 );

}

