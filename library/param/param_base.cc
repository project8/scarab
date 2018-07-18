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

    bool param::has_subset( const param& /*a_subset*/ ) const
    {
        // this version of has_subset should only ever be called if a_subset is a null param (i.e. not one of the derived classes)
        return true;
    }

    SCARAB_API std::ostream& operator<<(std::ostream& out, const param& a_value)
    {
        return out << a_value.to_string();
    }

} /* namespace scarab */
