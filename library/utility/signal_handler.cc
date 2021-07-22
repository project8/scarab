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
        signal_handler::unhandle_signals();
        --signal_handler::s_ref_count;
    }
/*
    void signal_handler::add_cancelable( std::shared_ptr< scarab::cancelable > a_cancelable )
    {
        signal_handler::add_cancelable_s( a_cancelable );
        return;
    }

    void signal_handler::remove_cancelable( std::shared_ptr< scarab::cancelable > a_cancelable )
    {
        signal_handler::remove_cancelable_s( a_cancelable );
        return;
    }

    void signal_handler::remove_cancelable( scarab::cancelable* a_cancelable )
    {
        signal_handler::remove_cancelable_s( a_cancelable );
        return;
    }
*/
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

        // setup to handle SIGABRT
        if( ! s_handling_sig_abrt && signal( SIGABRT, signal_handler::handle_exit_error ) == SIG_ERR )
        {
            LWARN( slog_constr, "Unable to setup handling of SIGABRT: abort() and unhandled exceptions will result in an unclean exit" );
        }
        else
        {
            LDEBUG( slog_constr, "Handling SIGABRT (abort() and unhandled exceptions)" );
            s_handling_sig_abrt = true;
        }

        // setup to handle SIGTERM
        if( ! s_handling_sig_term && signal( SIGTERM, signal_handler::handle_exit_error ) == SIG_ERR )
        {
            LWARN( slog_constr, "Unable to setup handling of SIGTERM: SIGTERM will result in an unclean exit" );
        }
        else
        {
            LDEBUG( slog_constr, "Handling SIGTERM" );
            s_handling_sig_term = true;
        }

        // setup to handle SIGINT
        if( ! s_handling_sig_int && signal( SIGINT, signal_handler::handle_exit_success ) == SIG_ERR )
        {
            LWARN( slog_constr, "Unable to setup handling of SIGINT: ctrl-c cancellation will result in an unclean exit" );
        }
        else
        {
            LDEBUG( slog_constr, "Handling SIGINT (ctrl-c)" );
            s_handling_sig_int = true;
        }

#ifndef _WIN32
        // setup to handle SIGQUIT
        if( ! s_handling_sig_quit && signal( SIGQUIT, signal_handler::handle_exit_success ) == SIG_ERR )
        {
            LWARN( slog_constr, "Unable to setup handling of SIGQUIT: ctrl-\\ cancellation will result in an unclean exit" );
        }
        else
        {
            LDEBUG( slog_constr, "Handling SIGQUIT (ctrl-\\)" );
            s_handling_sig_quit = true;
        }

        if( signal(SIGPIPE, SIG_IGN) == SIG_ERR )
        {
            throw error() << "Unable to ignore SIGPIPE\n";
        }
#endif
        return;
    }

    void signal_handler::unhandle_signals()
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );

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

    bool signal_handler::is_handling( int a_signal )
    {
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
        LERROR( slog, "Handling termination due to an error condition; signal <" << a_sig << ">" );
        exit( RETURN_ERROR );
        return;
    }

    void signal_handler::handle_exit_success( int a_sig )
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        LPROG( slog, "Handling termination; signal <" << a_sig << ">" );
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
