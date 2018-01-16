/*
 * test_time.cc
 *
 *  Created on: Jan 15, 2018
 *      Author: obla999
 */

#include "time.hh"

#include "logger.hh"

LOGGER( timelog, "test_time" );

int main()
{
    LINFO( timelog, scarab::get_formatted_now() );
    return 0;
}


