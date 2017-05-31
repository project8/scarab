/*
 * param.hh
 *
 *  Created on: Jan 14, 2014
 *      Author: nsoblath
 */

#ifndef SCARAB_PARAM_HH_
#define SCARAB_PARAM_HH_

#include <stdint.h>
#include <deque>
#include <map>
#include <sstream>
#include <string>

#include "error.hh"
#include "error.hh"
//#include "logger.hh"
#include "path.hh"


namespace scarab
{
    //LOGGER(dlog_p, "param");
    class param_value;
    class param_array;
    class param_node;


    class SCARAB_API param
    {
        public:
            param();
            param(const param& orig);
            virtual ~param();

            virtual param* clone() const;

            virtual bool is_null() const;
            virtual bool is_value() const;
            virtual bool is_array() const;
            virtual bool is_node() const;

            virtual bool has_subset( const param& a_subset ) const;

            param_value& as_value();
            param_array& as_array();
            param_node& as_node();

            const param_value& as_value() const;
            const param_array& as_array() const;
            const param_node& as_node() const;

            /// Assumes that the parameter is a value, and returns a reference to itself.
            const param_value& operator()() const;
            /// Assumes that the parameter is a value, and returns a reference to itself.
            param_value& operator()();

            /// Assumes that the parameter is an array, and returns a reference to the param at a_index.
            /// Behavior is undefined if a_index is out-of-range.
            const param& operator[]( unsigned a_index ) const;
            /// Assumes that the parameter is an array, and returns a reference to the param at a_index.
            /// Behavior is undefined if a_index is out-of-range.
            param& operator[]( unsigned a_index );

            /// Assumes that the parameter is a node, and returns a reference to the param corresponding to a_name.
            /// Throws a scarab::error if a_name is not present.
            const param& operator[]( const std::string& a_name ) const;
            /// Assumes that the parameter is a node, and returns a reference to the param corresponding to a_name.
            /// Throws a scarab::error if a_name is not present.
            param& operator[]( const std::string& a_name );

            virtual std::string to_string() const;

            static unsigned s_indent_level;
    };


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


    class SCARAB_API param_array : public param
    {
        public:
            typedef std::deque< param* > contents;
            typedef contents::iterator iterator;
            typedef contents::const_iterator const_iterator;
            typedef contents::reverse_iterator reverse_iterator;
            typedef contents::const_reverse_iterator const_reverse_iterator;
            typedef contents::value_type contents_type;

        public:
            param_array();
            param_array( const param_array& orig );
            virtual ~param_array();

            param_array& operator=( const param_array& rhs );

            virtual param* clone() const;

            virtual bool is_null() const;
            virtual bool is_array() const;

            virtual bool has_subset( const param& a_subset ) const;

            unsigned size() const;
            bool empty() const;

            /// sets the size of the array
            /// if smaller than the current size, extra elements are deleted
            void resize( unsigned a_size );

            /// Returns the result of ParamValue::get if a_name is present and is of type param_value
            /// Throws an error if a_name is not present or is not of type param_value
            std::string get_value( unsigned a_index ) const;
            /// Returns the result of ParamValue::get if a_name is present and is of type param_value
            /// Throws an error if a_name is not present or is not of type param_value
            template< typename XValType >
            XValType get_value( unsigned a_index ) const;

            /// Returns the result of ParamValue::get if a_name is present and is of type param_value
            /// Returns a_default if a_name is not present or is not of type param_value
            std::string get_value( unsigned a_index, const std::string& a_default ) const;
            std::string get_value( unsigned a_index, const char* a_default ) const;
            /// Returns the result of ParamValue::get if a_name is present and is of type param_value
            /// Returns a_default if a_name is not present or is not of type param_value
            template< typename XValType >
            XValType get_value( unsigned a_index, XValType a_default ) const;

            /// Returns the param corresponding to a_name.
            /// Throws a scarab::error if a_index is out-of-range.
            const param& at( unsigned a_index ) const;
            /// Returns the param corresponding to a_name.
            /// Throws a scarab::error if a_index is out-of-range.
            param& at( unsigned a_index );

            /// Returns the param_value corresponding to a_name.
            /// Throws a scarab::error if a_index is not present
            const param_value& value_at( unsigned a_index ) const;
            /// Returns the param_value corresponding to a_name.
            /// Throws a scarab::error if a_index is out-of-range.
            param_value& value_at( unsigned a_index );

            /// Returns the param_array corresponding to a_name.
            /// Throws a scarab::error if a_index is out-of-range.
            const param_array& array_at( unsigned a_index ) const;
            /// Returns the param_array corresponding to a_name.
            /// Throws a scarab::error if a_index is out-of-range.
            param_array& array_at( unsigned a_index );

            /// Returns the param_node corresponding to a_name.
            /// Throws a scarab::error if a_index is not present
            const param_node& node_at( unsigned a_index ) const;
            /// Returns the param_node corresponding to a_name.
            /// Throws a scarab::error if a_index is out-of-range.
            param_node& node_at( unsigned a_index );

            /// Returns a reference to the param at a_index.
            /// Behavior is undefined if a_index is out-of-range.
            const param& operator[]( unsigned a_index ) const;
            /// Returns a reference to the param at a_index.
            /// Behavior is undefined if a_index is out-of-range.
            param& operator[]( unsigned a_index );

            const param& front() const;
            param& front();

            const param& back() const;
            param& back();

            // assign a copy of a_value to the array at a_index
            void assign( unsigned a_index, const param& a_value );
            // directly assign a_value_ptr to the array at a_index
            void assign( unsigned a_index, param* a_value_ptr );

            void push_back( const param& a_value );
            void push_back( param* a_value_ptr );

            void push_front( const param& a_value );
            void push_front( param* a_value_ptr );

            void append( const param_array& an_array );

            void erase( unsigned a_index );
            param* remove( unsigned a_index );
            void clear();

            iterator begin();
            const_iterator begin() const;

            iterator end();
            const_iterator end() const;

            reverse_iterator rbegin();
            const_reverse_iterator rbegin() const;

            reverse_iterator rend();
            const_reverse_iterator rend() const;

            virtual std::string to_string() const;

        protected:
            contents f_contents;
    };



    class SCARAB_API param_node : public param
    {
        public:
            typedef std::map< std::string, param* > contents;
            typedef contents::iterator iterator;
            typedef contents::const_iterator const_iterator;
            typedef contents::value_type contents_type;

            param_node();
            param_node( const param_node& orig );
            virtual ~param_node();

            param_node& operator=( const param_node& rhs );

            virtual param* clone() const;

            virtual bool is_null() const;
            virtual bool is_node() const;

            virtual bool has_subset( const param& a_subset ) const;

            unsigned size() const;
            bool empty() const;

            bool has( const std::string& a_name ) const;
            unsigned count( const std::string& a_name ) const;

            /// Returns the result of param_value::get if a_name is present and is of type param_value
            /// Throws an error if a_name is not present or is not of type param_value
            std::string get_value( const std::string& a_name ) const;
            /// Returns the result of ParamValue::get if a_name is present and is of type param_value
            /// Throws an error if a_name is not present or is not of type param_value
            template< typename XValType >
            XValType get_value( const std::string& a_name ) const;

            /// Returns the result of param_value::get if a_name is present and is of type param_value
            /// Returns a_default if a_name is not present or is not of type param_value
            std::string get_value( const std::string& a_name, const std::string& a_default ) const;
            std::string get_value( const std::string& a_name, const char* a_default ) const;
            /// Returns the result of ParamValue::get if a_name is present and is of type param_value
            /// Returns a_default if a_name is not present or is not of type param_value
            template< typename XValType >
            XValType get_value( const std::string& a_name, XValType a_default ) const;

            /// Returns the param corresponding to a_name.
            /// Throws a scarab::error if a_name is not present.
            const param& at( const std::string& a_name ) const;
            /// Returns the param corresponding to a_name.
            /// Throws a scarab::error if a_name is not present.
            param& at( const std::string& a_name );

            /// Returns the param_value corresponding to a_name.
            /// Throws a scarab::error if a_name is not present.
            const param_value& value_at( const std::string& a_name ) const;
            /// Returns the param_value corresponding to a_name.
            /// Throws a scarab::error if a_name is not present.
            param_value& value_at( const std::string& a_name );

            /// Returns the param_array corresponding to a_name.
            /// Throws a scarab::error if a_name is not present.
            const param_array& array_at( const std::string& a_name ) const;
            /// Returns the param_array corresponding to a_name.
            /// Throws a scarab::error if a_name is not present.
            param_array& array_at( const std::string& a_name );

            /// Returns the param_node corresponding to a_name.
            /// Throws a scarab::error if a_name is not present.
            const param_node& node_at( const std::string& a_name ) const;
            /// Returns the param_node corresponding to a_name.
            /// Throws a scarab::error if a_name is not present.
            param_node& node_at( const std::string& a_name );

            /// Returns a reference to the param corresponding to a_name.
            /// Throws an error if a_name is not present.
            const param& operator[]( const std::string& a_name ) const;
            /// Returns a reference to the param corresponding to a_name.
            /// Adds a new value if a_name is not present.
            param& operator[]( const std::string& a_name );

            /// creates a copy of a_value
            bool add( const std::string& a_name, const param& a_value );
            /// directly adds (without copying) a_value_ptr
            bool add( const std::string& a_name, param* a_value_ptr );

            /// creates a copy of a_value
            void replace( const std::string& a_name, const param& a_value );
            /// directly adds (without copying) a_value_ptr
            void replace( const std::string& a_name, param* a_value_ptr );

            /// Merges the contents of a_object into this object.
            /// If names in the contents of a_object exist in this object,
            /// the values in this object corresponding to the matching names will be replaced.
            void merge( const param_node& a_object );

            void erase( const std::string& a_name );
            param* remove( const std::string& a_name );
            void clear();

            iterator begin();
            const_iterator begin() const;

            iterator end();
            const_iterator end() const;

            virtual std::string to_string() const;

        protected:
            contents f_contents;

    };

    inline param* param::clone() const
    {
        //std::cout << "param::clone()" << std::endl;
        return new param( *this );
    }

    inline bool param::is_null() const
    {
        return true;
    }

    inline bool param::is_value() const
    {
        return false;
    }

    inline bool param::is_array() const
    {
        return false;
    }

    inline bool param::is_node() const
    {
        return false;
    }

    inline param_value& param::as_value()
    {
        if( this->is_value() ) return *static_cast< param_value* >( this);
        throw error() << "Param object is not a value";
    }

    inline param_array& param::as_array()
    {
        if( this->is_value() ) return *static_cast< param_array* >( this);
        throw error() << "Param object is not an array";
    }

    inline param_node& param::as_node()
    {
        if( this->is_value() ) return *static_cast< param_node* >( this);
        throw error() << "Param object is not a node";
    }

    inline const param_value& param::as_value() const
    {
        if( this->is_value() ) return *static_cast< const param_value* >( this);
        throw error() << "Param object is not a value";
    }

    inline const param_array& param::as_array() const
    {
        if( this->is_value() ) return *static_cast< const param_array* >( this);
        throw error() << "Param object is not an array";
    }

    inline const param_node& param::as_node() const
    {
        if( this->is_value() ) return *static_cast< const param_node* >( this);
        throw error() << "Param object is not a node";
    }

    inline const param_value& param::operator()() const
    {
        return as_value();
    }

    inline param_value& param::operator()()
    {
        return as_value();
    }

    inline const param& param::operator[]( unsigned a_index ) const
    {
        return as_array()[ a_index ];
    }

    inline param& param::operator[]( unsigned a_index )
    {
        return as_array()[ a_index ];
    }

    inline const param& param::operator[]( const std::string& a_name ) const
    {
        return as_node()[ a_name ];
    }

    inline param& param::operator[]( const std::string& a_name )
    {
        return as_node()[ a_name ];
    }

    inline std::string param::to_string() const
    {
        return std::string();
    }


    //************************************
    //***********  VALUE  ****************
    //************************************


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



    //************************************
    //***********  ARRAY  ****************
    //************************************

    template< typename XValType >
    XValType param_array::get_value( unsigned a_index ) const
    {
        return value_at( a_index ).get< XValType >();
    }

    template< typename XValType >
    XValType param_array::get_value( unsigned a_index, XValType a_default ) const
    {
        return a_index < size() ? value_at( a_index ).get< XValType >() : a_default;
    }

    inline param* param_array::clone() const
    {
        return new param_array( *this );
    }

    inline bool param_array::is_null() const
    {
        return false;
    }

    inline bool param_array::is_array() const
    {
        return true;
    }

    inline unsigned param_array::size() const
    {
        return f_contents.size();
    }
    inline bool param_array::empty() const
    {
        return f_contents.empty();
    }

    inline std::string param_array::get_value( unsigned a_index ) const
    {
        return value_at( a_index ).to_string();
    }

    inline std::string param_array::get_value( unsigned a_index, const std::string& a_default ) const
    {
        return a_index < size() ? value_at( a_index ).to_string() : a_default;
    }

    inline std::string param_array::get_value( unsigned a_index, const char* a_default ) const
    {
        return get_value( a_index, std::string( a_default ) );
    }

    inline const param& param_array::at( unsigned a_index ) const
    {
        return *f_contents.at( a_index );
    }
    inline param& param_array::at( unsigned a_index )
    {
        return *f_contents.at( a_index );
    }

    inline const param_value& param_array::value_at( unsigned a_index ) const
    {
        return at( a_index ).as_value();
    }
    inline param_value& param_array::value_at( unsigned a_index )
    {
        return at( a_index ).as_value();
    }

    inline const param_array& param_array::array_at( unsigned a_index ) const
    {
        return at( a_index ).as_array();
    }
    inline param_array& param_array::array_at( unsigned a_index )
    {
        return at( a_index ).as_array();
    }

    inline const param_node& param_array::node_at( unsigned a_index ) const
    {
        return at( a_index ).as_node();
    }
    inline param_node& param_array::node_at( unsigned a_index )
    {
        return at( a_index ).as_node();
    }

    inline const param& param_array::operator[]( unsigned a_index ) const
    {
        return *f_contents[ a_index ];
    }
    inline param& param_array::operator[]( unsigned a_index )
    {
        return *f_contents[ a_index ];
    }

    inline const param& param_array::front() const
    {
        return *f_contents.front();
    }
    inline param& param_array::front()
    {
        return *f_contents.front();
    }

    inline const param& param_array::back() const
    {
        return *f_contents.back();
    }
    inline param& param_array::back()
    {
        return *f_contents.back();
    }

    // assign a copy of a_value to the array at a_index
    inline void param_array::assign( unsigned a_index, const param& a_value )
    {
        erase( a_index );
        f_contents[ a_index ] = a_value.clone();
        return;
    }
    // directly assign a_value_ptr to the array at a_index
    inline void param_array::assign( unsigned a_index, param* a_value_ptr )
    {
        erase( a_index );
        f_contents[ a_index ] = a_value_ptr;
        return;
    }

    inline void param_array::push_back( const param& a_value )
    {
        f_contents.push_back( a_value.clone() );
        return;
    }
    inline void param_array::push_back( param* a_value_ptr )
    {
        f_contents.push_back( a_value_ptr );
        return;
    }

    inline void param_array::push_front( const param& a_value )
    {
        f_contents.push_front( a_value.clone() );
        return;
    }
    inline void param_array::push_front( param* a_value_ptr )
    {
        f_contents.push_front( a_value_ptr );
        return;
    }

    inline void param_array::append( const param_array& an_array )
    {
        for( param_array::const_iterator it = an_array.begin(); it != an_array.end(); ++it )
        {
            push_back( *(*it) );
        }
        return;
    }

    inline void param_array::erase( unsigned a_index )
    {
        delete f_contents[ a_index ];
        return;
    }
    inline param* param_array::remove( unsigned a_index )
    {
        param* t_current = f_contents[ a_index ];
        f_contents[ a_index ] = NULL;
        return t_current;
    }
    inline void param_array::clear()
    {
        for( unsigned ind = 0; ind < f_contents.size(); ++ind )
        {
            delete f_contents[ ind ];
        }
        f_contents.clear();
        return;
    }

    inline param_array::iterator param_array::begin()
    {
        return f_contents.begin();
    }
    inline param_array::const_iterator param_array::begin() const
    {
        return f_contents.begin();
    }

    inline param_array::iterator param_array::end()
    {
        return f_contents.end();
    }
    inline param_array::const_iterator param_array::end() const
    {
        return f_contents.end();
    }

    inline param_array::reverse_iterator param_array::rbegin()
    {
        return f_contents.rbegin();
    }
    inline param_array::const_reverse_iterator param_array::rbegin() const
    {
        return f_contents.rbegin();
    }

    inline param_array::reverse_iterator param_array::rend()
    {
        return f_contents.rend();
    }
    inline param_array::const_reverse_iterator param_array::rend() const
    {
        return f_contents.rend();
    }



    //************************************
    //***********  NODE  *****************
    //************************************


    template< typename XValType >
    inline XValType param_node::get_value( const std::string& a_name ) const
    {
        return value_at( a_name ).get< XValType >();
    }

    template< typename XValType >
    inline XValType param_node::get_value( const std::string& a_name, XValType a_default ) const
    {
        return has( a_name ) ? value_at( a_name ).get< XValType >() : a_default;
    }

    inline param* param_node::clone() const
    {
        //std::cout << "param_node::clone" << std::endl;
        return new param_node( *this );
    }

    inline bool param_node::is_null() const
    {
        return false;
    }

    inline bool param_node::is_node() const
    {
        return true;
    }

    inline unsigned param_node::size() const
    {
        return f_contents.size();
    }
    inline bool param_node::empty() const
    {
        return f_contents.empty();
    }

    inline bool param_node::has( const std::string& a_name ) const
    {
        return f_contents.count( a_name ) > 0;
    }

    inline unsigned param_node::count( const std::string& a_name ) const
    {
        return f_contents.count( a_name );
    }

    inline std::string param_node::get_value( const std::string& a_name ) const
    {
        return value_at( a_name ).to_string();
    }

    inline std::string param_node::get_value( const std::string& a_name, const std::string& a_default ) const
    {
        return has( a_name ) ? value_at( a_name ).to_string() : a_default;
    }

    inline std::string param_node::get_value( const std::string& a_name, const char* a_default ) const
    {
        return get_value( a_name, std::string( a_default ) );
    }

    inline const param& param_node::at( const std::string& a_name ) const
    {
        return *f_contents.at( a_name );
    }

    inline param& param_node::at( const std::string& a_name )
    {
        return *f_contents.at( a_name );
    }

    inline const param_value& param_node::value_at( const std::string& a_name ) const
    {
        return at( a_name ).as_value();
    }

    inline param_value& param_node::value_at( const std::string& a_name )
    {
        return at( a_name ).as_value();
    }

    inline const param_array& param_node::array_at( const std::string& a_name ) const
    {
        return at( a_name ).as_array();
    }

    inline param_array& param_node::array_at( const std::string& a_name )
    {
        return at( a_name ).as_array();
    }

    inline const param_node& param_node::node_at( const std::string& a_name ) const
    {
        return at( a_name ).as_node();
    }

    inline param_node& param_node::node_at( const std::string& a_name )
    {
        return at( a_name ).as_node();
    }

    inline const param& param_node::operator[]( const std::string& a_name ) const
    {
        return *f_contents.at( a_name );
    }

    inline param& param_node::operator[]( const std::string& a_name )
    {
        return *f_contents[ a_name ];
    }

    inline bool param_node::add( const std::string& a_name, const param& a_value )
    {
        iterator it = f_contents.find( a_name );
        if( it == f_contents.end() )
        {
            f_contents.insert( contents_type( a_name, a_value.clone() ) );
            return true;
        }
        return false;
    }

    inline bool param_node::add( const std::string& a_name, param* a_value )
    {
        iterator it = f_contents.find( a_name );
        if( it == f_contents.end() )
        {
            f_contents.insert( contents_type( a_name, a_value ) );
            return true;
        }
        return false;
    }

    inline void param_node::replace( const std::string& a_name, const param& a_value )
    {
        erase( a_name );
        f_contents[ a_name ] = a_value.clone();
        return;
    }

    inline void param_node::replace( const std::string& a_name, param* a_value )
    {
        erase( a_name );
        f_contents[ a_name ] = a_value;
        return;
    }

    inline void param_node::erase( const std::string& a_name )
    {
        iterator it = f_contents.find( a_name );
        if( it != f_contents.end() )
        {
            delete it->second;
            f_contents.erase( it );
        }
        return;
    }

    inline param* param_node::remove( const std::string& a_name )
    {
        iterator it = f_contents.find( a_name );
        if( it != f_contents.end() )
        {
            param* removed = it->second;
            f_contents.erase( it );
            return removed;
        }
        return NULL;
    }

    inline void param_node::clear()
    {
        for( iterator it = f_contents.begin(); it != f_contents.end(); ++it )
        {
            delete it->second;
        }
        f_contents.clear();
        return;
    }

    inline param_node::iterator param_node::begin()
    {
        return f_contents.begin();
    }

    inline param_node::const_iterator param_node::begin() const
    {
        return f_contents.begin();
    }

    inline param_node::iterator param_node::end()
    {
        return f_contents.end();
    }

    inline param_node::const_iterator param_node::end() const
    {
        return f_contents.end();
    }



    SCARAB_API std::ostream& operator<<(std::ostream& out, const param& value);
    SCARAB_API std::ostream& operator<<(std::ostream& out, const param_value& value);
    SCARAB_API std::ostream& operator<<(std::ostream& out, const param_array& value);
    SCARAB_API std::ostream& operator<<(std::ostream& out, const param_node& value);


} /* namespace scarab */

#endif /* SCARAB_PARAM_HH_ */
