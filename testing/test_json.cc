/*
 * test_json.cc
 *
 *  Created on: Jul 18, 2018
 *      Author: N.S. Oblath
 *
 */

#include "param.hh"
#include "param_json.hh"

#include "catch.hpp"

#include <iostream>


using scarab::param_node;
using scarab::param_value;
using scarab::param_array;
using scarab::param_ptr_t;
using scarab::param_output_json;
using scarab::param_input_json;

TEST_CASE( "json", "[param][param_codec]" )
{
    param_ptr_t t_input( new param_node() );
    param_node& t_input_node = t_input->as_node();

    t_input_node.add( "string", "hello, world" );
    
    param_array t_input_array;
    t_input_array.push_back( 10 );
    t_input_array.push_back( "I'm in an array!" );

    param_node t_input_nested_node;
    t_input_nested_node.add( "first", "abcdefg" );
    t_input_nested_node.add( "second", "hijklmnop" );
    t_input_nested_node.add( "third", "qrstuv" );
    t_input_nested_node.add( "fourth", "wxyz" );

    t_input_node.add( "array", std::move(t_input_array) );
    t_input_node.add( "node", std::move(t_input_nested_node) );

    param_output_json t_writer;
    std::string t_written;
    t_writer.write_string( *t_input, t_written );

    param_input_json t_reader;
    param_ptr_t t_output( t_reader.read_string( t_written ) );

    REQUIRE( t_output->is_node() );
    param_node& t_output_node = t_input->as_node();

    REQUIRE( t_output_node.size() == 3 );

    REQUIRE( t_output_node.has("string") );
    REQUIRE( t_output_node["string"].is_value() );
    REQUIRE( t_output_node["string"]().as_string() == "hello, world" );

    REQUIRE( t_output_node.has("array") );
    REQUIRE( t_output_node["array"].is_array() );
    REQUIRE( t_output_node["array"].as_array().size() == 2 );
    REQUIRE( t_output_node["array"][0].is_value() );
    REQUIRE( t_output_node["array"][0]().as_uint() == 10 );
    REQUIRE( t_output_node["array"][1].is_value() );
    REQUIRE( t_output_node["array"][1]().as_string() == "I'm in an array!" );

    REQUIRE( t_output_node.has("node") );
    REQUIRE( t_output_node["node"].is_node() );
    REQUIRE( t_output_node["node"].as_node().size() == 4 );
    REQUIRE( t_output_node["node"]["first"]().as_string() == "abcdefg" );
    REQUIRE( t_output_node["node"]["second"].is_value() );
    REQUIRE( t_output_node["node"]["second"]().as_string() == "hijklmnop" );
}
