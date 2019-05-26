/*
 * signal_handler.hh
 *
 *  Created on: Dec 3, 2013
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_SIGNAL_HANDLER_HH_
#define SCARAB_SIGNAL_HANDLER_HH_

#include "scarab_api.hh"

#include <memory>
#include <mutex>
#include <set>

namespace scarab
{
    class cancelable;

    class SCARAB_API signal_handler
    {
        public:
            signal_handler();
            virtual ~signal_handler();

            /// Add a cancelable object
            void add_cancelable( cancelable* a_cancelable );
            /// Remove a cancelable object
            void remove_cancelable( cancelable* a_cancelable );

            /// Remove all cancelables
            void reset();

            /// Check whether an exit signal has been received
            static bool got_exit_signal();

            /// Signal handler function
            static void handler_cancel_threads( int _ignored );

            /// Asynchronous call to exit the process with the given exit code
            static void cancel_all( int a_code );

        private:
            static void print_message();

            typedef std::set< cancelable* > cancelers;
            typedef cancelers::const_iterator cancelers_cit_t;
            typedef cancelers::iterator cancelers_it_t;

            static cancelers f_cancelers;
            static std::mutex f_mutex;

            static bool f_got_exit_signal;

            static bool f_handling_sig_quit;
            static bool f_handling_sig_int;

    };

} /* namespace scarab */
#endif /* SCARAB_SIGNAL_HANDLER_HH_ */
