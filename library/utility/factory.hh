/*
 * mt_factory.hh
 *
 *  created on: Jul 31, 2012
 *      Author: nsoblath
 */

#ifndef SCARAB_FACTORY_HH_
#define SCARAB_FACTORY_HH_

#include "singleton.hh"

#include "lock_guard.hh"
#include "logger.hh"
#include "mutex.hh"

#include <map>
#include <string>
#include <utility>

namespace scarab
{
    LOGGER( slog_fact, "factory" );

    template< class XBaseType, typename ... XArgs >
    class factory;

    template< class XBaseType, typename ... XArgs >
    class base_registrar
    {
        public:
            base_registrar() {}
            virtual ~base_registrar() {}

        public:
            friend class factory< XBaseType, XArgs... >;

        protected:
            virtual XBaseType* create( XArgs ... args ) const = 0;

    };

    template< class XBaseType, class XDerivedType, typename ... XArgs >
    class registrar : public base_registrar< XBaseType, XArgs... >
    {
        public:
            registrar( const std::string& a_class_name );
            virtual ~registrar();

        protected:
            void register_class( const std::string& a_class_name ) const;

            XBaseType* create( XArgs ... args ) const;

    };


    template< class XBaseType, typename ... XArgs >
    class factory : public singleton< factory< XBaseType, XArgs... > >
    {
        public:
            typedef std::map< std::string, const base_registrar< XBaseType, XArgs... >* > FactoryMap;
            typedef typename FactoryMap::value_type FactoryEntry;
            typedef typename FactoryMap::iterator FactoryIt;
            typedef typename FactoryMap::const_iterator FactoryCIt;

        public:
            XBaseType* create(const std::string& a_class_name, XArgs ... args);
            XBaseType* create(const FactoryCIt& iter, XArgs ... args);

            void register_class(const std::string& a_class_name, const base_registrar< XBaseType, XArgs... >* base_registrar);

            FactoryCIt begin() const;
            FactoryCIt end() const;

        protected:
            FactoryMap* fMap;
            mutex f_factory_mutex;

        protected:
            friend class singleton< factory >;
            friend class destroyer< factory >;
            factory();
            ~factory();
    };

    template< class XBaseType, typename ... XArgs >
    XBaseType* factory< XBaseType, XArgs... >::create(const FactoryCIt& iter, XArgs ... args)
    {
        lock_guard( this->f_factory_mutex );
        return iter->second->create( args... );
    }

    template< class XBaseType, typename ... XArgs >
    XBaseType* factory< XBaseType, XArgs... >::create(const std::string& a_class_name, XArgs ... args)
    {
        //std::cout << "this factory (" << this << ") has " << fMap->size() << " entries" << std::endl;
        //for( FactoryCIt iter = fMap->begin(); iter != fMap->end(); ++iter )
        //{
        //    std::cout << "this factory has: " << iter->first << std::endl;
        //}
        lock_guard( this->f_factory_mutex );
        FactoryCIt it = fMap->find( a_class_name );
        if( it == fMap->end() )
        {
            LERROR( slog_fact, "Did not find factory for <" << a_class_name << ">." );
            return NULL;
        }

        return it->second->create();
    }

    template< class XBaseType, typename ... XArgs >
    void factory< XBaseType, XArgs... >::register_class(const std::string& a_class_name, const base_registrar< XBaseType, XArgs... >* a_registrar)
    {
        lock_guard( this->f_factory_mutex );
        FactoryCIt it = fMap->find(a_class_name);
        if (it != fMap->end())
        {
            LERROR( slog_fact, "Already have factory registered for <" << a_class_name << ">." );
            return;
        }
        fMap->insert(std::pair< std::string, const base_registrar< XBaseType, XArgs... >* >(a_class_name, a_registrar));
        //std::cout << "registered a factory for class " << a_class_name << ", factory #" << fMap->size()-1 << " for " << this << std::endl;
    }

    template< class XBaseType, typename ... XArgs >
    factory< XBaseType, XArgs... >::factory() :
        fMap(new FactoryMap()),
        f_factory_mutex()
    {}

    template< class XBaseType, typename ... XArgs >
    factory< XBaseType, XArgs... >::~factory()
    {
        delete fMap;
    }

    template< class XBaseType, typename ... XArgs >
    typename factory< XBaseType, XArgs... >::FactoryCIt factory< XBaseType, XArgs... >::begin() const
    {
        lock_guard( this->f_factory_mutex );
        return fMap->begin();
    }

    template< class XBaseType, typename ... XArgs >
    typename factory< XBaseType, XArgs... >::FactoryCIt factory< XBaseType, XArgs... >::end() const
    {
        lock_guard( this->f_factory_mutex );
        return fMap->end();
    }






    template< class XBaseType, class XDerivedType, typename ... XArgs >
    registrar< XBaseType, XDerivedType, XArgs... >::registrar( const std::string& a_class_name ) :
            base_registrar< XBaseType, XArgs... >()
    {
        register_class( a_class_name );
    }

    template< class XBaseType, class XDerivedType, typename ... XArgs >
    registrar< XBaseType, XDerivedType, XArgs... >::~registrar()
    {}

    template< class XBaseType, class XDerivedType, typename ... XArgs >
    void registrar< XBaseType, XDerivedType, XArgs... >::register_class( const std::string& a_class_name ) const
    {
        factory< XBaseType, XArgs... >::get_instance()->register_class( a_class_name, this );
        return;
    }

    template< class XBaseType, class XDerivedType, typename ... XArgs >
    XBaseType* registrar< XBaseType, XDerivedType, XArgs... >::create( XArgs... args ) const
    {
        return dynamic_cast< XBaseType* >( new XDerivedType( args... ) );
    }

} /* namespace scarab */
#endif /* SCARAB_FACTORY_HH_ */
