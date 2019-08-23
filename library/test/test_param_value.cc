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

#include "param_value.hh"

#include "logger.hh"

LOGGER( testlog, "test_param_value" )

using namespace scarab;

int main()
{
    LINFO( testlog, "Bool Value Tests" );
    LINFO( testlog, "================" );

    param_value bool_val( true );

    LINFO( testlog, "Bool value via string: " << bool_val );

    LINFO( testlog, "Int Value Tests" );
    LINFO( testlog, "===============" );

    param_value int_val( 5 );
    int val_out = int_val.as_int();

    LINFO( testlog, "Int value via int: " << val_out );
    LINFO( testlog, "Int value via string: " << int_val );

    LINFO( testlog, "String Value Tests" );
    LINFO( testlog, "==================" );

    param_value string_val( "hello world" );
    LINFO( testlog, "String value: " << string_val );

    string_val.set( "10" );
    LINFO( testlog, "String containing a number via as_int: " << string_val.as_int() );

    string_val.set( "true" );
    LINFO( testlog, "String containing a bool via as_bool: " << string_val.as_bool() );

    LINFO( testlog, "Strict Equality Tests" );
    LINFO( testlog, "=====================" );

    bool_val = true;
    param_value bool_val_2( false );
    LINFO( testlog, bool_val << "(bool) == " << bool_val_2 << "(bool) ?: " << bool_val.strict_is_equal_to(bool_val_2) );
    bool_val_2 = true;
    LINFO( testlog, bool_val << "(bool) == " << bool_val_2 << "(bool) ?: " << bool_val.strict_is_equal_to(bool_val_2) );

    string_val = "hello";
    param_value string_val_2( "hello" );
    LINFO( testlog, string_val << "(string) == " << string_val_2 << "(string) ?: " << string_val.strict_is_equal_to(string_val_2) );
    string_val_2 = "world";
    LINFO( testlog, string_val << "(string) == " << string_val_2 << "(string) ?: " << string_val.strict_is_equal_to(string_val_2) );

    int_val = 1;
    LINFO( testlog, bool_val << "(bool) == " << int_val << "(int) ?: " << bool_val.strict_is_equal_to(int_val) );
    param_value uint_val( 1U );
    LINFO( testlog, uint_val << "(uint) == " << int_val << "(int) ?: " << uint_val.strict_is_equal_to(int_val) );
    string_val = "true";
    LINFO( testlog, bool_val << "(bool) == " << string_val << "(string) ?: " << bool_val.strict_is_equal_to(string_val) );

    param_value double_val( 1. );
    param_value double_val_2( 1. );
    LINFO( testlog, double_val << "(double) == " << double_val_2 << "(double) ?: " << double_val.strict_is_equal_to(double_val_2) );
    double_val_2 = 2.;
    LINFO( testlog, double_val << "(double) == " << double_val_2 << "(double) ?: " << double_val.strict_is_equal_to(double_val_2) );
    LINFO( testlog, double_val << "(double) == " << int_val << "(int) ?: " << double_val.strict_is_equal_to(int_val) );


    LINFO( testlog, "Loose Equality Tests" );
    LINFO( testlog, "====================" );

    bool_val = true;
    bool_val_2 = false;
    LINFO( testlog, bool_val << "(bool) == " << bool_val_2 << "(bool) ?: " << bool_val.loose_is_equal_to(bool_val_2) );
    bool_val_2 = true;
    LINFO( testlog, bool_val << "(bool) == " << bool_val_2 << "(bool) ?: " << bool_val.loose_is_equal_to(bool_val_2) );

    string_val = "hello";
    string_val_2 = "hello";
    LINFO( testlog, string_val << "(string) == " << string_val_2 << "(string) ?: " << string_val.loose_is_equal_to(string_val_2) );
    string_val_2 = "world";
    LINFO( testlog, string_val << "(string) == " << string_val_2 << "(string) ?: " << string_val.loose_is_equal_to(string_val_2) );

    int_val = 1;
    LINFO( testlog, bool_val << "(bool) == " << int_val << "(int) ?: " << bool_val.loose_is_equal_to(int_val) );
    uint_val = 1U;
    LINFO( testlog, uint_val << "(uint) == " << int_val << "(int) ?: " << uint_val.loose_is_equal_to(int_val) );
    string_val = "true";
    LINFO( testlog, bool_val << "(bool) == " << string_val << "(string) ?: " << bool_val.loose_is_equal_to(string_val) );

    double_val = 1.;
    double_val_2 = 1.;
    LINFO( testlog, double_val << "(double) == " << double_val_2 << "(double) ?: " << double_val.loose_is_equal_to(double_val_2) );
    double_val_2 = 2.;
    LINFO( testlog, double_val << "(double) == " << double_val_2 << "(double) ?: " << double_val.loose_is_equal_to(double_val_2) );
    LINFO( testlog, double_val << "(double) == " << int_val << "(int) ?: " << double_val.loose_is_equal_to(int_val) );

    return 0;
}
