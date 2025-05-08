/*
 * test_static_initialization.cc
 *
 *  Created on: Oct 2, 2024
 *      Author: N.S. Oblath
 * 
 *  Use: 
 *    > bin/test_static_initialization
 * 
 *  There should be no issue creating and destructing the static object, and printing to the log from the constructor and destructor.
 * 
 *  Example output (irrelevant lines removed):
 * 
 * 2024-10-10 05:03:56.071 [PROD] (2054134) test_static_initialization.cc:48 -> Constructing test_class object
 * 2024-10-10 05:03:56.071 [PROD] (2054134) test_static_initialization.cc:61 -> Printing something
 * 2024-10-10 05:03:56.071 [PROD] (2054134) test_static_initialization.cc:56 -> Destructing test_class object
 *  
 */

#include "logger.hh"

LOGGER( tlog, "test_static_initialization" );

namespace scarab
{
    struct test_class
    {
        test_class()
        {
            // We create a dedicated logger in the constructor because we know this will be used for a static object
            // This avoid any static initialization order issue
            LOGGER( tlog_const, "test_static_initialization" );
            LPROG( tlog_const, "Constructing test_class object");
        }

        ~test_class()
        {
            // We create a dedicated logger in the destructor because we know this will be used for a static object
            // This avoid any static destruction order issue
            LOGGER( tlog_dest, "test_static_initialization" );
            LPROG( tlog_dest, "Destructing test_class object" );
        }

        void print_something()
        {
            LPROG( tlog, "Printing something" );
            return;
        }
    };
}

static scarab::test_class s_static_tester;

int main(int , char ** )
{
    s_static_tester.print_something();

    //STOP_LOGGING;

    return( EXIT_SUCCESS );
}
