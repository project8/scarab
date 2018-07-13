/*
 * test_param_value.cc
 *
 *  Created on: Jul 13, 2018
 *      Author: N.S. Oblath
 */

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
