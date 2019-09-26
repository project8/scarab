/*
 * scarab_version.hh
 *
 *  Created on: Mar 20, 2013
 *      Author: N.S. Oblath
 */

#ifndef SCARAB_VERSION_HH_
#define SCARAB_VERSION_HH_

#include "version_semantic.hh"
    
namespace scarab
{
    
    class SCARAB_API version : public version_semantic
    {
        public:
            version();
            ~version();
    };

} // namespace scarab

#endif /* SCARAB_VERSION_HH_ */
