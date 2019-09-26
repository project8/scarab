/*
 * version_ifc.cc
 *
 *  Created on: Jul 23, 2018
 *      Author: N.S. Oblath
 */

#define SCARAB_API_EXPORTS

#include "version_ifc.hh"

namespace scarab
{
    version_ifc::version_ifc()
    {}

    version_ifc::version_ifc( const version_ifc& )
    {}

    version_ifc::~version_ifc()
    {}

    version_ifc& version_ifc::operator=( const version_ifc& )
    {
        return *this;
    }

} /* namespace scarab */
