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
#include <future>
#include <signal.h>

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

    std::recursive_mutex signal_handler::s_mutex;
    signal_handler::cancelers signal_handler::s_cancelers;

    bool signal_handler::s_is_handling = false;

    std::mutex signal_handler::s_handle_mutex;

    signal_handler::signal_map_t signal_handler::s_handled_signals = {
        {SIGABRT, signal_handler::handled_signal_info{"SIGABRT", false, {}, true, RETURN_ERROR}},
        {SIGTERM, signal_handler::handled_signal_info{"SIGTERM", false, {}, true, RETURN_ERROR}},
        {SIGINT,  signal_handler::handled_signal_info{"SIGINT",  false, {}, false, RETURN_SUCCESS}},
        {SIGQUIT, signal_handler::handled_signal_info{"SIGQUIT", false, {}, false, RETURN_SUCCESS}}
    };

    volatile sig_atomic_t signal_handler::s_signal_received = 0;

    signal_handler::signal_handler()
    {
        ++signal_handler::s_ref_count;
        if( signal_handler::s_ref_count == 1 )
        {
            signal_handler::start_handling_signals();
        }
    }

    signal_handler::~signal_handler()
    {
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

    void signal_handler::print_cancelables()
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        LOGGER( slog_print, "signal_handler print_cancelables" );
        for( const auto& [t_key, t_value] : s_cancelers )
        {
            LINFO( slog_print, "Canceler: " << t_key );
        }
        return;
    }

    void signal_handler::start_handling_signals()
    {
        if( signal_handler::get_is_handling() || signal_handler::is_waiting() ) return;

        std::unique_lock t_lock( s_handle_mutex );

        LOGGER( slog_constr, "signal_handler::start_handling_signals" );
        LDEBUG( slog_constr, "Taking over signal handling for SIGABRT, SIGTERM, SIGINT, and SIGQUIT" );

        struct sigaction t_action;
        t_action.sa_handler = signal_handler::handle_signal;
        sigemptyset(&t_action.sa_mask);
        t_action.sa_flags = 0;

        for( auto it = signal_handler::s_handled_signals.begin(); it != signal_handler::s_handled_signals.end(); ++it )
        {
            if( ! it->second.handling && sigaction( it->first, &t_action, &it->second.old_action ) != 0 )
            {
                LWARN( slog_constr, "Unable to setup handling of " << it->second.name );
            }
            else
            {
                LTRACE( slog_constr, "Handling " <<  it->second.name );
                it->second.handling = true;
            }
        }

        signal_handler::s_is_handling = true;

        return;
    }

    bool signal_handler::is_handling()
    {
        return signal_handler::get_is_handling();
    }

    void signal_handler::unhandle_signals()
    {
        if( ! signal_handler::get_is_handling() ) return;

        if( signal_handler::is_waiting() ) signal_handler::abort_wait();

        std::unique_lock t_lock( s_handle_mutex );

        LDEBUG( slog, "Returning signal handling for SIGABRT, SIGTERM, SIGINT, and SIGQUIT" );

        for( auto it = signal_handler::s_handled_signals.begin(); it != signal_handler::s_handled_signals.end(); ++it )
        {
            if( it->second.handling && sigaction( it->first, &it->second.old_action, nullptr ) != 0 )
            {
                LWARN( slog, "Unable to switch " << it->second.name << " to previous handler" );
            }
            else
            {
                LTRACE( slog, "Stopped handling " <<  it->second.name );
                it->second.handling = false;
            }
        }

        signal_handler::s_is_handling = false;

        return;
    }

    int signal_handler::wait_for_signals( bool call_exit )
    {
        // Try to lock the mutex; if it didn't lock, then return
        std::unique_lock t_lock( s_handle_mutex, std::defer_lock_t() );
        if( ! t_lock.try_lock() ) return -1;

        while( signal_handler::s_signal_received == 0 )
        {
            std::this_thread::sleep_for( std::chrono::milliseconds(500)) ;
        }

        // Handle situation where a signal was recieved
        if( s_signal_received > 0 )
        {
            // this is a valid signal
            LDEBUG( slog, "Exiting loop for signals after getting signal " << s_signal_received );
            if( signal_handler::s_handled_signals.count( int(signal_handler::s_signal_received) ) > 0 )
            {
                handled_signal_info& sig_info = signal_handler::s_handled_signals.at( int(signal_handler::s_signal_received) );
                if( sig_info.indicates_error )
                {
                    LERROR( slog, "Received signal " << sig_info.name << "<" << s_signal_received << "> as an error condition; return code: " << sig_info.return_code );
                }
                else
                {
                    LDEBUG( slog, "Received signal " << sig_info.name << "<" << s_signal_received << "> as an exit condition; return code: " << sig_info.return_code );
                }

                // Call exit to cancel cancelables if requested
                if( call_exit ) exit( sig_info.return_code );
            }
            else
            {
                LERROR( slog, "Received unknown signal <" << s_signal_received << ">; using with return code " << RETURN_ERROR );
                if( call_exit ) exit( RETURN_ERROR );
            }

            return s_signal_received; 
        }

        // Handle the situation where no signal was received; wait loop was aborted
        // s_signal_received < 0 --> requested to exit wait for signals
        LDEBUG( slog, "Exited loop waiting for signals (wait aborted)" );
        return s_signal_received;

    }

    void signal_handler::abort_wait()
    {
        LDEBUG( slog, "Requesting stop of signal wait" );
        signal_handler::s_signal_received = -1;
        return;
    }

    bool signal_handler::is_waiting()
    {
        std::unique_lock t_lock( s_handle_mutex, std::defer_lock_t() );
        return ! t_lock.try_lock();
    }

    void signal_handler::reset()
    {
        LDEBUG( slog, "Resetting signal_handler" );

        signal_handler::unhandle_signals();

        s_exited = false;
        s_return_code = RETURN_SUCCESS;
        s_cancelers.clear();

        return;
    }

    [[noreturn]] void signal_handler::handle_terminate() noexcept
    {
        terminate( RETURN_ERROR );
    }

    void signal_handler::handle_signal( int a_sig )
    {
        signal_handler::s_signal_received = a_sig;

        const char str[] = "Received a signal\n";
        write( STDERR_FILENO, str, sizeof(str)-1 );

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
        std::_Exit( a_code );
    }

    void signal_handler::exit( int a_code )
    {
        std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        s_exited = true;
        s_return_code = a_code;
        print_current_exception( true );
        // Removed stack trace printing because it's just the trace at the point exit() is called, which is often the wait_for_signals(), which isn't very useful
        //if( a_code > 0 )
        //{
        //    print_stack_trace( true );
        //}
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
            catch( const scarab::error& e ) 
            {
                if( a_use_logging ) 
                { 
                    LERROR( slog, "Caught unhandled exception. what(): " << e.what() ); 
                    LERROR( slog, "\twhere(): " << e.where() );
                }
                else 
                { 
                    std::cerr << "Caught unhandled exception. what(): " << e.what() << std::endl; 
                    std::cerr << "\twhere(): " << e.where() << std::endl;;
                }

            }
            catch( const std::exception& e ) 
            {
                if( a_use_logging ) { LERROR( slog, "Caught unhandled exception. what(): " << e.what() ); }
                else { std::cerr << "Caught unhandled exception. what(): " << e.what() << std::endl; }
            }
            catch( ... ) 
            {
                if( a_use_logging ) { LERROR( slog, "Caught unknown (non-std::exception) & unhandled exception." ); }
                else { std::cerr << "Caught unknown (non-std::exception) & unhandled exception." << std::endl; }
            }
        }
        return;
    }

    void signal_handler::print_stack_trace( bool a_use_logging )
    {
        // no mutex locking needed here
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

        return;
    }

} /* namespace scarab */
