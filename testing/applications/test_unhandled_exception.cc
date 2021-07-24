/*
 * test_unhandled_exception.cc
 *
 *  Created on: May 23, 2017
 *      Author: N.S. Oblath
 * 
 *  Use: 
 *    > bin/terminate
 * 
 *  The unhandled exception should result in terminate() being called.
 *  The exception should be printed, and a stack trace displayed.
 */

#include "signal_handler.hh"

#include "logger.hh"

LOGGER( slog, "test_unhandled_exception" );

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
    scarab::signal_handler t_handler;

    // cause the unhandled exception to be thrown, which should exit immediately
    scarab::foo1();

    LERROR( slog, "This should never print" );

    return( EXIT_SUCCESS );
}
