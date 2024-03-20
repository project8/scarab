/*
 * test_param_visitor.cc
 *
 *  Created on: Jul 13, 2018
 *      Author: N.S. Oblath
 */

#include "param_visitor.hh"

#include "param.hh"
#include "param_helpers_impl.hh"

#include "catch.hpp"

using namespace scarab;

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

    REQUIRE( test_nested.is_node() );


}

