/*
 * test_member_variable.cc
 *
 *  Created on: May 30, 2017
 *      Author: N.S. Oblath
 */

#include "test_member_variables.hh"

#include "catch.hpp"

TEST_CASE( "member_variables", "[utility]" )
{
    SECTION( "accessible" )
    {
        scarab_testing::test_acc t_acc;
        t_acc.set_mv_acc_int( 5 );
        REQUIRE( t_acc.get_mv_acc_int() == 5 );
        REQUIRE( t_acc.get_mv_acc_ns_int() == 10 );

        scarab_testing::test_acc_static::set_mv_acc_int( 5 );
        REQUIRE( scarab_testing::test_acc_static::get_mv_acc_int() == 5 );
        REQUIRE( scarab_testing::test_acc_static::get_mv_acc_ns_int() == 10 );

        scarab_testing::test_acc_mutable t_acc_mut;
        t_acc_mut.set_mv_acc_int( 5 );
        REQUIRE( t_acc_mut.get_mv_acc_int() == 5 );
        REQUIRE( t_acc_mut.get_mv_acc_ns_int() == 10 );
    }

    SECTION( "referrable" )
    {
        scarab_testing::test_ref t_ref;
        t_ref.mv_ref_int() = 5;
        REQUIRE( t_ref.mv_ref_int() == 5 );
        REQUIRE( t_ref.mv_ref_c_int() == 10 );

        scarab_testing::test_ref_static::mv_ref_int() = 5;
        REQUIRE( scarab_testing::test_ref_static::mv_ref_int() == 5 );

        scarab_testing::test_ref_mutable t_ref_mut;
        t_ref_mut.mv_ref_int() = 5;
        REQUIRE( t_ref_mut.mv_ref_int() == 5 );
    }

    SECTION( "pointer" )
    {

    }

    SECTION( "shared_ptr" )
    {

    }

    SECTION( "atomic" )
    {

    }
}
