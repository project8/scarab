/*
 * unique_typelist.hh
 *
 *  Created on: Oct 3, 2018
 *      Author: N.S. Oblath
 *
 *  This file allows you to create type lists with unique elements.
 *
 *  The typelist itself is a standard type list, and the uniqueness of the elements is
 *  enforced as elements are appended using `unique_append`.
 *
 *  Test code is commented out below as an example of how to use the append functionality.
 *
 *  The current version works for types; a version that made integer lists was explored but a problem was found
 *  that didn't have an obvious work-around.  That path was abandoned because the type version can work for
 *  integers using std::integer_constant.  The integer code is commented out below in case a work-around can be found.
 */

#ifndef SCARAB_UNIQUE_TYPELIST_HH_
#define SCARAB_UNIQUE_TYPELIST_HH_

#include <type_traits>

namespace scarab
{
    // unique type check
    template< typename T, typename FirstType, typename... OtherTypes  >
    struct is_unique_impl;

    template< typename T, typename FirstType >
    struct is_unique_impl< T, FirstType >
    {
        static const bool value = ! std::is_same< T, FirstType >::value;
    };

    template< typename T, typename FirstType, typename... OtherTypes  >
    struct is_unique_impl
    {
        static const bool value = std::is_same< T, FirstType >::value ? false : is_unique_impl< T, OtherTypes... >::value;
    };

    template< typename T, typename FirstType, typename... OtherTypes >
    struct is_unique : std::integral_constant< bool, is_unique_impl< T, FirstType, OtherTypes... >::value >
    {};

    // type list
    template< typename... Types >
    struct type_list {};

    // appending
    template< typename T, typename... ListItems >
    struct unique_append_impl;

    template< typename T >
    struct unique_append_impl< T, type_list<> >
    {
        using type = type_list< T >;
    };

    template< typename T, typename... ListItems >
    struct unique_append_impl< T, type_list< ListItems... > >
    {
        static_assert(is_unique< T, ListItems... >::value, "Non-unique type found");
        using type = type_list< T, ListItems... >;
    };

    template< typename T, typename List >
    using unique_append = typename unique_append_impl< T, List >::type;

    // for debug printing
    //template< typename T > struct debug_t;

    // test implementation
/*    // this should work
    using test_list_void = type_list<>;
#define TEST_LIST test_list_void

    typedef std::integral_constant< unsigned, 0 > zero_t;
    using test_list_zero_t = unique_append< zero_t, TEST_LIST >;
#undef TEST_LIST
#define TEST_LIST test_list_zero_t

    typedef std::integral_constant< unsigned, 1 > one_t;
    using test_list_one_t = unique_append< one_t, TEST_LIST >;
#undef TEST_LIST
#define TEST_LIST test_list_one_t
*/
/*    // this should fail
    typedef std::integral_constant< unsigned, 1 > duplicate_t;
    using test_list_duplicate_t = unique_append< duplicate_t, TEST_LIST >;
#undef TEST_LIST
#define TEST_LIST test_list_duplicate_t
    //debug_t< TEST_LIST > d;
*/



    // Integer lists
    // Unfortunately this section doesn't quite work
    // The problem comes in the partial specialization:  struct append_impl_int< T, int_list< ListItems... > >
    // This works when the unspecialized template uses a type parameter pack, but not when it uses an int parameter pack.
    // The error I get is:
    //   error: template argument for non-type template parameter must be an expression
/*
    // unique int check
    template< int T, int FirstType, int... OtherTypes  >
    struct is_unique_impl_int;

    template< int T, int FirstType >
    struct is_unique_impl_int< T, FirstType >
    {
        static const bool value =  T != FirstType;
    };

    template< int T, int FirstType, int... OtherTypes  >
    struct is_unique_impl_int
    {
        static const bool value = T == FirstType ? false : is_unique_impl_int< T, OtherTypes... >::value;
    };

    template< int T, int FirstType, int... OtherTypes >
    struct is_unique_int : std::integral_constant< bool, is_unique_impl_int< T, FirstType, OtherTypes... >::value >
    {};

    // int list
    template< int... Types >
    struct int_list {};

    // appending
    template< int T, int... ListItems >
    struct unique_append_impl_int;

    template< int T >
    struct unique_append_impl_int< T >
    {
        using type = int_list< T >;
    };

    template< int T, int... ListItems >
    struct unique_append_impl_int< T, int_list< ListItems... > >
    {
        static_assert(is_unique_int< T, ListItems... >::value, "Non-unique return code found");
        using type = int_list< T, ListItems... >;
    };

    template< int T, typename List >
    using unique_append_int = typename unique_append_impl_int< T, List >::type;


    // test implementation

    using test_list_void_int = list_int<>;
#define TEST_LIST_INT test_list_void_int

    const static int zero_int = 0;
    using test_list_zero_int = append_int< zero_int, TEST_LIST_INT >;
#undef TEST_LIST_INT
#define TEST_LIST_INT test_list_zero_int

    const static int one_int = 1;
    using test_list_one_int = unique_append_int< one_int, TEST_LIST_INT >;
#undef TEST_LIST_INT
#define TEST_LIST_INT test_list_one_int
*/
/*    // this should fail
    const static int duplicate_int = 1;
    using test_list_duplicate_t = unique_append< duplicate_int, TEST_LIST >;
#undef TEST_LIST
#define TEST_LIST test_list_duplicate_int
    //debug_t< TEST_LIST > d;
*/

}

#endif /* SCARAB_UNIQUE_TYPELIST_HH_ */
