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
#include <stdexcept>
#include <thread>

#ifdef _WIN32
#include <csignal>
#include <Windows.h>
//#include "processthreadsapi.h"
#endif

namespace {
    // function to catch unhandled exceptions
    // invoke set_terminate as part of global constant initialization
    static const bool SET_TERMINATE = std::set_terminate( scarab::signal_handler::handle_terminate );
}


namespace scarab
{
    LOGGER( slog, "signal_handler" );

    int signal_handler::s_ref_count = 0;

    bool signal_handler::s_exited = false;
    int signal_handler::s_return_code = RETURN_SUCCESS;

    bool signal_handler::s_handling_sig_abrt = false;
    bool signal_handler::s_handling_sig_term = false;
    bool signal_handler::s_handling_sig_int = false;
    bool signal_handler::s_handling_sig_quit = false;

#ifndef _WIN32
    struct sigaction signal_handler::s_old_sig_abrt_action;
    struct sigaction signal_handler::s_old_sig_term_action;
    struct sigaction signal_handler::s_old_sig_int_action;
    struct sigaction signal_handler::s_old_sig_quit_action;
#else // _WIN32
    signal_handler::handler_t signal_handler::s_old_sig_abrt_handler = SIG_DFL;
    signal_handler::handler_t signal_handler::s_old_sig_term_handler = SIG_DFL;
    signal_handler::handler_t signal_handler::s_old_sig_int_handler = SIG_DFL;
    signal_handler::handler_t signal_handler::s_old_sig_quit_handler = SIG_DFL;
#endif

    std::recursive_mutex signal_handler::s_mutex;
    signal_handler::cancelers signal_handler::s_cancelers;

    signal_handler::signal_handler()
    {
        //std::cerr << "signal_handler constructor" << std::endl;
        ++signal_handler::s_ref_count;

        signal_handler::handle_signals();
    }

    signal_handler::~signal_handler()
    {
        //std::cerr << "signal_handler destructor" << std::endl;
        --signal_handler::s_ref_count;
        if( signal_handler::s_ref_count == 0 )
        {
            signal_handler::unhandle_signals();
        }
    }

    void signal_handler::add_cancelable( std::shared_ptr< scarab::cancelable > a_cancelable )
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        s_cancelers.insert( std::make_pair(a_cancelable.get(), cancelable_wptr_t(a_cancelable) ) );
        return;
    }

    void signal_handler::remove_cancelable( std::shared_ptr< scarab::cancelable > a_cancelable )
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        s_cancelers.erase( a_cancelable.get() );
        return;
    }

    void signal_handler::remove_cancelable( scarab::cancelable* a_cancelable )
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        s_cancelers.erase( a_cancelable );
        return;
    }

    void signal_handler::handle_signals()
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );

        // we create a new logger here so that handle_signals() can be called from the signal_handler constructor during static initalization
        LOGGER( slog_constr, "signal_handler handle_signals" );

        LDEBUG( slog_constr, "Taking over signal handling for SIGABRT, SIGTERM, SIGINT, and SIGQUIT" );

#ifndef _WIN32  // on a POSIX system we use sigaction

        struct sigaction t_exit_error_action, t_exit_success_action;

        t_exit_error_action.sa_handler = signal_handler::handle_exit_error;
        sigemptyset(&t_exit_error_action.sa_mask);
        t_exit_error_action.sa_flags = 0;

        t_exit_success_action.sa_handler = signal_handler::handle_exit_success;
        sigemptyset(&t_exit_success_action.sa_mask);
        t_exit_success_action.sa_flags = 0;

        // setup to handle SIGABRT
        if( ! s_handling_sig_abrt && sigaction( SIGABRT, &t_exit_error_action, &s_old_sig_abrt_action ) != 0 )
        {
            LWARN( slog_constr, "Unable to setup handling of SIGABRT: abort() and unhandled exceptions will result in an unclean exit" );
        }
        else
        {
            LTRACE( slog_constr, "Handling SIGABRT (abort() and unhandled exceptions)" );
            s_handling_sig_abrt = true;
        }

        // setup to handle SIGTERM
        if( ! s_handling_sig_term && sigaction( SIGTERM, &t_exit_error_action, &s_old_sig_term_action ) != 0 )
        {
            LWARN( slog_constr, "Unable to setup handling of SIGTERM: SIGTERM will result in an unclean exit" );
        }
        else
        {
            LTRACE( slog_constr, "Handling SIGTERM" );
            s_handling_sig_term = true;
        }

        // setup to handle SIGINT
        if( ! s_handling_sig_int && sigaction( SIGINT, &t_exit_success_action, &s_old_sig_int_action ) != 0 )
        {
            LWARN( slog_constr, "Unable to setup handling of SIGINT: ctrl-c cancellation will result in an unclean exit" );
        }
        else
        {
            LTRACE( slog_constr, "Handling SIGINT (ctrl-c)" );
            s_handling_sig_int = true;
        }

        // setup to handle SIGQUIT
        if( ! s_handling_sig_quit && sigaction( SIGQUIT, &t_exit_success_action, &s_old_sig_quit_action ) != 0 )
        {
            LWARN( slog_constr, "Unable to setup handling of SIGQUIT: ctrl-\\ cancellation will result in an unclean exit" );
        }
        else
        {
            LTRACE( slog_constr, "Handling SIGQUIT (ctrl-\\)" );
            s_handling_sig_quit = true;
        }

        if( signal(SIGPIPE, SIG_IGN) == SIG_ERR )
        {
            throw error() << "Unable to ignore SIGPIPE\n";
        }

#else // _WIN32; on a Windows system we use std::signal

        // setup to handle SIGABRT
        if( ! s_handling_sig_abrt )
        {
            auto t_sig_ret = signal( SIGABRT, signal_handler::handle_exit_error );
            if( t_sig_ret == SIG_ERR )
            {
                LWARN( slog_constr, "Unable to setup handling of SIGABRT: abort() and unhandled exceptions will result in an unclean exit" );
            }
            else
            {
                s_handling_sig_abrt = true;
                s_old_sig_abrt_handler = t_sig_ret;
            }
        }
        if( s_handling_sig_abrt ) LTRACE( slog_constr, "Handling SIGABRT (abort() and unhandled exceptions)" );


        // setup to handle SIGTERM
        if( ! s_handling_sig_term )
        {
            auto t_sig_ret = signal( SIGTERM, signal_handler::handle_exit_error );
            if( t_sig_ret == SIG_ERR )
            {
                LWARN( slog_constr, "Unable to setup handling of SIGTERM: SIGTERM will result in an unclean exit" );
            }
            else
            {
                s_handling_sig_term = true;
                s_old_sig_abrt_handler = t_sig_ret;
            }
        }
        if( s_handling_sig_term ) LTRACE( slog_constr, "Handling SIGTERM" );

        // setup to handle SIGINT
        if( ! s_handling_sig_int )
        {
            auto t_sig_ret = signal( SIGINT, signal_handler::handle_exit_error );
            if( t_sig_ret == SIG_ERR )
            {
                LWARN( slog_constr, "Unable to setup handling of SIGINT: ctrl-c cancellation will result in an unclean exit" );
            }
            else
            {
                s_handling_sig_int = true;
                s_old_sig_abrt_handler = t_sig_ret;
            }
        }
        if( s_handling_sig_int ) LTRACE( slog_constr, "Handling SIGINT (ctrl-c))" );

#endif
        return;
    }

    void signal_handler::unhandle_signals()
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );

        LDEBUG( slog, "Returning signal handling for SIGABRT, SIGTERM, SIGINT, and SIGQUIT" );

#ifndef _WIN32

        if( s_handling_sig_abrt && sigaction( SIGABRT, &s_old_sig_abrt_action, nullptr ) != 0 )
        {
            LWARN( slog, "Unable to switch SIGABRT to previous handler" );
        }
        else
        {
            s_handling_sig_abrt = false;
        }
        
        if( s_handling_sig_term && sigaction( SIGTERM, &s_old_sig_term_action, nullptr ) != 0 )
        {
            LWARN( slog, "Unable to switch SIGTERM to previous handler" );
        }
        else
        {
            s_handling_sig_term = false;
        }
        
        if( s_handling_sig_int && sigaction( SIGINT, &s_old_sig_int_action, nullptr ) != 0 )
        {
            LWARN( slog, "Unable to switch SIGINT to previous handler" );
        }
        else
        {
            s_handling_sig_int = false;
        }
        
        if( s_handling_sig_quit && sigaction( SIGQUIT, &s_old_sig_quit_action, nullptr ) != 0 )
        {
            LWARN( slog, "Unable to switch SIGQUIT to previous handler" );
        }
        else
        {
            s_handling_sig_quit = false;
        }

#else // _WIN32

        if( s_handling_sig_abrt && signal( SIGABRT, s_old_sig_abrt_handler ) == SIG_ERR )
        {
            LWARN( slog, "Unable to switch SIGABRT to previous handler" );
        }
        else
        {
            s_handling_sig_abrt = false;
        }
        
        if( s_handling_sig_term && signal( SIGTERM, s_old_sig_term_handler ) == SIG_ERR )
        {
            LWARN( slog, "Unable to switch SIGTERM to previous handler" );
        }
        else
        {
            s_handling_sig_term = false;
        }
        
        if( s_handling_sig_int && signal( SIGINT, s_old_sig_int_handler ) == SIG_ERR )
        {
            LWARN( slog, "Unable to switch SIGINT to previous handler" );
        }
        else
        {
            s_handling_sig_int = false;
        }
       
#endif
        return;
    }

    bool signal_handler::is_handling( int a_signal )
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );

        bool t_is_handled = false;
        switch( a_signal )
        {
            case SIGABRT:
                if( s_handling_sig_abrt) t_is_handled = true;
                break;
            case SIGTERM:
                if( s_handling_sig_term) t_is_handled = true;
                break;
            case SIGINT:
                if( s_handling_sig_int) t_is_handled = true;
                break;
#ifndef _WIN32
            case SIGQUIT:
                if( s_handling_sig_quit) t_is_handled = true;
                break;
#endif
            default:
                break;
        }
        return t_is_handled;
    }

    void signal_handler::reset()
    {
        LDEBUG( slog, "Resetting signal_handler" );
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );

        s_exited = false;
        s_return_code = RETURN_SUCCESS;
        s_cancelers.clear();

        signal_handler::unhandle_signals();
        return;
    }

    [[noreturn]] void signal_handler::handle_terminate() noexcept
    {
        terminate( RETURN_ERROR );
    }

    void signal_handler::handle_exit_error( int a_sig )
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        LERROR( slog, "Handling signal <" << a_sig << "> as an error condition; return code: " << RETURN_ERROR );
        exit( RETURN_ERROR );
        return;
    }

    void signal_handler::handle_exit_success( int a_sig )
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        LPROG( slog, "Handling signal <" << a_sig << ">; return code: " << RETURN_SUCCESS );
        exit( RETURN_SUCCESS );
        return;
    }

    [[noreturn]] void signal_handler::terminate( int a_code ) noexcept
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        print_current_exception( false );
        if( a_code > 0 )
        {
            print_stack_trace( false );
        }
        std::cerr << "Exiting abruptly" << std::endl;
        std::_Exit( a_code );
    }

    void signal_handler::exit( int a_code )
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        s_exited = true;
        s_return_code = a_code;
        print_current_exception( true );
        if( a_code > 0 )
        {
            print_stack_trace( true );
        }
        cancel_all( a_code );
        return;
    }

    void signal_handler::print_current_exception( bool a_use_logging )
    {
        // no mutex locking needed here

        // if there's a current exception, rethrow to print out what()
        if( auto t_exc_ptr = std::current_exception() ) 
        { 
            // we have an exception
            try
            {
                if( a_use_logging ) { LDEBUG( slog, "Rethrowing current exception" ); }
                else { std::cerr << "Rethrowing current exception" << std::endl; }
                std::rethrow_exception( t_exc_ptr ); // throw to recognize the type
            }
            catch( const std::exception& e ) {
                if( a_use_logging ) { LERROR( slog, "Caught unhandled exception. what(): " << e.what() ); }
                else { std::cerr << "Caught unhandled exception. what(): " << e.what() << std::endl; }
            }
            catch( ... ) {
                if( a_use_logging ) LERROR( slog, "Caught unknown (non-std::exception) & unhandled exception." )
                else { std::cerr << "Caught unknown (non-std::exception) & unhandled exception." << std::endl; }
            }
        }
        return;
    }

    void signal_handler::print_stack_trace( bool a_use_logging )
    {
        // no mutex locking needed here
#ifndef _WIN32 // stack trace printing not implemented for windows
        void* t_bt_array[50];
        int t_size = backtrace( t_bt_array, 50 );

        if( a_use_logging ) { LERROR( slog, "Backtrace returned " << t_size << " frames\n" ); }
        else { std::cerr << "Backtrace returned " << t_size << " frames\n" << std::endl; }

        char** t_messages = backtrace_symbols( t_bt_array, t_size );

        std::stringstream t_bt_str;
        for( int i = 0; i < t_size && t_messages != nullptr; ++i )
        {
            t_bt_str << "[bt]: (" << i << ") " << t_messages[i] << '\n';
        }
        if( a_use_logging ) { LERROR( slog, "Backtrace:\n" << t_bt_str.str() ); }
        else { std::cerr << "Backtrace:\n" << t_bt_str.str() << std::endl; }

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
            auto t_canceler_it = s_cancelers.begin();
            if( ! t_canceler_it->second.expired() )
            {
                t_canceler_it->second.lock()->cancel( a_code );
            }
            s_cancelers.erase( t_canceler_it );
            std::this_thread::sleep_for( std::chrono::seconds(1) );
        }

#ifdef _WIN32
        ExitProcess( a_code );
#endif

        return;
    }

} /* namespace scarab */
