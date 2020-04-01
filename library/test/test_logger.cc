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
    tlog.SetGlobalLevel( logger::ELevel::eTrace );
    std::cout << "Printing TRACE through FATAL messages" << std::endl;
    LTRACE( tlog, "This is a TRACE message" );
    LDEBUG( tlog, "This is a DEBUG message" );
    LINFO( tlog, "This is an LINFO message" );
    LPROG( tlog, "This is a PROG message" );
    LWARN( tlog, "This is a WARN message" );
    LERROR( tlog, "This is an ERROR message" );
    LFATAL( tlog, "This is a FATAL message" );
    std::cout << "This is a cout message" << std::endl;

    return 0;
}


