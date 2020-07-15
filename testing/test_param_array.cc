/*
 * test_param_array.cc
 *
 *  Created on: Jul 17, 2018
 *      Author: N.S. Oblath
 *
 *  Output should be something like:
 *
2018-07-18 01:33:38 [ INFO] (tid 0x7fff9c9dc380) t_param_array.cc(54): Creating param array
2018-07-18 01:33:38 [ INFO] (tid 0x7fff9c9dc380) t_param_array.cc(57): Adding a value
2018-07-18 01:33:38 [ INFO] (tid 0x7fff9c9dc380) t_param_array.cc(60): Adding an array
2018-07-18 01:33:38 [ INFO] (tid 0x7fff9c9dc380) t_param_array.cc(69): Printing contents:
[
    5

    {
        five-hundred : 500
    }


    [
        5000
    ]

]

2018-07-18 01:33:38 [ INFO] (tid 0x7fff9c9dc380) t_param_array.cc(71): After move, should be full:
{
    five-hundred : 500
}

2018-07-18 01:33:38 [ INFO] (tid 0x7fff9c9dc380) t_param_array.cc(72): After move, should be empty:
[
]

2018-07-18 01:33:38 [ INFO] (tid 0x7fff9c9dc380) t_param_array.cc(74): Access:
2018-07-18 01:33:38 [ INFO] (tid 0x7fff9c9dc380) t_param_array.cc(75): 5
2018-07-18 01:33:38 [ INFO] (tid 0x7fff9c9dc380) t_param_array.cc(76): 5
2018-07-18 01:33:38 [ INFO] (tid 0x7fff9c9dc380) t_param_array.cc(77): 500
2018-07-18 01:33:38 [ INFO] (tid 0x7fff9c9dc380) t_param_array.cc(78): 5000
 *
 */

#include "param.hh"

#include "error.hh"

#include "catch.hpp"

using scarab::param_array;
using scarab::param_node;
using scarab::param_value;
using scarab::param_ptr_t;

TEST_CASE( "param_array", "[param]" )
{
    param_array array;
    REQUIRE( array.empty() );
    REQUIRE( array.size() == 0 );

    array.push_back( 5 );
    REQUIRE_FALSE( array.empty() );
    REQUIRE( array.size() == 1 );

    param_node subnode;
    subnode.add( "five-hundred", 500 );
    array.push_back( subnode );
    REQUIRE( array.size() == 2 );
    REQUIRE_FALSE( subnode.empty() );

    param_array subarray;
    subarray.push_back( "5000" );
    array.push_back( std::move(subarray) );
    REQUIRE( array.size() == 3 );
    REQUIRE( subarray.empty() );

    // access
    REQUIRE( array[0]().as_int() == 5 );
    REQUIRE( array.get_value(0, 99999) == 5 );
    REQUIRE( array.get_value(100, 99999) == 99999 );
    REQUIRE_THROWS_AS( array[1](), scarab::error );
    REQUIRE_THROWS_AS( array[2](), scarab::error );
    REQUIRE( array[1]["five-hundred"]().as_int() == 500 );
    REQUIRE( array[2][0]().as_int() == 5000 );

    // front and back
    REQUIRE( array.front()().as_int() == 5 );
    REQUIRE( array.back()[0]().as_int() == 5000 );

    // assign
    param_value to_assign( 10 );
    array.assign( 0, to_assign );
    REQUIRE( array[0]().as_int() == 10 );
    REQUIRE( to_assign.as_int() == 10 );

    to_assign = 15;
    array.assign( 0, std::move(to_assign ) );
    REQUIRE( array[0]().as_int() == 15 );
    REQUIRE_FALSE( to_assign.as_int() == 0 );

    array.assign( 0, 20 );
    REQUIRE( array[0]().as_int() == 20 );

    // append and merge
    param_array to_append;
    to_append.push_back( -1 );
    array.append( to_append );
    REQUIRE( array.size() == 4 );
    REQUIRE( array[3]().as_int() == -1 );

    param_array to_merge;
    to_merge.push_back( -2 );
    array.merge( to_merge );
    REQUIRE( array.size() == 4 );
    REQUIRE( array[0]().as_int() == -2 );

    // iterator, begin, and end
    param_array::iterator it = array.begin();
    unsigned count = 0;
    for( ; it != array.end(); ++it, ++count );
    REQUIRE( count == array.size() );

    // remove, erase and clear
    param_ptr_t removed = array.remove( 0 );
    REQUIRE( removed->is_value() );
    REQUIRE( removed->as_value().as_int() == -2 );

    array.erase( 1 );

    REQUIRE( array.size() == 4 );

    array.clear();
    REQUIRE( array.size() == 0 );

}


