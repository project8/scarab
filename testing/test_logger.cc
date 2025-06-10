/*
 * test_logger.cc
 *
 *  Created on: Jan 4, 2016
 *      Author: N.S. Oblath
 */

#include "logger.hh"

#include "stream_check.hh"

#include "catch2/catch_test_macros.hpp"
#include "catch2/matchers/catch_matchers_all.hpp"

//using scarab::testing::compare_and_clear;
using scarab::logger;

TEST_CASE( "logger_buffer", "[logger]" )
{
    using Catch::Matchers::Equals;

    LOGGER( tlog, "test_logger" );
    REQUIRE( std::addressof(tlog) == std::addressof(tlog.ref()) );

    REQUIRE( tlog.buffer().empty() );

    tlog << "foo";
    REQUIRE_THAT( tlog.buffer(), Equals("foo") );

    tlog << "bar"; // buffer is replaced, not appended
    REQUIRE_THAT( tlog.buffer(), Equals("bar") );

}

TEST_CASE( "logger_output", "[logger]" )
{
    LOGGER( tlog, "test_logger" );

    REQUIRE_FALSE( logger::is_quill_stopped() );

    //std::stringstream* t_outstream = new std::stringstream();
    //scarab::logger::SetOutStream( t_outstream );

    //std::stringstream* t_errstream = new std::stringstream();
    //scarab::logger::SetErrStream( t_errstream );

    // all levels enabled
    logger::set_global_threshold( scarab::ELevel::eTrace );

    LTRACE( tlog, "test TRACE" );
    //REQUIRE( compare_and_clear( t_outstream, "test TRACE" ) );

    LDEBUG( tlog, "test DEBUG" );
    //REQUIRE( compare_and_clear( t_outstream, "test DEBUG" ) );

    LINFO( tlog, "test INFO" );
    //REQUIRE( compare_and_clear( t_outstream, "test INFO" ) );

    LPROG( tlog, "test PROG" );
    //REQUIRE( compare_and_clear( t_outstream, "test PROG" ) );

    LWARN( tlog, "test WARN" );
    //REQUIRE( compare_and_clear( t_errstream, "test WARN" ) );

    LERROR( tlog, "test ERROR" );
    //REQUIRE( compare_and_clear( t_errstream, "test ERROR" ) );

    LFATAL( tlog, "test FATAL" );
    //REQUIRE( compare_and_clear( t_errstream, "test FATAL" ) );


    // Info and above
    logger::set_global_threshold( scarab::ELevel::eInfo );

    LTRACE( tlog, "test TRACE" );
    //REQUIRE( compare_and_clear( t_outstream, "test TRACE" ) );

    LDEBUG( tlog, "test DEBUG" );
    //REQUIRE( compare_and_clear( t_outstream, "test DEBUG" ) );

    LINFO( tlog, "test INFO" );
    //REQUIRE( compare_and_clear( t_outstream, "test INFO" ) );

    LPROG( tlog, "test PROG" );
    //REQUIRE( compare_and_clear( t_outstream, "test PROG" ) );

    LWARN( tlog, "test WARN" );
    //REQUIRE( compare_and_clear( t_errstream, "test WARN" ) );

    LERROR( tlog, "test ERROR" );
    //REQUIRE( compare_and_clear( t_errstream, "test ERROR" ) );

    LFATAL( tlog, "test FATAL" );
    //REQUIRE( compare_and_clear( t_errstream, "test FATAL" ) );


    // Prog and above
    tlog.set_threshold( scarab::ELevel::eProg );

    LTRACE( tlog, "test TRACE" );
    //REQUIRE( compare_and_clear( t_outstream, "test TRACE" ) );

    LDEBUG( tlog, "test DEBUG" );
    //REQUIRE( compare_and_clear( t_outstream, "test DEBUG" ) );

    LINFO( tlog, "test INFO" );
    //REQUIRE( compare_and_clear( t_outstream, "test INFO" ) );

    LPROG( tlog, "test PROG" );
    //REQUIRE( compare_and_clear( t_outstream, "test PROG" ) );

    LWARN( tlog, "test WARN" );
    //REQUIRE( compare_and_clear( t_errstream, "test WARN" ) );

    LERROR( tlog, "test ERROR" );
    //REQUIRE( compare_and_clear( t_errstream, "test ERROR" ) );

    LFATAL( tlog, "test FATAL" );
    //REQUIRE( compare_and_clear( t_errstream, "test FATAL" ) );


    logger::stop_quill();

    REQUIRE( logger::is_quill_stopped() );

    // all levels enabled
    logger::set_global_threshold( scarab::ELevel::eTrace );

    LTRACE( tlog, "test TRACE" );
    //REQUIRE( compare_and_clear( t_outstream, "test TRACE" ) );

    LDEBUG( tlog, "test DEBUG" );
    //REQUIRE( compare_and_clear( t_outstream, "test DEBUG" ) );

    LINFO( tlog, "test INFO" );
    //REQUIRE( compare_and_clear( t_outstream, "test INFO" ) );

    LPROG( tlog, "test PROG" );
    //REQUIRE( compare_and_clear( t_outstream, "test PROG" ) );

    LWARN( tlog, "test WARN" );
    //REQUIRE( compare_and_clear( t_errstream, "test WARN" ) );

    LERROR( tlog, "test ERROR" );
    //REQUIRE( compare_and_clear( t_errstream, "test ERROR" ) );

    LFATAL( tlog, "test FATAL" );
    //REQUIRE( compare_and_clear( t_errstream, "test FATAL" ) );

}

TEST_CASE( "global_threshold", "[logger]" )
{
    LOGGER( tlog1, "test_logger_1" );
    LOGGER( tlog2, "test_logger_2" );

    logger::set_global_threshold( scarab::ELevel::eTrace );
    REQUIRE( tlog1.get_threshold() == scarab::ELevel::eTrace );
    REQUIRE( tlog2.get_threshold() == scarab::ELevel::eTrace );

    logger::set_global_threshold( scarab::ELevel::eProg );
    REQUIRE( tlog1.get_threshold() == scarab::ELevel::eProg );
    REQUIRE( tlog2.get_threshold() == scarab::ELevel::eProg );

    // individual setting only seen on the logger called
    tlog1.set_threshold( scarab::ELevel::eWarn );
    REQUIRE( tlog1.get_threshold() == scarab::ELevel::eWarn );
    REQUIRE( tlog2.get_threshold() == scarab::ELevel::eProg );

    // global threshold setting overrides individual setting
    logger::set_global_threshold( scarab::ELevel::eDebug );
    REQUIRE( tlog1.get_threshold() == scarab::ELevel::eDebug );
    REQUIRE( tlog2.get_threshold() == scarab::ELevel::eDebug );
}
