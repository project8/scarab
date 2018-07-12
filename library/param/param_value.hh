/*
 * param_value.hh
 *
 *  Created on: Jan 14, 2014
 *      Author: nsoblath
 */

#ifndef SCARAB_PARAM_VALUE_HH_
#define SCARAB_PARAM_VALUE_HH_

#include "param_base.hh"

#include "path.hh"

#include <boost/variant.hpp>

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
            param_value( param_value&& orig );
            virtual ~param_value();

            param_value& operator=( const param_value& rhs );
            param_value& operator=( param_value&& rhs );

            virtual param_ptr_t clone() const;
            virtual param_ptr_t move_clone();

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
            boost::variant< bool, uint64_t, int64_t, double, std::string > f_value;
            /*
            union Values
            {
                bool f_bool;
                uint64_t f_uint;
                int64_t f_int;
                double f_double;
                std::unique_ptr< std::string > f_string;
                Values() {f_bool = false;}
                ~Values() {}
            } f_value;
             */
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

            //*********************
            // Visitor Classes
            //*********************

            class as_bool_visitor : public boost::static_visitor<>
            {
                public:
                    typedef bool result_type;
                    bool operator()( bool a_value ) const
                    {
                        return a_value;
                    }
                    bool operator()( const std::string& a_value ) const
                    {
                        if( a_value.empty() ) return false;

                        std::string t_str_val;
                        bool t_is_numeric = true;
                        for( std::string::const_iterator t_val_it = a_value.begin(); t_val_it != a_value.end(); ++t_val_it )
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
                    template< typename T >
                    bool operator()( T a_value ) const
                    {
                        return a_value != 0;
                    }
            };

            class as_uint_visitor : public boost::static_visitor<>
            {
                public:
                    typedef uint64_t result_type;
                    uint64_t operator()( const std::string& a_value ) const
                    {
                        std::stringstream t_conv;
                        t_conv << a_value;
                        uint64_t t_return;
                        t_conv >> t_return;
                        return t_return;
                    }
                    template< typename T >
                    uint64_t operator()( T a_value ) const
                    {
                        return (uint64_t)a_value;
                    }
            };

            class as_int_visitor : public boost::static_visitor<>
            {
                public:
                    typedef int64_t result_type;
                    int64_t operator()( const std::string& a_value ) const
                    {
                        std::stringstream t_conv;
                        t_conv << a_value;
                        int64_t t_return;
                        t_conv >> t_return;
                        return t_return;
                    }
                    template< typename T >
                    int64_t operator()( T a_value ) const
                    {
                        return (int64_t)a_value;
                    }
            };

            class as_double_visitor : public boost::static_visitor<>
            {
                public:
                    typedef double result_type;
                    double operator()( const std::string& a_value ) const
                    {
                        std::stringstream t_conv;
                        t_conv << a_value;
                        double t_return;
                        t_conv >> t_return;
                        return t_return;
                    }
                    template< typename T >
                    double operator()( T a_value ) const
                    {
                        return (double)a_value;
                    }
            };

            class as_string_visitor : public boost::static_visitor<>
            {
                public:
                    typedef const std::string& result_type;
                    const std::string& operator()( bool a_value ) const
                    {
                        f_buffer = a_value ? "true" : "false";
                        return f_buffer;
                    }
                    const std::string& operator()( const std::string& a_value ) const
                    {
                        return a_value;
                    }
                    template< typename T >
                    const std::string& operator()( T a_value ) const
                    {
                        std::stringstream t_conv;
                        t_conv << a_value;
                        t_conv >> f_buffer;
                        return f_buffer;
                    }
                private:
                    mutable std::string f_buffer;
            };

            class as_path_visitor : public boost::static_visitor<>
            {
                public:
                    typedef scarab::path result_type;
                    scarab::path operator()( const std::string& a_value ) const
                    {
                        return scarab::path( a_value );
                    }
                    template< typename T >
                    scarab::path operator()( T ) const
                    {
                        return scarab::path();
                    }
            };

            class clear_visitor : public boost::static_visitor<>
            {
                public:
                    typedef void result_type;
                    void operator()( bool& a_value ) const
                    {
                        a_value = false;
                    }
                    void operator()( std::string& a_value ) const
                    {
                        a_value.clear();
                    }
                    template< typename T >
                    void operator()( T& a_value ) const
                    {
                        a_value = 0;
                    }
            };


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
            t_conv << boost::get< std::string >( f_value );
            XValType t_return;
            t_conv >> t_return;
            return t_return;
        }
        return XValType();
    }


    inline param_ptr_t param_value::clone() const
    {
        //std::cout << "param_value::clone" << std::endl;
        return param_ptr_t( new param_value( *this ) );
    }

    inline param_ptr_t param_value::move_clone()
    {
        return param_ptr_t( new param_value( std::move(*this) ) );
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

    inline bool param_value::as_bool() const
    {
        return boost::apply_visitor( as_bool_visitor(), f_value );
    }

    inline uint64_t param_value::as_uint() const
    {
        return boost::apply_visitor( as_uint_visitor(), f_value );
    }

    inline int64_t param_value::as_int() const
    {
        return boost::apply_visitor( as_int_visitor(), f_value );
    }

    inline double param_value::as_double() const
    {
        return boost::apply_visitor( as_double_visitor(), f_value );
    }

    inline const std::string& param_value::as_string() const
    {
        return boost::apply_visitor( as_string_visitor(), f_value );
    }

    inline path param_value::as_path() const
    {
        return boost::apply_visitor( as_path_visitor(), f_value );
    }

    inline void param_value::set( bool a_value )
    {
        f_value_type = k_bool;
        f_value = a_value;
        return;
    }

    inline void param_value::set( uint8_t a_value )
    {
        f_value_type = k_uint;
        f_value = uint64_t(a_value);
        return;
    }

    inline void param_value::set( uint16_t a_value )
    {
        f_value_type = k_uint;
        f_value = uint64_t(a_value);
        return;
    }

    inline void param_value::set( uint32_t a_value )
    {
        f_value_type = k_uint;
        f_value = uint64_t(a_value);
        return;
    }

    inline void param_value::set( uint64_t a_value )
    {
        f_value_type = k_uint;
        f_value = a_value;
        return;
    }

    inline void param_value::set( int8_t a_value )
    {
        f_value_type = k_int;
        f_value = int64_t(a_value);
        return;
    }

    inline void param_value::set( int16_t a_value )
    {
        f_value_type = k_int;
        f_value = int64_t(a_value);
        return;
    }

    inline void param_value::set( int32_t a_value )
    {
        f_value_type = k_int;
        f_value = int64_t(a_value);
        return;
    }

    inline void param_value::set( int64_t a_value )
    {
        f_value_type = k_int;
        f_value = a_value;
        return;
    }

    inline void param_value::set( float a_value )
    {
        f_value_type = k_double;
        f_value = a_value;
        return;
    }

    inline void param_value::set( double a_value )
    {
        f_value_type = k_double;
        f_value = a_value;
        return;
    }

    inline void param_value::set( const std::string& a_value )
    {
        f_value_type = k_string;
        f_value = a_value;
        return;
    }

    inline void param_value::set( const char* a_value )
    {
        f_value_type = k_string;
        f_value = std::string( a_value );
        return;
    }

    inline std::string param_value::to_string() const
    {
        return as_string();
    }

    inline void param_value::clear()
    {
        boost::apply_visitor( clear_visitor(), f_value );
        return;
    }

} /* namespace scarab */

#endif /* SCARAB_PARAM_VALUE_HH_ */
