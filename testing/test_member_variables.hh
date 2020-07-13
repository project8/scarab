/*
 * test_member_variable.hh
 *
 *  Created on: May 30, 2017
 *      Author: obla999
 *
 *  The variables in the test_acc struct include a test of doxygen-compliant documentation
 */

#ifndef SCARAB_TEST_MEMBER_VARIABLE_HH_
#define SCARAB_TEST_MEMBER_VARIABLE_HH_

#include <atomic>
#include <memory>
#include <string>

#include "member_variables.hh"

namespace scarab_testing
{
    
    //**********
    // normal
    //**********

    struct test_acc
    {
        test_acc() : f_mv_acc_ns_int( 10 ), f_mv_acc_int( 0 ) {}
        /// Very special variable f_acc_ns_int
        mv_accessible_noset( int, mv_acc_ns_int )
        /**
         *  Another special variable: f_acc_int
         *  This variable is so special it requires
         *  a multi-line comment
         */
        mv_accessible( int, mv_acc_int )
    };

    struct test_acc_static
    {
        mv_accessible_static_noset( int, mv_acc_ns_int )
        mv_accessible_static( int, mv_acc_int )
    };

    int test_acc_static::s_mv_acc_ns_int = 10;
    int test_acc_static::s_mv_acc_int = 0;

    struct test_acc_mutable
    {
        test_acc_mutable() : f_mv_acc_ns_int( 10 ), f_mv_acc_int( 0 ) {}
        mv_accessible_mutable_noset( int, mv_acc_ns_int )
        mv_accessible_mutable( int, mv_acc_int )
    };

    //**************
    // referrable
    //**************

    struct test_ref
    {
        test_ref() : f_mv_ref_c_int( 10 ), f_mv_ref_int( 0 ) {}
        mv_referrable_const( int, mv_ref_c_int )
        mv_referrable( int, mv_ref_int )
    };

    struct test_ref_static
    {
        mv_referrable_static( int, mv_ref_int )
    };

    int test_ref_static::s_mv_ref_int = 0;

    struct test_ref_mutable
    {
        test_ref_mutable() : f_mv_ref_int( 0 ) {}
        mv_referrable_mutable( int, mv_ref_int )
    };


    //***********
    // pointer
    //***********

    struct test_ptr
    {
        mv_assignable_noset( int, mv_ptr_ns_int )
        mv_assignable( int, mv_ptr_int )
    };

    struct test_ptr_static
    {
        mv_assignable_static_noset( int, mv_ptr_ns_int )
        mv_assignable_static( int, mv_ptr_int )
    };

    struct test_ptr_mutable
    {
        mv_assignable_mutable_noset( int, mv_ptr_ns_int )
        mv_assignable_mutable( int, mv_ptr_int )
    };


    //**************
    // shared_ptr
    //**************

    struct test_shptr
    {
        mv_shared_ptr_const( int, mv_shptr_c_int )
        mv_shared_ptr( int, mv_shptr_int )
    };

    struct test_shptr_static
    {
        mv_shared_ptr_static( int, mv_shptr_int )
    };

    struct test_shptr_mutable
    {
        mv_shared_ptr_mutable( int, mv_shptr_int )
    };


    //**********
    // atomic
    //**********

    struct test_atm
    {
        mv_atomic_noset( int, mv_atm_ns_int )
        mv_atomic( int, mv_atm_int )
    };

    struct test_atm_static
    {
        mv_atomic_static_noset( int, mv_atm_ns_int )
        mv_atomic_static( int, mv_atm_int )
    };

    struct test_atm_mutable
    {
        mv_atomic_mutable_noset( int, mv_atm_ns_int )
        mv_atomic_mutable( int, mv_atm_int )
    };
}

#endif /* SCARAB_TEST_MEMBER_VARIABLE_HH_ */
