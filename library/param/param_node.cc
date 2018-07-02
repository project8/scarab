/*
 * param_node.cc
 *
 *  Created on: Jan 14, 2014
 *      Author: nsoblath
 */

#define SCARAB_API_EXPORTS

#include <sstream>
using std::string;
using std::stringstream;

#include "param_node.hh"

#include "param_array.hh"
#include "param_base_impl.hh"


namespace scarab
{

    param_node::param_node() :
            param(),
            f_contents()
    {
    }

    param_node::param_node( const param_node& orig ) :
            param( orig ),
            f_contents()
    {
        for( contents::const_iterator it = orig.f_contents.begin(); it != orig.f_contents.end(); ++it )
        {
            add( it->first, it->second->clone() );
        }
    }

    param_node::~param_node()
    {
        clear();
    }

    param_node& param_node::operator=( const param_node& rhs )
    {
        clear();
        for( contents::const_iterator it = rhs.f_contents.begin(); it != rhs.f_contents.end(); ++it )
        {
            this->replace( it->first, *it->second );
        }
        return *this;
    }

    bool param_node::has_subset( const param& a_subset ) const
    {
        if( ! a_subset.is_node() ) return false;
        const param_node& t_subset_node = a_subset.as_node();
        if( t_subset_node.size() > f_contents.size() ) return false;
        for( contents::const_iterator t_subset_it = t_subset_node.f_contents.begin(); t_subset_it != t_subset_node.f_contents.end(); ++t_subset_it )
        {
            if( ! has( t_subset_it->first ) ) return false;
            if( ! f_contents.at( t_subset_it->first )->has_subset( *t_subset_it->second ) ) return false;
        }
        return true;
    }

    void param_node::merge( const param_node& a_object )
    {
        //LDEBUG( dlog, "merging object with " << a_object.size() << " items:\n" << a_object );
        for( contents::const_iterator it = a_object.f_contents.begin(); it != a_object.f_contents.end(); ++it )
        {
            if( ! has( it->first ) )
            {
                //LDEBUG( dlog, "do not have object <" << it->first << "> = <" << *it->second << ">" );
                add( it->first, *it->second );
                continue;
            }
            param& t_param = (*this)[ it->first ];
            if( t_param.is_value() )
            {
                //LDEBUG( dlog, "replacing the value of \"" << it->first << "\" <" << get_value( it->first ) << "> with <" << *it->second << ">" );
                replace( it->first, *it->second );
                continue;
            }
            if( t_param.is_node() && it->second->is_node() )
            {
                //LDEBUG( dlog, "merging nodes")
                t_param.as_node().merge( it->second->as_node() );
                continue;
            }
            if( t_param.is_array() && it->second->is_array() )
            {
                //LDEBUG( dlog, "appending array" );
                t_param.as_array().append( it->second->as_array() );
                continue;
            }
            //LDEBUG( dlog, "generic replace" );
            this->replace( it->first, *it->second );
        }
    }

    std::string param_node::to_string() const
    {
        stringstream out;
        string indentation;
        for ( unsigned i=0; i<param::s_indent_level; ++i )
            indentation += "    ";
        out << '\n' << indentation << "{\n";
        param::s_indent_level++;
        for( contents::const_iterator it = f_contents.begin(); it != f_contents.end(); ++it )
        {
            out << indentation << "    " << it->first << " : " << *(it->second) << '\n';
        }
        param::s_indent_level--;
        out << indentation << "}\n";
        return out.str();
    }

    SCARAB_API std::ostream& operator<<(std::ostream& out, const param_node& a_value)
    {
        return out << a_value.to_string();
    }

} /* namespace scarab */
