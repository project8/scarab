/*
 * factory.hh
 *
 *  created on: Jul 31, 2012
 *      Author: N.S. Oblath
 * 
 *  The factory is a special kind of indexed_factory with a string index.
 * 
 *  Why is factory based on indexed_factory?  Because factory existed first with the string index, 
 *  and then factories with non-string indices were desired, 
 *  so indexed_factory was created as a generalization of factory.
 * 
 *  Note on registrar lifespan: when a registrar is deleted, it un-registers itself from the factory.
 *     This is because the registrar itself is used to create objects, and the act of registering 
 *     stores a pointer in the factory.  The factory does _not_ assume ownership of the registrar.
 *     Therefore the registrar needs to exist for as long as it might be needed.
 */

#ifndef SCARAB_FACTORY_HH_
#define SCARAB_FACTORY_HH_

#include "indexed_factory.hh"

#include <string>

namespace scarab
{

    template< class XBaseType, class... XArgs >
    using factory = indexed_factory< std::string, XBaseType, XArgs... >;

    template< class XBaseType, class... XArgs >
    using registrar = indexed_registrar< std::string, XBaseType, XArgs... >;
    
} /* namespace scarab */
#endif /* SCARAB_FACTORY_HH_ */
