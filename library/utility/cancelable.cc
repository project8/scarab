/*
 * cancelable.cc
 *
 *  Created on: Jan 24, 2016
 *      Author: nsoblath
 */

#define SCARAB_API_EXPORTS

#include "cancelable.hh"

#include "logger.hh"

namespace scarab
{
    LOGGER( slog, "cancelable" );

    cancelable::cancelable() :
            f_canceled( false )
    {
    }

    cancelable::~cancelable()
    {
    }

    void cancelable::do_cancellation( int a_code )
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

} /* namespace scarab */
