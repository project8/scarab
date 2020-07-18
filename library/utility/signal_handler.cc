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

#include <execinfo.h>
#include <signal.h>
#include <stdexcept>
#include <thread>

//#include <signal.h>
//#include <string.h>

//#include <cstdlib>
//#include <mutex>


#ifdef _WIN32
#include <Windows.h>
//#include "processthreadsapi.h"
#endif

namespace {
    // function to catch unhandled exceptions
    // invoke set_terminate as part of global constant initialization
    static const bool SET_TERMINATE = std::set_terminate( scarab::signal_handler::handle_termination );
    static const scarab::signal_handler HANDLER;
}


namespace scarab
{
    LOGGER( slog, "signal_handler" );

    bool signal_handler::s_terminated = false;
    int signal_handler::s_return_code = RETURN_SUCCESS;

    bool signal_handler::s_handling_sig_abrt = false;
    bool signal_handler::s_handling_sig_term = false;
    bool signal_handler::s_handling_sig_int = false;
    bool signal_handler::s_handling_sig_quit = false;

    std::recursive_mutex signal_handler::s_mutex;
    signal_handler::cancelers signal_handler::s_cancelers;

    signal_handler::signal_handler()
    {
        // setup to handle SIGABRT
        if( ! s_handling_sig_abrt && signal( SIGABRT, signal_handler::handle_terminate_error ) == SIG_ERR )
        {
            LWARN( slog, "Unable to setup handling of SIGABRT: abort() and unhandled exceptions will result in an unclean exit" );
        }
        else
        {
            LDEBUG( slog, "Handling SIGABRT (abort() and unhandled exceptions)" );
            s_handling_sig_abrt = true;
        }

        // setup to handle SIGTERM
        if( ! s_handling_sig_term && signal( SIGTERM, signal_handler::handle_terminate_error ) == SIG_ERR )
        {
            LWARN( slog, "Unable to setup handling of SIGTERM: SIGTERM will result in an unclean exit" );
        }
        else
        {
            LDEBUG( slog, "Handling SIGTERM" );
            s_handling_sig_term = true;
        }

        // setup to handle SIGINT
        if( ! s_handling_sig_int && signal( SIGINT, signal_handler::handle_terminate_success ) == SIG_ERR )
        {
            LWARN( slog, "Unable to setup handling of SIGINT: ctrl-c cancellation will result in an unclean exit" );
        }
        else
        {
            LDEBUG( slog, "Handling SIGINT (ctrl-c)" );
            s_handling_sig_int = true;
        }

#ifndef _WIN32
        // setup to handle SIGQUIT
        if( ! s_handling_sig_quit && signal( SIGQUIT, signal_handler::handle_terminate_success ) == SIG_ERR )
        {
            LWARN( slog, "Unable to setup handling of SIGQUIT: ctrl-\\ cancellation will result in an unclean exit" );
        }
        else
        {
            LDEBUG( slog, "Handling SIGQUIT (ctrl-\\)" );
            s_handling_sig_quit = true;
        }

        if( signal(SIGPIPE, SIG_IGN) == SIG_ERR )
        {
            throw error() << "Unable to ignore SIGPIPE\n";
        }
#endif
    }

    signal_handler::~signal_handler()
    {
        reset();
    }

    void signal_handler::add_cancelable( scarab::cancelable* a_cancelable )
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        s_cancelers.insert( a_cancelable );
        return;
    }

    void signal_handler::remove_cancelable( scarab::cancelable* a_cancelable )
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        s_cancelers.erase( a_cancelable );
        return;
    }

    void signal_handler::reset()
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        LDEBUG( slog, "Resetting signal_handler" );
        s_terminated = false;
        s_return_code = RETURN_SUCCESS;
        s_cancelers.clear();
        if( s_handling_sig_abrt && signal( SIGABRT, SIG_DFL ) == SIG_ERR )
        {
            LWARN( slog, "Unable to switch SIGABRT to default handler" );
        }
        else
        {
            s_handling_sig_abrt = false;
        }
        
        if( s_handling_sig_term && signal( SIGTERM, SIG_DFL ) == SIG_ERR )
        {
            LWARN( slog, "Unable to switch SIGTERM to default handler" );
        }
        else
        {
            s_handling_sig_term = false;
        }
        
        if( s_handling_sig_int && signal( SIGINT, SIG_DFL ) == SIG_ERR )
        {
            LWARN( slog, "Unable to switch SIGINT to default handler" );
        }
        else
        {
            s_handling_sig_int = false;
        }
        
#ifndef _WIN32
        if( s_handling_sig_quit && signal( SIGQUIT, SIG_DFL ) == SIG_ERR )
        {
            LWARN( slog, "Unable to switch SIGQUIT to default handler" );
        }
        else
        {
            s_handling_sig_quit = false;
        }
        
#endif
        return;
    }

    void signal_handler::handle_termination()
    {
        handle_terminate_error( SIGABRT );
        return;
    }

    void signal_handler::handle_terminate_error( int a_sig )
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        LERROR( slog, "Handling termination due to an error condition; signal <" << a_sig << ">" );
        terminate( RETURN_ERROR );
        return;
    }

    void signal_handler::handle_terminate_success( int a_sig )
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        LPROG( slog, "Handling termination; signal <" << a_sig << ">" );
        terminate( RETURN_SUCCESS );
        return;
    }

    void signal_handler::terminate( int a_code )
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        s_terminated = true;
        s_return_code = a_code;
        print_current_exception();
        if( a_code > 0 )
        {
            print_stack_trace();
        }
        cancel_all( a_code );
        return;
    }

    void signal_handler::print_current_exception()
    {
        // no mutex locking needed here

        // if there's a current exception, rethrow to print out what()
        std::exception_ptr t_exc_ptr = std::current_exception();
        if( t_exc_ptr )
        {
            static bool t_tried_throw = false;

            try
            {
                // try once to re-throw currently active exception
                if( ! t_tried_throw )
                {
                    LDEBUG( slog, "Rethrowing current exception" );
                    //t_tried_throw = true;
                    throw;
                }
            }
            catch( const std::exception &e )
            {
                LERROR( slog, "Caught unhandled exception. what(): " << e.what() );
            }
            catch (...)
            {
                LERROR( slog, "Caught unknown (non-std::exception) & unhandled exception." );
            }
        }
        return;
    }

    void signal_handler::print_stack_trace()
    {
        // no mutex locking needed here
#ifndef _WIN32 // stack trace printing not implemented for windows
        void* t_bt_array[50];
        int t_size = backtrace( t_bt_array, 50 );

        LERROR( slog, "Backtrace from terminate() returned "
                << t_size << " frames\n" );

        char** t_messages = backtrace_symbols( t_bt_array, t_size );

        std::stringstream t_bt_str;
        for( int i = 0; i < t_size && t_messages != nullptr; ++i )
        {
            t_bt_str << "[bt]: (" << i << ") " << t_messages[i] << '\n';
        }
        LERROR( slog, "Backtrace:\n" << t_bt_str.str() );

        free( t_messages );
#endif
        return;
    }

    void signal_handler::cancel_all( int a_code )
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        LDEBUG( slog, "Canceling all cancelables" );

        while( ! s_cancelers.empty() )
        {
            (*s_cancelers.begin())->cancel( a_code );
            s_cancelers.erase( s_cancelers.begin() );
            std::this_thread::sleep_for( std::chrono::seconds(1) );
        }

#ifdef _WIN32
        ExitProcess( a_code );
#endif

        return;
    }

} /* namespace scarab */
