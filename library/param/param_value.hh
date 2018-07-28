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

//#include "logger.hh"
//LOGGER(pv_h, "param_value.hh")

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
            std::string as_string() const;
            path as_path() const;

            template< typename XValType >
            XValType as() const;

            template< typename XValType, typename std::enable_if< std::is_convertible< XValType, param_value >::value, XValType >::type* = nullptr >
            void set( XValType a_value );

            //template< typename XStreamableType >
            //param_value& operator<<( const XStreamableType& a_streamable );

            virtual std::string to_string() const;

            void clear();

        private:
            boost::variant< bool, uint64_t, int64_t, double, std::string > f_value;

            //*********************
            // Visitor Classes
            //*********************

            class type_visitor : public boost::static_visitor<>
            {
                public:
                    typedef std::string result_type;
                    std::string operator()( bool ) const
                    {
                        return "bool";
                    }
                    std::string operator()( uint64_t ) const
                    {
                        return "uint";
                    }
                    std::string operator()( int64_t ) const
                    {
                        return "int";
                    }
                    std::string operator()( double ) const
                    {
                        return "double";
                    }
                    std::string operator()( const std::string& ) const
                    {
                        return "string";
                    }
            };

            template< typename XValType >
            class get_visitor : public boost::static_visitor<>
            {
                public:
                    typedef XValType result_type;
                    XValType operator()( bool a_value ) const
                    {
                        return static_cast< XValType >( a_value );
                    }
                    XValType operator()( uint64_t a_value ) const
                    {
                        return static_cast< XValType >( a_value );
                    }
                    XValType operator()( int64_t a_value ) const
                    {
                        return static_cast< XValType >( a_value );
                    }
                    XValType operator()( double a_value ) const
                    {
                        return static_cast< XValType >( a_value );
                    }
                    XValType operator()( const std::string& a_value ) const
                    {
                        std::stringstream t_conv;
                        t_conv << a_value;
                        XValType t_return;
                        t_conv >> t_return;
                        return t_return;
                    }
            };

            class is_bool_visitor : public boost::static_visitor<>
            {
                public:
                    typedef bool result_type;
                    bool operator()( bool ) const
                    {
                        return true;
                    }
                    template< typename T >
                    bool operator()( T ) const
                    {
                        return false;
                    }
            };

            class is_int_visitor : public boost::static_visitor<>
            {
                public:
                    typedef bool result_type;
                    bool operator()( int64_t ) const
                    {
                        return true;
                    }
                    template< typename T >
                    bool operator()( T ) const
                    {
                        return false;
                    }
            };

            class is_uint_visitor : public boost::static_visitor<>
            {
                public:
                    typedef bool result_type;
                    bool operator()( uint64_t ) const
                    {
                        return true;
                    }
                    template< typename T >
                    bool operator()( T ) const
                    {
                        return false;
                    }
            };

            class is_double_visitor : public boost::static_visitor<>
            {
                public:
                    typedef bool result_type;
                    bool operator()( double ) const
                    {
                        return true;
                    }
                    template< typename T >
                    bool operator()( T ) const
                    {
                        return false;
                    }
            };

            class is_string_visitor : public boost::static_visitor<>
            {
                public:
                    typedef bool result_type;
                    bool operator()( const std::string& ) const
                    {
                        return true;
                    }
                    template< typename T >
                    bool operator()( T ) const
                    {
                        return false;
                    }
            };

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
                        return std::stoull( a_value );
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
                        return std::stoll( a_value );
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
                        return std::stod( a_value );
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
                    typedef std::string result_type;
                    std::string operator()( bool a_value ) const
                    {
                        return a_value ? "true" : "false";
                    }
                    std::string operator()( const std::string& a_value ) const
                    {
                        return a_value;
                    }
                    template< typename T >
                    std::string operator()( T a_value ) const
                    {
                        return std::to_string( a_value );
                    }
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
    inline bool param_value::as< bool >() const
    {
        return as_bool();
    }

    template<>
    inline uint64_t param_value::as< uint64_t >() const
    {
        return as_uint();
    }

    template<>
    inline int64_t param_value::as< int64_t >() const
    {
        return as_int();
    }

    template<>
    inline double param_value::as< double >() const
    {
        return as_double();
    }

    template<>
    inline std::string param_value::as< std::string >() const
    {
        return as_string();
    }

    template<>
    inline scarab::path param_value::as< scarab::path >() const
    {
        return as_path();
    }

    template< typename XValType >
    XValType param_value::as() const
    {
        return boost::apply_visitor( get_visitor< XValType >(), f_value );
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

    inline std::string param_value::type() const
    {
        return boost::apply_visitor( type_visitor(), f_value );
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
        return boost::apply_visitor( is_bool_visitor(), f_value );
    }

    inline bool param_value::is_uint() const
    {
        return boost::apply_visitor( is_uint_visitor(), f_value );
    }

    inline bool param_value::is_int() const
    {
        return boost::apply_visitor( is_int_visitor(), f_value );
    }

    inline bool param_value::is_double() const
    {
        return boost::apply_visitor( is_double_visitor(), f_value );
    }

    inline bool param_value::is_string() const
    {
        return boost::apply_visitor( is_string_visitor(), f_value );
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

    inline std::string param_value::as_string() const
    {
        return boost::apply_visitor( as_string_visitor(), f_value );
    }

    inline path param_value::as_path() const
    {
        return boost::apply_visitor( as_path_visitor(), f_value );
    }

    template< typename XValType, typename std::enable_if< std::is_convertible< XValType, param_value >::value, XValType >::type* >
    void param_value::set( XValType a_value )
    {
        f_value = a_value;
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
