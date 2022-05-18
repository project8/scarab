/*
 * param_base.cc
 *
 *  Created on: Jan 14, 2014
 *      Author: nsoblath
 */

#define SCARAB_API_EXPORTS

#include <sstream>
using std::string;
using std::stringstream;

#include "param_base.hh"

#include "param_base_impl.hh"

namespace scarab
{

    SCARAB_API unsigned param::s_indent_level = 0;

    param::param()
    {
    }

    param::param( const param& )
    {
    }

    param::param( param&& )
    {
    }

    param::~param()
    {
    }

    param_ptr_t param::clone() const
    {
        std::cerr << "param::clone()" << std::endl;
        return param_ptr_t( new param( *this ) );
    }

    param_ptr_t param::move_clone()
    {
        std::cerr << "param::clone()" << std::endl;
        return param_ptr_t( new param( std::move(*this) ) );
    }

    bool param::is_null() const
    {
        return true;
    }

    bool param::is_value() const
    {
        return false;
    }

    bool param::is_array() const
    {
        return false;
    }

    bool param::is_node() const
    {
        return false;
    }

    bool param::has_subset( const param& /*a_subset*/ ) const
    {
        // this version of has_subset should only ever be called if a_subset is a null param (i.e. not one of the derived classes)
        return true;
    }

    void param::merge( const param& a_param )
    {
        if( is_node() && a_param.is_node() )
        {
            as_node().merge( a_param.as_node() );
            return;
        }
        if( is_array() && a_param.is_array() )
        {
            as_array().merge( a_param.as_array() );
            return;
        }
        if( is_value() && a_param.is_value() )
        {
            as_value() = a_param.as_value();
            return;
        }
        if( is_null() && a_param.is_null() ) return;
        throw error() << "Invalid merge command with incompatible param types";
    }

    SCARAB_API std::ostream& operator<<(std::ostream& out, const param& a_value)
    {
        return out << a_value.to_string();
    }

} /* namespace scarab */
