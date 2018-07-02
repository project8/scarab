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

#include <map>

namespace scarab
{
    class param_value;
    class param_array;

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

    typedef std::map< std::string, param* > param_node_contents;

    typedef map_deref_iterator< std::string, param, param_node_contents::iterator > param_node_iterator;
    typedef map_deref_iterator< std::string, const param, param_node_contents::const_iterator > param_node_const_iterator;


    class SCARAB_API param_node : public param
    {
        public:
            typedef param_node_contents contents;
            typedef param_node_iterator iterator;
            typedef param_node_const_iterator const_iterator;
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
        contents::iterator it = f_contents.find( a_name );
        if( it == f_contents.end() )
        {
            f_contents.insert( contents_type( a_name, a_value.clone() ) );
            return true;
        }
        return false;
    }

    inline bool param_node::add( const std::string& a_name, param* a_value )
    {
        contents::iterator it = f_contents.find( a_name );
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
        contents::iterator it = f_contents.find( a_name );
        if( it != f_contents.end() )
        {
            delete it->second;
            f_contents.erase( it );
        }
        return;
    }

    inline param* param_node::remove( const std::string& a_name )
    {
        contents::iterator it = f_contents.find( a_name );
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
        for( contents::iterator it = f_contents.begin(); it != f_contents.end(); ++it )
        {
            delete it->second;
        }
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
