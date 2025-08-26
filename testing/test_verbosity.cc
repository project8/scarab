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

    std::cerr << "levels:" << std::endl;
    std::cerr << SPDLOG_LEVEL_TRACE << "  " << static_cast<unsigned>(scarab::logger::ELevel::eTrace) << std::endl;
    std::cerr << SPDLOG_LEVEL_DEBUG << "  " << static_cast<unsigned>(scarab::logger::ELevel::eDebug) << std::endl;
    std::cerr << SPDLOG_LEVEL_INFO << "  " << static_cast<unsigned>(scarab::logger::ELevel::eInfo) << std::endl;
    std::cerr << SPDLOG_LEVEL_NOTICE << "  " << static_cast<unsigned>(scarab::logger::ELevel::eProg) << std::endl;
    std::cerr << SPDLOG_LEVEL_WARN << "  " << static_cast<unsigned>(scarab::logger::ELevel::eWarn) << std::endl;
    std::cerr << SPDLOG_LEVEL_ERROR << "  " << static_cast<unsigned>(scarab::logger::ELevel::eError) << std::endl;
    std::cerr << SPDLOG_LEVEL_CRITICAL << "  " << static_cast<unsigned>(scarab::logger::ELevel::eFatal) << std::endl;

    std::cerr << "logger threshold: " << static_cast<unsigned>(scarab::logger::get_global_threshold()) << std::endl;

    scarab::main_app t_app;

    std::cerr << "1 logger threshold: " << static_cast<unsigned>(scarab::logger::get_global_threshold()) << "; app verbosity: " << t_app.get_global_verbosity() << std::endl;

    // test default verbosity
    REQUIRE( scarab::logger::get_global_threshold() == scarab::logger::ELevel::eProg );

    std::cerr << "2 logger threshold: " << static_cast<unsigned>(scarab::logger::get_global_threshold()) << "; app verbosity: " << t_app.get_global_verbosity() << std::endl;

    // test increasing verbosity
    t_app.increase_global_verbosity( 1 );
    std::cerr << "3 logger threshold: " << static_cast<unsigned>(scarab::logger::get_global_threshold()) << "; app verbosity: " << t_app.get_global_verbosity() << std::endl;
    REQUIRE( scarab::logger::get_global_threshold() == scarab::logger::ELevel::eInfo );

    // test increasing past the end
    t_app.increase_global_verbosity( 10 );
    std::cerr << "4 logger threshold: " << static_cast<unsigned>(scarab::logger::get_global_threshold()) << "; app verbosity: " << t_app.get_global_verbosity() << std::endl;
    REQUIRE( scarab::logger::get_global_threshold() == scarab::logger::ELevel::eTrace );

    // test decreasing past the beginning
    t_app.decrease_global_verbosity( 100 );
    std::cerr << "5 logger threshold: " << static_cast<unsigned>(scarab::logger::get_global_threshold()) << "; app verbosity: " << t_app.get_global_verbosity() << std::endl;
    REQUIRE( scarab::logger::get_global_threshold() == scarab::logger::ELevel::eFatal );

    // test setting verbosity
    t_app.set_global_verbosity( 4 );
    std::cerr << "6 logger threshold: " << static_cast<unsigned>(scarab::logger::get_global_threshold()) << "; app verbosity: " << t_app.get_global_verbosity() << std::endl;
    REQUIRE( scarab::logger::get_global_threshold() == scarab::logger::ELevel::eWarn );

    // test decreasing past the beginning
    t_app.set_global_verbosity( 100 );
    std::cerr << "7 logger threshold: " << static_cast<unsigned>(scarab::logger::get_global_threshold()) << "; app verbosity: " << t_app.get_global_verbosity() << std::endl;
    REQUIRE( scarab::logger::get_global_threshold() == scarab::logger::ELevel::eFatal );

}

TEST_CASE( "verbosity_via_logger", "[logger]" )
{
    LOGGER( tlog, "test_verbosity" );

    scarab::logger::set_global_threshold( scarab::logger::ELevel::eTrace );
    LFATAL( tlog, "test FATAL" );
    LERROR( tlog, "test ERROR" );
    LWARN( tlog, "test WARN" );
    LPROG( tlog, "test PROG" );
    LINFO( tlog, "test INFO" );
    LDEBUG( tlog, "test DEBUG" );
    LTRACE( tlog, "test TRACE" );

    scarab::logger::set_global_threshold( scarab::logger::ELevel::eDebug );
    LFATAL( tlog, "test FATAL" );
    LERROR( tlog, "test ERROR" );
    LWARN( tlog, "test WARN" );
    LPROG( tlog, "test PROG" );
    LINFO( tlog, "test INFO" );
    LDEBUG( tlog, "test DEBUG" );
    LTRACE( tlog, "test TRACE" );

    scarab::logger::set_global_threshold( scarab::logger::ELevel::eInfo );
    LFATAL( tlog, "test FATAL" );
    LERROR( tlog, "test ERROR" );
    LWARN( tlog, "test WARN" );
    LPROG( tlog, "test PROG" );
    LINFO( tlog, "test INFO" );
    LDEBUG( tlog, "test DEBUG" );
    LTRACE( tlog, "test TRACE" );

    scarab::logger::set_global_threshold( scarab::logger::ELevel::eProg );
    LFATAL( tlog, "test FATAL" );
    LERROR( tlog, "test ERROR" );
    LWARN( tlog, "test WARN" );
    LPROG( tlog, "test PROG" );
    LINFO( tlog, "test INFO" );
    LDEBUG( tlog, "test DEBUG" );
    LTRACE( tlog, "test TRACE" );

    scarab::logger::set_global_threshold( scarab::logger::ELevel::eWarn );
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

