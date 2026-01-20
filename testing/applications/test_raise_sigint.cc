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
 *  This program uses the automatic start of the waiting thread (from signal_handler's constructor), 
 *  and automatic join at the end (from signal_handler's destructor).
 * 
 *  Example output (irrelevant lines removed):
 * 
 *    2026-01-14 16:07:57.584 [DEBUG] (5691010) signal_handler.cc:122 -> Taking over signal handling for SIGABRT, SIGTERM, SIGINT, and SIGQUIT
 *    2026-01-14 16:07:57.584 [TRACE] (5691010) signal_handler.cc:137 -> Handling SIGINT
 *    2026-01-14 16:07:57.584 [TRACE] (5691010) signal_handler.cc:137 -> Handling SIGQUIT
 *    2026-01-14 16:07:57.584 [TRACE] (5691010) signal_handler.cc:137 -> Handling SIGABRT
 *    2026-01-14 16:07:57.584 [TRACE] (5691010) signal_handler.cc:137 -> Handling SIGTERM
 *    2026-01-14 16:07:57.584 [TRACE] (5691033) signal_handler.cc:185 -> Started wait_for_signals
 *    2026-01-14 16:07:58.588 [INFO] (5691010) test_raise_sigint.cc:41 -> Raising SIGINT
 *    [signal_handler::handle_signal()] Received a signal
 *    2026-01-14 16:07:58.588 [INFO] (5691010) test_raise_sigint.cc:43 -> Raised
 *    2026-01-14 16:07:58.588 [DEBUG] (5691010) signal_handler.cc:232 -> Requesting stop of signal wait
 *    2026-01-14 16:07:58.589 [DEBUG] (5691033) signal_handler.cc:225 -> Exited loop waiting for signals (wait aborted)
 *    2026-01-14 16:07:58.589 [DEBUG] (5691010) signal_handler.cc:160 -> Returning signal handling for SIGABRT, SIGTERM, SIGINT, and SIGQUIT
 *    2026-01-14 16:07:58.589 [TRACE] (5691010) signal_handler.cc:170 -> Stopped handling SIGINT
 *    2026-01-14 16:07:58.589 [TRACE] (5691010) signal_handler.cc:170 -> Stopped handling SIGQUIT
 *    2026-01-14 16:07:58.589 [TRACE] (5691010) signal_handler.cc:170 -> Stopped handling SIGABRT
 *    2026-01-14 16:07:58.589 [TRACE] (5691010) signal_handler.cc:170 -> Stopped handling SIGTERM * 
 */

#include "signal_handler.hh"
#include "logger.hh"

#include <signal.h>
#include <thread>

LOGGER_ST( testlog, "test_raise_sigint" );

int main(int , char ** )
{
    scarab::signal_handler t_sh( true );

    // let the signal-waiting thread start up
    std::this_thread::sleep_for( std::chrono::seconds(1) );

    LINFO( testlog, "Raising SIGINT" );
    raise( SIGINT );
    LINFO( testlog, "Raised" );

    return( EXIT_SUCCESS );
}
