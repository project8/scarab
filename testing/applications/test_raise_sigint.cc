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


#include <signal.h>


int main(int , char ** )
{
    scarab::signal_handler t_handler;

    raise( SIGINT );

    return( EXIT_SUCCESS );
}
