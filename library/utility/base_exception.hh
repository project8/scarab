/*
 * base_exception.hh
 *
 *  Created on: Mar 17, 2020
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_BASE_EXCEPTION_HH_
#define SCARAB_BASE_EXCEPTION_HH_

#include "path.hh"
#include "scarab_api.hh"

#include <exception>
#include <sstream>


namespace scarab
{
    /*!
     @class base_exception
     @author N.S. Oblath
     @brief Base class for exceptions with what() and where() functions
    */
    class SCARAB_API base_exception : public ::std::exception
    {
        public:
            base_exception() noexcept = default;
            virtual ~base_exception() noexcept = default;

            // what() inherited from std::exception

            virtual const char* where() const noexcept = 0;
    };

    /*!
     @class typed_exception
     @author N.S. Oblath
     @brief Base class for exceptions with streaming operators and filename/line number location

     @details
     This class provides streaming operators for building up the `what()` message.
     It's meant to be the base class for any exceptions wanting to use that feature.

     The `where()` message can be filled with the filename and line number using 
     a constructor or operator().

     This class uses the Curiously Recurring Template Pattern (CRTP) to get the 
     derived class type to appear in the base class.
     In particular, we need to return x_derived& from the various operator<<() 
     so that those functions can be used in a throw statement and the user can 
     still catch the derived type.
    */
   template< typename x_derived >
    class SCARAB_API typed_exception : public base_exception
    {
        public:
            typed_exception() noexcept;
            typed_exception( const std::string& a_filename, int a_line ) noexcept;
            typed_exception( const typed_exception< x_derived >& a_orig ) noexcept;
            virtual ~typed_exception() noexcept;

            typed_exception< x_derived >& operator=( const typed_exception< x_derived >& a_orig );

            template< class x_streamable >
            x_derived& operator<<( x_streamable a_fragment );
            x_derived& operator<<( const std::string& a_fragment );
            x_derived& operator<<( const char* a_fragment );

            x_derived& operator()( const std::string& a_filename, int a_line );

            virtual const char* what() const noexcept;
            virtual const char* where() const noexcept;

            const std::string& filename() const;
            int line() const;

        protected:
            mutable std::string f_what;
            mutable std::string f_where;

            std::string f_filename;
            int f_line;
    };

    template< typename x_derived >
    typed_exception< x_derived >::typed_exception() noexcept :
            base_exception(),
            f_what(),
            f_where(),
            f_filename(),
            f_line(0)
    {}

    template< typename x_derived >
    typed_exception< x_derived >::typed_exception( const std::string& a_filename, int a_line) noexcept :
            base_exception(),
            f_what(),
            f_where(),
            f_filename(),
            f_line( a_line )
    {
        try
        {
            f_filename = a_filename;
        }
        catch( ... )
        {
            f_filename.clear();
        }
    }

    template< typename x_derived >
    typed_exception< x_derived >::typed_exception( const typed_exception< x_derived >& a_orig ) noexcept :
            base_exception( a_orig ),
            f_what( a_orig.f_what ),
            f_where(),
            f_filename(),
            f_line( a_orig.f_line )
    {
        try
        {
            f_filename = a_orig.f_filename;
        }
        catch( ... )
        {
            f_filename.clear();
        }
    }

    template< typename x_derived >
    typed_exception< x_derived >::~typed_exception() noexcept
    {}

    template< typename x_derived >
    typed_exception< x_derived >& typed_exception< x_derived >::operator=( const typed_exception< x_derived >& a_orig )
    {
        f_what = a_orig.f_error;
        return *this;
    }

    template< typename x_derived >
    const char* typed_exception< x_derived >::what() const noexcept
    {
        return f_what.c_str();
    }

    template< typename x_derived >
    const char* typed_exception< x_derived >::where() const noexcept
    {
        try
        {
            f_where = path(f_filename).filename().string() + "(" + std::to_string(f_line) + ")";
        }
        catch( ... )
        {
            f_where.clear();
        }
        return f_where.c_str();
    }

    template< typename x_derived >
    template< class x_streamable >
    x_derived& typed_exception< x_derived >::operator<<( x_streamable a_fragment )
    {
        std::stringstream stream;
        stream << a_fragment;
        f_what += stream.str();
        return *static_cast< x_derived* >(this);
    }

    template< typename x_derived >
    x_derived& typed_exception< x_derived >::operator<<( const std::string& a_fragment )
    {
        f_what += a_fragment;
        return *static_cast< x_derived* >(this);
    }

    template< typename x_derived >
    x_derived& typed_exception< x_derived >::operator<<( const char* a_fragment )
    {
        f_what += std::string( a_fragment );
        return *static_cast< x_derived* >(this);
    }

    template< typename x_derived >
    x_derived& typed_exception< x_derived >::operator()( const std::string& a_filename, int a_line )
    {
        f_filename = a_filename;
        f_line = a_line;
        return *static_cast< x_derived* >(this);
    }

    template< typename x_derived >
    const std::string& typed_exception< x_derived >::filename() const
    {
        return f_filename;
    }

    template< typename x_derived >
    int typed_exception< x_derived >::line() const
    {
        return f_line;
    }
}

#endif /* SCARAB_BASE_EXCEPTION_HH_ */
