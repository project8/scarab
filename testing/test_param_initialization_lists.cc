/*
 * test_param_array.cc
 *
 *  Created on: Jul 17, 2018
 *      Author: N.S. Oblath
 * 
 */


#include "param.hh"

#include "error.hh"

#include "catch.hpp"

#include <iostream>

using scarab::param;
using scarab::param_array;
using scarab::param_node;
using scarab::param_value;
using scarab::param_ptr_t;
using scarab::ni;

TEST_CASE( "param_init_lists", "[param]" )
{
    param::v tv(50);
    param_ptr_t tv2_ptr( new param::v(500) );
    param_array test_array( 1, 2.2, "hello", tv, tv2_ptr );
    std::cout << test_array << std::endl;
    REQUIRE( test_array.size() == 5 );
    REQUIRE( test_array[0]().is_int() );
    REQUIRE( test_array[0]().as_int() == 1 );
    REQUIRE( test_array[1]().as_double() == 2.2 );
    REQUIRE( test_array[2]().as_string() == "hello" );
    REQUIRE( test_array[3]().as_int() == 50 );
    REQUIRE( test_array[4]().as_int() == 500 );

    param_node test_node( scarab::ni("one", 1), scarab::ni("two-point-two", 2.2), scarab::ni("hello", "hello") );
    REQUIRE( test_node.size() == 3 );
    REQUIRE( test_node["one"]().as_int() == 1 );
    REQUIRE( test_node["two-point-two"]().as_double() == 2.2 );
    REQUIRE( test_node["hello"]().as_string() == "hello" );

    param_node test_nested(  
        ni("null", param()),
        ni("one", 1),
        ni("array", param_array(
            5, 
            5.0, 
            "five", 
            param_array(5), 
            param_node(ni("five", 5))
        )),
        ni("node", param_node(
            ni("one-thousand", 1000)
        ))
    );
    REQUIRE( test_nested.size() == 4 );
    REQUIRE( test_nested["null"].is_null() );
    REQUIRE( test_nested["one"].is_value() );
    REQUIRE( test_nested["array"].is_array() );
    REQUIRE( test_nested["node"].is_node() );
    REQUIRE( test_nested["array"].as_array().size() == 5 );
    REQUIRE( test_nested["array"][0].is_value() );
    REQUIRE( test_nested["array"][3][0].is_value() );
    REQUIRE( test_nested["array"][3][0]().as_int() == 5 );
    REQUIRE( test_nested["array"][4].is_node() );
    REQUIRE( test_nested["node"].is_node() );
    REQUIRE( test_nested["node"]["one-thousand"]().as_int() == 1000 );
}