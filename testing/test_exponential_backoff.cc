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
    unsigned t_attempts = 0; // how many attempts the increment function should wait; set this to the desired number of attempts before a test
    unsigned t_attempt = 0; // used by the function to track how many attempts have been attempted; reset this to 1 before a test
    unsigned t_max_attempts = 10; // maximum attempts
    unsigned t_return = 0; // stores the return value; should match t_attempt

    // this function will wait for a specified number of attempts (t_attempts), 
    // returning false each time until t_attempt == t_attempts; then it returns true.
    auto t_increment = [&]()->bool
    {
        if( t_attempt < t_attempts )
        {
            ++t_attempt;
            return false;
        }

        ++t_value;
        return true;
    };

    // TODO: test exception is thrown on 0 max attempts

    // the exponential-backoff test object
    scarab::exponential_backoff<> t_exp_back( t_increment );

    // test: just verify that the function gets called
    // t_value == 0
    t_attempts = 1; t_attempt = 1;
    t_return = t_exp_back.do_action();
    REQUIRE( t_value == 1 );
    REQUIRE( t_attempt == 1 );
    REQUIRE( t_return == 1 );

    // test: do multiple attempts
    // t_value == 1
    t_attempts = 3; t_attempt = 1;
    t_return = t_exp_back.do_action();
    REQUIRE( t_value == 2 );
    REQUIRE( t_attempt == 3 );
    REQUIRE( t_return == 3 );

    // TODO: test that max attempts is not bypassed

}
