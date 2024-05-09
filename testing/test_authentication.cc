/*
 * test_logger.cc
 *
 *  Created on: Jan 4, 2016
 *      Author: N.S. Oblath
 */

#include "authentication.hh"

#include "catch.hpp"


TEST_CASE( "authentication", "[authentication]" )
{
    using Catch::Matchers::Equals;

    scarab::authentication t_auth;

    t_auth.process_design();

    REQUIRE_THAT( t_auth.get("scarab", "user"), Equals("some_user") );
}
