/*
 * param.hh
 *
 *  Created on: Jan 14, 2014
 *      Author: nsoblath
 */

#ifndef SCARAB_PARAM_VALUE_HH_
#define SCARAB_PARAM_VALUE_HH_

#include "param_base.hh"

#include "path.hh"

#include <stdint.h>


namespace scarab
{
    class param_array;
    class param_node;

    class SCARAB_API param_value : public param
    {
        public:
            param_value();
            param_value( bool a_value );
            param_value( uint8_t a_value );
            param_value( uint16_t a_value );
            param_value( uint32_t a_value );
            param_value( uint64_t a_value );
            param_value( int8_t a_value );
            param_value( int16_t a_value );
            param_value( int32_t a_value );
            param_value( int64_t a_value );
            param_value( float a_value );
            param_value( double a_value );
            param_value( const std::string& a_value );
            param_value( const char* a_value );
            param_value( const param_value& orig );
            virtual ~param_value();

            param_value& operator=( const param_value& rhs );

            virtual param* clone() const;

            bool empty() const;

            virtual bool is_null() const;
            virtual bool is_value() const;

            virtual bool has_subset( const param& a_subset ) const;

            std::string type() const;
            bool is_bool() const;
            bool is_uint() const;
            bool is_int() const;
            bool is_double() const;
            bool is_string() const;

            bool as_bool() const;
            uint64_t as_uint() const;
            int64_t as_int() const;
            double as_double() const;
            const std::string& as_string() const;
            path as_path() const;

            template< typename XValType >
            XValType get() const;

            void set( bool a_value );
            void set( uint8_t a_value );
            void set( uint16_t a_value );
            void set( uint32_t a_value );
            void set( uint64_t a_value );
            void set( int8_t a_value );
            void set( int16_t a_value );
            void set( int32_t a_value );
            void set( int64_t a_value );
            void set( float a_value );
            void set( double a_value );
            void set( const std::string& a_value );
            void set( const char* a_value );
            //template< typename XValType >
            //void set( XValType a_value );

            //template< typename XStreamableType >
            //param_value& operator<<( const XStreamableType& a_streamable );

            virtual std::string to_string() const;

            void clear();

        private:
            union Values
            {
                bool f_bool;
                uint64_t f_uint;
                int64_t f_int;
                double f_double;
                std::string* f_string;
            } f_value;

            enum ValueTypes
            {
                k_bool,
                k_uint,
                k_int,
                k_double,
                k_string,
                k_invalid
            } f_value_type;

            mutable std::string f_buffer;
    };

    SCARAB_API std::ostream& operator<<(std::ostream& out, const param_value& value);

    template<>
    inline bool param_value::get< bool >() const
    {
        return as_bool();
    }

    template<>
    inline uint64_t param_value::get< uint64_t >() const
    {
        return as_uint();
    }

    template<>
    inline int64_t param_value::get< int64_t >() const
    {
        return as_int();
    }

    template<>
    inline double param_value::get< double >() const
    {
        return as_double();
    }

    template<>
    inline std::string param_value::get< std::string >() const
    {
        return as_string();
    }

    template<>
    inline scarab::path param_value::get< scarab::path >() const
    {
        return as_path();
    }

    template< typename XValType >
    XValType param_value::get() const
    {
        if( f_value_type == k_bool ) return static_cast< XValType >( as_bool() );
        else if( f_value_type == k_uint ) return static_cast< XValType >( as_uint() );
        else if( f_value_type == k_int ) return static_cast< XValType >( as_int() );
        else if( f_value_type == k_double ) return static_cast< XValType >( as_double() );
        else if( f_value_type == k_string )
        {
            std::stringstream t_conv;
            t_conv << *f_value.f_string;
            XValType t_return;
            t_conv >> t_return;
            return t_return;
        }
        return XValType();
    }


    inline param* param_value::clone() const
    {
        //std::cout << "param_value::clone" << std::endl;
        return new param_value( *this );
    }

    inline bool param_value::is_null() const
    {
        return false;
    }

    inline bool param_value::is_value() const
    {
        return true;
    }

    inline bool param_value::is_bool() const
    {
        return f_value_type == k_bool;
    }

    inline bool param_value::is_uint() const
    {
        return f_value_type == k_uint;
    }

    inline bool param_value::is_int() const
    {
        return f_value_type == k_int;
    }

    inline bool param_value::is_double() const
    {
        return f_value_type == k_double;
    }

    inline bool param_value::is_string() const
    {
        return f_value_type == k_string;
    }

    inline void param_value::set( bool a_value )
    {
        if( f_value_type == k_string ) delete f_value.f_string;
        f_value_type = k_bool;
        f_value.f_bool = a_value;
        return;
    }

    inline void param_value::set( uint8_t a_value )
    {
        if( f_value_type == k_string ) delete f_value.f_string;
        f_value_type = k_uint;
        f_value.f_uint = a_value;
        return;
    }

    inline void param_value::set( uint16_t a_value )
    {
        if( f_value_type == k_string ) delete f_value.f_string;
        f_value_type = k_uint;
        f_value.f_uint = a_value;
        return;
    }

    inline void param_value::set( uint32_t a_value )
    {
        if( f_value_type == k_string ) delete f_value.f_string;
        f_value_type = k_uint;
        f_value.f_uint = a_value;
        return;
    }

    inline void param_value::set( uint64_t a_value )
    {
        if( f_value_type == k_string ) delete f_value.f_string;
        f_value_type = k_uint;
        f_value.f_uint = a_value;
        return;
    }

    inline void param_value::set( int8_t a_value )
    {
        if( f_value_type == k_string ) delete f_value.f_string;
        f_value_type = k_int;
        f_value.f_int = a_value;
        return;
    }

    inline void param_value::set( int16_t a_value )
    {
        if( f_value_type == k_string ) delete f_value.f_string;
        f_value_type = k_int;
        f_value.f_int = a_value;
        return;
    }

    inline void param_value::set( int32_t a_value )
    {
        if( f_value_type == k_string ) delete f_value.f_string;
        f_value_type = k_int;
        f_value.f_int = a_value;
        return;
    }

    inline void param_value::set( int64_t a_value )
    {
        if( f_value_type == k_string ) delete f_value.f_string;
        f_value_type = k_int;
        f_value.f_int = a_value;
        return;
    }

    inline void param_value::set( float a_value )
    {
        if( f_value_type == k_string ) delete f_value.f_string;
        f_value_type = k_double;
        f_value.f_double = a_value;
        return;
    }

    inline void param_value::set( double a_value )
    {
        if( f_value_type == k_string ) delete f_value.f_string;
        f_value_type = k_double;
        f_value.f_double = a_value;
        return;
    }

    inline void param_value::set( const std::string& a_value )
    {
        if( f_value_type == k_string ) delete f_value.f_string;
        f_value_type = k_string;
        f_value.f_string = new std::string( a_value );
        return;
    }

    inline void param_value::set( const char* a_value )
    {
        if( f_value_type == k_string ) delete f_value.f_string;
        f_value_type = k_string;
        f_value.f_string = new std::string( a_value );
        return;
    }

    inline std::string param_value::to_string() const
    {
        return as_string();
    }
} /* namespace scarab */

#endif /* SCARAB_PARAM_VALUE_HH_ */
