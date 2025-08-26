/*
 * test_app_with_subcommands.cc
 *
 *  Created on: Jul 24, 2018
 *      Author: N.S. Oblath
 *  Output examples:
 *
> bin/test_app_with_subcommands

A subcommand is required
Run with --help for more information.


> bin/test_app_with_subcommands get

2018-07-24 14:06:45 [ PROG] (tid 0x7fff9c9dc380) i/application.cc(92): Final configuration:

{
}

2018-07-24 14:06:45 [ PROG] (tid 0x7fff9c9dc380) i/application.cc(93): Ordered args:

[
]

2018-07-24 14:06:45 [ PROG] (tid 0x7fff9c9dc380) h_subcommands.cc(65): Value is: 5


> bin/test_app_with_subcommands set value=10

2018-07-24 14:06:59 [ PROG] (tid 0x7fff9c9dc380) i/application.cc(92): Final configuration:

{
    value : 10
}

2018-07-24 14:06:59 [ PROG] (tid 0x7fff9c9dc380) i/application.cc(93): Ordered args:

[
]

2018-07-24 14:06:59 [ PROG] (tid 0x7fff9c9dc380) h_subcommands.cc(72): Just to check: 10

 *
 */

#include "application.hh"

#include "logger.hh"

LOGGER( testlog, "test_app_with_subcommands" );

namespace scarab
{
    struct get_or_set
    {
        get_or_set() : f_value( 5 ) {}

        void setup_subcommands( main_app& an_app )
        {
            app* t_sc_get = an_app.add_subcommand( "get", "Get the value" );
            t_sc_get->callback([this]() { this->get(); } );

            app* t_sc_set = an_app.add_subcommand( "set", "Set the value" );
            t_sc_set->callback([&an_app, this]() { this->set( an_app ); } );

            return;
        }

        void get()
        {
            LPROG( testlog, "Value is: " << f_value );
            return;
        }

        void set( const main_app& an_app )
        {
            f_value = an_app.primary_config().get_value( "value", f_value );
            LPROG( testlog, "Just to check: " << f_value );
            return;
        }

        int f_value;
    };
}


using namespace scarab;

int main( int argc, char **argv )
{
    main_app the_main;
    the_main.require_subcommand();
    the_main.fallthrough();

    get_or_set t_gos;
    t_gos.setup_subcommands( the_main );

    CLI11_PARSE( the_main, argc, argv );

    return 0;
}
