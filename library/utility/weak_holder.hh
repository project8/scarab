/*
 * weak_holder.hh
 *
 *  Created on: Apr 21, 2025
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_WEAK_HOLDER_HH_
#define SCARAB_WEAK_HOLDER_HH_

#include <memory>

#ifdef USE_PYTHON
#include "pybind11/pybind11.h"
#endif

namespace scarab
{
    template< typename T >
    struct weak_holder
    {
        typedef T held_t;

        std::weak_ptr< held_t > f_weak_ptr;

        weak_holder() = default;
        weak_holder( const std::shared_ptr< held_t > a_ptr, bool _from_py = false );
        weak_holder( const weak_holder& ) = default;
        weak_holder( weak_holder&& ) = default;

        void load( const std::shared_ptr< held_t > a_ptr );
        std::shared_ptr< held_t > lock() const;

#ifdef USE_PYTHON
        pybind11::weakref f_py_weak_ref;
        void load_from_py( const std::shared_ptr< T > a_ptr );
#endif

    };

    template< typename T >
    weak_holder< T >::weak_holder( const std::shared_ptr< T > a_ptr, bool _from_py )
    {
#ifdef USE_PYTHON
        if( _from_py )
        {
            load_from_py( a_ptr );
        }
        else
        {
            load( a_ptr );
        }
#else
        load( a_ptr );
#endif
    }

    template< typename T >
    void weak_holder< T >::load( const std::shared_ptr< T > a_ptr )
    {
        f_weak_ptr = a_ptr;
        return;
    }

#ifdef USE_PYTHON
    template< typename T >
    void weak_holder< T >::load_from_py( const std::shared_ptr< T > a_ptr )
    {
        f_weak_ptr = a_ptr;
        f_py_weak_ref = pybind11::cast( a_ptr.get() );
        return;
    }
#endif

    template< typename T >
    std::shared_ptr< T > weak_holder< T >::lock() const
    {
#ifdef USE_PYTHON
        if( f_py_weak_ref )
        {
            pybind11::object t_py_obj = pybind11::object(f_py_weak_ref);
            return std::shared_ptr< T >( t_py_obj.cast< T* >() );
        }
#endif
        return f_weak_ptr.lock();
    }

} /* namespace scarab */

#endif /* SCARAB_WEAK_HOLDER_HH_ */