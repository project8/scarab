/*
 * test_app_with_callback.cc
 *
 *  Created on: Jul 24, 2018
 *      Author: N.S. Oblath
 *  Output examples:
 *
> bin/test_app_with_callback
2018-07-24 11:40:07 [ PROG] (tid 0x7fff9c9dc380) i/application.cc(92): Final configuration:

{
}

2018-07-24 11:40:07 [ PROG] (tid 0x7fff9c9dc380) i/application.cc(93): Ordered args:

[
]

2018-07-24 11:40:07 [ PROG] (tid 0x7fff9c9dc380) with_callback.cc(27): My value is: 5


> bin/test_app_with_callback value=10

2018-07-24 11:40:49 [ PROG] (tid 0x7fff9c9dc380) i/application.cc(92): Final configuration:

{
    value : 10
}

2018-07-24 11:40:49 [ PROG] (tid 0x7fff9c9dc380) i/application.cc(93): Ordered args:

[
]

2018-07-24 11:40:49 [ PROG] (tid 0x7fff9c9dc380) with_callback.cc(27): My value is: 10
 *
 */

#include "application.hh"

#include "logger.hh"

LOGGER( testlog, "test_app_with_callback" );

namespace scarab
{
    struct do_a_thing
    {
        do_a_thing() : f_value( 5 ) {}

        void execute( const main_app& an_app )
        {
            // configure to run
            f_value = an_app.primary_config().get_value( "value", f_value );

            // do a thing!
            LPROG( testlog, "My value is: " << f_value );

            return;
        }

        int f_value;
    };
}


using namespace scarab;

int main( int argc, char **argv )
{
    main_app the_main;

    auto t_dat_callback = [&](){
        do_a_thing t_dat;
        t_dat.execute( the_main );
    };

    the_main.callback( t_dat_callback );

    CLI11_PARSE( the_main, argc, argv );

    return 0;
}


