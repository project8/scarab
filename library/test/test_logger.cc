/*
 * test_logger.cc
 *
 *  Created on: Jan 4, 2016
 *      Author: nsoblath
 */

#include "logger.hh"

using namespace scarab;

LOGGER( tlog, "test_logger" );

int main()
{
    DEBUG( tlog, "This is a DEBUG message" );
    INFO( tlog, "This is an INFO message" );
    WARN( tlog, "This is a WARN message" );
    ERROR( tlog, "This is an ERROR message" );

    return 0;
}


