/*
 * test_param_visitor.cc
 *
 *  Created on: Mar 20, 2024
 *      Author: N.S. Oblath
 */

#include "test_param_modifiers_visitors.hh"

#include "param_visitor.hh"

#include "param.hh"
#include "param_helpers_impl.hh"

#include "catch2/catch_test_macros.hpp"

using namespace scarab;
using namespace scarab_testing;

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

