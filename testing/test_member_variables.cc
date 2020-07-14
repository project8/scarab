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
        scarab_testing::test_ptr t_ptr;
        t_ptr.set_mv_ptr_int( new int(5) );
        REQUIRE( *(t_ptr.get_mv_ptr_int()) == 5 );
        REQUIRE( *(t_ptr.get_mv_ptr_ns_int()) == 10 );

        scarab_testing::test_ptr_static::set_mv_ptr_int( new int(5) );
        REQUIRE( *(scarab_testing::test_ptr_static::get_mv_ptr_int()) == 5 );

        scarab_testing::test_ptr_mutable t_ptr_mut;
        t_ptr_mut.set_mv_ptr_int( new int(5) );
        REQUIRE( *(t_ptr_mut.get_mv_ptr_int()) == 5 );
        REQUIRE( *(t_ptr_mut.get_mv_ptr_ns_int()) == 10 );
    }

    SECTION( "shared_ptr" )
    {
        scarab_testing::test_shptr t_shptr;
        *t_shptr.mv_shptr_int() = 5;
        REQUIRE( *(t_shptr.mv_shptr_int()) == 5 );
        REQUIRE( *(t_shptr.mv_shptr_c_int()) == 10 );

        *scarab_testing::test_shptr_static::mv_shptr_int() = 5;
        REQUIRE( *(scarab_testing::test_shptr_static::mv_shptr_int()) == 5 );

        scarab_testing::test_shptr_mutable t_shptr_mut;
        *t_shptr_mut.mv_shptr_int() = 5;
        REQUIRE( *(t_shptr_mut.mv_shptr_int()) == 5 );
        REQUIRE( *(t_shptr_mut.mv_shptr_c_int()) == 10 );
    }

    SECTION( "atomic" )
    {
        scarab_testing::test_atm t_atm;
        t_atm.set_mv_atm_int( 5 );
        REQUIRE( t_atm.get_mv_atm_int() == 5 );
        REQUIRE( t_atm.get_mv_atm_ns_int() == 10 );

        scarab_testing::test_atm_static::set_mv_atm_int( 5 );
        REQUIRE( scarab_testing::test_atm_static::get_mv_atm_int() == 5 );

        scarab_testing::test_atm_mutable t_atm_mut;
        t_atm_mut.set_mv_atm_int( 5 );
        REQUIRE( t_atm_mut.get_mv_atm_int() == 5 );
        REQUIRE( t_atm_mut.get_mv_atm_ns_int() == 10 );
    }
}
