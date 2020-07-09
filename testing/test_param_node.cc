/*
 * test_param_node.cc
 *
 *  Created on: Jul 13, 2018
 *      Author: N.S. Oblath
 *
 *  Output should be something like:
 *
2018-07-13 15:53:19 [ INFO] (tid 0x7fff9c9dc380) st_param_node.cc(18): Creating param node
2018-07-13 15:53:19 [ INFO] (tid 0x7fff9c9dc380) st_param_node.cc(21): Adding a value
2018-07-13 15:53:19 [ INFO] (tid 0x7fff9c9dc380) st_param_node.cc(24): Adding an array
2018-07-13 15:53:19 [ INFO] (tid 0x7fff9c9dc380) st_param_node.cc(33): Printing contents:
{
    five : 5
    subarray1 :
    [
        500
    ]

    subarray2 :
    [
        5000
    ]

}

2018-07-13 15:53:19 [ INFO] (tid 0x7fff9c9dc380) st_param_node.cc(35): After copy, should be full:
[
    500
]

2018-07-13 15:53:19 [ INFO] (tid 0x7fff9c9dc380) st_param_node.cc(36): After move, should be empty:
[
]

2018-07-13 15:53:19 [ INFO] (tid 0x7fff9c9dc380) st_param_node.cc(38): Access:
2018-07-13 15:53:19 [ INFO] (tid 0x7fff9c9dc380) st_param_node.cc(39): 5
2018-07-13 15:53:19 [ INFO] (tid 0x7fff9c9dc380) st_param_node.cc(40): 5
2018-07-13 15:53:19 [ INFO] (tid 0x7fff9c9dc380) st_param_node.cc(41): 500
2018-07-13 15:53:19 [ INFO] (tid 0x7fff9c9dc380) st_param_node.cc(42): 5000
 *
 */

#include "param.hh"

#include "catch.hpp"

#include <type_traits>
#include <iostream>

using scarab::param;
using scarab::param_array;
using scarab::param_node;
using scarab::param_value;
using scarab::param_ptr_t;

TEST_CASE( "param_node", "[param]" )
{
    param_node node;
    REQUIRE( node.empty() );
    REQUIRE( node.size() == 0 );

    node.add( "five", 5 );
    REQUIRE_FALSE( node.empty() );
    REQUIRE( node.size() == 1 );

    param_array subarray1;
    subarray1.push_back( 500 );
    node.add( "subarray1", subarray1 );
    REQUIRE( node.size() == 2 );
    REQUIRE_FALSE( subarray1.empty() );

    param_array subarray2;
    subarray2.push_back( "5000" );
    node.add( "subarray2", std::move(subarray2) );
    REQUIRE( node.size() == 3 );
    REQUIRE( subarray2.empty() );

    // has and count
    REQUIRE( node.has("five") );
    REQUIRE_FALSE( node.has("ten") );
    REQUIRE( node.count("five") == 1 );
    REQUIRE( node.count("ten") == 0 );

    // access
    REQUIRE( node["five"]().as_int() == 5 );
    REQUIRE( node.get_value("five", 99999) == 5 );
    REQUIRE( node.get_value("ten", 99999) == 99999 );
    REQUIRE_THROWS_AS( node["subarray1"](), scarab::error );
    REQUIRE_THROWS_AS( node["subarray2"](), scarab::error );
    REQUIRE( node["subarray1"][0]().as_int() == 500 );
    REQUIRE( node["subarray2"][0]().as_int() == 5000 );

    // add and replace
    REQUIRE( node.add("ten", 10) );
    REQUIRE_FALSE( node.add("five", -5) );
    REQUIRE_NOTHROW( node.replace("ten", -10) );
    REQUIRE( node.has("ten") );
    REQUIRE( node["ten"]().as_int() == -10 );

    // merge
    param_node to_merge;
    to_merge.add( "minus-two", -2 );
    node.merge( to_merge );
    REQUIRE( node.size() == 5 );
    REQUIRE( node["minus-two"]().as_int() == -2 );

    // iterator, begin, and end
    param_node::iterator it = node.begin();
    unsigned count = 0;
    for( ; it != node.end(); ++it, ++count );
    REQUIRE( count == node.size() );

    // remove, erase, and clear
    param_ptr_t removed = node.remove( "ten" );
    REQUIRE( removed->is_value() );
    REQUIRE( removed->as_value().as_int() == -10 );

    node.erase( "minus-two" );
    REQUIRE( node.size() == 3 );

    node.clear();
    REQUIRE( node.size() == 0 );

}

