/*
 * cancelable.cc
 *
 *  Created on: Jan 24, 2016
 *      Author: nsoblath
 */

#define SCARAB_API_EXPORTS

#include "cancelable.hh"

#include "logger.hh"
#include "signal_handler.hh"

namespace scarab
{
    LOGGER( slog, "cancelable" );

    cancelable::cancelable() :
            f_canceled( false ),
            f_has_moved( false )
    {}

    cancelable::cancelable( const cancelable& a_orig ) :
            f_canceled( a_orig.f_canceled.load() ),
            f_has_moved( false )
    {}

    cancelable::cancelable( cancelable&& a_orig ) :
            f_canceled( a_orig.f_canceled.load() ),
            f_has_moved( false )
    {
        a_orig.f_canceled.store( false );
        a_orig.f_has_moved = true;
    }

    cancelable::~cancelable()
    {
        signal_handler::remove_cancelable( this );
    }

    cancelable& cancelable::operator=( const cancelable& a_orig )
    {
        f_canceled.store( a_orig.f_canceled.load() );
        f_has_moved = false;
        return *this;
    }

    cancelable& cancelable::operator=( cancelable&& a_orig )
    {
        // if a_orig already has been moved from, we don't duplicate that action
        if( a_orig.f_has_moved ) return *this;
        f_canceled.store( a_orig.f_canceled.load() );
        f_has_moved = false;
        a_orig.f_canceled.store( false );
        a_orig.f_has_moved = true;
        return *this;
    }

#ifdef NDEBUG
    void cancelable::do_cancellation( int )
#else
    void cancelable::do_cancellation( int a_code )
#endif
    {
        // override in derived class
        LDEBUG( slog, "cancelable::do_cancellation with code <" << a_code << ">" );
        return;
    }

    void cancelable::do_reset_cancellation()
    {
        // override in derived class
        return;
    }

    // cancelable_wrapper
    cancelable_wrapper::cancelable_wrapper( cancelable& a_cancelable ) :
            f_wrapped( &a_cancelable )
    {}

    cancelable_wrapper::~cancelable_wrapper() 
    {}

    // wrap_cancelable
    SCARAB_API std::shared_ptr< cancelable_wrapper > wrap_cancelable( cancelable& a_cancelable )
    {
        return std::make_shared< cancelable_wrapper >( a_cancelable );
    }


} /* namespace scarab */
