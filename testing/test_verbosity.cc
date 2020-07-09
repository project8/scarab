/*
 * test_verbosity.cc
 *
 *  Created on: Mar 31, 2020
 *      Author: N.S. Oblath
 */

#include "application.hh"

#include "catch.hpp"


TEST_CASE( "verbosity", "[cli][logger]" )
{
    LOGGER( tlog, "test_verbosity" );

    scarab::main_app t_app;

    // test default verbosity
    REQUIRE( tlog.GetLevel() == scarab::logger::ELevel::eProg );

    // test increasing verbosity
    t_app.increase_global_verbosity( 1 );
    REQUIRE( tlog.GetLevel() == scarab::logger::ELevel::eInfo );

    // test increasing past the end
    t_app.increase_global_verbosity( 10 );
    REQUIRE( tlog.GetLevel() == scarab::logger::ELevel::eTrace );

    // test decreasing past the beginning
    t_app.decrease_global_verbosity( 100 );
    REQUIRE( tlog.GetLevel() == scarab::logger::ELevel::eFatal );

    // test setting verbosity
    t_app.set_global_verbosity( 30 );
    REQUIRE( tlog.GetLevel() == scarab::logger::ELevel::eWarn );

    // test decreasing past the beginning
    t_app.set_global_verbosity( 100 );
    REQUIRE( tlog.GetLevel() == scarab::logger::ELevel::eFatal );

}
/*
using namespace scarab;

LOGGER( tlog, "test_verbosity" );

int main()
{
    main_app t_app;

    // test default verbosity
    LPROG( tlog, "Verbosity should be PROG (" << static_cast<unsigned>(logger::ELevel::eProg) << "): " << static_cast<unsigned>(tlog.GetLevel()) );

    // test increasing verbosity
    t_app.increase_global_verbosity( 1 );
    LINFO( tlog, "Verbosity should be INFO (" << static_cast<unsigned>(logger::ELevel::eInfo) << "): " << static_cast<unsigned>(tlog.GetLevel()) );

    // test increasing past the end
    t_app.increase_global_verbosity( 10 );
    LTRACE( tlog, "Verbosity should be TRACE (" << static_cast<unsigned>(logger::ELevel::eTrace) << "): " << static_cast<unsigned>(tlog.GetLevel()) );

    // test decreasing past the beginning
    t_app.decrease_global_verbosity( 100 );
    LFATAL( tlog, "Verbosity should be FATAL (" << static_cast<unsigned>(logger::ELevel::eFatal) << "): " << static_cast<unsigned>(tlog.GetLevel()) );

    // test setting verbosity
    t_app.set_global_verbosity( 30 );
    LWARN( tlog, "Verbosity should be WARN (" << static_cast<unsigned>(logger::ELevel::eWarn) << "): " << static_cast<unsigned>(tlog.GetLevel()) );

    // test decreasing past the beginning
    t_app.set_global_verbosity( 100 );
    LFATAL( tlog, "Verbosity should be FATAL (" << static_cast<unsigned>(logger::ELevel::eFatal) << "): " << static_cast<unsigned>(tlog.GetLevel()) );

    return 0;
}
*/

