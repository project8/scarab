/*
 * test_nonoption_parser.cc
 *
 *  Created on: Jul 23, 2018
 *      Author: N.S. Oblath
 *
 *  Terminal output should be something like this:
2018-07-23 17:12:03 [ INFO] (tid 0x7fff9c9dc380) option_parser.cc(19): Creating arguments
2018-07-23 17:12:03 [ INFO] (tid 0x7fff9c9dc380) option_parser.cc(33): Parsing
2018-07-23 17:12:03 [ INFO] (tid 0x7fff9c9dc380) option_parser.cc(36): Checking parsed args
2018-07-23 17:12:03 [ INFO] (tid 0x7fff9c9dc380) option_parser.cc(37):  Ordered args:

[
    We
    are
    5
    ordered
    args
]

2018-07-23 17:12:03 [ INFO] (tid 0x7fff9c9dc380) option_parser.cc(38):  Keyword args:

{
    And : 1
    are :
    [






        3.000000
    ]

    args : 500000000.000000
    key :
    {
        word : -4
    }

    we : 2
}

2020-07-06 10:37:15 [ INFO] (tid 0x1117845c0) option_parser.cc(79): Testing negative number vs illegal option
2020-07-06 10:37:15 [ INFO] (tid 0x1117845c0) option_parser.cc(83): Parsing `-5`
2020-07-06 10:37:15 [ INFO] (tid 0x1117845c0) option_parser.cc(85): Is ord_args[0] == -5? 1
2020-07-06 10:37:15 [ INFO] (tid 0x1117845c0) option_parser.cc(89): Parsing `-five`
2020-07-06 10:37:15 [ INFO] (tid 0x1117845c0) option_parser.cc(97): Caught exception while parsing:
Cannot parse an option with the nonoption_parser: -five
 *
 */

#include "nonoption_parser.hh"

#include "logger.hh"

using namespace scarab;

LOGGER( testlog, "test_nonoption_parser" );

int main()
{
    LINFO( testlog, "Creating arguments" );
    std::vector< std::string > t_args;
    t_args.push_back( "We" );
    t_args.push_back( "are" );
    t_args.push_back( "5" );
    t_args.push_back( "ordered" );
    t_args.push_back( "args" );

    t_args.push_back( "And=1" );
    t_args.push_back( "we=2" );
    t_args.push_back( "are.6=3.0" );
    t_args.push_back( "key.word=-4" );
    t_args.push_back( "args=5e8" );

    LINFO( testlog, "Parsing" );
    nonoption_parser t_parser( t_args );

    LINFO( testlog, "Checking parsed args" );
    LINFO( testlog, "\tOrdered args:\n" << t_parser.ord_args() );
    LINFO( testlog, "\tKeyword args:\n" << t_parser.kw_args() );


    LINFO( testlog, "Testing negative number vs illegal option" );
    std::vector< std::string > t_args_2;
    t_args_2.push_back( "-5" );

    LINFO( testlog, "Parsing `-5`" );
    nonoption_parser t_parser_2( t_args_2 );
    LINFO( testlog, "Is ord_args[0] == -5? " << (t_parser_2.ord_args()[0]().as_int() == -5) );

    t_args_2[0] = "-five";

    LINFO( testlog, "Parsing `-five`" );
    try
    {
        nonoption_parser t_parser_2_2( t_args_2 );
        LERROR( testlog, "Did not catch exception while parsing;\nord_args: " << t_parser_2_2.ord_args() << "\nkw_args: " << t_parser_2_2.kw_args() );
    } 
    catch( error& e )
    {
        LINFO( testlog, "Caught exception while parsing:\n" << e.what() );
    }
    return 0;
}

