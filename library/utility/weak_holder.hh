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

#include <iostream>

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
        bool expired() const;
        std::shared_ptr< held_t > lock() const;

#ifdef USE_PYTHON
        pybind11::handle f_py_weak_ref;
        void load_from_py( const std::shared_ptr< T > a_ptr );
#endif

    };

    template< typename T >
    weak_holder< T >::weak_holder( const std::shared_ptr< T > a_ptr, bool _from_py )
    {
#ifdef USE_PYTHON
        if( _from_py )
        {
            std::cerr << "A" << std::endl;
            load_from_py( a_ptr );
        }
        else
        {
            std::cerr << "B" << std::endl;
            load( a_ptr );
        }
#else
        load( a_ptr );
#endif
    }

    template< typename T >
    void weak_holder< T >::load( const std::shared_ptr< T > a_ptr )
    {
        std::cerr << "C" << std::endl;
        f_weak_ptr = a_ptr;
        return;
    }

#ifdef USE_PYTHON
    template< typename T >
    void weak_holder< T >::load_from_py( const std::shared_ptr< T > a_ptr )
    {
        std::cerr << "D" << std::endl;
        f_weak_ptr = a_ptr;
        f_py_weak_ref = pybind11::cast( a_ptr.get() );
        std::cerr << "  " << f_weak_ptr.expired() << " and " << f_py_weak_ref.operator bool() << std::endl;
        return;
    }
#endif

    template< typename T >
    bool weak_holder< T >::expired() const
    {
        std::cerr << "G" << std::endl;
#ifdef USE_PYTHON
        std::cerr << "  " << f_weak_ptr.expired() << " and " << f_py_weak_ref.operator bool() << std::endl;
        return f_weak_ptr.expired() || ! f_py_weak_ref.operator bool();
#else
        std::cerr << "  " << f_weak_ptr.expired() << std::endl;
        return f_weak_ptr.expired();
#endif
    }

    template< typename T >
    std::shared_ptr< T > weak_holder< T >::lock() const
    {
#ifdef USE_PYTHON
        if( f_py_weak_ref.operator bool() )
        {
            std::cerr << "E" << std::endl;
            std::cerr << "  " << f_weak_ptr.expired() << " and " << f_py_weak_ref.operator bool() << std::endl;
            //pybind11::object t_py_obj = pybind11::object(f_py_weak_ref.ref());
            return std::shared_ptr< T >( f_py_weak_ref.ptr() );
        }
#endif
        std::cerr << "H" << std::endl;
        std::cerr << "  " << f_weak_ptr.expired() << std::endl;
        return f_weak_ptr.lock();
    }

} /* namespace scarab */

#endif /* SCARAB_WEAK_HOLDER_HH_ */