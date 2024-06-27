/*
 * error.cc
 *
 *  Created on: May 10, 2024
 *      Author: N.S. Oblath
 */

#define SCARAB_API_EXPORTS

#include "error.hh"

namespace scarab
{
    error::error( const std::string& a_filename, int a_line ) :
            typed_exception< error >( a_filename, a_line )
    {}
}
