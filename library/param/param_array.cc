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


namespace scarab
{

    param_array::param_array() :
            param(),
            f_contents()
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
        this->param::operator=( std::move(rhs) );
        clear();
        resize( rhs.size()) ;
        for( unsigned ind = 0; ind < rhs.f_contents.size(); ++ind )
        {
            f_contents[ind] = rhs.f_contents[ ind ]->move_clone();
        }
        rhs.clear();
        return *this;
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
