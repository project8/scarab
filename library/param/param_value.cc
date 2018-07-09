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


namespace scarab
{

    param_value::param_value() :
            param(),
            f_value_type( k_invalid ),
            f_buffer()
    {
        //LWARN( dlog, "param_value constructor: k_invalid" );
    }

    param_value::param_value( bool a_value ) :
            param(),
            f_value_type( k_bool ),
            f_buffer()
    {
        f_value.f_bool = a_value;
        //LWARN( dlog, "param_value constructor: bool --> bool" );
    }

    param_value::param_value( uint8_t a_value ) :
            param(),
            f_value_type( k_uint ),
            f_buffer()
    {
        f_value.f_uint = a_value;
        //LWARN( dlog, "param_value constructor: uint8 --> uint" );
    }

    param_value::param_value( uint16_t a_value ) :
            param(),
            f_value_type( k_uint ),
            f_buffer()
    {
        f_value.f_uint = a_value;
        //LWARN( dlog, "param_value constructor: uint16 --> uint" );
    }

    param_value::param_value( uint32_t a_value ) :
            param(),
            f_value_type( k_uint ),
            f_buffer()
    {
        f_value.f_uint = a_value;
        //LWARN( dlog, "param_value constructor: uint32 --> uint" );
    }

    param_value::param_value( uint64_t a_value ) :
            param(),
            f_value_type( k_uint ),
            f_buffer()
    {
        f_value.f_uint = a_value;
        //LWARN( dlog, "param_value constructor: uint64 --> uint" );
    }

    param_value::param_value( int8_t a_value ) :
            param(),
            f_value_type( k_int ),
            f_buffer()
    {
        f_value.f_int = a_value;
        //LWARN( dlog, "param_value constructor: int8 --> int" );
    }

    param_value::param_value( int16_t a_value ) :
            param(),
            f_value_type( k_int ),
            f_buffer()
    {
        f_value.f_int = a_value;
        //LWARN( dlog, "param_value constructor: int16 --> int" );
    }


    param_value::param_value( int32_t a_value ) :
            param(),
            f_value_type( k_int ),
            f_buffer()
    {
        f_value.f_int = a_value;
        //LWARN( dlog, "param_value constructor: int32 --> int" );
    }

    param_value::param_value( int64_t a_value ) :
            param(),
            f_value_type( k_int ),
            f_buffer()
    {
        f_value.f_int = a_value;
        //LWARN( dlog, "param_value constructor: int64 --> int" );
    }

    param_value::param_value( float a_value ) :
            param(),
            f_value_type( k_double ),
            f_buffer()
    {
        f_value.f_double = a_value;
        //LWARN( dlog, "param_value constructor: float --> double" );
    }

    param_value::param_value( double a_value ) :
            param(),
            f_value_type( k_double ),
            f_buffer()
    {
        f_value.f_double = a_value;
        //LWARN( dlog, "param_value constructor: double --> double" );
    }

    param_value::param_value( const char* a_value ) :
            param(),
            f_value_type( k_string ),
            f_buffer()
    {
        f_value.f_string = new string( a_value );
        //LWARN( dlog, "param_value constructor: char* --> k_string" );
    }

    param_value::param_value( const string& a_value ) :
            param(),
            f_value_type( k_string ),
            f_buffer()
    {
        f_value.f_string = new string( a_value );
        //LWARN( dlog, "param_value constructor: string --> k_string" );
    }

    param_value::param_value( const param_value& orig ) :
            param( orig ),
            f_value( orig.f_value ),
            f_value_type( orig.f_value_type ),
            f_buffer()
    {
        if( f_value_type == k_string )
        {
            f_value.f_string = new string( *orig.f_value.f_string );
        }
        //LWARN( dlog, "param_value copy constructor: " << type() );
    }

    param_value::param_value( param_value&& orig ) :
            param( std::move( orig ) ),
            f_value( std::move( orig.f_value ) ),
            f_value_type( std::move( orig.f_value_type ) ),
            f_buffer()
    {
    }

    param_value::~param_value()
    {
    }

    param_value& param_value::operator=( const param_value& rhs )
    {
        if( &rhs == this ) return *this;

        f_value = rhs.f_value;
        f_value_type = rhs.f_value_type;

        return *this;
    }

    param_value& param_value::operator=( param_value&& rhs )
    {
        if( &rhs == this ) return *this;

        f_value = std::move( rhs.f_value );
        f_value_type = std::move( rhs.f_value_type );

        return *this;
    }

    std::string param_value::type() const
    {
        switch( f_value_type )
        {
            case k_invalid:
                return string( "invalid" );
                break;
            case k_bool:
                return string( "bool" );
                break;
            case k_uint:
                return string( "uint" );
                break;
            case k_int:
                return string( "int" );
               break;
            case k_double:
                return string( "double" );
                break;
            case k_string:
                return string( "string" );
                break;
        }
        return string( "unknown" );
    }

    bool param_value::as_bool() const
    {
        if( f_value_type == k_bool ) return f_value.f_bool;
        else if( f_value_type == k_uint ) return f_value.f_uint != 0;
        else if( f_value_type == k_int ) return f_value.f_int != 0;
        else if( f_value_type == k_double ) return f_value.f_double != 0.;
        else if( f_value_type == k_string )
        {
            if( f_value.f_string->empty() ) return false;

            std::string t_str_val;
            bool t_is_numeric = true;
            for( std::string::const_iterator t_val_it = f_value.f_string->begin(); t_val_it != f_value.f_string->end(); ++t_val_it )
            {
                t_is_numeric = t_is_numeric && ::isdigit( *t_val_it );
                t_str_val.push_back( ::tolower( *t_val_it ) );
            }

            if( t_is_numeric ) return std::stoi( t_str_val );

            std::istringstream t_iss_val( t_str_val );
            bool t_bool_val;
            t_iss_val >> std::boolalpha >> t_bool_val;
            return t_bool_val;
        }
        return false;
    }

    uint64_t param_value::as_uint() const
    {
        if( f_value_type == k_bool ) return (uint64_t)f_value.f_bool;
        else if( f_value_type == k_uint ) return f_value.f_uint;
        else if( f_value_type == k_int ) return (uint64_t)f_value.f_int;
        else if( f_value_type == k_double ) return (uint64_t)f_value.f_double;
        else if( f_value_type == k_string )
        {
            std::stringstream t_conv;
            t_conv << *f_value.f_string;
            uint64_t t_return;
            t_conv >> t_return;
            return t_return;
        }
        return 0.;
    }
    int64_t param_value::as_int() const
    {
        if( f_value_type == k_bool ) return (int64_t)f_value.f_bool;
        else if( f_value_type == k_uint ) return (int64_t)f_value.f_uint;
        else if( f_value_type == k_int ) return f_value.f_int;
        else if( f_value_type == k_double ) return (int64_t)f_value.f_double;
        else if( f_value_type == k_string )
        {
            std::stringstream t_conv;
            t_conv << *f_value.f_string;
            int64_t t_return;
            t_conv >> t_return;
            return t_return;
        }
        return 0.;
    }
    double param_value::as_double() const
    {
        if( f_value_type == k_bool ) return f_value.f_bool;
        else if( f_value_type == k_uint ) return (double)f_value.f_uint;
        else if( f_value_type == k_int ) return (double)f_value.f_int;
        else if( f_value_type == k_double ) return f_value.f_double;
        else if( f_value_type == k_string )
        {
            std::stringstream t_conv;
            t_conv << *f_value.f_string;
            double t_return;
            t_conv >> t_return;
            return t_return;
        }
        return 0.;
    }
    const string& param_value::as_string() const
    {
        if( f_value_type == k_string ) return *f_value.f_string;

        std::stringstream t_conv;
        if( f_value_type == k_bool ) t_conv << (as_bool() ? "true" : "false");
        else if( f_value_type == k_uint ) t_conv << as_uint();
        else if( f_value_type == k_int ) t_conv << as_int();
        else if( f_value_type == k_double ) t_conv << as_double();

        t_conv >> f_buffer;
        return f_buffer;
    }

    path param_value::as_path() const
    {
        if( f_value_type == k_string ) return path( *f_value.f_string );
        return path();
    }

    void param_value::clear()
    {
        if( f_value_type == k_bool ) f_value.f_bool = false;
        else if( f_value_type == k_uint ) f_value.f_uint = 0;
        else if( f_value_type == k_int ) f_value.f_int = 0;
        else if( f_value_type == k_double ) f_value.f_double = 0.;
        else if( f_value_type == k_string ) f_value.f_string->clear();
        return;
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
