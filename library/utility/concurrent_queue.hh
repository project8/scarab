/*
 * concurrent_queue.hh
 *
 *  Created on: July 8, 2015
 *      Author: nsoblath
 *
 *      FIFO Queue
 *
 *      Based almost exactly on the class concurrent_queue from:
 *      http://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
 *      Post author: Anthony Williams
 *      Copyright  2005-2013 Just Software Solutions Ltd. All rights reserved.
 */

#ifndef SCARAB_CONCURRENT_QUEUE_HH_
#define SCARAB_CONCURRENT_QUEUE_HH_

#include "logger.hh"

#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>

namespace scarab
{
    LOGGER( slog_cq, "concurrent_queue" );

    template< class XDataType >
    class concurrent_queue
    {
        public:
            typedef std::deque< XDataType > queue;

            struct queue_not_empty
            {
                queue& f_queue;
                queue_not_empty( queue& a_queue ) :
                    f_queue( a_queue )
                {}
                bool operator()() const
                {
                    return ! f_queue.empty();
                }
            };

            typedef std::unique_lock< std::mutex > cq_lock;

        public:
            concurrent_queue() :
                f_queue(),
                f_interrupt( false ),
                f_timeout( std::chrono::milliseconds( 1000 ) ),
                f_mutex(),
                f_condition_var()
            {
            }

            virtual ~concurrent_queue()
            {
                f_queue.clear();
            }

        private:
            queue f_queue;
            bool f_interrupt;

            std::chrono::milliseconds f_timeout; /// Timeout duration in milliseconds

            mutable std::mutex f_mutex;
            std::condition_variable f_condition_var;

        public:
            void push( XDataType const& a_data )
            {
                LDEBUG( slog_cq, "Attempting to push to queue" );
                cq_lock lock( f_mutex );
                LDEBUG( slog_cq, "Pushing to concurrent queue; size: " << f_queue.size() );
                f_queue.push_back( a_data );
                lock.unlock();
                f_condition_var.notify_one();
                return;
            }

            bool empty() const
            {
                cq_lock lock( f_mutex );
                return f_queue.empty();
            }

            unsigned size() const
            {
                cq_lock lock( f_mutex );
                return f_queue.size();
            }

            bool try_pop( XDataType& a_popped_value )
            {
                cq_lock lock( f_mutex );
                f_interrupt = false;
                if( f_queue.empty() )
                {
                    return false;
                }

                a_popped_value = f_queue.front();
                f_queue.pop_front();
                return true;
            }

            bool wait_and_pop( XDataType& a_popped_value )
            {
                cq_lock lock( f_mutex );
                f_interrupt = false;
                f_condition_var.wait( lock, queue_not_empty( f_queue ) );
                if( f_interrupt )
                {
                    f_interrupt = false;
                    return false;
                }

                a_popped_value = f_queue.front();
                f_queue.pop_front();
                LDEBUG( slog_cq, "Popping from concurrent queue; size: " << f_queue.size() );
                return true;
            }

            bool timed_wait_and_pop( XDataType& a_popped_value )
            {
                cq_lock lock( f_mutex );
                f_interrupt = false;
                std::chrono::steady_clock::time_point const waitUntil = std::chrono::steady_clock::now() + f_timeout;
                if( ! f_condition_var.wait_until( lock, waitUntil, queue_not_empty( f_queue ) ) )
                {
                    //LDEBUG( mtlog_cq, "Queue wait has timed out" );
                    return false;
                }
                if( f_interrupt )
                {
                    f_interrupt = false;
                    return false;
                }

                a_popped_value = f_queue.front();
                f_queue.pop_front();
                LDEBUG( slog_cq, "Popping from concurrent queue; size: " << f_queue.size() );
                return true;
            }

            void interrupt()
            {
                f_interrupt = true;
                f_condition_var.notify_one();
                return;
            }

            inline unsigned get_timeout() const
            {
                return f_timeout.count();
            }

            inline void set_timeout( unsigned a_duration )
            {
                f_timeout = std::chrono::milliseconds( a_duration );
                return;
            }
    };

} /* namespace scarab */

#endif /* SCARAB_CONCURRENT_QUEUE_HH_ */
