/*
 * test_param_modifier.cc
 *
 *  Created on: Mar 20, 2024
 *      Author: N.S. Oblath
 */

#include "test_param_modifiers_visitors.hh"

#include "param_helpers_impl.hh"

#include "catch.hpp"

LOGGER( tlog, "test_param_modifier" )

using namespace scarab;
using namespace scarab_testing;

TEST_CASE( "param_modifier", "[param]" )
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
    param_modifier_callback_tester t_modifier;

    test_nested.accept( t_visitor );
    test_nested.accept( t_modifier );
    test_nested.accept( t_visitor );

    REQUIRE( t_modifier.get_param_count() == 1 );
    REQUIRE( t_modifier.get_param_array_count() == 2 );
    REQUIRE( t_modifier.get_param_node_count() == 3 );
    REQUIRE( t_modifier.get_param_value_count() == 7 );

}

