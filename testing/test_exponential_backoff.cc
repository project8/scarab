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
    unsigned t_value = 0; // value to increment, allowing verification that the function executed
    unsigned t_retries = 0; // how many retries the increment function should wait

    auto t_increment = [&t_value]()->bool
    {
        ++t_value;
        return true;
    };
    std::cout << "hello?" << std::endl;
    // just verify that the function gets called
    scarab::exponential_backoff<> t_exp_back( t_increment );
    t_exp_back.do_action();
    REQUIRE( t_value == 1 );
}
