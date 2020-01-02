/*
 * indexed_factory.hh
 *
 *  created on: Oct 4, 2019
 *      Author: N.S. Oblath
 * 
 *  This factory has the same functionality as is found in factory.hh plus the option of choosing the index type.
 *
 *  Note on registrar lifespan: when a registrar is deleted, it un-registers itself from the factory.
 *     This is because the registrar itself is used to create objects, and the act of registering 
 *     stores a pointer in the factory.  The factory does _not_ assume ownership of the registrar.
 *     Therefore the registrar needs to exist for as long as it might be needed.
 */

#ifndef SCARAB_INDEXED_FACTORY_HH_
#define SCARAB_INDEXED_FACTORY_HH_

#include "singleton.hh"

#include "error.hh"
#include "logger.hh"

#include <map>
#include <string>
#include <utility>

namespace scarab
{
    LOGGER( slog_ind_fact, "indexed_factory" );

    //********************
    // Class definitions
    //********************

    template< class XIndexType, class XBaseType, typename ... XArgs >
    class indexed_factory;

    // base_registrar

    template< class XBaseType, typename ... XArgs >
    class base_registrar
    {
        public:
            base_registrar() {}
            virtual ~base_registrar() {}

            virtual XBaseType* create( XArgs ... args ) const = 0;

    };

    // registrar

    template< class XIndexType, class XBaseType, class XDerivedType, typename ... XArgs >
    class indexed_registrar : public base_registrar< XBaseType, XArgs... >
    {
        public:
            indexed_registrar( const XIndexType& a_index );
            virtual ~indexed_registrar();

            void register_class() const;

            XBaseType* create( XArgs ... args ) const;

        protected:
            XIndexType f_index;
    };

    // indexed_factory

    template< class XIndexType, class XBaseType, typename ... XArgs >
    class indexed_factory : public singleton< indexed_factory< XIndexType, XBaseType, XArgs... > >
    {
        public:
            typedef std::map< XIndexType, const base_registrar< XBaseType, XArgs... >* > FactoryMap;
            typedef typename FactoryMap::value_type FactoryEntry;
            typedef typename FactoryMap::iterator FactoryIt;
            typedef typename FactoryMap::const_iterator FactoryCIt;

        public:
            XBaseType* create( const XIndexType& a_index, XArgs ... args );
            XBaseType* create( const FactoryCIt& iter, XArgs ... args );

            void register_class( const XIndexType& a_index, const base_registrar< XBaseType, XArgs... >* base_registrar );
            bool has_class( const XIndexType& a_index ) const;
            void remove_class( const XIndexType& a_index );

            FactoryCIt begin() const;
            FactoryCIt end() const;

        protected:
            FactoryMap* fMap;
            mutable std::mutex f_factory_mutex;

        protected:
            allow_singleton_access( indexed_factory );
            indexed_factory();
            ~indexed_factory();
    };


    //*****************************************
    // Partial specialization for empty XArgs
    //*****************************************

    template< class XBaseType >
    class base_registrar< XBaseType, void >
    {
        public:
            base_registrar() {}
            virtual ~base_registrar() {}

            virtual XBaseType* create() const = 0;

    };

    template< class XIndexType, class XBaseType, class XDerivedType >
    class indexed_registrar< XIndexType, XBaseType, XDerivedType, void > : public base_registrar< XBaseType >
    {
        public:
            indexed_registrar( const XIndexType& a_index );
            virtual ~indexed_registrar();

            void register_class() const;

            XBaseType* create() const;

        protected:
            XIndexType f_index;
    };


    template< class XIndexType, class XBaseType >
    class indexed_factory< XIndexType, XBaseType, void > : public singleton< indexed_factory< XIndexType, XBaseType > >
    {
        public:
            typedef std::map< XIndexType, const base_registrar< XBaseType >* > FactoryMap;
            typedef typename FactoryMap::value_type FactoryEntry;
            typedef typename FactoryMap::iterator FactoryIt;
            typedef typename FactoryMap::const_iterator FactoryCIt;

        public:
            XBaseType* create( const XIndexType& a_index );
            XBaseType* create( const FactoryCIt& iter );

            void register_class( const XIndexType& a_index, const base_registrar< XBaseType >* base_registrar );
            bool has_class( const XIndexType& a_index ) const;
            void remove_class( const XIndexType& a_index );

            FactoryCIt begin() const;
            FactoryCIt end() const;

        protected:
            FactoryMap* fMap;
            std::mutex f_factory_mutex;

        protected:
            allow_singleton_access( indexed_factory );
            indexed_factory();
            ~indexed_factory();
    };


    //******************
    // Implementations
    //******************

    // indexed_factory

    template< class XIndexType, class XBaseType, typename ... XArgs >
    XBaseType* indexed_factory< XIndexType, XBaseType, XArgs... >::create( const FactoryCIt& iter, XArgs ... args )
    {
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        return iter->second->create( args... );
    }

    template< class XIndexType, class XBaseType, typename ... XArgs >
    XBaseType* indexed_factory< XIndexType, XBaseType, XArgs... >::create( const XIndexType& a_index, XArgs ... args )
    {
        //std::cout << "this factory (" << this << ") has " << fMap->size() << " entries" << std::endl;
        //for( FactoryCIt iter = fMap->begin(); iter != fMap->end(); ++iter )
        //{
        //    std::cout << "this factory has: " << iter->first << " at " << iter->second << std::endl;
        //}
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        FactoryCIt it = fMap->find( a_index );
        if( it == fMap->end() )
        {
            LERROR( slog_ind_fact, "Did not find indexed_factory for <" << a_index << ">." );
            return nullptr;
        }

        return it->second->create( args... );
    }

    template< class XIndexType, class XBaseType, typename ... XArgs >
    void indexed_factory< XIndexType, XBaseType, XArgs... >::register_class( const XIndexType& a_index, const base_registrar< XBaseType, XArgs... >* a_registrar )
    {
        // A local (static) logger is created inside this function to avoid static initialization order problems
        LOGGER( slog_ind_factory_reg, "indexed_factory-register");

        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        FactoryCIt it = fMap->find(a_index);
        if (it != fMap->end())
        {
            throw error() << "Already have indexed_factory registered for <" << a_index << ">.";
        }
        fMap->insert(std::pair< XIndexType, const base_registrar< XBaseType, XArgs... >* >(a_index, a_registrar));
        LDEBUG( slog_ind_factory_reg, "Registered a indexed_factory for class " << a_index << " at " << (*fMap)[ a_index ] << ", indexed_factory #" << fMap->size()-1 << " for " << this );
    }

    template< class XIndexType, class XBaseType, typename ... XArgs >
    bool indexed_factory< XIndexType, XBaseType, XArgs... >::has_class(const XIndexType& a_index ) const
    {
        return fMap->find( a_index ) != fMap->end();
    }

    template< class XIndexType, class XBaseType, typename ... XArgs >
    void indexed_factory< XIndexType, XBaseType, XArgs... >::remove_class(const XIndexType& a_index )
    {
        LDEBUG( slog_ind_fact, "Removing indexed_factory for class " << a_index << " from " << this );
        FactoryIt iter = fMap->find( a_index );
        if( iter != fMap->end() ) fMap->erase( iter );
        return;
    }

    template< class XIndexType, class XBaseType, typename ... XArgs >
    indexed_factory< XIndexType, XBaseType, XArgs... >::indexed_factory() :
        fMap(new FactoryMap()),
        f_factory_mutex()
    {}

    template< class XIndexType, class XBaseType, typename ... XArgs >
    indexed_factory< XIndexType, XBaseType, XArgs... >::~indexed_factory()
    {
        delete fMap;
    }

    template< class XIndexType, class XBaseType, typename ... XArgs >
    typename indexed_factory< XIndexType, XBaseType, XArgs... >::FactoryCIt indexed_factory< XIndexType, XBaseType, XArgs... >::begin() const
    {
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        return fMap->begin();
    }

    template< class XIndexType, class XBaseType, typename ... XArgs >
    typename indexed_factory< XIndexType, XBaseType, XArgs... >::FactoryCIt indexed_factory< XIndexType, XBaseType, XArgs... >::end() const
    {
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        return fMap->end();
    }

    // registrar

    template< class XIndexType, class XBaseType, class XDerivedType, typename ... XArgs >
    indexed_registrar< XIndexType, XBaseType, XDerivedType, XArgs... >::indexed_registrar( const XIndexType& a_index ) :
            base_registrar< XBaseType, XArgs... >(),
            f_index( a_index )
    {
        register_class();
    }

    template< class XIndexType, class XBaseType, class XDerivedType, typename ... XArgs >
    indexed_registrar< XIndexType, XBaseType, XDerivedType, XArgs... >::~indexed_registrar()
    {
        indexed_factory< XIndexType, XBaseType, XArgs... >::get_instance()->remove_class( f_index );
    }

    template< class XIndexType, class XBaseType, class XDerivedType, typename ... XArgs >
    void indexed_registrar< XIndexType, XBaseType, XDerivedType, XArgs... >::register_class() const
    {
        indexed_factory< XIndexType, XBaseType, XArgs... >::get_instance()->register_class( f_index, this );
        return;
    }

    template< class XIndexType, class XBaseType, class XDerivedType, typename ... XArgs >
    XBaseType* indexed_registrar< XIndexType, XBaseType, XDerivedType, XArgs... >::create( XArgs... args ) const
    {
        return dynamic_cast< XBaseType* >( new XDerivedType( args... ) );
    }


    //*******************************************
    // Implementations (partial specialization)
    //*******************************************

    // indexed_factory

    template< class XIndexType, class XBaseType >
    XBaseType* indexed_factory< XIndexType, XBaseType, void >::create( const FactoryCIt& iter )
    {
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        return iter->second->create();
    }

    template< class XIndexType, class XBaseType >
    XBaseType* indexed_factory< XIndexType, XBaseType, void >::create( const XIndexType& a_index )
    {
        //std::cout << "this indexed_factory (" << this << ") has " << fMap->size() << " entries" << std::endl;
        //for( FactoryCIt iter = fMap->begin(); iter != fMap->end(); ++iter )
        //{
        //    std::cout << "this indexed_factory has: " << iter->first << " at " << iter->second << std::endl;
        //}
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        FactoryCIt it = fMap->find( a_index );
        if( it == fMap->end() )
        {
            LERROR( slog_ind_fact, "Did not find indexed_factory for <" << a_index << ">." );
            return nullptr;
        }

        return it->second->create();
    }

    template< class XIndexType, class XBaseType >
    void indexed_factory< XIndexType, XBaseType, void >::register_class( const XIndexType& a_index, const base_registrar< XBaseType >* a_registrar )
    {
        // A local (static) logger is created inside this function to avoid static initialization order problems
        LOGGER( slog_ind_factory_reg, "indexed_factory-register");

        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        FactoryCIt it = fMap->find(a_index);
        if (it != fMap->end())
        {
            throw error() << "Already have indexed_factory registered for <" << a_index << ">.";
            return;
        }
        fMap->insert(std::pair< std::string, const base_registrar< XIndexType, XBaseType >* >(a_index, a_registrar));
        LDEBUG( slog_ind_factory_reg, "Registered a indexed_factory for class " << a_index << ", indexed_factory #" << fMap->size()-1 << " for " << this );
    }

    template< class XIndexType, class XBaseType >
    bool indexed_factory< XIndexType, XBaseType, void >::has_class(const XIndexType& a_index ) const
    {
        return fMap->find( a_index ) != fMap->end();
    }

    template< class XIndexType, class XBaseType >
    void indexed_factory< XIndexType, XBaseType, void >::remove_class(const XIndexType& a_index )
    {
        LDEBUG( slog_ind_fact, "Removing indexed_factory for class " << a_index << " from " << this );
        FactoryIt iter = fMap->find( a_index );
        if( iter != fMap->end() ) fMap->erase( iter );
        return;
    }

    template< class XIndexType, class XBaseType >
    indexed_factory< XIndexType, XBaseType, void >::indexed_factory() :
        fMap(new FactoryMap()),
        f_factory_mutex()
    {}

    template< class XIndexType, class XBaseType >
    indexed_factory< XIndexType, XBaseType, void >::~indexed_factory()
    {
        delete fMap;
    }

    template< class XIndexType, class XBaseType >
    typename indexed_factory< XIndexType, XBaseType, void >::FactoryCIt indexed_factory< XIndexType, XBaseType, void >::begin() const
    {
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        return fMap->begin();
    }

    template< class XIndexType, class XBaseType >
    typename indexed_factory< XIndexType, XBaseType, void >::FactoryCIt indexed_factory< XIndexType, XBaseType, void >::end() const
    {
        std::unique_lock< std::mutex > t_lock( this->f_factory_mutex );
        return fMap->end();
    }

    // registrar

    template< class XIndexType, class XBaseType, class XDerivedType >
    indexed_registrar< XIndexType, XBaseType, XDerivedType, void >::indexed_registrar( const XIndexType& a_index ) :
            base_registrar< XBaseType >(),
            f_index( a_index )
    {
        register_class();
    }

    template< class XIndexType, class XBaseType, class XDerivedType >
    indexed_registrar< XIndexType, XBaseType, XDerivedType, void >::~indexed_registrar()
    {
        indexed_factory< XIndexType, XBaseType >::get_instance()->remove_class( f_index );
    }

    template< class XIndexType, class XBaseType, class XDerivedType >
    void indexed_registrar< XIndexType, XBaseType, XDerivedType, void >::register_class() const
    {
        indexed_factory< XIndexType, XBaseType >::get_instance()->register_class( f_index, this );
        return;
    }

    template< class XIndexType, class XBaseType, class XDerivedType >
    XBaseType* indexed_registrar< XIndexType, XBaseType, XDerivedType, void >::create() const
    {
        return dynamic_cast< XBaseType* >( new XDerivedType() );
    }

} /* namespace scarab */
#endif /* SCARAB_INDEXED_FACTORY_HH_ */
