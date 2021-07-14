/*
 * base_exception.hh
 *
 *  Created on: Mar 17, 2020
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_BASE_EXCEPTION_HH_
#define SCARAB_BASE_EXCEPTION_HH_

#include "macros.hh"
#include "scarab_api.hh"

#include <sstream>

#define throw_from_here(obj) throw obj.add_location( __FILE__, __LINE__, __FUNC__ );

namespace scarab
{

    /*!
     @class base_exception
     @author N.S. Oblath
     @brief Base class for exceptions with streaming operators

     @details
     This class provides streaming operators for building up the `what()` message
     and location awareness (using the throw_from_here macro).
     It's meant to be the base class for any exceptions wanting to use those features.

     To throw derived class my_exception with location awareness:

       throw_from_here( my_exception() << "some error message" );

     This class uses the Curiously Recurring Template Pattern (CRTP) to get the 
     derived class type to appear in the base class.
     In particular, we need to return x_derived& from the various operator<<() 
     so that those functions can be used in a throw statement and the user can 
     still catch the derived type.
    */
   template< typename x_derived >
    class SCARAB_API base_exception : public ::std::exception
    {
        public:
            base_exception();
            base_exception( const base_exception< x_derived >& a_orig );
            virtual ~base_exception() noexcept;

            base_exception< x_derived >& operator=( const base_exception< x_derived >& a_orig );

            template< class x_streamable >
            x_derived& operator<<( x_streamable a_fragment );
            x_derived& operator<<( const std::string& a_fragment );
            x_derived& operator<<( const char* a_fragment );

            virtual const char* what() const noexcept;

            x_derived& add_location( const std::string& a_filename, int a_line_number, const std::string& a_function_name );

            virtual const char* locations() const noexcept;

        protected:
            mutable std::string f_error;
            mutable std::string f_locations;
            mutable unsigned f_loc_count;
    };

    template< typename x_derived >
    base_exception< x_derived >::base_exception() :
            ::std::exception(),
            f_error(),
            f_locations(),
            f_loc_count( 0 )
    {}

    template< typename x_derived >
    base_exception< x_derived >::base_exception( const base_exception< x_derived >& a_orig ) :
            std::exception( a_orig ),
            f_error( a_orig.f_error ),
            f_locations( a_orig.f_locations ),
            f_loc_count( a_orig.f_loc_count )
    {}

    template< typename x_derived >
    base_exception< x_derived >::~base_exception() noexcept
    {}

    template< typename x_derived >
    base_exception< x_derived >& base_exception< x_derived >::operator=( const base_exception< x_derived >& a_orig )
    {
        f_error = a_orig.f_error;
        f_locations = a_orig.f_locations;
        f_loc_count = a_orig.f_loc_count;
        return *this;
    }

    template< typename x_derived >
    const char* base_exception< x_derived >::what() const noexcept
    {
        return f_error.c_str();
    }

    template< typename x_derived >
    const char* base_exception< x_derived >::locations() const noexcept
    {
        return f_locations.c_str();
    }

    template< typename x_derived >
    template< class x_streamable >
    x_derived& base_exception< x_derived >::operator<<( x_streamable a_fragment )
    {
        std::stringstream stream;
        stream << a_fragment;
        f_error += stream.str();
        return *static_cast< x_derived* >(this);
    }

    template< typename x_derived >
    x_derived& base_exception< x_derived >::operator<<( const std::string& a_fragment )
    {
        f_error += a_fragment;
        return *static_cast< x_derived* >(this);
    }

    template< typename x_derived >
    x_derived& base_exception< x_derived >::operator<<( const char* a_fragment )
    {
        f_error += std::string( a_fragment );
        return *static_cast< x_derived* >(this);
    }

    template< typename x_derived >
    x_derived& base_exception< x_derived >::add_location( const std::string& a_filename, int a_line_number, const std::string& a_function_name )
    {
        std::stringstream stream;
        stream << "\n#" << f_loc_count++ << " -- " << a_filename << " -- line " << a_line_number << " -- " << a_function_name;
        f_locations += stream.str();
        return *static_cast< x_derived* >(this);
    }

}

#endif /* SCARAB_BASE_EXCEPTION_HH_ */
