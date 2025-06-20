#include "test_class.hh"
#include "yala.hh"

#include "quill/LogMacros.h"

//#include <iostream>
#include <string>
#include <utility>


int main()
{
    yala::logger t_logger( "yala" );
  
    // A log message with number 123
    int a = 12345;
    std::string l = "log";
    LOG_INFO(t_logger.quill(), "A {} quill message with number {}", l, a);
    LINFO(t_logger, "A " << l << " logger message with number " << a);

    yala::test_class t_tc;
    t_tc.print_something();
  
    return 0;
}
