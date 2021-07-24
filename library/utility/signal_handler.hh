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
#include <map>

#ifndef _WIN32
#include <signal.h>  // for struct sigaction, which is in signal.h but not csignal
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#ifndef __USE_GNU
#define __USE_GNU
#endif


namespace scarab
{
    class cancelable;


    /*!
     @class signal_handler
     @author N. S. Oblath

     @brief Deals with cleanly exiting an application, and includes signal and std::terminate handler functions

     @details
     # Exiting and Terminating

     This class differentiates between two different ways of finishing a program:
     - Cleanly "exiting" a program, allowing threads to be canceled and memory to be cleaned up, and
     - Unclealy "terminating" a program, which exits more-or-less immediately.

     Termination occurs in situations where the program must finish immediately.  These are the situations in which
     std::terminate is used, for example, or there are unhandled exceptions.  This can occur during execution, or even
     during static initialization and cleanup.  Therefore only minimal global resources are assumed to be available
     (e.g. std::cerr).  See https://akrzemi1.wordpress.com/2011/10/05/using-stdterminate/ for more information about
     the way in which termination can be handled.  Termination can be commenced by:

     - Unhandled exceptions
     - Any other way in which std::terminate() is used (see, e.g. https://en.cppreference.com/w/cpp/error/terminate)
     - Client code calling std::terminate() directly
     - Client code calling signal_handler::terminate( code )

     Exiting occurs in situations where cleanly shutting down is possible.  This includes both successful exiting and
     exiting with an error condition.  signal_handler will be responsible for cancelling all threads that it knows about
     (via add_cancelable()).  After the exit process is complete, it's the responsiblity of the program to stop execution.
     The program can use get_exited() to check whether the exit process has been used.  Exiting can be commenced by:

     - Catching SIGABRT or SIGTERM (exits with an error code)
     - Catching SIGINT (usually ctrl-c) or SIGQUIT (usually ctrl-\) (exits with a success code)
     - Client code calling signal_handler::exit( code )

     To cleanly exit, signal_handler takes responsibility for canceling the cancelable objects that it knows about.
     Responsibility for canceling a cancelable is given to the signal_handler using add_cancelable(), and it's taken away
     using remove_cancelable().

     # Signal Handling

     To perform the tasks of exiting and terminating applications, `signal_handler` allows the user to control when signals
     are handled, and when they aren't.  In most cases, a user will control signal handling through the existence of
     a `signal_handler` instance.  The user should create the instance of `signal_handler` when they want `signal_handler`
     to start handling those signals, and it should be destructed when handling the signals should cease.
     Here are several examples of how this could be done:

     1. The instance is created as a global static object at static initialization time.  Signals will be handled for the entire
     time the application is running.
     2. The instance is created in the `main()` function of the application and exists until the application exits.
     3. The instance is created in some function for a specific limited time to cover a particular action.
     This could be for the entire scope of the function, or it could be destructed when signal handling is no longer wanted.

     Note that signals are handled as long as one or more instances of `signal_handler` exist.  If two instance exist, and one
     goes out of scope, signals are still handled until the second instance is destructed.

     In addition to this typical behavior, `signal_handler` includes an interface for more fine-grained control over when
     signals are handled while the `signal_handler` exists (i.e. handle_signals(), unhandle_signals(), and is_handling()),
     but the recommended use case is as described above, where signal handling is controled by the existence of a `signal_handler` instance.

     Note that the interface for `signal_handler` uses all static functions, so there is no need to create an extra instance of
     the class to use any part of that interface.  So the creation and destruction of instances can be used to control
     whether or not signals are handled, and the rest of the interface can be used without an instance.

     # Cancelables

     This class communicates the need to exit to other objects via the \ref cancelable interface.  Simply put, a \ref cancelable object
     can be canceled, or told to exit, asynchronously by the `signal_handler`.  Only the cancelable objects that the user wants
     to be directly canceled by `signal_handler` should be added with add_cancelable().  In some cases that might be all of the
     cancelable objects.  In other cases, where there's some hierarchy of cancelables, it would only be the top-level cancelables.

     While add_cancelable() takes a std::shared_ptr<cancelable> as an argument, it stores only std::weak_ptr<cancelable>, and
     claims no ownership or responsibility for the lifetime of the cancelable object.

     # Manual Cancelation/Exiting/Terminating

     In addition to acting automatically via handled signals, a user can manually terminate, exit, or cancel using
     terminate(), exit(), or cancel_all(), respectively.  cancel_all() will only affect the canceled objects.
     exit() perform a clean exit using cancel_all(), in addition to setting a return code, though it assumes the
     application will take care of actually exiting.  And terminate() will immediately exit the application using std::_Exit().
    */
    class SCARAB_API signal_handler
    {
        public:
            signal_handler();
            virtual ~signal_handler();

            /// Static version: add a cancelable object
            static void add_cancelable( std::shared_ptr< cancelable > a_cancelable );
            /// Static version: remove a cancelable object
            static void remove_cancelable( std::shared_ptr< cancelable > a_cancelable );
            /// Static version: remove a cancelable object with a plain pointer
            static void remove_cancelable( cancelable* a_cancelable );

            /// Remove all cancelables and signal handling
            static void reset();

            /// Start handling signals
            static void handle_signals();
            /// Stop handling signals
            static void unhandle_signals();
            /// Check if a signal is handled
            static bool is_handling( int a_signal );

            /// Handler for std::terminate -- does not cleanup memory or threads
            [[noreturn]] static void handle_terminate() noexcept;

            /// Handler for error signals -- cleanly exits
            static void handle_exit_error( int a_sig );

            /// Handler for success signals -- cleanly exits
            static void handle_exit_success( int a_sig );

            /// Main terminate function -- does not cleanup memory or threads
            [[noreturn]] static void terminate( int a_code ) noexcept;

            /// Main exit function -- cleanly exits
            static void exit( int a_code );

            /// Prints the current exception, if there is one
            static void print_current_exception( bool a_use_logging );

            /// Prints the stack trace
            static void print_stack_trace( bool a_use_logging );

            /// Asynchronous call to cancel all cancelables with the given exit code
            static void cancel_all( int a_code );

        private:

            typedef std::weak_ptr< cancelable > cancelable_wptr_t;
            // technical note: std::map<cancelable*, cancelable_wptr_t> is used instead of std::set<cancelable_wptr_t> because
            // weak_ptr cannot be used as a key because weak_ptrs are not immutable (see, e.g. https://stackoverflow.com/a/32668849 and one of the comments therein).
            // the raw pointer is only used as a key here.  the weak_ptr is used for accessing the cancelable.
            typedef std::map< cancelable*, cancelable_wptr_t > cancelers;
            typedef cancelers::const_iterator cancelers_cit_t;
            typedef cancelers::iterator cancelers_it_t;

            static cancelers s_cancelers;
            static std::recursive_mutex s_mutex;

        public:
            mv_accessible_static_noset( int, ref_count );

            mv_accessible_static_noset( bool, exited );
            mv_accessible_static( int, return_code );

            mv_accessible_static_noset( bool, handling_sig_abrt );
            mv_accessible_static_noset( bool, handling_sig_term );
            mv_accessible_static_noset( bool, handling_sig_int );
            mv_accessible_static_noset( bool, handling_sig_quit );

#ifndef _WIN32
            mv_referrable_static( struct sigaction, old_sig_abrt_action );
            mv_referrable_static( struct sigaction, old_sig_term_action );
            mv_referrable_static( struct sigaction, old_sig_int_action );
            mv_referrable_static( struct sigaction, old_sig_quit_action );
#else // _WIN32
            typedef void (*handler_t)(int);
            mv_referrable_static( handler_t, old_sig_abrt_handler );
            mv_referrable_static( handler_t, old_sig_term_handler );
            mv_referrable_static( handler_t, old_sig_int_handler );
            mv_referrable_static( handler_t, old_sig_quit_handler );
#endif
    };

} /* namespace scarab */
#endif /* SCARAB_SIGNAL_HANDLER_HH_ */
