/*
 * modifier.hh
 *
 *  Created on: Feb 12, 2019
 *      Author: N. Buzinsky
 */

#ifndef SCARAB_MODIFIER_HH_
#define SCARAB_MODIFIER_HH_

#include "param_node.hh"

namespace scarab
{

    class SCARAB_API modifier
    {
        public:
            modifier();
            ~modifier();

        public:
            virtual void modify(param_node &a_config) = 0;

    };

} /* namespace scarab */

#endif /* SCARAB_MODIFIER_HH_ */
