/*
 * thread_local_singleton.hh
 *
 *  Created on: Dec 31, 2019
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_THREAD_SINGLETON_HH_
#define SCARAB_THREAD_SINGLETON_HH_

#include "destroyer.hh"
#include "error.hh"

#include <cstddef>
#include <mutex>

namespace scarab
{
    /*!
     \def allow_thread_singleton_access( class_name )
     Gives friend access to your class from scarab::singleton< class_name > and scarab::destroyer< class_name > 
     so that those classes can control the life of your singleton class.
    */
#define allow_thread_singleton_access( class_name ) \
    friend class scarab::thread_singleton< class_name >; \
    friend class scarab::destroyer< class_name >;

    /*!
      @class thread_singleton
      @author N.S. Oblath
      @brief Base class that turns a class into a thread-local singleton

      @details
      A thread-local singleton is similar to a singleton except that a different instance exists in each thread.

      To use:
        1. Inherit your class from thread_singleton< your_class >
        2. Make your constructor and destructor protected (or private)
        3. Add the `allow_thread_singleton_access( your_class )` macro to your class definition to allow the base classes to access your class

      The mutex f_mutex provides thread safety for creation and destruction of an instance of the thread_singleton.  It's available to the 
      derived class for thread-safe access to the object in general.
    */
    template< class x_type >
    class thread_singleton
    {
        public:
            static x_type* get_instance();
            static void kill_instance();

            template< class... x_args >
            static x_type* create_instance( x_args... args );

        private:
            static void construct_instance();
            static void delete_instance();

        private:
            thread_local static x_type* f_instance;
            thread_local static destroyer< x_type > f_destroyer;

        protected:
            thread_local static std::mutex f_mutex;

        protected:
            thread_singleton();

            friend class destroyer< x_type >;
            ~thread_singleton();
    };

    template< class x_type >
    thread_local x_type* thread_singleton< x_type >::f_instance = nullptr;

    template< class x_type >
    thread_local destroyer< x_type > thread_singleton< x_type >::f_destroyer;

    template< class x_type >
    thread_local std::mutex thread_singleton< x_type >::f_mutex;

    template< class x_type >
    x_type* thread_singleton< x_type >::get_instance()
    {
        if( f_instance == nullptr )
        {
            std::unique_lock< std::mutex > t_lock( f_mutex );
            construct_instance();
        }
        return f_instance;
    }

    template< class x_type >
    void thread_singleton< x_type >::kill_instance()
    {
        if( f_instance != nullptr )
        {
            std::unique_lock< std::mutex > t_lock( f_mutex );
            delete_instance();
        }
        return;
    }

    template< class x_type >
    template< class... x_args >
    x_type* thread_singleton< x_type >::create_instance( x_args... args )
    {
        if( f_instance != nullptr )
        {
            throw error() << "Instance already exists; create_instance can only be called before the instance exists";
        }
        std::unique_lock< std::mutex > t_lock( f_mutex );
        f_instance = new x_type( args... );
        f_destroyer.set_doomed( f_instance );
        return f_instance;
    }

    template< class x_type >
    void thread_singleton< x_type >::construct_instance()
    {
        if( f_instance == nullptr )
        {
            f_instance = new x_type();
            f_destroyer.set_doomed( f_instance );
        }
    }

    template< class x_type >
    void thread_singleton< x_type >::delete_instance()
    {
        if( f_instance != nullptr )
        {
            delete f_instance;
            f_instance = nullptr;
            f_destroyer.set_doomed( nullptr );
        }
    }

    template< class x_type >
    thread_singleton< x_type >::thread_singleton()
    {
    }
    template< class x_type >
    thread_singleton< x_type >::~thread_singleton()
    {
    }

} /* namespace scarab */

#endif /* SCARAB_THREAD_SINGLETON_HH_ */
