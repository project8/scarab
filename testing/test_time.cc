/*
 * test_time.cc
 *
 *  Created on: Jan 15, 2018
 *      Author: obla999
 */

#include "time.hh"

#include "catch.hpp"

#include <regex>

TEST_CASE( "time", "[utility]" )
{
    std::regex t_regex( "[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]+Z" );
    REQUIRE( std::regex_match( scarab::get_formatted_now(), t_regex ) );
}
