// nested_print.cc
// Author: N.S. Oblath
// Date: Jun 4, 2020

#include "nested_print.hh"

#include "logger.hh"

LOGGER( plog, "nested_print" );

namespace nested_pbtest
{
    void print( const scarab::param& a_param )
    {
        LINFO( plog, "NESTED:\n" << a_param );
        return;
    }
}
