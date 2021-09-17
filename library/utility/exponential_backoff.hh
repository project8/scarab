/*
 * exponential_backoff.hh
 *
 *  Created on: Jun 17, 2021
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_EXPONENTIAL_BACKOFF_HH_
#define SCARAB_EXPONENTIAL_BACKOFF_HH_

#include "error.hh"
#include "logger.hh"
#include "member_variables.hh"
#include "scarab_api.hh"

#include <chrono>
#include <functional>
#include <thread>


LOGGER( expbacklog_hh, "exponential_backoff.hh" )

namespace scarab
{
    /*!
     @class exponential_backoff
     @author N.S. Oblath
     @brief Class that performs an action using exponential backoff

     @details
     
    */
    template< typename ... x_args >
    class SCARAB_API exponential_backoff
    {
        public:
            exponential_backoff( std::function< bool (x_args...) > an_action, unsigned a_max_attempts = 10, unsigned a_base_delay_ms = 100 );
            //exponential_backoff( const exponential_backoff< x_args... >& a_orig );
            virtual ~exponential_backoff();

            //exponential_backoff< x_args... >& operator=( const exponential_backoff< x_args... >& a_orig );

            /// returns the number of attempts made at calling the function (including the last, successful or not)
            unsigned go( x_args... args );

            mv_referrable( std::function< bool (x_args...) >, action );

            mv_accessible( unsigned, max_attempts );
            mv_accessible( unsigned, base_delay_ms );
    };


    template< typename ... x_args >
    exponential_backoff< x_args... >::exponential_backoff( std::function< bool (x_args...) > an_action, unsigned a_max_attempts, unsigned a_base_delay_ms ) :
            f_action( an_action ),
            f_max_attempts( a_max_attempts ),
            f_base_delay_ms( a_base_delay_ms )
    {
        if( f_max_attempts == 0 ) throw error() << "Max attempts must be > 0";
    }

    template< typename ... x_args >
    exponential_backoff< x_args... >::~exponential_backoff()
    {}

    template< typename ... x_args >
    unsigned exponential_backoff< x_args... >::go( x_args... args )
    {
        //LDEBUG( expbacklog_hh, "doing action" );
        unsigned t_attempts = 1;
        bool t_success = f_action( args... );
        LDEBUG( expbacklog_hh, "function called first time: " << t_success );
        while( ! t_success && t_attempts < f_max_attempts )
        {
            LDEBUG( expbacklog_hh, "attempt: " << t_attempts + 1 << " after delay: " << (1<<t_attempts) * f_base_delay_ms );
            std::this_thread::sleep_for( std::chrono::milliseconds( (1<<t_attempts) * f_base_delay_ms ) );
            t_success = f_action( args... );
            LDEBUG( expbacklog_hh, "function called: " << t_success );
            ++t_attempts;
        }

        return t_success ? t_attempts : 0;
    }

} /* namespace scarab */

#endif /* SCARAB_EXPONENTIAL_BACKOFF_HH_ */
