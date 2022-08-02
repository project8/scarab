/*
 * param_node.hh
 *
 *  Created on: Jan 14, 2014
 *      Author: nsoblath
 */

#ifndef SCARAB_PARAM_NODE_HH_
#define SCARAB_PARAM_NODE_HH_

#include "param_value.hh"

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/utility/enable_if.hpp>

#include <initializer_list>
#include <map>

#include <iostream>

namespace scarab
{
    class param_value;
    class param_array;

    // This special iterator class is used to allow the param_node iterator to point to a `param` object instead of a `std::unique_ptr<param>` object.
    // In param_array we just used boost::indirect_iterator, but that doesn't work quite as simply for map-like objects.
    // Note that unlike a normal map iterator, *iterator gives the `param` object, and iterator.name() gives the key.
    template< class x_key, class x_value, class x_iiterator >
    class map_deref_iterator : public boost::iterator_adaptor< map_deref_iterator< x_key, x_value, x_iiterator >, x_iiterator, x_value, boost::bidirectional_traversal_tag >
    {
        private:
            // used for the conversion constructor below
            struct enabler {};

        public:
            map_deref_iterator() :
                    map_deref_iterator::iterator_adaptor_()
            {}
            map_deref_iterator( const x_iiterator& other ) :
                    map_deref_iterator::iterator_adaptor_( other )
            {}

            // converts from iterator to const_iterator, but the enable_if business prevents converting from const_iterator to iterator
            template< class x_other_value, class x_other_iiterator >
            map_deref_iterator( const map_deref_iterator< x_key, x_other_value, x_other_iiterator > & other, typename boost::enable_if< boost::is_convertible< x_other_value, x_value >, enabler >::type = enabler() ) :
                    map_deref_iterator::iterator_adaptor_( other.base )
            {}

            const x_key& name() const
            {
                return this->base()->first;
            }

        private:
            friend class boost::iterator_core_access;

            x_value& dereference() const
            {
                return *this->base()->second;
            }

    };

    typedef std::map< std::string, std::unique_ptr< param > > param_node_contents;

    typedef map_deref_iterator< std::string, param, param_node_contents::iterator > param_node_iterator;
    typedef map_deref_iterator< std::string, const param, param_node_contents::const_iterator > param_node_const_iterator;

    template< typename T >
    using node_item = std::pair<std::string, T&&>;
    template< typename T >
    node_item<T> ni( const std::string& a_name, T an_element )
    {
        return std::make_pair( a_name, std::forward<T>(an_element) );
    }

    class SCARAB_API param_node : public param
    {
        public:
            typedef param_node_contents contents;
            typedef param_node_iterator iterator;
            typedef param_node_const_iterator const_iterator;
            typedef contents::value_type contents_type;

            param_node();
            template< typename T, typename... Ts >
            param_node( node_item<T> an_item, node_item<Ts>... an_items );
            param_node( const param_node& orig );
            param_node( param_node&& orig );
            virtual ~param_node();

            param_node& operator=( const param_node& rhs );
            param_node& operator=( param_node&& rhs );

            virtual param_ptr_t clone() const;
            virtual param_ptr_t move_clone();

            virtual bool is_null() const;
            virtual bool is_node() const;

            virtual bool has_subset( const param& a_subset ) const;

            unsigned size() const;
            bool empty() const;

            bool has( const std::string& a_name ) const;
            unsigned count( const std::string& a_name ) const;

            /// Returns the result of param_value::get if a_name is present and is of type param_value
            /// Returns a_default if a_name is not present or is not of type param_value
            std::string get_value( const std::string& a_name, const std::string& a_default ) const;
            std::string get_value( const std::string& a_name, const char* a_default ) const;
            /// Returns the result of param_value::get if a_name is present and is of type param_value
            /// Returns a_default if a_name is not present or is not of type param_value
            template< typename XValType >
            XValType get_value( const std::string& a_name, XValType a_default ) const;

            /// Returns a reference to the param corresponding to a_name.
            /// Throws an std::out_of_range if a_name is not present.
            const param& operator[]( const std::string& a_name ) const;
            /// Returns a reference to the param corresponding to a_name.
            /// Throws an std::out_of_range if a_name is not present.
            param& operator[]( const std::string& a_name );

            template< typename T >
            bool add( const std::string& a_name, T an_element );

            /// Adds an item or items to the node if items with the given names aren't already present.
            /// Any items whose names are not present in the node are added; if a name is present, the corresponding item is not added.
            /// Returns true if none of the names were already present, and returns false if even one name is.
            /// Valid types include param_ptr_t, param objects (copy or move), and types convertible to param_value
            template< typename T, typename... Ts >
            bool add( node_item<T> an_item, node_item<Ts>... an_items );

            template< typename T >
            void replace( const std::string& a_name, T an_element );

            /// Adds an item or items to the node, overwriting items if the keys are already present.
            /// Valid types include param_ptr_t, param objects (copy or move), and types convertible to param_value
            template< typename T, typename... Ts >
            void replace( node_item<T>&& an_item, node_item<Ts>&&... an_items );

            /// Merges the contents of a_object into this object.
            /// If names in the contents of a_object exist in this object,
            /// the values in this object corresponding to the matching names will be replaced.
            void merge( const param_node& a_object );

            void erase( const std::string& a_name );
            param_ptr_t remove( const std::string& a_name );
            void clear();

            iterator begin();
            const_iterator begin() const;

            iterator end();
            const_iterator end() const;

            virtual std::string to_string() const;

        protected:
            bool add(); // end the parameter pack recursion
            void replace(); // end the parameter pack recursion

            contents f_contents;

    };

    using n = param_node;

    template< typename T, typename... Ts >
    param_node::param_node( node_item<T> an_item, node_item<Ts>... an_items )
    {
        this->replace( an_item, an_items... );
    }

    template< typename XValType >
    inline XValType param_node::get_value( const std::string& a_name, XValType a_default ) const
    {
        return has( a_name ) ? operator[]( a_name ).as_value().as< XValType >() : a_default;
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

    inline std::string param_node::get_value( const std::string& a_name, const std::string& a_default ) const
    {
        return has( a_name ) ? operator[]( a_name ).to_string() : a_default;
    }

    inline std::string param_node::get_value( const std::string& a_name, const char* a_default ) const
    {
        return get_value( a_name, std::string( a_default ) );
    }

    inline const param& param_node::operator[]( const std::string& a_name ) const
    {
        return *f_contents.at( a_name );
    }

    inline param& param_node::operator[]( const std::string& a_name )
    {
        return *f_contents.at( a_name );
    }

    template< typename T >
    bool param_node::add( const std::string& a_name, T an_element )
    {
        return this->add( node_item<T>( a_name, std::forward<T>(an_element) ) );
    }

    template< typename T, typename... Ts >
    bool param_node::add( node_item<T> an_item, node_item<Ts>... an_items )
    {
        using T_noref = typename std::remove_reference< T >::type;
        static_assert( std::is_same_v< param_ptr_t, T_noref > || 
                       std::is_base_of_v< param, T_noref > || 
                       std::is_convertible_v< T_noref, param_value > );

        bool ret = false;
        contents::iterator it = f_contents.find( an_item.first );
        if( it == f_contents.end() )
        {
            ret = true;
            if constexpr ( std::is_same_v< param_ptr_t, T_noref > )
            {
                std::cerr << "adding as a param_ptr_t: " << *an_item.second << std::endl;
                f_contents.insert( contents_type( an_item.first, std::move(an_item.second) ) );
            }
            if constexpr ( std::is_base_of_v< param, T_noref > )
            {
                if constexpr ( std::is_lvalue_reference_v< T > ) // then do a copy
                {
                    std::cerr << "adding as an lvalue param (copy): " << an_item.second << std::endl;
                    f_contents.insert( contents_type( an_item.first, an_item.second.clone() ) );
                }
                else // then do a move
                {
                    std::cerr << "adding as an rvalue param (move): " << an_item.second.is_node() << an_item.second << std::endl;
                    f_contents.insert( contents_type( an_item.first, an_item.second.move_clone() ) );
                }
            }
            else if constexpr ( std::is_convertible_v< T_noref, param_value > )
            {
                std::cerr << "adding as convertible to value: " << an_item.second << std::endl;
                f_contents.insert( contents_type( an_item.first, param_ptr_t( new param_value( an_item.second ) ) ) );
            }
        }
        return ret && add( std::forward<Ts>( an_items )... );
    }

    inline bool param_node::add()
    {
        return true;
    }

/*
    inline bool param_node::add( const std::string& a_name, const param& a_value )
    {
        contents::iterator it = f_contents.find( a_name );
        if( it == f_contents.end() )
        {
            f_contents.insert( contents_type( a_name, a_value.clone() ) );
            return true;
        }
        return false;
    }

    inline bool param_node::add( const std::string& a_name, param&& a_value )
    {
        contents::iterator it = f_contents.find( a_name );
        if( it == f_contents.end() )
        {
            f_contents.insert( contents_type( a_name, a_value.move_clone() ) );
            return true;
        }
        return false;
    }

    inline bool param_node::add( const std::string& a_name, param_ptr_t a_value_ptr )
    {
        contents::iterator it = f_contents.find( a_name );
        if( it == f_contents.end() )
        {
            f_contents.insert( contents_type( a_name, std::move(a_value_ptr) ) );
            return true;
        }
        return false;
    }

    template< typename T, typename std::enable_if< std::is_convertible< T, param_value >::value, T >::type* >
    inline bool param_node::add( const std::string& a_name, T a_value )
    {
        //static_assert(std::is_convertible< T, param_value >::value, "Cannot convert type to param_value" );
        contents::iterator it = f_contents.find( a_name );
        if( it == f_contents.end() )
        {
            f_contents.insert( contents_type( a_name, param_ptr_t( new param_value( a_value ) ) ) );
            return true;
        }
        return false;
    }
*/
    template< typename T >
    void param_node::replace( const std::string& a_name, T an_element )
    {
        this->replace( node_item<T>( a_name, std::forward<T>(an_element) ) );
        return;
    }

    template< typename T, typename... Ts >
    void param_node::replace( node_item<T>&& an_item, node_item<Ts>&&... an_items )
    {
        using T_noref = typename std::remove_reference< T >::type;
        static_assert( std::is_same_v< param_ptr_t, T_noref > || 
                       std::is_base_of_v< param, T_noref > || 
                       std::is_convertible_v< T_noref, param_value > );

        if constexpr ( std::is_same_v< param_ptr_t, T_noref > )
        {
            std::cerr << "replacing as a param_ptr_t: " << *an_item.second << std::endl;
            f_contents[ an_item.first ] = std::move(an_item.second);
        }
        if constexpr ( std::is_base_of_v< param, T_noref > )
        {
            if constexpr ( std::is_lvalue_reference_v< T > ) // then do a copy
            {
                std::cerr << "replacing as an lvalue param (copy): " << an_item.second << std::endl;
                f_contents[ an_item.first ] = an_item.second.clone();
            }
            else // then do a move
            {
                std::cerr << "replacing as an rvalue param (move): " << an_item.second.is_node() << an_item.second << std::endl;
                f_contents[ an_item.first ] = an_item.second.move_clone();
            }
        }
        else if constexpr ( std::is_convertible_v< T_noref, param_value > )
        {
            std::cerr << "replacing as convertible to value: " << an_item.second << std::endl;
            f_contents[ an_item.first ] = param_ptr_t( new param_value( an_item.second ) );
        }
        replace( std::forward<Ts>( an_items )... );
        return;
    }

    inline void param_node::replace()
    {}
/*
    inline void param_node::replace( const std::string& a_name, const param& a_value )
    {
        f_contents[ a_name ] = a_value.clone();
        return;
    }

    inline void param_node::replace( const std::string& a_name, param&& a_value )
    {
        f_contents[ a_name ] = a_value.move_clone();
        return;
    }

    inline void param_node::replace( const std::string& a_name, param_ptr_t a_value_ptr )
    {
        f_contents[ a_name ] = std::move(a_value_ptr);
        return;
    }

    template< typename T, typename std::enable_if< std::is_convertible< T, param_value >::value, T >::type* >
    inline void param_node::replace( const std::string& a_name, T a_value )
    {
        f_contents[ a_name ] = param_ptr_t( new param_value( a_value ) );
        return;
    }
*/

    inline void param_node::erase( const std::string& a_name )
    {
        contents::iterator it = f_contents.find( a_name );
        if( it != f_contents.end() )
        {
            f_contents.erase( it );
        }
        return;
    }

    inline param_ptr_t param_node::remove( const std::string& a_name )
    {
        contents::iterator it = f_contents.find( a_name );
        if( it != f_contents.end() )
        {
            param_ptr_t removed( std::move( it->second ) );
            f_contents.erase( it );
            return removed;
        }
        return param_ptr_t();
    }

    inline void param_node::clear()
    {
        f_contents.clear();
        return;
    }

    inline param_node::iterator param_node::begin()
    {
        return iterator( f_contents.begin() );
    }

    inline param_node::const_iterator param_node::begin() const
    {
        return const_iterator( f_contents.cbegin() );
    }

    inline param_node::iterator param_node::end()
    {
        return iterator( f_contents.end() );
    }

    inline param_node::const_iterator param_node::end() const
    {
        return const_iterator( f_contents.cend() );
    }

    SCARAB_API std::ostream& operator<<(std::ostream& out, const param_node& value);

} /* namespace scarab */

#endif /* SCARAB_PARAM_NODE_HH_ */
