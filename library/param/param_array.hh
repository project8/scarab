/*
 * param_array.hh
 *
 *  Created on: Jan 14, 2014
 *      Author: nsoblath
 */

#ifndef SCARAB_PARAM_ARRAY_HH_
#define SCARAB_PARAM_ARRAY_HH_

#include "param_modifier.hh"
#include "param_value.hh"
#include "param_visitor.hh"

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/iterator/indirect_iterator.hpp>

#include <deque>
#include <type_traits>

namespace scarab
{
    class param_value;
    class param_node;
    class args;

    typedef std::deque< std::unique_ptr< param > > param_array_contents;

    typedef boost::indirect_iterator< param_array_contents::iterator, param > param_array_iterator;
    typedef boost::indirect_iterator< param_array_contents::const_iterator, const param > param_array_const_iterator;

    /*!
     @class param_array
     @author N. S. Oblath

     @brief Array/list-like param structure

     @details
    */
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
            param_array( args&& args );
            param_array( const param_array& orig );
            param_array( param_array&& orig );
            virtual ~param_array();

            param_array& operator=( const param_array& rhs );
            param_array& operator=( param_array&& rhs );

            virtual param_ptr_t clone() const;
            virtual param_ptr_t move_clone();

            virtual void accept( const param_modifier& a_modifier );
            virtual void accept( const param_visitor& a_visitor ) const;

            virtual bool is_null() const;
            virtual bool is_array() const;

            virtual bool has_subset( const param& a_subset ) const;

            unsigned size() const;
            bool empty() const;

            /// sets the size of the array
            /// if smaller than the current size, extra elements are deleted
            void resize( unsigned a_size );

            /// Returns the result of param_value::get if a_name is present and is of type param_value
            /// Returns a_default if a_name is not present or is not of type param_value
            std::string get_value( unsigned a_index, const std::string& a_default ) const;
            std::string get_value( unsigned a_index, const char* a_default ) const;
            /// Returns the result of param_value::get if a_name is present and is of type param_value
            /// Returns a_default if a_name is not present or is not of type param_value
            template< typename XValType >
            XValType get_value( unsigned a_index, XValType a_default ) const;

            /// Returns a reference to the param at a_index.
            /// Throws a std::out_of_range if a_index is out-of-range.
            const param& at( unsigned a_index ) const;
            /// Returns a reference to the param at a_index.
            /// Throws a std::out_of_range if a_index is out-of-range.
            param& at( unsigned a_index );

            /// Returns a reference to the param at a_index.
            /// Throws a std::out_of_range if a_index is out-of-range.
            /// Note that this behavior differs from the C++ STL sequence-like container behavior
            const param& operator[]( unsigned a_index ) const;
            /// Returns a reference to the param at a_index.
            /// Throws a std::out_of_range if a_index is out-of-range.
            /// Note that this behavior differs from the C++ STL sequence-like container behavior
            param& operator[]( unsigned a_index );

            const param& front() const;
            param& front();

            const param& back() const;
            param& back();

            /// Assigns the value to the array at a_index
            /// Valid types include param_ptr_t, param objects (copy or move), and types convertible to param_value
            template< typename T >
            void assign( unsigned a_index, T&& a_value );

            /// Adds an item or items to the back of the array (last item pushed will end up at the back of the array)
            /// Valid types include param_ptr_t, param objects (copy or move), and types convertible to param_value
            template< typename T, typename... Ts >
            void push_back( T&& a_value, Ts&&... a_values );

            /// Adds an item or items to the front of the array (last item pushed will end up at the front of the array)
            /// Valid types include param_ptr_t, param objects (copy or move), and types convertible to param_value
            template< typename T, typename... Ts >
            void push_front( T&& a_value, Ts&&... a_values );

            void append( const param_array& an_array );

            void merge( const param_array& an_array );

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
            void push_back(); // end the parameter pack recursion
            void push_front(); // end the parameter pack recursion

            contents f_contents;
    };

    template< typename XValType >
    XValType param_array::get_value( unsigned a_index, XValType a_default ) const
    {
        return a_index < size() ? operator[]( a_index ).as_value().as< XValType >() : a_default;
    }

    inline unsigned param_array::size() const
    {
        return f_contents.size();
    }
    inline bool param_array::empty() const
    {
        return f_contents.empty();
    }

    inline void param_array::accept( const param_modifier& a_modifier )
    {
        a_modifier( *this );
        return;
    }

    inline void param_array::accept( const param_visitor& a_visitor ) const
    {
        a_visitor( *this );
        return;
    }

    inline std::string param_array::get_value( unsigned a_index, const std::string& a_default ) const
    {
        return a_index < size() ? operator[]( a_index ).to_string() : a_default;
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

    inline const param& param_array::operator[]( unsigned a_index ) const
    {
        return *f_contents.at( a_index );
    }
    inline param& param_array::operator[]( unsigned a_index )
    {
        return *f_contents.at( a_index );
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

    template< typename T >
    void param_array::assign( unsigned a_index, T&& a_value )
    {
        using T_noref = typename std::remove_reference< T >::type;
        static_assert( std::is_same_v< param_ptr_t, T_noref > || 
                       std::is_base_of_v< param, T_noref > || 
                       std::is_convertible_v< T_noref, param_value > );

        erase( a_index );
        if constexpr ( std::is_same_v< param_ptr_t, T_noref > )
        {
            f_contents.at( a_index ) = std::move(a_value);
        }
        if constexpr ( std::is_base_of_v< param, T_noref > )
        {
            if constexpr ( std::is_lvalue_reference_v< T > ) // then do a copy
            {
                f_contents.at( a_index ) = a_value.clone();
            }
            else // then do a move
            {
                f_contents.at( a_index ) = a_value.move_clone();
            }
        }
        else if constexpr ( std::is_convertible_v< T_noref, param_value > )
        {
            f_contents.at( a_index ) = param_ptr_t( new param_value( a_value ) );
        }
        return;
    }

    template< typename T, typename... Ts >
    void param_array::push_back( T&& a_value, Ts&&... a_values )
    {
        using T_noref = typename std::remove_reference< T >::type;
        static_assert( std::is_same_v< param_ptr_t, T_noref > || 
                       std::is_base_of_v< param, T_noref > || 
                       std::is_convertible_v< T_noref, param_value > );

        if constexpr ( std::is_same_v< param_ptr_t, T_noref > )
        {
            f_contents.push_back( std::move(a_value) );
        }
        if constexpr ( std::is_base_of_v< param, T_noref > )
        {
            if constexpr ( std::is_lvalue_reference_v< T > ) // then do a copy
            {
                f_contents.push_back( a_value.clone() );
            }
            else // then do a move
            {
                f_contents.push_back( a_value.move_clone() );
            }
        }
        else if constexpr ( std::is_convertible_v< T_noref, param_value > )
        {
            f_contents.push_back( param_ptr_t( new param_value( a_value ) ) );
        }
        push_back( std::forward<Ts>( a_values )... );
        return;
    }

    inline void param_array::push_back()
    {}

    template< typename T, typename... Ts >
    void param_array::push_front( T&& a_value, Ts&&... a_values )
    {
        using T_noref = typename std::remove_reference< T >::type;
        static_assert( std::is_same_v< param_ptr_t, T_noref > || 
                       std::is_base_of_v< param, T_noref > || 
                       std::is_convertible_v< T_noref, param_value > );

        if constexpr ( std::is_same_v< param_ptr_t, T_noref > )
        {
            f_contents.push_front( std::move(a_value) );
        }
        if constexpr ( std::is_base_of_v< param, T_noref > )
        {
            if constexpr ( std::is_lvalue_reference_v< T > ) // then do a copy
            {
                f_contents.push_front( a_value.clone() );
            }
            else // then do a move
            {
                f_contents.push_front( a_value.move_clone() );
            }
        }
        else if constexpr ( std::is_convertible_v< T_noref, param_value > )
        {
            f_contents.push_front( param_ptr_t( new param_value( a_value ) ) );
        }
        push_front( std::forward<Ts>( a_values )... );
        return;
    }

    inline void param_array::push_front()
    {}

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
        f_contents.at( a_index ).reset();
        return;
    }
    inline param_ptr_t param_array::remove( unsigned a_index )
    {
        param_ptr_t t_current( std::move( f_contents.at( a_index ) ) );
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
