/*
 * test_exponential_backoff.cc
 *
 *  Created on: Jun 17, 2021
 *      Author: N.S. Oblath
 */

#include "exponential_backoff.hh"

#include "catch.hpp"


TEST_CASE( "exponential-backoff", "[utility]" )
{
    unsigned t_value = 0; // value to increment, allowing verification that the function executed; check that this has changed from the before-test value
    unsigned t_retries = 0; // how many retries the increment function should wait; set this to the desired number of retries before a test
    unsigned t_retry = 0; // used by the function to track how many retries have been attempted; reset this to 0 before a test

    // this function will wait for a specified number of retries (t_retries), 
    // returning false each time until t_retry == t_retries; then it returns true.
    auto t_increment = [&]()->bool
    {
        if( t_retry < t_retries )
        {
            ++t_retry;
            return false;
        }

        ++t_value;
        return true;
    };

    // the exponential-backoff test object
    scarab::exponential_backoff<> t_exp_back( t_increment );

    // test: just verify that the function gets called
    // t_value == 0
    t_retries = 0; t_retry = 0;
    t_exp_back.do_action();
    REQUIRE( t_value == 1 );
    REQUIRE( t_retry == 0 );

    // test: do multiple retries
    // t_value == 1
    t_retries = 3; t_retry = 0;
    t_exp_back.do_action();
    REQUIRE( t_value == 2 );
    REQUIRE( t_retry == 3 );

}
