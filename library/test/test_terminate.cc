/*
 * test_terminate.cc
 *
 *  Created on: May 23, 2017
 *      Author: N.S. Oblath
 */

#include "terminate.hh"

#include "logger.hh"

LOGGER( slog, "test_terminate" );

namespace scarab
{
    int throw_exception()
    {
        // throw an unhandled runtime error
        throw std::runtime_error("RUNTIME ERROR!");
        return 0;
    }

    int foo2()
    {
        throw_exception();
        return 0;
    }

    int foo1()
    {
        foo2();
        return 0;
    }
}

int main(int , char ** )
{
    // Use for option 2 (signal handling)
    //struct sigaction sigact;

    //sigact.sa_sigaction = scarab::crit_err_hdlr;
    //sigact.sa_flags = SA_RESTART | SA_SIGINFO;

    // set the error handler
    //if( sigaction(SIGABRT, &sigact, (struct sigaction *)NULL) != 0 )
    //{
    //    LERROR( slog, "error setting handler for signal " << SIGABRT
    //            << " (" << strsignal(SIGABRT) << ")" );
    //    exit(EXIT_FAILURE);
    //}

    // cause the exception to be thrown
    scarab::foo1();

    exit(EXIT_SUCCESS);
}
