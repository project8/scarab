/*
 * param_array.cc
 *
 *  Created on: Jan 14, 2014
 *      Author: nsoblath
 */

#define SCARAB_API_EXPORTS

#include <sstream>
using std::string;
using std::stringstream;

#include "param_array.hh"

#include "param_base_impl.hh"
#include "param_helpers.hh"
#include "param_node.hh"

#include <iostream>

namespace scarab
{

    param_array::param_array() :
            param(),
            f_contents()
    {
    }

    param_array::param_array( pa_args&& args ) :
            param_array( std::move( static_cast< param_array&& >(*args.f_array_ptr)) )
    {
    }

    param_array::param_array( const param_array& orig ) :
            param( orig ),
            f_contents( orig.f_contents.size() )
    {
        for( unsigned ind = 0; ind < f_contents.size(); ++ind )
        {
            f_contents[ind] = orig.f_contents[ ind ]->clone();
        }
    }

    param_array::param_array( param_array&& orig ) :
            param( std::move(orig) ),
            f_contents( orig.f_contents.size() )
    {
        for( unsigned ind = 0; ind < f_contents.size(); ++ind )
        {
            f_contents[ind] = orig.f_contents[ ind ]->move_clone();
        }
        orig.clear();
    }

    param_array::~param_array()
    {
    }

    param_array& param_array::operator=( const param_array& rhs )
    {
        std::cerr << "Copying an array with operator=()" << std::endl;
        this->param::operator=( rhs );
        clear();
        resize( rhs.size()) ;
        for( unsigned ind = 0; ind < rhs.f_contents.size(); ++ind )
        {
            f_contents[ind] = rhs.f_contents[ ind ]->clone();
        }
        return *this;
    }

    param_array& param_array::operator=( param_array&& rhs )
    {
        std::cerr << "Moving an array with operator=()" << std::endl;
        this->param::operator=( std::move(rhs) );
        clear();
        resize( rhs.size()) ;
        for( unsigned ind = 0; ind < rhs.f_contents.size(); ++ind )
        {
            f_contents[ind] = rhs.f_contents[ ind ]->move_clone();
        }
        rhs.clear();
        std::cerr << "after move:" << rhs << std::endl;
        return *this;
    }

    param_ptr_t param_array::clone() const
    {
        std::cerr << "Copy-cloning an array" << std::endl;
        param_ptr_t array_ptr( new param_array() );
        static_cast< param_array* >( array_ptr.get() )->operator=( *this );
        return array_ptr;
    }

    param_ptr_t param_array::move_clone()
    {
        std::cerr << "Move-cloning an array" << std::endl;
        param_ptr_t array_ptr( new param_array() );
        static_cast< param_array* >( array_ptr.get() )->operator=( std::move(*this) );
        return array_ptr;
    }

    bool param_array::is_null() const
    {
        return false;
    }

    bool param_array::is_array() const
    {
        return true;
    }

    void param_array::resize( unsigned a_size )
    {
        f_contents.resize( a_size );
        for( auto it = f_contents.begin(); it != f_contents.end(); ++it )
        {
            if( ! *it ) it->reset( new param() );
        }
        return;
    }

    bool param_array::has_subset( const param& a_subset ) const
    {
        if( ! a_subset.is_array() ) return false;
        const param_array& t_subset_array = a_subset.as_array();
        if( t_subset_array.size() > f_contents.size() ) return false;
        contents::const_iterator t_this_it = f_contents.begin();
        contents::const_iterator t_that_it = t_subset_array.f_contents.begin();
        while( t_that_it != t_subset_array.f_contents.end() ) // loop condition is on a_subset because it's smaller or equal to this
        {
            if( ! (*t_this_it)->has_subset( **t_that_it ) ) return false;
            ++t_this_it;
            ++t_that_it;
        }
        return true;
    }

    void param_array::merge( const param_array& a_object )
    {
        //LDEBUG( dlog, "merging array with " << a_object.size() << " items:\n" << a_object );
        if( size() < a_object.size() ) resize( a_object.size() );

        for( unsigned index = 0; index < size() && index < a_object.size(); ++index )
        {
            // directly assign if destination location is empty
            if( f_contents.at( index )->is_null() )
            {
                //LDEBUG( dlog, "have a null object at <" << index << ">; adding <" << a_object[index] << ">" );
                assign( index, a_object[index] );
                continue;
            }

            // overwrite/recurse if destination location matches incoming type
            param& t_param = (*this)[index];
            if( t_param.is_value() && a_object[index].is_value() )
            {
                //LDEBUG( dlog, "replacing the value at <" << index << "> with <" << a_object[index] << ">" );
                t_param.as_value() = a_object[index].as_value();
                continue;
            }
            if( t_param.is_node() && a_object[index].is_node() )
            {
                //LDEBUG( dlog, "merging nodes at <" << index << ">" )
                t_param.as_node().merge( a_object[index].as_node() );
                continue;
            }
            if( t_param.is_array() && a_object[index].is_array() )
            {
                //LDEBUG( dlog, "merging array at <" << index << ">" );
                t_param.as_array().merge( a_object[index].as_array() );
                continue;
            }

            // overwrite via direct assignment if destination location does not match incoming type
            //LDEBUG( dlog, "generic replace" );
            assign( index, a_object[index] );
        }
        return;
     }


    std::string param_array::to_string() const
    {
        stringstream out;
        string indentation;
        for ( unsigned i=0; i<param::s_indent_level; ++i )
            indentation += "    ";
        out << '\n' << indentation << "[\n";
        param::s_indent_level++;
        for( contents::const_iterator it = f_contents.begin(); it != f_contents.end(); ++it )
        {
            out << indentation << "    " << **it << '\n';
        }
        param::s_indent_level--;
        out << indentation << "]\n";
        return out.str();
    }


    SCARAB_API std::ostream& operator<<(std::ostream& out, const param_array& a_value)
    {
        return out << a_value.to_string();
    }

} /* namespace scarab */
