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

} /* namespace scarab */

#endif /* SCARAB_PARAM_VALUE_HH_ */
