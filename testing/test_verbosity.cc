/*
 * test_verbosity.cc
 *
 *  Created on: Mar 31, 2020
 *      Author: N.S. Oblath
 */

#include "application.hh"
#include "logger.hh"

#include "catch2/catch_test_macros.hpp"


TEST_CASE( "verbosity_via_app", "[cli][logger]" )
{
    LOGGER( tlog, "test_verbosity" );

    scarab::main_app t_app;

    // test default verbosity
    REQUIRE( scarab::logger::get_global_threshold() == scarab::ELevel::eProg );

    // test increasing verbosity
    t_app.increase_global_verbosity( 1 );
    REQUIRE( scarab::logger::get_global_threshold() == scarab::ELevel::eInfo );

    // test increasing past the end
    t_app.increase_global_verbosity( 10 );
    REQUIRE( scarab::logger::get_global_threshold() == scarab::ELevel::eTrace );

    // test decreasing past the beginning
    t_app.decrease_global_verbosity( 100 );
    REQUIRE( scarab::logger::get_global_threshold() == scarab::ELevel::eFatal );

    // test setting verbosity
    t_app.set_global_verbosity( 6 );
    REQUIRE( scarab::logger::get_global_threshold() == scarab::ELevel::eWarn );

    // test decreasing past the beginning
    t_app.set_global_verbosity( 100 );
    REQUIRE( scarab::logger::get_global_threshold() == scarab::ELevel::eFatal );

}

TEST_CASE( "verbosity_via_logger", "[logger]" )
{
    LOGGER( tlog, "test_verbosity" );

    scarab::logger::set_global_threshold( scarab::ELevel::eTrace );
    LFATAL( tlog, "test FATAL" );
    LERROR( tlog, "test ERROR" );
    LWARN( tlog, "test WARN" );
    LPROG( tlog, "test PROG" );
    LINFO( tlog, "test INFO" );
    LDEBUG( tlog, "test DEBUG" );
    LTRACE( tlog, "test TRACE" );

    scarab::logger::set_global_threshold( scarab::ELevel::eDebug );
    LFATAL( tlog, "test FATAL" );
    LERROR( tlog, "test ERROR" );
    LWARN( tlog, "test WARN" );
    LPROG( tlog, "test PROG" );
    LINFO( tlog, "test INFO" );
    LDEBUG( tlog, "test DEBUG" );
    LTRACE( tlog, "test TRACE" );

    scarab::logger::set_global_threshold( scarab::ELevel::eInfo );
    LFATAL( tlog, "test FATAL" );
    LERROR( tlog, "test ERROR" );
    LWARN( tlog, "test WARN" );
    LPROG( tlog, "test PROG" );
    LINFO( tlog, "test INFO" );
    LDEBUG( tlog, "test DEBUG" );
    LTRACE( tlog, "test TRACE" );

    scarab::logger::set_global_threshold( scarab::ELevel::eProg );
    LFATAL( tlog, "test FATAL" );
    LERROR( tlog, "test ERROR" );
    LWARN( tlog, "test WARN" );
    LPROG( tlog, "test PROG" );
    LINFO( tlog, "test INFO" );
    LDEBUG( tlog, "test DEBUG" );
    LTRACE( tlog, "test TRACE" );

    scarab::logger::set_global_threshold( scarab::ELevel::eWarn );
    LFATAL( tlog, "test FATAL" );
    LERROR( tlog, "test ERROR" );
    LWARN( tlog, "test WARN" );
    LPROG( tlog, "test PROG" );
    LINFO( tlog, "test INFO" );
    LDEBUG( tlog, "test DEBUG" );
    LTRACE( tlog, "test TRACE" );
}

/*
using namespace scarab;

LOGGER( tlog, "test_verbosity" );

int main()
{
    main_app t_app;

    // test default verbosity
    LPROG( tlog, "Verbosity should be PROG (" << static_cast<unsigned>(ELevel::eProg) << "): " << static_cast<unsigned>(tlog.GetLevel()) );

    // test increasing verbosity
    t_app.increase_global_verbosity( 1 );
    LINFO( tlog, "Verbosity should be INFO (" << static_cast<unsigned>(ELevel::eInfo) << "): " << static_cast<unsigned>(tlog.GetLevel()) );

    // test increasing past the end
    t_app.increase_global_verbosity( 10 );
    LTRACE( tlog, "Verbosity should be TRACE (" << static_cast<unsigned>(ELevel::eTrace) << "): " << static_cast<unsigned>(tlog.GetLevel()) );

    // test decreasing past the beginning
    t_app.decrease_global_verbosity( 100 );
    LFATAL( tlog, "Verbosity should be FATAL (" << static_cast<unsigned>(ELevel::eFatal) << "): " << static_cast<unsigned>(tlog.GetLevel()) );

    // test setting verbosity
    t_app.set_global_threshold( 30 );
    LWARN( tlog, "Verbosity should be WARN (" << static_cast<unsigned>(ELevel::eWarn) << "): " << static_cast<unsigned>(tlog.GetLevel()) );

    // test decreasing past the beginning
    t_app.set_global_threshold( 100 );
    LFATAL( tlog, "Verbosity should be FATAL (" << static_cast<unsigned>(ELevel::eFatal) << "): " << static_cast<unsigned>(tlog.GetLevel()) );

    return 0;
}
*/

