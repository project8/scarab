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

#include "logger.hh"

LOGGER( testlog, "test_param_array" )

using namespace scarab;

int main()
{
    LINFO( testlog, "Creating param array" );
    param_array array;

    LINFO( testlog, "Adding a value" );
    array.push_back( 5 );

    LINFO( testlog, "Adding an array" );
    param_node subnode;
    subnode.add( "five-hundred", 500 );
    array.push_back( subnode );

    param_array subarray;
    subarray.push_back( "5000" );
    array.push_back( std::move(subarray) );

    LINFO( testlog, "Printing contents:" << array );

    LINFO( testlog, "After copy, should be full: " << subnode );
    LINFO( testlog, "After move, should be empty: " << subarray );

    LINFO( testlog, "Access:" );
    LINFO( testlog, array.get_value< int >(0) );
    LINFO( testlog, array[0]() );
    LINFO( testlog, array[1]["five-hundred"]() );
    LINFO( testlog, array[2].get_value( 0, "value doesn't exist" ) );

    return 0;
}



