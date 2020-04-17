/*
 * signal_handler.cc
 *
 *  Created on: Dec 3, 2013
 *      Author: N.S. Oblath
 */

#define SCARAB_API_EXPORTS

#include "signal_handler.hh"

#include "cancelable.hh"
#include "error.hh"
#include "logger.hh"

#include <signal.h>
#include <thread>

#ifdef _WIN32
#include <Windows.h>
//#include "processthreadsapi.h"
#endif

namespace scarab
{
    LOGGER( slog, "signal_handler" );

    bool signal_handler::f_got_exit_signal = false;
    int signal_handler::f_return_code = RETURN_SUCCESS;

    bool signal_handler::f_handling_sig_int = false;
    bool signal_handler::f_handling_sig_quit = false;

    std::mutex signal_handler::f_mutex;
    signal_handler::cancelers signal_handler::f_cancelers;

    signal_handler::signal_handler()
    {
        // setup to handle SIGINT
        if( ! f_handling_sig_int && signal( SIGINT, signal_handler::handler_cancel_threads ) == SIG_ERR )
        {
            throw error() << "Unable to handle SIGINT\n";
        }
        else
        {
            f_handling_sig_int = true;
        }

#ifndef _WIN32
        // setup to handle SIGQUIT
        if( ! f_handling_sig_quit && signal( SIGQUIT, signal_handler::handler_cancel_threads ) == SIG_ERR )
        {
            throw error() << "Unable to handle SIGQUIT\n";
        }
        else
        {
            f_handling_sig_quit = true;
        }

        if( signal(SIGPIPE, SIG_IGN) == SIG_ERR )
        {
            throw error() << "Unable to ignore SIGPIPE\n";
        }
#endif
    }

    signal_handler::~signal_handler()
    {
    }

    void signal_handler::add_cancelable( scarab::cancelable* a_cancelable )
    {
        f_mutex.lock();
        f_cancelers.insert( a_cancelable );
        f_mutex.unlock();
        return;
    }

    void signal_handler::remove_cancelable( scarab::cancelable* a_cancelable )
    {
        f_mutex.lock();
        f_cancelers.erase( a_cancelable );
        f_mutex.unlock();
        return;
    }

    void signal_handler::reset()
    {
        f_mutex.lock();
        f_got_exit_signal = false;
        f_return_code = RETURN_SUCCESS;
        f_handling_sig_int = false;
        f_handling_sig_quit = false;
        f_cancelers.clear();
        f_mutex.unlock();
        return;
    }

    bool signal_handler::got_exit_signal()
    {
        return f_got_exit_signal;
    }

    int signal_handler::get_return_code()
    {
        return f_return_code;
    }

    void signal_handler::handler_cancel_threads( int )
    {
        LPROG( slog, "\n\nHello!  Your signal is being handled by signal_handler.\n"
             << "Have a nice day!\n" );
        cancel_all( RETURN_SUCCESS );
        return;
    }

    void signal_handler::cancel_all( int a_code )
    {
        LDEBUG( slog, "Canceling all cancelables" );

        f_mutex.lock();
        f_got_exit_signal = true;
        f_return_code = a_code;
        while( ! f_cancelers.empty() )
        {
            (*f_cancelers.begin())->cancel( a_code );
            f_cancelers.erase( f_cancelers.begin() );
            std::this_thread::sleep_for( std::chrono::seconds(1) );
        }
        f_mutex.unlock();

#ifdef _WIN32
        ExitProcess( a_code );
#endif

        return;
    }

} /* namespace scarab */
