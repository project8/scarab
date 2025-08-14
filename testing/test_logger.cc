/*
 * test_logger.cc
 *
 *  Created on: Jan 4, 2016
 *      Author: N.S. Oblath
 */

#include "logger.hh"

#include "stream_check.hh"

#include "spdlog/async.h"

#include "catch2/catch_test_macros.hpp"

using scarab::testing::compare_and_clear;
using scarab::logger;

TEST_CASE( "logger", "[logger]" )
{
    LOGGER( tlog, "test_logger" );

    REQUIRE( logger::using_spd_async() );

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

}

TEST_CASE( "local_logger", "[logger]" )
{
    LOCAL_LOGGER( tlog, "test_logger" );

    REQUIRE( logger::using_spd_async() );

    LPROG( tlog, "test PROG" );
    LINFO( tlog, "test INFO" );
    LDEBUG( tlog, "test DEBUG" );
    LTRACE( tlog, "test TRACE" );
    LWARN( tlog, "test WARN" );
    LERROR( tlog, "test ERROR" );
    LFATAL( tlog, "test FATAL" );
}

TEST_CASE( "async_logging_stop", "[logger]" )
{
    LOGGER( tlog, "test_logger" );

    REQUIRE( logger::using_spd_async() );
    std::shared_ptr< spdlog::async_logger > t_async = std::dynamic_pointer_cast<spdlog::async_logger>( tlog.spdlogger() );
    REQUIRE( t_async );

    REQUIRE_NOTHROW( logger::stop_using_spd_async() );
    t_async = std::dynamic_pointer_cast<spdlog::async_logger>( tlog.spdlogger() );
    REQUIRE( ! t_async );

    REQUIRE_NOTHROW( logger::reset_using_spd_async() );
    t_async = std::dynamic_pointer_cast<spdlog::async_logger>( tlog.spdlogger() );
    REQUIRE( t_async );

}