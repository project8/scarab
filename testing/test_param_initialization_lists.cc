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

TEST_CASE( "param_init_lists", "[param]" )
{
    param_array test_array( {param::v(1), param::v(2.2), param::v("hello")} );
    test_array.push_front( 50, 500 );
    std::cout << test_array << std::endl;
    REQUIRE( test_array.size() == 4 );
    REQUIRE( test_array[0]().is_int() );
    REQUIRE( test_array[0]().as_int() == 50 );
    REQUIRE( test_array[1]().as_double() == 2.2 );
    REQUIRE( test_array[2]().as_string() == "hello" );

    param_node test_node( { {"one", param::v(1)}, {"two-point-two", param::v(2.2)}, {"hello", param::v("hello")} } );
    REQUIRE( test_node.size() == 3 );
    REQUIRE( test_node["one"]().as_int() == 1 );
    REQUIRE( test_node["two-point-two"]().as_double() == 2.2 );
    REQUIRE( test_node["hello"]().as_string() == "hello" );

    param_node test_nested( { 
        {"null", param()},
        {"one", param::v(1)},
        {"array", param_array({
            param::v(5), param::v(5.0), param::v("five"), param_array({param::v(5)}), param_node({"five", param::v(5)})
        })},
        {"node", param_node({
            {"one-thousand", param::v(1000)}
        })}
    } );
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