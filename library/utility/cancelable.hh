/*
 * cancelable.hh
 *
 *  Created on: Jan 24, 2016
 *      Author: nsoblath
 *
 *  Base class for thread-safe asynchronous cancellation
 */

#ifndef SCARAB_CANCELABLE_HH_
#define SCARAB_CANCELABLE_HH_

#include "macros.hh"
#include "scarab_api.hh"

#include <atomic>
#include <memory>

namespace scarab
{
    /*!
     @class cancelable
     @author N. S. Oblath

     @brief Base class for a cancelable object (i.e. an object that can be canceled by scarab::signal_handler or other client code)

     @details
     A cancelable object is one which has some execution operation that should be asynchronously cancelable.  
     Cancellation is performed using cancelable::cancel().

     For example, if the object of a derived class is executing in thread 1, 
     client code in thread 2 or a signal (e.g. SIGINT) from the user or a system 
     should be able to cancel execution.

     If a derived class has a function execute() that performs the main execution loop, that loop should include 
     a periodic check of cancelable::is_canceled() to know whether to exit the loop.

     Any actions that should take place when cancelable::cancel() is used should be implemented by the derived class in do_cancellation().

     Cancelable objects can exist within a parent/child hierarchy to ensure that execution can be cleanly shut down.  
     Parent classes should implement do_cancellation(int) to cancel their children.

     For example, if a parent object in thread 1 starts the execution of two children, each in their own thread (i.e. threads 2 and 3),
     then the parent object should be added to the signal_handler (using signal_handler::add_cancelable()).  The parent's implementation of 
     do_cancellation() should take care of calling cancelable::cancel() on each of the children.  The children should not be separately added to 
     the signal_handler.

     Genreally it's assumed that a top-level-parent cancelable object will be created on the heap using std::shared_ptr
     (hence why signal_handler adds cancelables using std::shared_ptr<cancelable>).
    */
    class SCARAB_API cancelable
    {
        public:
            cancelable();
            cancelable( const cancelable& a_orig );
            cancelable( cancelable&& a_orig );
            virtual ~cancelable();

            cancelable& operator=( const cancelable& a_orig );
            cancelable& operator=( cancelable&& a_orig );

            /// asynchronous cancel function
            void cancel( int a_code = RETURN_SUCCESS );

            /// reset to non-canceled state
            void reset_cancel();

            /// check canceled state
            bool is_canceled() const;

        private:
            virtual void do_cancellation( int a_code );
            virtual void do_reset_cancellation();

        protected:
            std::atomic< bool > f_canceled;
    };

    /*!
     @struct cancelable_wrapper
     @author N. S. Oblath

     @brief Lightweight wrapper for a cancelable object only temporarily added to signal_handler.

     @details
     Sometimes a cancelable object is created on the stack in a local scope, or it's temporarily added to signal_handler 
     while a specific action is performed.  In this case you can use this lightweight wrapper to add the cancelable object to the signal handler.

     NOTE: the wrapper does not track the lifetime of the wrapped object.  Only use this wrapper if you can guarantee that the lifetime of the 
     wrapped object is not shorter than that of the wrapper.

     Example:
       {
           my_cancelable t_obj;  // here's the local object we want to have temporarily added to signal_handler
           auto t_obj_wrap = wrap_cancelable( t_obj );  // use wrap_cancelable() (declared below) to create the wrapper
           scarab::signal_handler t_handler;  // creating the signal_handler sets the signal-handling functions, if it hasn't been done before
           t_handler.add_cancelable( t_obj_wrap );  // add the wrapper
           t_obj.execute();  // do the cancelable action
           // end of scope: t_obj_wrap is destroyed, which removes it from signal_handler automatically
       }
    */

    struct SCARAB_API cancelable_wrapper : cancelable
    {
        cancelable_wrapper( cancelable& a_cancelable );
        virtual ~cancelable_wrapper();
        virtual void do_cancellation( int a_code );
        cancelable* f_wrapped;
    };

    /// Convenience function to create a cancelable_wrapper
    SCARAB_API std::shared_ptr< cancelable_wrapper > wrap_cancelable( cancelable& a_cancelable );


    inline void cancelable::cancel( int a_code )
    {
        if( f_canceled.load() ) return;
        f_canceled.store( true );
        this->do_cancellation( a_code );
    }

    inline void cancelable::reset_cancel()
    {
        if( ! f_canceled.load() ) return;
        f_canceled.store( false );
        this->do_reset_cancellation();
    }

    inline bool cancelable::is_canceled() const
    {
        return f_canceled.load();
    }


    inline void cancelable_wrapper::do_cancellation( int a_code )
    {
        f_wrapped->cancel( a_code );
    }

} /* namespace scarab */

#endif /* SCARAB_CANCELABLE_HH_ */
