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

#include "catch.hpp"

TEST_CASE( "nonoption_parser", "[param]" )
{
    SECTION( "args" )
    {
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

        scarab::nonoption_parser t_parser( t_args );

        REQUIRE( t_parser.ord_args()[0]().as_string() == "We" );
        REQUIRE( t_parser.ord_args()[1]().as_string() == "are" );
        REQUIRE( t_parser.ord_args()[2]().as_uint() == 5 );
        REQUIRE( t_parser.ord_args()[3]().as_string() == "ordered" );
        REQUIRE( t_parser.ord_args()[4]().as_string() == "args" );

        REQUIRE( t_parser.kw_args()["And"]().as_uint() == 1 );
        REQUIRE( t_parser.kw_args()["we"]().as_uint() == 2 );
        REQUIRE_THAT( t_parser.kw_args()["are"][6]().as_double(), Catch::WithinRel( 3.0 ) );
        REQUIRE( t_parser.kw_args()["key"]["word"]().as_int() == -4 );
        REQUIRE_THAT( t_parser.kw_args()["args"]().as_double(), Catch::WithinRel( 5.e8 ) );
    }

    SECTION( "negative" )
    {
        std::vector< std::string > t_args;
        t_args.push_back( "-5" );

        scarab::nonoption_parser t_parser( t_args );
        REQUIRE( t_parser.ord_args()[0]().as_int() == -5 );
    }

    SECTION( "option" )
    {
        std::vector< std::string > t_args;
        t_args.push_back( "-five" );

        REQUIRE_THROWS_AS( scarab::nonoption_parser( t_args ), scarab::error );
    }
}
