/*
 * test_weak_holder.cc
 *
 *  Created on: Apr 23, 2025
 *      Author: N.S. Oblath
 *
 */

#include "weak_holder.hh"

#include "catch2/catch_test_macros.hpp"
 
#include <memory>

namespace scarab_testing
{
    struct tester
    {
        virtual int value() {return 100;}
    };
}

using scarab_testing::tester;
using scarab::weak_holder;

TEST_CASE( "weak_holder_default", "[utility]" )
{
    // empty weak holder should return an empty shared_ptr from weak_holder::lock()
    weak_holder< tester > t_tester_wh;

    REQUIRE_FALSE( t_tester_wh.lock() );

}

TEST_CASE( "weak_holder", "[utility]" )
{
    // Weak holder should behave like a std::weak_ptr
    // After deleting the tester, the weak references should return empty shared_ptr's

    std::shared_ptr< tester > t_tester_sp = std::make_shared< tester >();

    REQUIRE( t_tester_sp->value() == 100 );

    std::weak_ptr< tester > t_tester_wp( t_tester_sp );
    weak_holder< tester > t_tester_wh( t_tester_sp );

    REQUIRE( t_tester_wp.lock() );
    REQUIRE( t_tester_wh.lock() );
    REQUIRE( t_tester_wh.lock() == t_tester_wp.lock() );

    REQUIRE( t_tester_wp.lock()->value() == 100 );
    REQUIRE( t_tester_wh.lock()->value() == 100 );

    t_tester_sp.reset();
    REQUIRE_FALSE( t_tester_wp.lock() );
    REQUIRE_FALSE( t_tester_wh.lock() );
}
