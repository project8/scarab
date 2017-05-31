/*
 * terminate.cc
 *
 *  Created on: May 23, 2017
 *      Author: N.S. Oblath
 */

#include "terminate.hh"

#include "logger.hh"

//#include <boost/core/demangle.hpp>

#include <execinfo.h>
//#include <signal.h>
#include <string.h>

//#include <cstdlib>
#include <mutex>
#include <stdexcept>

namespace {
    // invoke set_terminate as part of global constant initialization
    static const bool SET_TERMINATE = std::set_terminate( scarab::terminate );
}

LOGGER( slog, "terminate" );

namespace scarab
{
    // Option 1: Use the terminate() function
    void terminate()
    {
        static std::mutex t_using_terminate;
        std::unique_lock< std::mutex > t_terminate_locked( t_using_terminate );

        static bool t_tried_throw = false;

        try
        {
            // try once to re-throw currently active exception
            if( ! t_tried_throw )
            {
                t_tried_throw = true;
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

        void* t_bt_array[50];
        int size = backtrace( t_bt_array, 50 );

        LERROR( slog, "Backtrace from terminate() returned "
                << size << " frames\n" );

        char** t_messages = backtrace_symbols( t_bt_array, size );

        std::stringstream t_bt_str;
        for( int i = 0; i < size && t_messages != nullptr; ++i )
        {
            t_bt_str << "[bt]: (" << i << ") " << t_messages[i] << '\n';
        }
        LERROR( slog, "Backtrace:\n" << t_bt_str.str() );

        free( t_messages );

        abort();
    }

    // Option 2: Handle SIGABRT
/*
    void crit_err_hdlr(int sig_num, siginfo_t * info, void * ucontext)
    {
        LERROR( slog, "signal " << sig_num
                << " (" << strsignal(sig_num) << "), address is "
                << info->si_addr );//<< " from "
                //<< caller_address );

        void* array[50];
        int size = backtrace(array, 50);

        LERROR(slog, "Backtrace from signal handler returned " << size << " frames\n" );

        char** messages = backtrace_symbols(array, size);

        // skip first stack frame (points here)
        std::stringstream bt_str;
        for( int i = 1; i < size && messages != NULL; ++i )
        {
            bt_str << "[bt]: (" << i << ") " << messages[i] << '\n';
        }
        LERROR( slog, "Backtrace:\n" << bt_str.str() );

        free(messages);

        exit(EXIT_FAILURE);
    }
*/

} /* namespace scarab */
