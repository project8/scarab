/*
 * test_param_value.cc
 *
 *  Created on: Jul 13, 2018
 *      Author: N.S. Oblath
 *
 */

#include "param_value.hh"

#include "catch.hpp"

using scarab::param_value;

TEST_CASE( "param_value", "[param]" )
{
    SECTION( "initialization" )
    {
        // boolean
        param_value bool_val( true );
        REQUIRE( bool_val.as_bool() );
        REQUIRE( bool_val.as_string() == "true" );

        bool_val.set( false );
        REQUIRE_FALSE( bool_val.as_bool() );
        REQUIRE( bool_val.as_string() == "false" );

        bool_val = true;
        REQUIRE( bool_val.as_bool() );

        // integer
        param_value int_val( 5 );
        REQUIRE( int_val.as_int() == 5 );
        REQUIRE( int_val.as_string() == "5" );
        REQUIRE_THAT( int_val.as_double(), Catch::WithinULP( 5., 10 ) );

        int_val = 10;
        REQUIRE( int_val.as_int() == 10 );

        // string
        param_value string_val( "hello world" );
        REQUIRE( string_val.as_string() == "hello world" );

        string_val = "oh, hai!";
        REQUIRE( string_val.as_string() == "oh, hai!" );

        string_val.set( "abracadabra" );
        REQUIRE( string_val.as_string() == "abracadabra" );

        // single-quote stripping
        param_value string_val_sq( "'hello world'" );
        REQUIRE( string_val_sq.as_string() == "hello world" );

        string_val.set( "'hello world'" );
        REQUIRE( string_val.as_string() == "hello world" );

        // string to int
        string_val.set( "10" );
        REQUIRE( string_val.as_string() == "10" );
        REQUIRE( string_val.as_int() == 10 );

        // string to bool
        string_val.set( "true" );
        REQUIRE( string_val.as_bool() );

        string_val.set( "false" );
        REQUIRE_FALSE( string_val.as_bool() );
    }


    SECTION( "quotation_options" )
    {
        param_value quote_opts_val( 1 );
        REQUIRE( quote_opts_val.as_string() == "1" );
        REQUIRE( quote_opts_val.as_int() == 1 );
        REQUIRE_FALSE( quote_opts_val.is_uint() );
        REQUIRE( quote_opts_val.is_int() );
        REQUIRE_FALSE( quote_opts_val.is_string() );

        quote_opts_val.set( "1" );
        REQUIRE( quote_opts_val.as_string() == "1" );
        REQUIRE( quote_opts_val.as_int() == 1 );
        REQUIRE_FALSE( quote_opts_val.is_uint() );
        REQUIRE_FALSE( quote_opts_val.is_int() );
        REQUIRE( quote_opts_val.is_string() );

        // Initializes as a char in integer form
        quote_opts_val.set( '1' );
        REQUIRE( quote_opts_val.as_string() == "49" );
        REQUIRE( quote_opts_val.as_int() == 49 );
        REQUIRE_FALSE( quote_opts_val.is_uint() );
        REQUIRE( quote_opts_val.is_int() );
        REQUIRE_FALSE( quote_opts_val.is_string() );

        quote_opts_val.set( "'1'" );
        REQUIRE( quote_opts_val.as_string() == "1" );
        REQUIRE( quote_opts_val.as_int() == 1 );
        REQUIRE_FALSE( quote_opts_val.is_uint() );
        REQUIRE_FALSE( quote_opts_val.is_int() );
        REQUIRE( quote_opts_val.is_string() );

        quote_opts_val.set( "\"'1'\"" );
        REQUIRE( quote_opts_val.as_string() == "\"'1'\"" );
        REQUIRE_FALSE( quote_opts_val.is_uint() );
        REQUIRE_FALSE( quote_opts_val.is_int() );
        REQUIRE( quote_opts_val.is_string() );

        quote_opts_val.set( "''1''" );
        REQUIRE( quote_opts_val.as_string() == "'1'" );
        REQUIRE_FALSE( quote_opts_val.is_uint() );
        REQUIRE_FALSE( quote_opts_val.is_int() );
        REQUIRE( quote_opts_val.is_string() );

        quote_opts_val.set( "'\'1\''" );
        REQUIRE( quote_opts_val.as_string() == "'1'" );
        REQUIRE_FALSE( quote_opts_val.is_uint() );
        REQUIRE_FALSE( quote_opts_val.is_int() );
        REQUIRE( quote_opts_val.is_string() );
    }

    SECTION( "strict_equality" )
    {
        param_value bool_val( true );
        param_value bool_val_2( false );
        REQUIRE_FALSE( bool_val.strict_is_equal_to(bool_val_2) );
        REQUIRE_FALSE( bool_val_2.strict_is_equal_to(bool_val) );
        bool_val_2 = true;
        REQUIRE( bool_val.strict_is_equal_to(bool_val_2) );
        REQUIRE( bool_val_2.strict_is_equal_to(bool_val) );

        param_value string_val( "hello" );
        param_value string_val_2( "hello" );
        REQUIRE( string_val.strict_is_equal_to(string_val_2) );
        string_val_2 = "world";
        REQUIRE_FALSE( string_val.strict_is_equal_to(string_val_2) );

        bool_val = true;
        param_value int_val( 1 );
        REQUIRE_FALSE( bool_val.strict_is_equal_to(int_val) );
        param_value uint_val( 1U );
        REQUIRE_FALSE( int_val.strict_is_equal_to(uint_val) );
        string_val = "true";
        REQUIRE_FALSE( bool_val.strict_is_equal_to(string_val) );

        param_value double_val( 1. );
        param_value double_val_2( 1. );
        REQUIRE( double_val.strict_is_equal_to(double_val_2) );
        double_val_2 = 2.;
        REQUIRE_FALSE( double_val.strict_is_equal_to(double_val_2) );
        int_val = 1;
        REQUIRE_FALSE( double_val.strict_is_equal_to(int_val) );
    }

    SECTION( "loose_equality" )
    {
        param_value bool_val( true );
        param_value bool_val_2( false );
        REQUIRE_FALSE( bool_val.loose_is_equal_to(bool_val_2) );
        bool_val_2 = true;
        REQUIRE( bool_val.loose_is_equal_to(bool_val_2) );

        param_value string_val( "hello" );
        param_value string_val_2( "hello" );
        REQUIRE( string_val.loose_is_equal_to(string_val_2) );
        string_val_2 = "world";
        REQUIRE_FALSE( string_val.loose_is_equal_to(string_val_2) );

        param_value int_val( 1 );
        REQUIRE_FALSE( bool_val.loose_is_equal_to(int_val) );
        param_value uint_val( 1U );
        REQUIRE( uint_val.loose_is_equal_to(int_val) );
        string_val = "true";
        REQUIRE( bool_val.loose_is_equal_to(string_val) );

        param_value double_val( 1. );
        param_value double_val_2( 1. );
        REQUIRE( double_val.loose_is_equal_to(double_val_2) );
        double_val_2 = 2.;
        REQUIRE_FALSE( double_val.loose_is_equal_to(double_val_2) );
        REQUIRE_FALSE( double_val.loose_is_equal_to(int_val) );
    }
}

