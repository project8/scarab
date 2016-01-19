#ifndef SCARAB_SINGLETON_HH_
#define SCARAB_SINGLETON_HH_

#include "destroyer.hh"

#include <cstddef>

namespace scarab
{

    template< class XType >
    class singleton
    {
        public:
            static XType* get_instance();

        private:
            static XType* f_instance;
            static destroyer< XType > f_destroyer;

        protected:
            singleton();

            friend class destroyer< XType >;
            ~singleton();
    };

    template< class XType >
    XType* singleton< XType >::f_instance = NULL;

    template< class XType >
    destroyer< XType > singleton< XType >::f_destroyer;

    template< class XType >
    XType* singleton< XType >::get_instance()
    {
        if( f_instance == NULL )
        {
            f_instance = new XType();
            f_destroyer.set_doomed( f_instance );
        }
        return f_instance;
    }

    template< class XType >
    singleton< XType >::singleton()
    {
    }
    template< class XType >
    singleton< XType >::~singleton()
    {
    }

} /* namespace scarab */

#endif /* SCARAB_SINGLETON_HH_ */
