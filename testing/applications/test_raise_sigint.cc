/*
 * test_raise_sigint.cc
 *
 *  Created on: Jul 20, 2020
 *      Author: N.S. Oblath
 * 
 *  Use: 
 *    > bin/test_raise_sigint
 * 
 *  The signal should be caught and the application exit cleanly.
 * 
 *  Example output (irrelevant lines removed):
 * 
 * ➜  build-sighandler-debug git:(scarab3/feature/termination) ✗ bin/test_raise_sigint       
 * 2020-07-20 14:35:30 [DEBUG] (tid 0x1098eb5c0) ignal_handler.cc(78): Handling SIGTERM
 * 2020-07-20 14:35:30 [DEBUG] (tid 0x1098eb5c0) ignal_handler.cc(89): Handling SIGINT (ctrl-c)
 * 2020-07-20 14:35:30 [DEBUG] (tid 0x1098eb5c0) ignal_handler.cc(101): Handling SIGQUIT (ctrl-\)
 * 2020-07-20 14:35:30 [ PROG] (tid 0x1098eb5c0) ignal_handler.cc(195): Handling termination; signal <2>
 * 2020-07-20 14:35:30 [DEBUG] (tid 0x1098eb5c0) ignal_handler.cc(273): Canceling all cancelables
 * 
 */

#include "signal_handler.hh"
#include "logger.hh"

#include <future>
#include <signal.h>
#include <thread>

LOGGER( testlog, "test_raise_sigint" );

int main(int , char ** )
{
    scarab::signal_handler t_sh;

    std::cerr << "Starting to wait-on-signals thread" << std::endl;
    std::thread t_sh_thread( &scarab::signal_handler::wait_on_signals );

    std::this_thread::sleep_for( std::chrono::seconds(1) );

    std::cerr << "Raising SIGINT" << std::endl;
    raise( SIGINT );
    std::cerr << "Raised" << std::endl;

    t_sh_thread.join();
    
    /*
    auto raise_signal = []() {
        std::this_thread::sleep_for( std::chrono::seconds(1) );
        LWARN( testlog, "Raising SIGINT" );
        std::cerr << "Raising SIGINT" << std::endl;
        raise( SIGINT );
        return;
    };
    auto ft = std::async( std::launch::async, raise_signal );

    // blocks
    std::cerr << "Starting to handle signals" << std::endl;
    scarab::signal_handler::do_handle_signals();
    std::cerr << "Exited from handling signals" << std::endl;
    ft.get();
    */

    return( EXIT_SUCCESS );
}
