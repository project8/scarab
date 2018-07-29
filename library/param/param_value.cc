/*
 * param_value.cc
 *
 *  Created on: Jan 14, 2014
 *      Author: nsoblath
 */

#define SCARAB_API_EXPORTS

#include <sstream>
using std::string;
using std::stringstream;

#include "param_value.hh"

#include "param_base_impl.hh"

//#include "logger.hh"
//LOGGER( dlog, "param_value" )

namespace scarab
{

    param_value::param_value() :
            param(),
            f_value( false )
    {
        //LWARN( dlog, "param_value constructor: default (bool)" );
    }

    param_value::param_value( bool a_value ) :
            param(),
            f_value( a_value )
    {
        f_value = a_value;
        //LWARN( dlog, "param_value constructor: bool --> bool" );
    }

    param_value::param_value( uint8_t a_value ) :
            param(),
            f_value( uint64_t(a_value) )
    {
        //LWARN( dlog, "param_value constructor: uint8 --> uint" );
    }

    param_value::param_value( uint16_t a_value ) :
            param(),
            f_value( uint64_t(a_value) )
    {
        //LWARN( dlog, "param_value constructor: uint16 --> uint" );
    }

    param_value::param_value( uint32_t a_value ) :
            param(),
            f_value( uint64_t(a_value) )
    {
        //LWARN( dlog, "param_value constructor: uint32 --> uint" );
    }

    param_value::param_value( uint64_t a_value ) :
            param(),
            f_value( uint64_t(a_value) )
    {
        //LWARN( dlog, "param_value constructor: uint64 --> uint" );
    }

    param_value::param_value( int8_t a_value ) :
            param(),
            f_value( int64_t(a_value) )
    {
        //LWARN( dlog, "param_value constructor: int8 --> int" );
    }

    param_value::param_value( int16_t a_value ) :
            param(),
            f_value( int64_t(a_value) )
    {
        //LWARN( dlog, "param_value constructor: int16 --> int" );
    }


    param_value::param_value( int32_t a_value ) :
            param(),
            f_value( int64_t(a_value) )
    {
        //LWARN( dlog, "param_value constructor: int32 --> int" );
    }

    param_value::param_value( int64_t a_value ) :
            param(),
            f_value( a_value )
    {
        //LWARN( dlog, "param_value constructor: int64 --> int" );
    }

    param_value::param_value( float a_value ) :
            param(),
            f_value( double(a_value) )
    {
        //LWARN( dlog, "param_value constructor: float --> double" );
    }

    param_value::param_value( double a_value ) :
            param(),
            f_value( a_value )
    {
        //LWARN( dlog, "param_value constructor: double --> double" );
    }

    param_value::param_value( const char* a_value ) :
            param(),
            f_value( string( a_value ) )
    {
        //LWARN( dlog, "param_value constructor: char* --> k_string" );
    }

    param_value::param_value( const string& a_value ) :
            param(),
            f_value( a_value )
    {
        //LWARN( dlog, "param_value constructor: string --> k_string" );
    }

    param_value::param_value( const param_value& orig ) :
            param( orig ),
            f_value( orig.f_value )
    {
        //LWARN( dlog, "param_value copy constructor: " << type() );
    }

    param_value::param_value( param_value&& orig ) :
            param( std::move( orig ) ),
            f_value( std::move( orig.f_value ) )
    {
    }

    param_value::~param_value()
    {
    }

    param_value& param_value::operator=( const param_value& rhs )
    {
        if( &rhs == this ) return *this;

        f_value = rhs.f_value;

        return *this;
    }

    param_value& param_value::operator=( param_value&& rhs )
    {
        if( &rhs == this ) return *this;

        f_value = std::move( rhs.f_value );

        return *this;
    }

    bool param_value::has_subset( const param& a_subset ) const
    {
        if( ! a_subset.is_value() ) return false;
        return true;
    }

    SCARAB_API std::ostream& operator<<(std::ostream& out, const param_value& a_value)
    {
        return out << a_value.as_string();
    }

} /* namespace scarab */
