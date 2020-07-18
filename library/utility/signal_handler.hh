/*
 * signal_handler.hh
 *
 *  Created on: Dec 3, 2013
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_SIGNAL_HANDLER_HH_
#define SCARAB_SIGNAL_HANDLER_HH_

#include "scarab_api.hh"
#include "member_variables.hh"

#include <memory>
#include <mutex>
#include <set>


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif


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

            /// Remove all cancelables and signal handling
            void reset();

            static void handle_termination();

            /// Handler for error signals
            static void handle_terminate_error( int a_sig );

            /// Handler for success signals
            static void handle_terminate_success( int a_sig );

            /// Main termination function
            static void terminate( int a_code );

            /// Prints the current exception, if there is one
            static void print_current_exception();

            /// Prints the stack trace
            static void print_stack_trace();

            /// Asynchronous call to cancel all cancelables with the given exit code
            static void cancel_all( int a_code );

        private:

            typedef std::set< cancelable* > cancelers;
            typedef cancelers::const_iterator cancelers_cit_t;
            typedef cancelers::iterator cancelers_it_t;

            static cancelers s_cancelers;
            static std::recursive_mutex s_mutex;

        public:
            mv_accessible_static_noset( bool, terminated );
            mv_accessible_static( int, return_code );

            mv_accessible_static_noset( bool, handling_sig_abrt );
            mv_accessible_static_noset( bool, handling_sig_term );
            mv_accessible_static_noset( bool, handling_sig_int );
            mv_accessible_static_noset( bool, handling_sig_quit );

    };

} /* namespace scarab */
#endif /* SCARAB_SIGNAL_HANDLER_HH_ */
