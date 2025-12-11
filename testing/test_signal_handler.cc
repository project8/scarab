/*
 * test_signal_handler.cc
 *
 *  Created on: Jul 17, 2020
 *      Author: N.S. Oblath
 * 
 *  Things we cannot test here due to the way in which catch2 is setup:
 *  - Signal catching
 *  - Termination
 * 
 */

#include "signal_handler.hh"

#include "cancelable.hh"
#include "logger.hh"
#include "stream_check.hh"

#include "catch2/catch_test_macros.hpp"

#include <thread>

using scarab::testing::compare_and_clear;


TEST_CASE( "signal_handler", "[utility]" )
{
    LOGGER( tlog, "test_signal_handler" );

    // Thread safety note
    // From: https://github.com/catchorg/Catch2/blob/devel/docs/thread-safety.md
    // Assertion macros are thread safe.
    // Section macros, generator macros, and test-case macros are not thread safe.
    //   So starting a test case or section and then spawning a thread inside is ok;
    //   Having multiple threads running in different sections or test cases in parallel is not ok.
    // Calling REQUIRE in a spawned thread is dangerous because if it is false, the exception thrown will not be caught properly, bringing down the whole process.
    //   Instead, call CHECK in a thread, which will record the proper failures/successes, but not crash the program.

    SECTION( "handling" )
    {
        REQUIRE( scarab::signal_handler::get_ref_count() == 0 );
        REQUIRE_FALSE( scarab::signal_handler::is_handling() );
        REQUIRE_FALSE( scarab::signal_handler::is_waiting() );
        REQUIRE( scarab::signal_handler::get_signal_received() == 0 );

        scarab::signal_handler t_handler;
        REQUIRE( scarab::signal_handler::get_ref_count() == 1 );
        REQUIRE( scarab::signal_handler::is_handling() );
        REQUIRE_FALSE( scarab::signal_handler::is_waiting() );

        t_handler.unhandle_signals();
        REQUIRE_FALSE( scarab::signal_handler::is_handling() );
    }

    SECTION( "handle while in scope" )
    {
        REQUIRE( scarab::signal_handler::get_ref_count() == 0 );
        REQUIRE_FALSE( scarab::signal_handler::is_handling() );

        {
            scarab::signal_handler t_handler;
            REQUIRE( scarab::signal_handler::get_ref_count() == 1 );
            REQUIRE( scarab::signal_handler::is_handling() );
        } // t_handler destructed, which should unhandle signals

        REQUIRE( scarab::signal_handler::get_ref_count() == 0 );
        REQUIRE_FALSE( scarab::signal_handler::is_handling() );

    }

    SECTION( "wait then abort" )
    {
        scarab::signal_handler t_handler;
        REQUIRE( scarab::signal_handler::is_handling() );
        REQUIRE_FALSE( scarab::signal_handler::is_waiting() );

        // start handling threads
        std::thread t_handler_thread( [](){ scarab::signal_handler::wait_for_signals(); } );
        std::this_thread::sleep_for( std::chrono::seconds(1) );
        REQUIRE( scarab::signal_handler::is_waiting() );

        scarab::signal_handler::abort_wait();
        t_handler_thread.join();
        REQUIRE( scarab::signal_handler::get_signal_received() == -1 );
        REQUIRE( scarab::signal_handler::is_handling() );
        REQUIRE_FALSE( scarab::signal_handler::is_waiting() );
    }

/*
    SECTION( "printing" )
    {
        REQUIRE_NOTHROW( scarab::signal_handler::print_current_exception( true ) );
        REQUIRE_NOTHROW( scarab::signal_handler::print_stack_trace( true ) );

        try
        {
            throw std::runtime_error( "test error" );
        }
        catch(const std::runtime_error& e)
        {
            REQUIRE_NOTHROW( scarab::signal_handler::print_current_exception( true ) );

            std::stringstream* t_errstream = new std::stringstream();
            scarab::logger::SetErrStream( t_errstream );

            scarab::signal_handler::print_current_exception( true );
            REQUIRE( compare_and_clear( t_errstream, "test error" ) );

            scarab::logger::SetErrStream( &std::cerr );

        }
    }
*/
    scarab::signal_handler t_handler;

    scarab::cancelable t_cancel;
    REQUIRE_FALSE( t_cancel.is_canceled() );

    // we use cancelable_wrapper to wrap t_cancel in a shared_ptr
    auto t_cwrap = scarab::wrap_cancelable( t_cancel );
    t_handler.add_cancelable( t_cwrap );

    SECTION( "canceling" )
    {
        t_handler.cancel_all( 0 );
        REQUIRE( t_cancel.is_canceled() );
    }

    SECTION( "terminating" )
    {
        t_handler.exit( 0 );
        REQUIRE( t_cancel.is_canceled() );
    }

    // delete the cancelable_wrapper, which removes it from the signal_handler
    t_cwrap.reset();

    SECTION( "not canceling" )
    {
        t_handler.cancel_all( 0 );
        REQUIRE_FALSE( t_cancel.is_canceled() );
    }

}
