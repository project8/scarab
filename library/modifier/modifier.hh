/*
 * modifier.hh
 *
 *  Created on: Feb 12, 2020
 *      Author: N. Buzinsky
 */

#ifndef SCARAB_MODIFIER_HH_
#define SCARAB_MODIFIER_HH_

#include "scarab_api.hh"

namespace scarab
{
    class param_node;

    class SCARAB_API modifier
    {
        public:
            modifier();
            virtual ~modifier();

        public:
            virtual void modify( param_node &a_config ) = 0;

    };

} /* namespace scarab */

#endif /* SCARAB_MODIFIER_HH_ */
