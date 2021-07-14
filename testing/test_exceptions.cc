/*
 * test_logger.cc
 *
 *  Created on: Jan 4, 2016
 *      Author: N.S. Oblath
 */

#include "error.hh"

#include "catch.hpp"

TEST_CASE( "error", "[exceptions]" )
{
    using Catch::Matchers::Contains;
    using Catch::Matchers::EndsWith;
    using Catch::Matchers::Message;
    using Catch::Matchers::StartsWith;

    // basic functionality
    REQUIRE_THROWS_MATCHES( throw scarab::error(), scarab::error, Message("") );

    // inherits from std::exception
    REQUIRE_THROWS_MATCHES( throw scarab::error(), std::exception, Message("") );

    // stream a message
    REQUIRE_THROWS_MATCHES( throw scarab::error() << "hello", scarab::error, Message("hello") );

    unsigned t_line_0 = 0, t_line_1 = 0;

    // single throw
    try
    {
        t_line_0 = __LINE__; throw_from_here( scarab::error() );
    }
    catch( const scarab::error& e )
    {
        REQUIRE_THAT( e.locations(), StartsWith("\n#0 -- " ) );
        std::stringstream t_stream_0;
        t_stream_0 << "line " << t_line_0;
        REQUIRE_THAT( e.locations(), EndsWith(" -- void ____C_A_T_C_H____T_E_S_T____0()") );
    }
    
    // nested throw
    try
    {
        t_line_0 = __LINE__; throw_from_here( scarab::error() );
    }
    catch( scarab::error& e_0 )
    {
        try
        {
            t_line_1 = __LINE__; throw_from_here( e_0 );
        }
        catch( const scarab::error& e_1 )
        {
            REQUIRE_THAT( e_1.locations(), StartsWith("\n#0 -- " ) );
            REQUIRE_THAT( e_1.locations(), Contains("#1 -- ") );
            std::stringstream t_stream_0;
            t_stream_0 << "line " << t_line_1;
            REQUIRE_THAT( e_1.locations(), Contains(t_stream_0.str()) );
            std::stringstream t_stream_1;
            t_stream_1 << "line " << t_line_1;
            REQUIRE_THAT( e_1.locations(), Contains(t_stream_1.str()) );
            REQUIRE_THAT( e_1.locations(), EndsWith(" -- void ____C_A_T_C_H____T_E_S_T____0()") );
        } 
    }

}
