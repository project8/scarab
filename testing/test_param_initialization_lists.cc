/*
 * test_param_array.cc
 *
 *  Created on: Jul 17, 2018
 *      Author: N.S. Oblath
 * 
 */


#include "param.hh"
#include "param_helpers_impl.hh"

#include "error.hh"

#include "catch.hpp"

#include <iostream>

using namespace scarab;

TEST_CASE( "param_init_lists", "[param]" )
{
    param::v tv(50);
    param_ptr_t tv2_ptr( new param::v(500) );
    param_array test_array( pa_args(1, 2.2, "hello", tv, tv2_ptr) );
    std::cout << test_array << std::endl;
    REQUIRE( test_array.size() == 5 );
    REQUIRE( test_array[0]().is_int() );
    REQUIRE( test_array[0]().as_int() == 1 );
    REQUIRE( test_array[1]().as_double() == 2.2 );
    REQUIRE( test_array[2]().as_string() == "hello" );
    REQUIRE( test_array[3]().as_int() == 50 );
    REQUIRE( test_array[4]().as_int() == 500 );

//    param_node test_node( scarab::ni("one", 1), scarab::ni("two-point-two", 2.2), scarab::ni("hello", "hello") );
//    std::cerr << test_node << std::endl;
//    REQUIRE( test_node.size() == 3 );
//    REQUIRE( test_node["one"]().as_int() == 1 );
//    REQUIRE( test_node["two-point-two"]().as_double() == 2.2 );
//    REQUIRE( test_node["hello"]().as_string() == "hello" );

    param_node test_node_2( named_arg("one")=1, named_arg("two-point-two")=2.2, named_arg("hello")="hello" );
    std::cout << test_node_2 << std::endl;
    REQUIRE( test_node_2.size() == 3 );
    REQUIRE( test_node_2["one"]().as_int() == 1 );
    REQUIRE( test_node_2["two-point-two"]().as_double() == 2.2 );
    REQUIRE( test_node_2["hello"]().as_string() == "hello" );

    param_node test_nested(  
        "null"_a=param(),
        "one"_a=1,
        "array"_a=param_array( pa_args(
            5, 
            5.0, 
            "five", 
            param_array( pa_args(5) ), 
            param_node("five"_a=5)
        )),
        named_arg("node")=param_node(
            "one-thousand"_a=1000
        )
    );
    std::cout << test_nested << std::endl;
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