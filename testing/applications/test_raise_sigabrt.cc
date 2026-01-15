/*
 * test_raise_sigabrt.cc
 *
 *  Created on: Jul 20, 2020
 *      Author: N.S. Oblath
 * 
 *  Use: 
 *    > bin/test_raise_sigabrt
 * 
 *  The signal should be caught and the application exit cleanly.
 *  Since this is an error signal, a stack trace should be displayed.
 * 
 *  This program manually starts signal_handler's waiting thread and manually joins it at the end.
 * 
 *  Example output (irrelevant lines removed):
 * 
 *    2026-01-14 16:03:53.398 [INFO] (5681309) test_raise_sigabrt.cc:50 -> Starting to wait-on-signals thread
 *    2026-01-14 16:03:53.398 [DEBUG] (5681309) signal_handler.cc:122 -> Taking over signal handling for SIGABRT, SIGTERM, SIGINT, and SIGQUIT
 *    2026-01-14 16:03:53.398 [TRACE] (5681309) signal_handler.cc:137 -> Handling SIGINT
 *    2026-01-14 16:03:53.398 [TRACE] (5681309) signal_handler.cc:137 -> Handling SIGQUIT
 *    2026-01-14 16:03:53.398 [TRACE] (5681309) signal_handler.cc:137 -> Handling SIGABRT
 *    2026-01-14 16:03:53.398 [TRACE] (5681309) signal_handler.cc:137 -> Handling SIGTERM
 *    2026-01-14 16:03:53.406 [TRACE] (5681502) signal_handler.cc:185 -> Started wait_for_signals
 *    2026-01-14 16:03:54.409 [INFO] (5681309) test_raise_sigabrt.cc:55 -> Raising SIGABRT
 *    [signal_handler::handle_signal()] Received a signal
 *    2026-01-14 16:03:54.409 [INFO] (5681309) test_raise_sigabrt.cc:57 -> Raised
 *    2026-01-14 16:03:54.414 [DEBUG] (5681502) signal_handler.cc:198 -> Exiting loop for signals after getting signal 6
 *    2026-01-14 16:03:54.414 [ERROR] (5681502) signal_handler.cc:204 -> Received signal SIGABRT <6> as an error condition; return code: 1
 *    2026-01-14 16:03:54.414 [DEBUG] (5681502) signal_handler.cc:388 -> Canceling all cancelables
 *    2026-01-14 16:03:54.414 [DEBUG] (5681309) signal_handler.cc:160 -> Returning signal handling for SIGABRT, SIGTERM, SIGINT, and SIGQUIT
 *    2026-01-14 16:03:54.414 [TRACE] (5681309) signal_handler.cc:170 -> Stopped handling SIGINT
 *    2026-01-14 16:03:54.414 [TRACE] (5681309) signal_handler.cc:170 -> Stopped handling SIGQUIT
 *    2026-01-14 16:03:54.414 [TRACE] (5681309) signal_handler.cc:170 -> Stopped handling SIGABRT
 *    2026-01-14 16:03:54.414 [TRACE] (5681309) signal_handler.cc:170 -> Stopped handling SIGTERM
 */

#include "signal_handler.hh"
#include "logger.hh"

#include <signal.h>
#include <thread>

LOGGER_ST( testlog, "test_raise_sigabrt" );

int main(int , char ** )
{
    scarab::signal_handler t_sh( false );

    LINFO( testlog, "Starting to wait-on-signals thread" );
    scarab::signal_handler::start_waiting_thread();

    std::this_thread::sleep_for( std::chrono::seconds(1) );

    LINFO( testlog, "Raising SIGABRT" );
    raise( SIGABRT );
    LINFO( testlog, "Raised" );

    scarab::signal_handler::join_waiting_thread();
    
    return( EXIT_SUCCESS );
}
