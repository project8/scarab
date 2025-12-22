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
#include <signal.h>
#include <string>
#include <thread>

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

     Note that the interface for `signal_handler` uses all static functions, so there is no need to create an extra instance of
     the class to use any part of that interface.  So the creation and destruction of instances can be used to control
     whether or not signals are handled, and the rest of the interface can be used without an instance.

     # Signal Handling

     The process of signal handling has several steps, some of which are automated:

     1. Create the signal handler
     2. Start handling signals --- the default function that gets called when a signal is raised is replaced with signal_handler's function
     3. Start the thread waiting for signals
     4. Run the application, including the possibility of raising a signal
     5. Join the thread waiting for signals
     6. Stop handling signals -- the default signal-handling function is put back in place
     7. Signal handler is destructed

     In practice, the user has several options for performing these steps in application code.  
     The recommended method is the fully automatic method described here.  
     For other options on thread management, see below.

     1. Create the signal handler with the default argument for `start_waiting_thread` (true)
       * Creates the signal handler
       * Starts handling signals
       * Runs the signal-waiting thread
     2. Run the application
     3. Join the signal-waiting thread with `signal_handler::join_waiting_thread()`
     4. Let signal handler be destructed automatically

     ## Controling the scope of signal handling and which signals are handled

     Here are several examples of how the lifetime of the signal_handler can be managed:

     1. The instance is created as a global static object at static initialization time.  Signals will be handled for the entire
     time the application is running.  
     2. The instance is created in the `main()` function of the application and exists until the application exits.
     3. The instance is created in some function for a specific limited time to cover a particular action.
     This could be for the entire scope of the function, or it could be destructed when signal handling is no longer wanted.

     Note that signals are handled as long as one or more instances of `signal_handler` exist.  If two instance exist, and one
     goes out of scope, signals are still handled until the second instance is destructed.  Signals are only reacted to 
     while the watcher function/thread is active.  If there is a delay between when the watching function/thread exits and when 
     the signal_handler is destructed (or unhandle_signals() is called), during that period signals will not cause 
     the application to exit.

     By default the signals handled are SIGABRT (abort() and unhandled exceptions), SIGTERM (shell command kill), 
     SIGINT (ctrl-c), and SIGQUIT (ctrl-\).  
     For customized signal handling, the set of signals can be edited by modifying the handled signals map 
     (via signal_handler::handled_signals()).

     In addition to this typical behavior, `signal_handler` includes an interface for more fine-grained control over when
     signals are handled while the `signal_handler` exists (i.e. handle_signals(), unhandle_signals(), and is_handling()),
     but the recommended use case is as described above, where signal handling is controled by the existence of a `signal_handler` instance.

     ## Waiting for signals

     The process of waiting for signals requires a separate thread, and that thread can be managed in three ways:

     1. Automatically -- The thread is started when the first `signal_handler` is constructed. 
        The thread should still be joined with `signal_handler::join_waiting_thread()` 
        at the appropriate time (after all application functionality has started).
        This option is the default, but can be avoided by setting the `start_waiting_thread` argument to the 
        `signal_handler` constructor to false.  
     2. Manually with internal thread management -- Create the `signal_handler` with `start_waiting_thread` set to false, 
        and then use `signal_handler::start_waiting_thread()` to manually start the thread.  
        Use `signal_handler::join_waiting_thread()` to join the thread at the appropriate time.
     3. Manually with user thread management -- Create the `signal_handler` with `start_waiting_thread` set to false, 
        and then start your own thread to run `signal_handler::wait_for_signals()`, which is a blocking call to wait 
        for the arrival of signals.

    The waiting for signals is cancelled with `signal_handler::abort_wait()` regardless of hwo the waiting thread is managed.

     ## Examples

     For simple examples of creating a signal_handler and running a watcher thread, see the test_raise_sig*.cc files.

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

     # Design details for signal handling

     We need a way to turn a raised signal into a call to cancel_all() (via exit()).  However, a POSIX signal handler 
     is only allowed to call a limited class of functions that are async-signal-safe (see https://pubs.opengroup.org/onlinepubs/9799919799/functions/V2_chap02.html#tag_16_04).
     So we need an asynchronous method for getting the information about the raised signal to exit() instead.  
     We are allowed to set variables of type volatile sig_atomic_t (an integer), so we can use that as an indicator 
     that exit() needs to be called.  We can call exit() in a function that runs in a separate thread and watches the value of the 
     volatile sig_atomic_t indicator.  So our functionality has two parts:

       1. "handling" signals: we replace the default handler functions for the four signals of interest 
       (SIGABRT, SIGTERM, SIGINT, and SIGQUIT).  It prints a simple message to the error stream (write() is allowed) and 
       sets the value of s_signal_received, a volatile sig_atomic_t static variable.
       2. "watching" for signals: The function watch_for_signals() starts a loop that waits for s_signal_received to be nonzero.  
       It then breaks out of the loop and calls exit() with the appropriate return value (depending on the specific signal raised).  
       The watch loop can be broken out of manually by calling abort_wait(), which sets s_signal_received to a negative number.  
       All signals are positive integers in the POSIX standard, so the negative number differentiates an abort (exit() is not called) 
       from a handled signal (exit() is called).
    */
    class SCARAB_API signal_handler
    {
        public:
            signal_handler( bool start_waiting_thread = true );
            signal_handler( const signal_handler& ) = delete;
            signal_handler( signal_handler&& ) = delete;
            virtual ~signal_handler();

            signal_handler& operator=( const signal_handler& ) = delete;
            signal_handler& operator=( signal_handler&& ) = delete;

            /// Add a cancelable object
            static void add_cancelable( std::shared_ptr< cancelable > a_cancelable );
            /// Remove a cancelable object
            static void remove_cancelable( std::shared_ptr< cancelable > a_cancelable );
            /// Remove a cancelable object with a plain pointer
            static void remove_cancelable( cancelable* a_cancelable );
            /// Print the pointers to the cancelables
            static void print_cancelables();

            /// Remove all cancelables and signal handling
            static void reset();

            /// Fine-grained signal-handling control; called from the constructor
            /// Uses handle_signal() as the signal-handling function for the relevant signals.
            static void start_handling_signals();
            /// Fine-grained signal-handling control; called from the destructor
            /// Returns the previous signal-handling functions to the signals.
            static void unhandle_signals();
            static bool is_handling(); // just calls get_is_handling(); is here to have parallel syntax to waiting

            /// Blocking call to handle signals; waits for a signal
            /// Returns immediately if it's already been called; otherwise blocks then returns the signal or error value
            /// Has an option to not call the exit() function; defaults to true (i.e. exit() will be called)
            static int wait_for_signals( bool call_exit = true );
            //static int wait_on_signals();
            /// Requests the aborting of waiting for a signal; causes do_handle_signal() to return
            static void abort_wait();
            /// Check if a signal is handled
            static bool is_waiting();

            /// Starts the thread waiting on signals (internally using wait_for_signals() and an internal thread storage)
            static void start_waiting_thread();
            /// Joins the internal wait-for-signals thread
            static void join_waiting_thread();
            /// Checks if the internal thread is in use
            static bool waiting_thread_joinable();

            /// Handler for std::terminate -- does not cleanup memory or threads
            [[noreturn]] static void handle_terminate() noexcept;

            /// Handler for the relevant signals (SIGABRT, SIGTERM, SIGINT, and SIGQUIT by default)
            static void handle_signal( int a_sig );

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
            static std::mutex s_cancelers_mutex;

        public:
            struct handled_signal_info
            {
                std::string name;
                bool handling;
                struct sigaction old_action;
                bool indicates_error;
                int return_code;
            };
            typedef std::map<int, handled_signal_info> signal_map_t;

            static sig_atomic_t get_signal_received() { return s_signal_received; }
            static signal_map_t handled_signals() { return s_handled_signals; }

            mv_accessible_static_noset( int, ref_count );

            mv_accessible_static_noset( bool, exited );
            mv_accessible_static( int, return_code );

            mv_accessible_static_noset( bool, is_handling );

            mv_referrable_static( std::thread, waiting_thread );

        protected:
            static std::mutex s_handle_mutex;

            static volatile sig_atomic_t s_signal_received;

            static signal_map_t s_handled_signals;

    };

} /* namespace scarab */
#endif /* SCARAB_SIGNAL_HANDLER_HH_ */
