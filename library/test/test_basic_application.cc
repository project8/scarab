/*
 * test_basic_application.cc
 *
 *  Created on: Jul 23, 2018
 *      Author: N.S. Oblath
 */

#include "application.hh"

using namespace scarab;

int main( int argc, char **argv )
{
    main_app the_main;

    CLI11_PARSE( the_main, argc, argv );

    return 0;
}

