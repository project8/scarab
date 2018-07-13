/*
 * param_array.hh
 *
 *  Created on: Jan 14, 2014
 *      Author: nsoblath
 */

#ifndef SCARAB_PARAM_ARRAY_HH_
#define SCARAB_PARAM_ARRAY_HH_

#include "param_value.hh"

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/iterator/indirect_iterator.hpp>

#include <deque>


namespace scarab
{
    class param_value;
    class param_node;

    typedef std::deque< std::unique_ptr< param > > param_array_contents;

    typedef boost::indirect_iterator< param_array_contents::iterator, param > param_array_iterator;
    typedef boost::indirect_iterator< param_array_contents::const_iterator, const param > param_array_const_iterator;

    class SCARAB_API param_array : public param
    {
        public:
            typedef param_array_contents contents;
            typedef param_array_iterator iterator;
            typedef param_array_const_iterator const_iterator;
            typedef contents::reverse_iterator reverse_iterator;
            typedef contents::const_reverse_iterator const_reverse_iterator;
            typedef contents::value_type contents_type;

        public:
            param_array();
            param_array( const param_array& orig );
            param_array( param_array&& orig );
            virtual ~param_array();

            param_array& operator=( const param_array& rhs );
            param_array& operator=( param_array&& rhs );

            virtual param_ptr_t clone() const;
            virtual param_ptr_t move_clone();

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
            void assign( unsigned a_index, param&& a_value_ptr );

            void push_back( const param& a_value );
            void push_back( param&& a_value );

            void push_front( const param& a_value );
            void push_front( param&& a_value );

            void append( const param_array& an_array );

            void erase( unsigned a_index );
            param_ptr_t remove( unsigned a_index );
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

    inline param_ptr_t param_array::clone() const
    {
        return std::unique_ptr< param_array >( new param_array( *this ) );
    }

    inline param_ptr_t param_array::move_clone()
    {
        return std::unique_ptr< param_array >( new param_array( std::move( *this ) ) );
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

    inline void param_array::resize( unsigned a_size )
    {
        f_contents.resize( a_size );
        return;
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
    inline void param_array::assign( unsigned a_index, param&& a_value )
    {
        erase( a_index );
        f_contents[ a_index ] = a_value.move_clone();
        return;
    }

    inline void param_array::push_back( const param& a_value )
    {
        f_contents.push_back( a_value.clone() );
        return;
    }
    inline void param_array::push_back( param&& a_value )
    {
        f_contents.push_back( a_value.move_clone() );
        return;
    }

    inline void param_array::push_front( const param& a_value )
    {
        f_contents.push_front( a_value.clone() );
        return;
    }
    inline void param_array::push_front( param&& a_value )
    {
        f_contents.push_front( a_value.move_clone() );
        return;
    }

    inline void param_array::append( const param_array& an_array )
    {
        for( const_iterator it = const_iterator(an_array.begin()); it != const_iterator(an_array.end()); ++it )
        {
            push_back( *it );
        }
        return;
    }

    inline void param_array::erase( unsigned a_index )
    {
        f_contents[ a_index ].reset();
        return;
    }
    inline param_ptr_t param_array::remove( unsigned a_index )
    {
        param_ptr_t t_current( std::move( f_contents[ a_index ] ) );
        return t_current;
    }
    inline void param_array::clear()
    {
        f_contents.clear();
        return;
    }

    inline param_array::iterator param_array::begin()
    {
        return iterator( f_contents.begin() );
    }
    inline param_array::const_iterator param_array::begin() const
    {
        return const_iterator( f_contents.cbegin() );
    }

    inline param_array::iterator param_array::end()
    {
        return iterator( f_contents.end() );
    }
    inline param_array::const_iterator param_array::end() const
    {
        return const_iterator( f_contents.cend() );
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
        return f_contents.crend();
    }

    SCARAB_API std::ostream& operator<<(std::ostream& out, const param_array& value);

} /* namespace scarab */

#endif /* SCARAB_PARAM_ARRAY_HH_ */
