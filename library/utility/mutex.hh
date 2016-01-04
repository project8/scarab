#ifndef SCARAB_MUTEX_HH_
#define SCARAB_MUTEX_HH_

#ifndef _WIN32
#include <pthread.h>
#include <cstddef>
#endif

#include "scarab_api.hh"

namespace scarab
{

    class SCARAB_API mutex
    {
        public:
            mutex();
            virtual ~mutex();

            bool trylock();

            void lock();
            void unlock();

        private:
#ifndef _WIN32
            pthread_mutex_t f_mutex;
#else
            CRITICAL_SECTION f_critical_section;
#endif
    };

}

#endif /* SCARAB_MUTEX_HH_ */
