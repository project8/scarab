/*
 * test_signal_handler.cc
 *
 *  Created on: Jul 17, 2020
 *      Author: N.S. Oblath
 */

#include "signal_handler.hh"

#include "cancelable.hh"
#include "logger.hh"
#include "stream_check.hh"

#include "catch.hpp"

#include <signal.h>

using scarab::testing::compare_and_clear;


TEST_CASE( "signal_handler", "[utility]" )
{
    scarab::signal_handler t_handler;

    SECTION( "handling_and_printing" )
    {
        REQUIRE( scarab::signal_handler::get_handling_sig_abrt() );
        REQUIRE( scarab::signal_handler::get_handling_sig_term() );
        REQUIRE( scarab::signal_handler::get_handling_sig_int() );
    #ifndef _WIN32
        REQUIRE( scarab::signal_handler::get_handling_sig_quit() );
    #endif

        REQUIRE_NOTHROW( scarab::signal_handler::print_current_exception() );
        REQUIRE_NOTHROW( scarab::signal_handler::print_stack_trace() );

        try
        {
            throw std::runtime_error( "test error" );
        }
        catch(const std::runtime_error& e)
        {
            REQUIRE_NOTHROW( scarab::signal_handler::print_current_exception() );

            std::stringstream* t_errstream = new std::stringstream();
            scarab::logger::SetErrStream( t_errstream );

            scarab::signal_handler::print_current_exception();
            REQUIRE( compare_and_clear( t_errstream, "test error" ) );

            scarab::logger::SetErrStream( &std::cerr );

        }
    }

    scarab::cancelable t_cancel;
    REQUIRE_FALSE( t_cancel.is_canceled() );
    t_handler.add_cancelable( &t_cancel );

    SECTION( "canceling" )
    {
        t_handler.cancel_all( 0 );
        REQUIRE( t_cancel.is_canceled() );
    }

    SECTION( "terminating" )
    {
        t_handler.terminate( 0 );
        REQUIRE( t_cancel.is_canceled() );
    }

    SECTION( "raise_sigabrt" )
    {
        raise( SIGABRT );
        REQUIRE( t_cancel.is_canceled() );
        REQUIRE( scarab::signal_handler::get_return_code() == RETURN_ERROR );
    }

    SECTION( "raise_sigterm" )
    {
        raise( SIGTERM );
        REQUIRE( t_cancel.is_canceled() );
        REQUIRE( scarab::signal_handler::get_return_code() == RETURN_ERROR );
    }

    SECTION( "raise_sigint" )
    {
        raise( SIGINT );
        REQUIRE( t_cancel.is_canceled() );
        REQUIRE( scarab::signal_handler::get_return_code() == RETURN_SUCCESS );
    }

#ifndef _WIN32
    SECTION( "raise_sigquit" )
    {
        raise( SIGQUIT );
        REQUIRE( t_cancel.is_canceled() );
        REQUIRE( scarab::signal_handler::get_return_code() == RETURN_SUCCESS );
    }
#endif

}
