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

    sigset_t signal_handler::s_sigset;
    std::mutex signal_handler::s_handle_mutex;

    signal_handler::signal_handler()
    {
        //std::cerr << "signal_handler constructor" << std::endl;
        ++signal_handler::s_ref_count;
        if( signal_handler::s_ref_count == 1 )
        {
            LWARN( slog, "Blocking signals SIGABRT, SIGTERM, SIGINT, SIGQUIT, and SIGUSR1" );

            sigemptyset( &s_sigset );
            sigaddset( &s_sigset, SIGABRT );
            sigaddset( &s_sigset, SIGTERM );
            sigaddset( &s_sigset, SIGINT );
            sigaddset( &s_sigset, SIGQUIT );
            sigaddset( &s_sigset, SIGUSR1);

            if( pthread_sigmask( SIG_BLOCK, &s_sigset, nullptr ) != 0 )
            {
                LERROR( slog, "Unable to block the required signals" );
                std::cerr << "Unable to block the required signals" << std::endl;
                throw error(__FILE_NAME__, __LINE__) << "Unable to block the required signals";
            }
        }
    }

    signal_handler::~signal_handler()
    {
        //std::cerr << "signal_handler destructor" << std::endl;
        --signal_handler::s_ref_count;
        if( signal_handler::s_ref_count == 0 )
        {
            abort_handle_signals();
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

    int signal_handler::do_handle_signals()
    {
        std::unique_lock t_lock( s_handle_mutex, std::defer_lock_t() );
        if( ! t_lock.try_lock() ) return -1;

        LWARN( slog, "Starting to handle signals SIGABRT, SIGTERM, SIGINT, SIGQUIT, and SIGUSR1" );

        auto sigwait_fcn = []() {
            LOGGER( sigwait_log, "signal_handler handle_signals" );

            std::cerr << "In sigwait lambda" << std::endl;

            // wait until a signal is delivered, an abort was requested, or an error occurs
            int signum = 0;
            int error_num = sigwait( &s_sigset, &signum );
            std::cerr << "Done with sigwait" << std::endl;

            if( error_num != 0 )
            {
                LERROR( sigwait_log, "An error occurred while waiting for signals: error code = " << error_num );
                std::cerr << "An error occurred while waiting for signals: error code = " << error_num << std::endl;
                return -1;
            }

            if( signum == SIGABRT || signum == SIGTERM )
            {
                // An error-like signal was received
                LERROR( sigwait_log, "Received signal <" << signum << "> as an error condition; return code: " << RETURN_ERROR );
                std::cerr << "Received signal <" << signum << "> as an error condition; return code: " << RETURN_ERROR << std::endl;
                exit( RETURN_ERROR );
            }
            else if( signum == SIGINT || signum == SIGQUIT )
            {
                // A non-error-like signal was received
                LWARN( sigwait_log, "Received signal <" << signum << ">; return code: " << RETURN_SUCCESS );
                std::cerr << "Received signal <" << signum << ">; return code: " << RETURN_SUCCESS << std::endl;
                exit( RETURN_SUCCESS );
            }
            else if( signum == SIGUSR1 )
            {
                // A request to stop handling signals was received
                LWARN( sigwait_log, "Received request to stop handling signals" );
                std::cerr << "Received request to stop handling signals" << std::endl;
            }
            else
            {
                // Unexpected signal
                LERROR( sigwait_log, "An unexpected signal was recieved: " << signum );
                LERROR( sigwait_log, "Quitting with an error condition" );
                std::cerr << "An unexpected signal was recieved: " << signum << std::endl;
                exit( RETURN_ERROR );
            }

            return signum;
        };

        auto ft_signal_handler = std::async( std::launch::async, sigwait_fcn );

        int sig_received = ft_signal_handler.get();
        LWARN( slog, "Signal-waiting function exited; received signal " << sig_received );
        std::cerr << "Signal-waiting function exited; received signal " << sig_received << std::endl;

        return sig_received;
    }

    int signal_handler::wait_on_signals()
    {
            LOGGER( sigwait_log, "signal_handler handle_signals" );

            LWARN( sigwait_log, "In wait-on-signals");
            std::cerr << "In wait-on-signals" << std::endl;

            // wait until a signal is delivered, an abort was requested, or an error occurs
            int signum = 0;
            int error_num = sigwait( &s_sigset, &signum );
            LWARN( sigwait_log, "Done with sigwait" );
            std::cerr << "Done with sigwait" << std::endl;

            if( error_num != 0 )
            {
                LERROR( sigwait_log, "An error occurred while waiting for signals: error code = " << error_num );
                std::cerr << "An error occurred while waiting for signals: error code = " << error_num << std::endl;
                return -1;
            }

            if( signum == SIGABRT || signum == SIGTERM )
            {
                // An error-like signal was received
                LERROR( sigwait_log, "Received signal <" << signum << "> as an error condition; return code: " << RETURN_ERROR );
                std::cerr << "Received signal <" << signum << "> as an error condition; return code: " << RETURN_ERROR << std::endl;
                exit( RETURN_ERROR );
            }
            else if( signum == SIGINT || signum == SIGQUIT )
            {
                // A non-error-like signal was received
                LWARN( sigwait_log, "Received signal <" << signum << ">; return code: " << RETURN_SUCCESS );
                std::cerr << "Received signal <" << signum << ">; return code: " << RETURN_SUCCESS << std::endl;
                exit( RETURN_SUCCESS );
            }
            else if( signum == SIGUSR1 )
            {
                // A request to stop handling signals was received
                LWARN( sigwait_log, "Received request to stop handling signals" );
                std::cerr << "Received request to stop handling signals" << std::endl;
            }
            else
            {
                // Unexpected signal
                LERROR( sigwait_log, "An unexpected signal was recieved: " << signum );
                LERROR( sigwait_log, "Quitting with an error condition" );
                std::cerr << "An unexpected signal was recieved: " << signum << std::endl;
                exit( RETURN_ERROR );
            }

            return signum;
    }

    void signal_handler::abort_handle_signals()
    {
        LDEBUG( slog, "Requesting stop of signal handling" );
        std::cerr << "Requesting stop of signal handling" << std::endl;
        raise( SIGUSR1 );

        std::this_thread::sleep_for( std::chrono::milliseconds(500) ); // pause before unblocking so the raised signal can be handled

        LDEBUG( slog, "Unblocking signals" );
        std::cerr << "Unblocking sigals" << std::endl;
        if( pthread_sigmask( SIG_UNBLOCK, &s_sigset, nullptr ) != 0 )
        {
            LERROR( slog, "Unable to unblock the required signals" );
            std::cerr << "Unable to unblock the required signals" << std::endl;
            throw error(__FILE_NAME__, __LINE__) << "Unable to unblock the required signals";
        }

        return;
    }

    bool signal_handler::is_handling()
    {
        std::unique_lock t_lock( s_handle_mutex, std::defer_lock_t() );
        return ! t_lock.try_lock();
    }

    void signal_handler::reset()
    {
        LDEBUG( slog, "Resetting signal_handler" );

        signal_handler::abort_handle_signals();

        s_exited = false;
        s_return_code = RETURN_SUCCESS;
        s_cancelers.clear();

        return;
    }

    [[noreturn]] void signal_handler::handle_terminate() noexcept
    {
        terminate( RETURN_ERROR );
    }
/*
    void signal_handler::handle_exit_error( int a_sig )
    {
        signal_handler::s_exit_signal = a_sig;
        //std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        //LERROR( slog, "Handling signal <" << a_sig << "> as an error condition; return code: " << RETURN_ERROR );
        //exit( RETURN_ERROR );
        return;
    }

    void signal_handler::handle_exit_success( int a_sig )
    {
        signal_handler::s_exit_signal = a_sig;
        //std::unique_lock< std::recursive_mutex > t_lock( s_mutex );
        //LPROG( slog, "Handling signal <" << a_sig << ">; return code: " << RETURN_SUCCESS );
        //exit( RETURN_SUCCESS );
        return;
    }
*/
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
