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

#include "logger.hh"

LOGGER( testlog, "test_param_node" )

using namespace scarab;

int main()
{
    LINFO( testlog, "Creating param node" );
    param_node node;

    LINFO( testlog, "Adding a value" );
    node.add( "five", 5 );

    LINFO( testlog, "Adding an array" );
    param_array subarray1;
    subarray1.push_back( 500 );
    node.add( "subarray1", subarray1 );

    param_array subarray2;
    subarray2.push_back( "5000" );
    node.add( "subarray2", std::move(subarray2) );

    LINFO( testlog, "Printing contents:" << node );

    LINFO( testlog, "After copy, should be full: " << subarray1 );
    LINFO( testlog, "After move, should be empty: " << subarray2 );

    LINFO( testlog, "Access:" );
    LINFO( testlog, node.get_value< int >("five") );
    LINFO( testlog, node["five"]() );
    LINFO( testlog, node["subarray1"][0]() );
    LINFO( testlog, node["subarray2"][0]() );

    return 0;
}



