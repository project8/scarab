/*
 * test_terminate.cc
 *
 *  Created on: May 23, 2017
 *      Author: N.S. Oblath
 */

#include "terminate.hh"

#include "catch.hpp"


extern bool DO_ABORT_ON_TERMINATE;

TEST_CASE( "terminate", "[utility]" )
{
  
    DO_ABORT_ON_TERMINATE = false;
    REQUIRE_NOTHROW( scarab::terminate() );
    DO_ABORT_ON_TERMINATE = true;
}
