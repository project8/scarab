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

namespace scarab
{
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

} /* namespace scarab */

#endif /* SCARAB_CANCELABLE_HH_ */
