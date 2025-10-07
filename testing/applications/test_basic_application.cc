/*
 * test_basic_application.cc
 *
 *  Created on: Jul 23, 2018
 *      Author: N.S. Oblath
 *
 *  Output examples:
 *
> bin/test_basic_application -v

2018-07-23 23:25:17 [ PROG] (tid 0x7fff9c9dc380) i/application.cc(36):
Executable: "test_basic_application"
Location: "/Users/obla999/Work/Project8/Software/scarab/library/build-cli-debug/bin"
Built with Scarab version 2.2.0
Git commit: 44d2aecaa570180d24cb3b5a2a2a64c792c10059

> bin/test_basic_application -h

Usage: bin/test_basic_application [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  -c,--config FILE            Config file filename
  --verbosity UINT            Global logger verosity
  -v,--version                Print the version message and exit

> bin/test_basic_application test=teststr test-ordered

2018-07-24 08:54:26 [ PROG] (tid 0x7fff9c9dc380) i/application.cc(122): Final configuration:

{
    test : teststr
}

2018-07-24 08:54:26 [ PROG] (tid 0x7fff9c9dc380) i/application.cc(123): Ordered args:

[
    test-ordered
]

 *
 */

#include "application.hh"
#include "logger.hh"

using namespace scarab;

LOGGER( slog, "basic_application" );

int main( int argc, char **argv )
{
    logger::set_global_threshold( logger::ELevel::eDebug );

    main_app the_main( false );

    // this isn't strictly necessary, but tests the splash screen feature
    the_main.splash() = [](){ LPROG( slog, "WELCOME TO BASIC APPLICATION!" ); };

    CLI11_PARSE( the_main, argc, argv );

    return 0;
}

