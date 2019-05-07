/*
 * test_param_by_pointer.cc
 *
 *  Created on: Jul 18, 2018
 *      Author: N.S. Oblath
 *
 *  Output should be something like:
2018-07-18 10:57:26 [ INFO] (tid 0x7fff9c9dc380) am_by_pointer.cc(18): Building node
2018-07-18 10:57:26 [ INFO] (tid 0x7fff9c9dc380) am_by_pointer.cc(34):
{
    eight : 8.000000
    subarray :
    [
        80
        800
    ]

}
 *
 */

#define SCARAB_API_EXPORTS

#include "param.hh"

#include "logger.hh"

LOGGER( testlog, "test_param_by_pointer" )

using namespace scarab;

int main()
{
    LINFO( testlog, "Building node" );
    param_ptr_t param_0( new param_node() );

    param_ptr_t param_1( new param_value(8.0) );
    param_0->as_node().add( "eight", std::move(param_1) );

    param_ptr_t param_2( new param_array() );

    param_ptr_t param_3( new param_value(80) );
    param_2->as_array().push_back( std::move(param_3) );

    param_ptr_t param_4( new param_value(800) );
    param_2->as_array().push_back( std::move(param_4) );

    param_0->as_node().add( "subarray", std::move(param_2) );

    LINFO( testlog, *param_0 );

    return 0;
}



