/*
 * member_variables.hh
 *
 *  Created on: Jan 6, 2016
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_MEMBER_VARIABLES_HH_
#define SCARAB_MEMBER_VARIABLES_HH_

/**
 * Macros for class member variables
 *
 * In all cases remember to initialize the variables!
 *
 * For "normal" variables
 * Defines accessors [type get_my_var() const], [void set_my_var( type )], and member variable [type f_my_var]
 * The set_ function is not available if the _noset macros are used
 *  - mv_accessible
 *  - mv_accessible_noset
 *  - mv_accessible_static
 *  - mv_accessible_static_noset
 *
 * For variables accessed by reference
 * Defines accessors [const type& my_var() const], [type& my_var()], and member variable [type f_my_var]
 * The non-const function is not available if the _const macros are used
 *  - mv_referrable
 *  - mv_referrable_const
 *  - mv_referrable_static
 *  - mv_referrable_static_const
 *
 * For pointer variables
 * Defines accessors [type* get_my_var() const], [void set_my_var( type* )], and member variable [type* f_my_var]
 * The set_ function is not available if the _noset macros are used
 *  - mv_assignable
 *  - mv_assignable_noset
 *  - mv_assignable_static
 *  - mv_assignable_static_noset
 *
 * For std::shared_ptr's
 * Defines accessors [const std::shared_ptr< type > my_var() const], [std::shared_ptr< type > my_var()], and member variable [std::shared_ptr< type > f_my_var]
 * The non-const function is not available if the _const macros are used
 *  - mv_shared_ptr
 *  - mv_shared_ptr_const
 *  - mv_shared_ptr_static
 *  - mv_shared_ptr_static_const
 *
 * For atomic variables
 * Defines accessors [type get_my_var() const], [void set_my_var( type )], and member variable [std::atomic< type > f_my_var]
 * The set_ function is not available if the _noset macros are used
 *  - mv_atomic
 *  - mv_atomic_noset
 *  - mv_atomic_static
 *  - mv_atomic_static_noset
 *
 */

#include "_snake_case_member_variables.hh"

#define mv_accessible                         snake_case_mv_accessible
#define mv_accessible_noset                   snake_case_mv_accessible_noset
#define mv_accessible_static                  snake_case_mv_accessible_static
#define mv_accessible_static_noset            snake_case_mv_accessible_static_noset
#define mv_accessible_mutable                 snake_case_mv_accessible_mutable
#define mv_accessible_mutable_noset           snake_case_mv_accessible_mutable_noset

#define mv_referrable                         snake_case_mv_referrable
#define mv_referrable_const                   snake_case_mv_referrable_const
#define mv_referrable_static                  snake_case_mv_referrable_static
#define mv_referrable_mutable                 snake_case_mv_referrable_mutable
#define mv_referrable_mutable_const           snake_case_mv_referrable_mutable_const

#define mv_assignable                         snake_case_mv_assignable
#define mv_assignable_noset                   snake_case_mv_assignable_noset
#define mv_assignable_static                  snake_case_mv_assignable_static
#define mv_assignable_static_noset            snake_case_mv_assignable_static_noset
#define mv_assignable_mutable                 snake_case_mv_assignable_mutable
#define mv_assignable_mutable_noset           snake_case_mv_assignable_mutable_noset

#define mv_shared_ptr                         snake_case_mv_shared_ptr
#define mv_shared_ptr_const                   snake_case_mv_shared_ptr_const
#define mv_shared_ptr_static                  snake_case_mv_shared_ptr_static
#define mv_shared_ptr_mutable                 snake_case_mv_shared_ptr_mutable
#define mv_shared_ptr_mutable_const           snake_case_mv_shared_ptr_mutable_const

#define mv_atomic                             snake_case_mv_atomic
#define mv_atomic_noset                       snake_case_mv_atomic_noset
#define mv_atomic_static                      snake_case_mv_atomic_static
#define mv_atomic_static_noset                snake_case_mv_atomic_static_noset
#define mv_atomic_mutable                     snake_case_mv_atomic_mutable
#define mv_atomic_mutable_noset               snake_case_mv_atomic_mutable_noset


#endif /* SCARAB_MEMBER_VARIABLES_HH_ */
