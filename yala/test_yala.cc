#include "test_class.hh"
#include "yala.hh"

//#include <iostream>
#include <string>
#include <utility>


int main()
{
    yala::logger_type< yala::spd_initializer_async_stdout_color_mt > t_logger( "yala" );
  
    t_logger.set_threshold( yala::logger::ELevel::eTrace );
    
    // A log message with number 123
    int a = 12345;
    std::string l = "log";
    t_logger.spdlogger()->info("A {} spd message with number {}", l, a);
    LINFO(t_logger, "A " << l << " logger message with number " << a);

    yala::test_class t_tc;
    t_tc.print_something();

    LPROG( t_logger, "test PROG" );
    //REQUIRE( compare_and_clear( t_outstream, "test PROG" ) );

    LINFO( t_logger, "test INFO" );
    //REQUIRE( compare_and_clear( t_outstream, "test INFO" ) );

    LDEBUG( t_logger, "test DEBUG" );
    //REQUIRE( compare_and_clear( t_outstream, "test DEBUG" ) );

    LTRACE( t_logger, "test TRACE" );
    //REQUIRE( compare_and_clear( t_outstream, "test TRACE" ) );

    // err

    LWARN( t_logger, "test WARN" );
    //REQUIRE( compare_and_clear( t_errstream, "test WARN" ) );

    LERROR( t_logger, "test ERROR" );
    //REQUIRE( compare_and_clear( t_errstream, "test ERROR" ) );

    LFATAL( t_logger, "test FATAL" );
    //REQUIRE( compare_and_clear( t_errstream, "test FATAL" ) );

  
    return 0;
}
