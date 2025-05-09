/*
 * test_logger.cc
 *
 *  Created on: Jan 4, 2016
 *      Author: N.S. Oblath
 */

#include "logger.hh"

#include "stream_check.hh"

#include "catch2/catch_test_macros.hpp"

using scarab::testing::compare_and_clear;
using scarab::logger;

TEST_CASE( "logger", "[logger]" )
{
    LOGGER( tlog, "test_logger" );

    REQUIRE_FALSE( logger::is_quill_stopped() );
    REQUIRE(logger::s_log_trace_fcn == &logger::log_trace_to_quill);
    REQUIRE(logger::s_log_debug_fcn == &logger::log_debug_to_quill);
    REQUIRE(logger::s_log_info_fcn == &logger::log_info_to_quill);
    REQUIRE(logger::s_log_prog_fcn == &logger::log_prog_to_quill);
    REQUIRE(logger::s_log_warn_fcn == &logger::log_warn_to_quill);
    REQUIRE(logger::s_log_error_fcn == &logger::log_error_to_quill);
    REQUIRE(logger::s_log_fatal_fcn == &logger::log_fatal_to_quill);

    //std::stringstream* t_outstream = new std::stringstream();
    //scarab::logger::SetOutStream( t_outstream );

    //std::stringstream* t_errstream = new std::stringstream();
    //scarab::logger::SetErrStream( t_errstream );

    // all levels enabled
    //scarab::logger::SetGlobalLevel( scarab::logger::ELevel::eTrace );

    // log

    LPROG( tlog, "test PROG" );
    //REQUIRE( compare_and_clear( t_outstream, "test PROG" ) );

    LINFO( tlog, "test INFO" );
    //REQUIRE( compare_and_clear( t_outstream, "test INFO" ) );

    LDEBUG( tlog, "test DEBUG" );
    //REQUIRE( compare_and_clear( t_outstream, "test DEBUG" ) );

    LTRACE( tlog, "test TRACE" );
    //REQUIRE( compare_and_clear( t_outstream, "test TRACE" ) );

    // err

    LWARN( tlog, "test WARN" );
    //REQUIRE( compare_and_clear( t_errstream, "test WARN" ) );

    LERROR( tlog, "test ERROR" );
    //REQUIRE( compare_and_clear( t_errstream, "test ERROR" ) );

    LFATAL( tlog, "test FATAL" );
    //REQUIRE( compare_and_clear( t_errstream, "test FATAL" ) );

/*
    // Info and above
    //tlog.SetGlobalLevel( scarab::logger::ELevel::eInfo );

    // log

    LPROG( tlog, "test PROG" );
    //REQUIRE( compare_and_clear( t_outstream, "test PROG" ) );

    LINFO( tlog, "test INFO" );
    //REQUIRE( compare_and_clear( t_outstream, "test INFO" ) );

    LDEBUG( tlog, "test DEBUG" );
    //REQUIRE( compare_and_clear( t_outstream, "" ) );

    LTRACE( tlog, "test TRACE" );
    //REQUIRE( compare_and_clear( t_outstream, "" ) );

    // err

    LWARN( tlog, "test WARN" );
    //REQUIRE( compare_and_clear( t_errstream, "test WARN" ) );

    LERROR( tlog, "test ERROR" );
    //REQUIRE( compare_and_clear( t_errstream, "test ERROR" ) );

    LFATAL( tlog, "test FATAL" );
    //REQUIRE( compare_and_clear( t_errstream, "test FATAL" ) );
*/

    logger::stop_quill();

    REQUIRE( logger::is_quill_stopped() );
    REQUIRE(logger::s_log_trace_fcn == &logger::log_trace_to_stdout);
    REQUIRE(logger::s_log_debug_fcn == &logger::log_debug_to_stdout);
    REQUIRE(logger::s_log_info_fcn == &logger::log_info_to_stdout);
    REQUIRE(logger::s_log_prog_fcn == &logger::log_prog_to_stdout);
    REQUIRE(logger::s_log_warn_fcn == &logger::log_warn_to_stdout);
    REQUIRE(logger::s_log_error_fcn == &logger::log_error_to_stdout);
    REQUIRE(logger::s_log_fatal_fcn == &logger::log_fatal_to_stdout);

    LPROG( tlog, "test PROG" );
    //REQUIRE( compare_and_clear( t_outstream, "test PROG" ) );

    LINFO( tlog, "test INFO" );
    //REQUIRE( compare_and_clear( t_outstream, "test INFO" ) );

    LDEBUG( tlog, "test DEBUG" );
    //REQUIRE( compare_and_clear( t_outstream, "test DEBUG" ) );

    LTRACE( tlog, "test TRACE" );
    //REQUIRE( compare_and_clear( t_outstream, "test TRACE" ) );

    // err

    LWARN( tlog, "test WARN" );
    //REQUIRE( compare_and_clear( t_errstream, "test WARN" ) );

    LERROR( tlog, "test ERROR" );
    //REQUIRE( compare_and_clear( t_errstream, "test ERROR" ) );

    LFATAL( tlog, "test FATAL" );
    //REQUIRE( compare_and_clear( t_errstream, "test FATAL" ) );

}
