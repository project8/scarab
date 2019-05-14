/*
 * mt_factory.hh
 *
 *  created on: Jul 31, 2012
 *      Author: nsoblath
 */

#ifndef SCARAB_FACTORY_HH_
#define SCARAB_FACTORY_HH_

#include "singleton.hh"

#include "logger.hh"

#include <map>
#include <string>
#include <utility>

namespace scarab
{
    LOGGER( slog_fact, "factory" );

    //********************
    // Class definitions
    //********************

    template< class XBaseType, typename ... XArgs >
    class factory;

    // base_registrar

    template< class XBaseType, typename ... XArgs >
    class SCARAB_API base_registrar
    {
        public:
            base_registrar() {}
            virtual ~base_registrar() {}

        public:
            friend class factory< XBaseType, XArgs... >;

        protected:
            virtual XBaseType* create( XArgs ... args ) const = 0;

    };

    // registrar

    template< class XBaseType, class XDerivedType, typename ... XArgs >
    class SCARAB_API registrar : public base_registrar< XBaseType, XArgs... >
    {
        public:
            registrar( const std::string& a_class_name );
            virtual ~registrar();

        protected:
            void register_class( const std::string& a_class_name ) const;

            XBaseType* create( XArgs ... args ) const;

            std::string f_class_name;
    };

    // factory

    template< class XBaseType, typename ... XArgs >
    class SCARAB_API factory : public singleton< factory< XBaseType, XArgs... > >
    {
        public:
            typedef std::map< std::string, const base_registrar< XBaseType, XArgs... >* > FactoryMap;
            typedef typename FactoryMap::value_type FactoryEntry;
            typedef typename FactoryMap::iterator FactoryIt;
            typedef typename FactoryMap::const_iterator FactoryCIt;

        public:
            XBaseType* create( const std::string& a_class_name, XArgs ... args );
            XBaseType* create( const FactoryCIt& iter, XArgs ... args );

            void register_class( const std::string& a_class_name, const base_registrar< XBaseType, XArgs... >* base_registrar );
            bool has_class( const std::string& a_class_name ) const;
            void remove_class( const std::string& a_class_name );

            FactoryCIt begin() const;
            FactoryCIt end() const;

        protected:
            FactoryMap* fMap;
            std::mutex f_factory_mutex;

        protected:
            allow_singleton_access( factory );
            factory();
            ~factory();
    };


    //*****************************************
    // Partial specialization for empty XArgs
    //*****************************************

    template< class XBaseType >
    class SCARAB_API base_registrar< XBaseType, void >
    {
        public:
            base_registrar() {}
            virtual ~base_registrar() {}

        public:
            friend class factory< XBaseType >;

        protected:
            virtual XBaseType* create() const = 0;

    };

    template< class XBaseType, class XDerivedType >
    class SCARAB_API registrar< XBaseType, XDerivedType, void > : public base_registrar< XBaseType >
    {
        public:
            registrar( const std::string& a_class_name );
            virtual ~registrar();

        protected:
            void register_class( const std::string& a_class_name ) const;

            XBaseType* create() const;

            std::string f_class_name;
    };


    template< class XBaseType >
    class SCARAB_API factory< XBaseType, void > : public singleton< factory< XBaseType > >
    {
        public:
            typedef std::map< std::string, const base_registrar< XBaseType >* > FactoryMap;
            typedef typename FactoryMap::value_type FactoryEntry;
            typedef typename FactoryMap::iterator FactoryIt;
            typedef typename FactoryMap::const_iterator FactoryCIt;

        public:
            XBaseType* create( const std::string& a_class_name );
            XBaseType* create( const FactoryCIt& iter );

            void register_class( const std::string& a_class_name, const base_registrar< XBaseType >* base_registrar );
            bool has_class( const std::string& a_class_name ) const;
            void remove_class( const std::string& a_class_name );

            FactoryCIt begin() const;
            FactoryCIt end() const;

        protected:
            FactoryMap* fMap;
            std::mutex f_factory_mutex;

        protected:
            friend class singleton< factory >;
            friend class destroyer< factory >;
            factory();
            ~factory();
    };


    //******************
    // Implementations
    //******************

    // factory

    template< class XBaseType, typename ... XArgs >
    XBaseType* factory< XBaseType, XArgs... >::create( const FactoryCIt& iter, XArgs ... args )
    {
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        return iter->second->create( args... );
    }

    template< class XBaseType, typename ... XArgs >
    XBaseType* factory< XBaseType, XArgs... >::create( const std::string& a_class_name, XArgs ... args )
    {
        //std::cout << "this factory (" << this << ") has " << fMap->size() << " entries" << std::endl;
        //for( FactoryCIt iter = fMap->begin(); iter != fMap->end(); ++iter )
        //{
        //    std::cout << "this factory has: " << iter->first << " at " << iter->second << std::endl;
        //}
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        FactoryCIt it = fMap->find( a_class_name );
        if( it == fMap->end() )
        {
            LERROR( slog_fact, "Did not find factory for <" << a_class_name << ">." );
            return NULL;
        }

        return it->second->create( args... );
    }

    template< class XBaseType, typename ... XArgs >
    void factory< XBaseType, XArgs... >::register_class( const std::string& a_class_name, const base_registrar< XBaseType, XArgs... >* a_registrar )
    {
        // A local (static) logger is created inside this function to avoid static initialization order problems
        LOGGER( slog_factory_reg, "factory-register");

        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        FactoryCIt it = fMap->find(a_class_name);
        if (it != fMap->end())
        {
            LERROR( slog_factory_reg, "Already have factory registered for <" << a_class_name << ">." );
            return;
        }
        fMap->insert(std::pair< std::string, const base_registrar< XBaseType, XArgs... >* >(a_class_name, a_registrar));
        LDEBUG( slog_factory_reg, "Registered a factory for class " << a_class_name << " at " << (*fMap)[ a_class_name ] << ", factory #" << fMap->size()-1 << " for " << this );
    }

    template< class XBaseType, typename ... XArgs >
    bool factory< XBaseType, XArgs... >::has_class(const std::string& a_class_name ) const
    {
        return fMap->find( a_class_name ) != fMap->end();
    }

    template< class XBaseType, typename ... XArgs >
    void factory< XBaseType, XArgs... >::remove_class(const std::string& a_class_name )
    {
        LDEBUG( slog_fact, "Removing factory for class " << a_class_name << " from " << this );
        FactoryIt iter = fMap->find( a_class_name );
        if( iter != fMap->end() ) fMap->erase( iter );
        return;
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
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        return fMap->begin();
    }

    template< class XBaseType, typename ... XArgs >
    typename factory< XBaseType, XArgs... >::FactoryCIt factory< XBaseType, XArgs... >::end() const
    {
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        return fMap->end();
    }

    // registrar

    template< class XBaseType, class XDerivedType, typename ... XArgs >
    registrar< XBaseType, XDerivedType, XArgs... >::registrar( const std::string& a_class_name ) :
            base_registrar< XBaseType, XArgs... >(),
            f_class_name( a_class_name )
    {
        register_class( a_class_name );
    }

    template< class XBaseType, class XDerivedType, typename ... XArgs >
    registrar< XBaseType, XDerivedType, XArgs... >::~registrar()
    {
        factory< XBaseType, XArgs... >::get_instance()->remove_class( f_class_name );
    }

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


    //*******************************************
    // Implementations (partial specialization)
    //*******************************************

    // factory

    template< class XBaseType >
    XBaseType* factory< XBaseType, void >::create( const FactoryCIt& iter )
    {
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        return iter->second->create();
    }

    template< class XBaseType >
    XBaseType* factory< XBaseType, void >::create( const std::string& a_class_name )
    {
        //std::cout << "this factory (" << this << ") has " << fMap->size() << " entries" << std::endl;
        //for( FactoryCIt iter = fMap->begin(); iter != fMap->end(); ++iter )
        //{
        //    std::cout << "this factory has: " << iter->first << " at " << iter->second << std::endl;
        //}
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        FactoryCIt it = fMap->find( a_class_name );
        if( it == fMap->end() )
        {
            LERROR( slog_fact, "Did not find factory for <" << a_class_name << ">." );
            return NULL;
        }

        return it->second->create();
    }

    template< class XBaseType >
    void factory< XBaseType, void >::register_class( const std::string& a_class_name, const base_registrar< XBaseType >* a_registrar )
    {
        // A local (static) logger is created inside this function to avoid static initialization order problems
        LOGGER( slog_factory_reg, "factory-register");

        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        FactoryCIt it = fMap->find(a_class_name);
        if (it != fMap->end())
        {
            LERROR( slog_factory_reg, "Already have factory registered for <" << a_class_name << ">." );
            return;
        }
        fMap->insert(std::pair< std::string, const base_registrar< XBaseType >* >(a_class_name, a_registrar));
        LDEBUG( slog_factory_reg, "Registered a factory for class " << a_class_name << ", factory #" << fMap->size()-1 << " for " << this );
    }

    template< class XBaseType >
    bool factory< XBaseType, void >::has_class(const std::string& a_class_name ) const
    {
        return fMap->find( a_class_name ) != fMap->end();
    }

    template< class XBaseType >
    void factory< XBaseType, void >::remove_class(const std::string& a_class_name )
    {
        LDEBUG( slog_fact, "Removing factory for class " << a_class_name << " from " << this );
        FactoryIt iter = fMap->find( a_class_name );
        if( iter != fMap->end() ) fMap->erase( iter );
        return;
    }

    template< class XBaseType >
    factory< XBaseType, void >::factory() :
        fMap(new FactoryMap()),
        f_factory_mutex()
    {}

    template< class XBaseType >
    factory< XBaseType, void >::~factory()
    {
        delete fMap;
    }

    template< class XBaseType >
    typename factory< XBaseType, void >::FactoryCIt factory< XBaseType, void >::begin() const
    {
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        return fMap->begin();
    }

    template< class XBaseType >
    typename factory< XBaseType, void >::FactoryCIt factory< XBaseType, void >::end() const
    {
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        return fMap->end();
    }

    // registrar

    template< class XBaseType, class XDerivedType >
    registrar< XBaseType, XDerivedType, void >::registrar( const std::string& a_class_name ) :
            base_registrar< XBaseType >(),
            f_class_name( a_class_name )
    {
        register_class( a_class_name );
    }

    template< class XBaseType, class XDerivedType >
    registrar< XBaseType, XDerivedType, void >::~registrar()
    {
        factory< XBaseType >::get_instance()->remove_class( f_class_name );
    }

    template< class XBaseType, class XDerivedType >
    void registrar< XBaseType, XDerivedType, void >::register_class( const std::string& a_class_name ) const
    {
        factory< XBaseType >::get_instance()->register_class( a_class_name, this );
        return;
    }

    template< class XBaseType, class XDerivedType >
    XBaseType* registrar< XBaseType, XDerivedType, void >::create() const
    {
        return dynamic_cast< XBaseType* >( new XDerivedType() );
    }

} /* namespace scarab */
#endif /* SCARAB_FACTORY_HH_ */
