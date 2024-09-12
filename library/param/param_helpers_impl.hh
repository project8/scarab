/*
 * param_helpers_impl.hh
 *
 *  Created on: Aug 8, 2022
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_PARAM_HELPERS_IMPL_HH_
#define SCARAB_PARAM_HELPERS_IMPL_HH_

#include "param_helpers.hh"

#include "param.hh"
#include "param_array.hh"
#include "param_node.hh"
#include "param_value.hh"

namespace scarab
{
    template< typename T, typename... Ts >
    args::args( T&& a_value, Ts&&... a_values ) :
            f_array_ptr( new param_array() )
    {
        static_cast< param_array* >(f_array_ptr.get())->push_back( std::forward<T>(a_value), std::forward<Ts>(a_values)... );
    }

    template< typename T >
    kwarg_v kwarg::operator=( T&& a_value )
    {
        return { std::move(*this), std::forward<T>(a_value) };
    }

    template< typename T >
    kwarg_v::kwarg_v( kwarg&& a_base, T&& a_value ) :
            kwarg( std::move(a_base) ),
            f_value()
    {
        f_derived = this;

        using T_noref = typename std::remove_reference< T >::type;
        static_assert( std::is_same_v< param_ptr_t, T_noref > || 
                       std::is_base_of_v< param, T_noref > || 
                       std::is_convertible_v< T_noref, param_value > );

        if constexpr ( std::is_same_v< param_ptr_t, T_noref > )
        {
            f_value = a_value->clone();
        }
        else if constexpr ( std::is_base_of_v< param, T_noref > )
        {
            if constexpr ( std::is_lvalue_reference_v< T > ) // then do a copy
            {
                f_value = a_value.clone();
            }
            else // then do a move
            {
                f_value = a_value.move_clone();
            }
        }
        else if constexpr ( std::is_convertible_v< T_noref, param_value > )
        {
            f_value = param_ptr_t( new param_value( a_value ) );
        }
    }

}

#endif /* SCARAB_PARAM_HELPERS_IMPL_HH_ */
