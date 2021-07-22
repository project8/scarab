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
 *  Since this is an error signal, a stack trace should be displayed.
 * 
 *  Example output (irrelevant lines removed):
 * 
 * 2020-07-20 14:39:53 [DEBUG] (tid 0x10dbef5c0) ignal_handler.cc(78): Handling SIGTERM
 * 2020-07-20 14:39:53 [DEBUG] (tid 0x10dbef5c0) ignal_handler.cc(89): Handling SIGINT (ctrl-c)
 * 2020-07-20 14:39:53 [DEBUG] (tid 0x10dbef5c0) ignal_handler.cc(101): Handling SIGQUIT (ctrl-\)
 * 2020-07-20 14:39:53 [ERROR] (tid 0x10dbef5c0) ignal_handler.cc(187): Handling termination due to an error condition; signal <15>
 * 2020-07-20 14:39:53 [ERROR] (tid 0x10dbef5c0) ignal_handler.cc(254): Backtrace from terminate() returned 9 frames
 * 
 * 2020-07-20 14:39:53 [ERROR] (tid 0x10dbef5c0) ignal_handler.cc(263): Backtrace:
 * [bt]: (0) 0   libScarab.dylib                     0x000000010233afca _ZN6scarab14signal_handler17print_stack_traceEv + 42
 * [bt]: (1) 1   libScarab.dylib                     0x000000010233aa34 _ZN6scarab14signal_handler9terminateEi + 68
 * [bt]: (2) 2   libScarab.dylib                     0x0000000102339bc2 _ZN6scarab14signal_handler22handle_terminate_errorEi + 434
 * [bt]: (3) 3   libsystem_platform.dylib            0x00007fff7300ab5d _sigtramp + 29
 * [bt]: (4) 4   ???                                 0x000000010dbbcf50 0x0 + 4525379408
 * [bt]: (5) 5   libsystem_c.dylib                   0x00007fff72e77d8a raise + 26
 * [bt]: (6) 6   test_raise_sigterm                  0x0000000102326f80 main + 32
 * [bt]: (7) 7   libdyld.dylib                       0x00007fff72e1f3d5 start + 1
 * [bt]: (8) 8   ???                                 0x0000000000000001 0x0 + 1
 * 
 * 2020-07-20 14:39:53 [DEBUG] (tid 0x10dbef5c0) ignal_handler.cc(273): Canceling all cancelables
 * 
 */

#include "signal_handler.hh"


#include <signal.h>


int main(int , char ** )
{
    scarab::signal_handler t_handler;

    raise( SIGTERM );

    return( EXIT_SUCCESS );
}
