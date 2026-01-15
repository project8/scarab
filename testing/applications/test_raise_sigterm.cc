/*
 * test_raise_sigterm.cc
 *
 *  Created on: Jul 20, 2020
 *      Author: N.S. Oblath
 * 
 *  Use: 
 *    > bin/test_raise_sigterm
 * 
 *  The signal should be caught and the application exit cleanly.
 * 
 *  This program does not use signal_handler's waiting thread, and supplies a thread of its own.
 * 
 *  Example output (irrelevant lines removed):
 * 
 *    2026-01-14 16:11:02.634 [DEBUG] (5699839) signal_handler.cc:122 -> Taking over signal handling for SIGABRT, SIGTERM, SIGINT, and SIGQUIT
 *    2026-01-14 16:11:02.634 [TRACE] (5699839) signal_handler.cc:137 -> Handling SIGINT
 *    2026-01-14 16:11:02.634 [INFO] (5699839) test_raise_sigterm.cc:50 -> Starting to wait-on-signals thread
 *    2026-01-14 16:11:02.634 [TRACE] (5699839) signal_handler.cc:137 -> Handling SIGQUIT
 *    2026-01-14 16:11:02.634 [TRACE] (5699839) signal_handler.cc:137 -> Handling SIGABRT
 *    2026-01-14 16:11:02.634 [TRACE] (5699839) signal_handler.cc:137 -> Handling SIGTERM
 *    2026-01-14 16:11:02.634 [TRACE] (5699854) signal_handler.cc:185 -> Started wait_for_signals
 *    2026-01-14 16:11:03.639 [INFO] (5699839) test_raise_sigterm.cc:55 -> Raising SIGTERM
 *    [signal_handler::handle_signal()] Received a signal
 *    2026-01-14 16:11:03.639 [INFO] (5699839) test_raise_sigterm.cc:57 -> Raised
 *    2026-01-14 16:11:03.644 [DEBUG] (5699854) signal_handler.cc:198 -> Exiting loop for signals after getting signal 15
 *    2026-01-14 16:11:03.644 [DEBUG] (5699854) signal_handler.cc:208 -> Received signal SIGTERM <15> as an exit condition; return code: 0
 *    2026-01-14 16:11:03.644 [DEBUG] (5699854) signal_handler.cc:388 -> Canceling all cancelables
 *    2026-01-14 16:11:03.644 [DEBUG] (5699839) signal_handler.cc:160 -> Returning signal handling for SIGABRT, SIGTERM, SIGINT, and SIGQUIT
 *    2026-01-14 16:11:03.644 [TRACE] (5699839) signal_handler.cc:170 -> Stopped handling SIGINT
 *    2026-01-14 16:11:03.644 [TRACE] (5699839) signal_handler.cc:170 -> Stopped handling SIGQUIT
 *    2026-01-14 16:11:03.644 [TRACE] (5699839) signal_handler.cc:170 -> Stopped handling SIGABRT
 *    2026-01-14 16:11:03.644 [TRACE] (5699839) signal_handler.cc:170 -> Stopped handling SIGTERM
 * 
 */

#include "signal_handler.hh"
#include "logger.hh"

#include <signal.h>
#include <thread>

LOGGER_ST( testlog, "test_raise_sigterm" );

int main(int , char ** )
{
    scarab::signal_handler t_sh( false );

    LINFO( testlog, "Starting to wait-on-signals thread" );
    std::thread t_sh_thread( [](){scarab::signal_handler::wait_for_signals();} );

    std::this_thread::sleep_for( std::chrono::seconds(1) );

    LINFO( testlog, "Raising SIGTERM" );
    raise( SIGTERM );
    LINFO( testlog, "Raised" );

    t_sh_thread.join();
    
    return( EXIT_SUCCESS );
}
