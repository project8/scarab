/*
 * null_modifier.cc
 *
 *  Created on: Feb 12, 2020
 *      Author: N. Buzinsky
 */

#define SCARAB_API_EXPORTS

#include "null_modifier.hh"
#include "logger.hh"

namespace scarab
{
    LOGGER( mmlog, "null_modifier" );

    void null_modifier::modify(param_node &a_config)
    {
        LDEBUG( mmlog, "Complex modifications going on ....." );
        return;
    }

} /* namespace scarab */
