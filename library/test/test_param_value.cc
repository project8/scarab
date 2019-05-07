/*
 * test_param_value.cc
 *
 *  Created on: Jul 13, 2018
 *      Author: N.S. Oblath
 *
 *  Output should be something like this:
 *
2018-07-13 15:25:14 [ INFO] (tid 0x7fff9c9dc380) t_param_value.cc(18): Bool Value Tests
2018-07-13 15:25:14 [ INFO] (tid 0x7fff9c9dc380) t_param_value.cc(21): Bool value via string: true
2018-07-13 15:25:14 [ INFO] (tid 0x7fff9c9dc380) t_param_value.cc(23): Int Value Tests
2018-07-13 15:25:14 [ INFO] (tid 0x7fff9c9dc380) t_param_value.cc(27): Int value via int: 5
2018-07-13 15:25:14 [ INFO] (tid 0x7fff9c9dc380) t_param_value.cc(28): Int value via string: 5
2018-07-13 15:25:14 [ INFO] (tid 0x7fff9c9dc380) t_param_value.cc(30): String Value Tests
2018-07-13 15:25:14 [ INFO] (tid 0x7fff9c9dc380) t_param_value.cc(32): String value: hello world
2018-07-13 15:25:14 [ INFO] (tid 0x7fff9c9dc380) t_param_value.cc(35): String containing a number via as_int: 10
2018-07-13 15:25:14 [ INFO] (tid 0x7fff9c9dc380) t_param_value.cc(38): String containing a bool via as_bool: 1
 *
 */

#define SCARAB_API_EXPORTS

#include "param_value.hh"

#include "logger.hh"

LOGGER( testlog, "test_param_value" )

using namespace scarab;

int main()
{
    LINFO( testlog, "Bool Value Tests" );
    param_value bool_val( true );

    LINFO( testlog, "Bool value via string: " << bool_val );

    LINFO( testlog, "Int Value Tests" );
    param_value int_val( 5 );
    int val_out = int_val.as_int();

    LINFO( testlog, "Int value via int: " << val_out );
    LINFO( testlog, "Int value via string: " << int_val );

    LINFO( testlog, "String Value Tests" );
    param_value string_val( "hello world" );
    LINFO( testlog, "String value: " << string_val );

    string_val.set( "10" );
    LINFO( testlog, "String containing a number via as_int: " << string_val.as_int() );

    string_val.set( "true" );
    LINFO( testlog, "String containing a bool via as_bool: " << string_val.as_bool() );

    return 0;
}
