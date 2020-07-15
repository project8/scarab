/*
 * test_param_by_pointer.cc
 *
 *  Created on: Jul 18, 2018
 *      Author: N.S. Oblath
 *
 */

#include "param.hh"

#include "catch.hpp"

using scarab::param_ptr_t;
using scarab::param_array;
using scarab::param_value;
using scarab::param_node;

TEST_CASE( "param_by_pointer", "[param]" )
{
    param_ptr_t param_0( new param_node() );
    REQUIRE( param_0 );
    REQUIRE( param_0->is_node() );

    param_ptr_t param_1( new param_value(8.0) );
    REQUIRE( param_1->is_value() );
    param_0->as_node().add( "eight", std::move(param_1) );
    REQUIRE_FALSE( param_1 );
    REQUIRE_THAT( param_0->as_node()["eight"]().as_double(), Catch::WithinULP(8., 10) );

    param_ptr_t param_2( new param_array() );
    REQUIRE( param_2->is_array() );

    param_ptr_t param_3( new param_value(80) );
    param_2->as_array().push_back( std::move(param_3) );

    param_ptr_t param_4( new param_value(800) );
    param_2->as_array().push_back( std::move(param_4) );

    param_0->as_node().add( "subarray", std::move(param_2) );
    REQUIRE( param_0->as_node().has( "subarray" ) );
    REQUIRE( param_0->as_node()["subarray"].is_array() );
    REQUIRE( param_0->as_node()["subarray"].as_array().size() == 2 );

    REQUIRE_FALSE( param_2 );
}
