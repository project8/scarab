#include "yala.hh"

#include <chrono>
//#include <iostream>
#include <string>
#include <thread>
#include <utility>

// use this to verify that only one thread is running

int main()
{
    yala::logger_type< yala::spd_initializer_stdout_color > t_logger( "yala" );
  
    // A log message with number 123
    int a = 12345;
    std::string l = "log";
    t_logger.spdlogger()->info("A {} spd message with number {}", l, a);
    LINFO(t_logger, "A " << l << " logger message with number " << a);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(20s);

    LINFO(t_logger, "After sleep");
  
    return 0;
}
