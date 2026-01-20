/*
 * test_raise_sigquit.cc
 *
 *  Created on: Jul 20, 2020
 *      Author: N.S. Oblath
 * 
 *  Use: 
 *    > bin/test_raise_sigquit
 * 
 *  The signal should be caught and the application exit cleanly.
 * 
 *  This program uses the automatic start of the waiting thread and manually joins it at the end.
 * 
 *  Example output (irrelevant lines removed):
 * 
 *    2026-01-14 16:09:44.284 [DEBUG] (5694901) signal_handler.cc:122 -> Taking over signal handling for SIGABRT, SIGTERM, SIGINT, and SIGQUIT
 *    2026-01-14 16:09:44.284 [TRACE] (5694901) signal_handler.cc:137 -> Handling SIGINT
 *    2026-01-14 16:09:44.284 [TRACE] (5694901) signal_handler.cc:137 -> Handling SIGQUIT
 *    2026-01-14 16:09:44.284 [TRACE] (5694901) signal_handler.cc:137 -> Handling SIGABRT
 *    2026-01-14 16:09:44.284 [TRACE] (5694901) signal_handler.cc:137 -> Handling SIGTERM
 *    2026-01-14 16:09:44.284 [TRACE] (5694921) signal_handler.cc:185 -> Started wait_for_signals
 *    2026-01-14 16:09:45.288 [INFO] (5694901) test_raise_sigquit.cc:40 -> Raising SIGQUIT
 *    [signal_handler::handle_signal()] Received a signal
 *    2026-01-14 16:09:45.288 [INFO] (5694901) test_raise_sigquit.cc:42 -> Raised
 *    2026-01-14 16:09:45.293 [DEBUG] (5694921) signal_handler.cc:198 -> Exiting loop for signals after getting signal 3
 *    2026-01-14 16:09:45.293 [DEBUG] (5694921) signal_handler.cc:208 -> Received signal SIGQUIT <3> as an exit condition; return code: 0
 *    2026-01-14 16:09:45.293 [DEBUG] (5694921) signal_handler.cc:388 -> Canceling all cancelables
 *    2026-01-14 16:09:45.293 [DEBUG] (5694901) signal_handler.cc:160 -> Returning signal handling for SIGABRT, SIGTERM, SIGINT, and SIGQUIT
 *    2026-01-14 16:09:45.293 [TRACE] (5694901) signal_handler.cc:170 -> Stopped handling SIGINT
 *    2026-01-14 16:09:45.293 [TRACE] (5694901) signal_handler.cc:170 -> Stopped handling SIGQUIT
 *    2026-01-14 16:09:45.293 [TRACE] (5694901) signal_handler.cc:170 -> Stopped handling SIGABRT
 *    2026-01-14 16:09:45.294 [TRACE] (5694901) signal_handler.cc:170 -> Stopped handling SIGTERM
 * 
 */

#include "signal_handler.hh"
#include "logger.hh"

#include <signal.h>
#include <thread>

LOGGER_ST( testlog, "test_raise_sigquit" );

int main(int , char ** )
{
    scarab::signal_handler t_sh( true );

    // let the signal-waiting thread start up
    std::this_thread::sleep_for( std::chrono::seconds(1) );

    LINFO( testlog, "Raising SIGQUIT" );
    raise( SIGQUIT );
    LINFO( testlog, "Raised" );

    scarab::signal_handler::join_waiting_thread();
    
    return( EXIT_SUCCESS );
}
